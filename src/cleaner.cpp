#include <QCoreApplication>
#include <QTextCodec>
#include "cleaner.h"
#include "script.h"

Cleaner::Cleaner(QObject *parent, const QString &inputFile, const QString &outputFile) :
    QObject(parent),
    _inputFile(inputFile),
    _outputFile(outputFile)
{}

void Cleaner::run()
{
    // Read input file
    if ( !_inputFile.open(QFile::ReadOnly | QFile::Text) )
    {
        qCritical() << QString("Can't read file \"%1\".").arg(_inputFile.fileName());
        QCoreApplication::exit(EXIT_FAILURE);
        return;
    }

    QTextStream inputStream(&_inputFile);
    Script::ScriptType scriptType = Script::DetectFormat(inputStream);
    Script::Script script;
    switch (scriptType)
    {
    case Script::SCR_SSA:
    case Script::SCR_ASS:
        if ( !Script::ParseSSA(inputStream, script) )
        {
            _inputFile.close();
            qCritical() << QString("\"%1\" isn't an SSA/ASS file.").arg(_inputFile.fileName());
            QCoreApplication::exit(EXIT_FAILURE);
            return;
        }
        break;

    default:
        _inputFile.close();
        qCritical() << QString("\"%1\" file format is unknown.").arg(_inputFile.fileName());
        QCoreApplication::exit(EXIT_FAILURE);
        return;
    }
    _inputFile.close();

    // Strip fonts and graphics
    script.fonts.clear();
    script.graphics.clear();

    // Write output file
    if ( !_outputFile.open(QFile::WriteOnly | QFile::Text) )
    {
        qCritical() << QString("Can't write file \"%1\".").arg(_inputFile.fileName());
        QCoreApplication::exit(EXIT_FAILURE);
        return;
    }

    QTextStream outputStream(&_outputFile);
    outputStream.setCodec( QTextCodec::codecForName("UTF-8") );
    outputStream.setGenerateByteOrderMark(true);
    switch (scriptType)
    {
    case Script::SCR_SSA:
        if ( !Script::GenerateSSA(outputStream, script) )
        {
            _outputFile.close();
            qCritical() << QString("SSA output error.").arg(_inputFile.fileName());
            QCoreApplication::exit(EXIT_FAILURE);
            return;
        }
        break;

    case Script::SCR_ASS:
        if ( !Script::GenerateASS(outputStream, script) )
        {
            _outputFile.close();
            qCritical() << QString("ASS output error.").arg(_inputFile.fileName());
            QCoreApplication::exit(EXIT_FAILURE);
            return;
        }
        break;

    default:
        _outputFile.close();
        qCritical() << QString("Houston, we have a problem.").arg(_inputFile.fileName());
        QCoreApplication::exit(EXIT_FAILURE);
        return;
    }
    _outputFile.close();

    emit finished();
}
