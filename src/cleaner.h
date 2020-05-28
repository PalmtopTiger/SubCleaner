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

#ifndef CLEANER_H
#define CLEANER_H

#include <QObject>
#include <QFile>

class Cleaner : public QObject
{
    Q_OBJECT

public:
    enum Option {
        StripComments  = 1 << 0,
        StripStyleInfo = 1 << 1
    };
    Q_DECLARE_FLAGS(Options, Option)

    explicit Cleaner(QObject *parent, const QString &inputFile, const QString &outputFile, const Options flags = Options());

signals:
    void finished();

public slots:
    void run();

private:
    QFile _inputFile;
    QFile _outputFile;
    Options _flags;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(Cleaner::Options)

#endif // CLEANER_H
