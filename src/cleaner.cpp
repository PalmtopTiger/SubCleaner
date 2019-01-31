/*
 * This file is part of SubCleaner.
 * Copyright (C) 2014-2019  Andrey Efremov <duxus@yandex.ru>
 *
 * SubCleaner is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SubCleaner is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SubCleaner.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "cleaner.h"
#include "script.h"
#include <QCoreApplication>
#include <QTextCodec>
#include <QSet>

Cleaner::Cleaner(QObject *parent, const QString &inputFile, const QString &outputFile, const Options flags) :
    QObject(parent),
    _inputFile(inputFile),
    _outputFile(outputFile),
    _flags(flags)
{}

void Cleaner::run()
{
    // Read input file
    if ( !_inputFile.open(QFile::ReadOnly | QFile::Text) )
    {
        fprintf(stderr, "%s\n", qPrintable(QString("Can't read file \"%1\".").arg(_inputFile.fileName())));
        QCoreApplication::exit(EXIT_FAILURE);
        return;
    }

    QTextStream inputStream(&_inputFile);
    const Script::ScriptType scriptType = Script::DetectFormat(inputStream);
    Script::Script script;
    switch (scriptType)
    {
    case Script::SCR_SSA:
    case Script::SCR_ASS:
        if ( !Script::ParseSSA(inputStream, script) )
        {
            _inputFile.close();
            fprintf(stderr, "%s\n", qPrintable(QString("\"%1\" isn't an SSA/ASS file.").arg(_inputFile.fileName())));
            QCoreApplication::exit(EXIT_FAILURE);
            return;
        }
        break;

    default:
        _inputFile.close();
        fprintf(stderr, "%s\n", qPrintable(QString("\"%1\" file format is unknown.").arg(_inputFile.fileName())));
        QCoreApplication::exit(EXIT_FAILURE);
        return;
    }
    _inputFile.close();

    // Strip comments
    if (_flags.testFlag(StripComments))
    {
        for (Script::Line::Named* const line : qAsConst(script.header.content)) {
            line->clearBefore();
        }
        script.header.clearAfter();

        for (Script::Line::Style* const line : qAsConst(script.styles.content)) {
            line->clearBefore();
        }
        script.styles.clearAfter();

        for (Script::Line::Event* const line : qAsConst(script.events.content)) {
            line->clearBefore();
        }
        script.events.clearAfter();

        script.clearBefore();
        script.clearAfter();
    }

    // Strip info lines
    if (_flags.testFlag(StripStyleInfo))
    {
        // ScriptType добавляется всегда
        const QSet<QString> importantLines = {
            QString("WrapStyle").toLower(),
            QString("PlayResX").toLower(),
            QString("PlayResY").toLower(),
            QString("ScaledBorderAndShadow").toLower(),
            QString("YCbCr Matrix").toLower()
        };

        auto isUnimportant = [importantLines](const Script::Line::Named* const line) {
            return !importantLines.contains(line->name().toLower());
        };
        script.header.content.erase(std::remove_if(script.header.content.begin(), script.header.content.end(), isUnimportant),
                                    script.header.content.end());
    }

    // Strip fonts and graphics
    script.fonts.clear();
    script.graphics.clear();

    // Write output file
    if ( !_outputFile.open(QFile::WriteOnly | QFile::Text) )
    {
        fprintf(stderr, "%s\n", qPrintable(QString("Can't write file \"%1\".").arg(_inputFile.fileName())));
        QCoreApplication::exit(EXIT_FAILURE);
        return;
    }

    QTextStream outputStream(&_outputFile);
    outputStream.setCodec( QTextCodec::codecForName("UTF-8") );
    outputStream.setGenerateByteOrderMark(true);
    switch (scriptType)
    {
    case Script::SCR_SSA:
        Script::GenerateSSA(outputStream, script);
        break;

    case Script::SCR_ASS:
        Script::GenerateASS(outputStream, script);
        break;

    default:
        _outputFile.close();
        fprintf(stderr, "%s\n", qPrintable(QString("Houston, we have a problem.").arg(_inputFile.fileName())));
        QCoreApplication::exit(EXIT_FAILURE);
        return;
    }
    _outputFile.close();

    emit finished();
}
