#ifndef CLEANER_H
#define CLEANER_H

#include <QObject>
#include <QFile>

class Cleaner : public QObject
{
    Q_OBJECT
public:
    explicit Cleaner(QObject *parent, const QString &inputFile, const QString &outputFile);

signals:
    void finished();

public slots:
    void run();

private:
    QFile _inputFile;
    QFile _outputFile;
};

#endif // CLEANER_H
