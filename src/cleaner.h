#ifndef CLEANER_H
#define CLEANER_H

#include <QObject>
#include <QFile>

class Cleaner : public QObject
{
    Q_OBJECT
public:
    enum Option {
        StripComments  = 1 << 0
    };
    Q_DECLARE_FLAGS(Options, Option)

    explicit Cleaner(QObject *parent, const QString &inputFile, const QString &outputFile, const Options flags = 0);

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
