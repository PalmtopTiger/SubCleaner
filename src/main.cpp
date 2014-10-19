#include <QCoreApplication>
#include <QCommandLineParser>
#include <QFileInfo>
#include <QDir>
#include <QTimer>
#include "cleaner.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationName("SubCleaner");
    app.setApplicationVersion("2.0");
    app.setOrganizationName("Unlimited Web Works");

    QCommandLineParser parser;
    parser.setApplicationDescription("This program strips fonts and graphics from SSA/ASS files.");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("input", "Input subtitle file.");
    parser.addPositionalArgument("output", "Output subtitle file.");
    parser.process(app);
    const QStringList args = parser.positionalArguments();

    if (args.empty())
    {
        fprintf(stderr, "%s\n", qPrintable("Input file doesn't set."));
        ::exit(EXIT_FAILURE);
    }
    QString inputFile = args.at(0),
            outputFile;

    if (args.length() >= 2)
    {
        outputFile = args.at(1);
    }
    else
    {
        QFileInfo fileInfo(inputFile);
        QStringList name;
        name << fileInfo.baseName()
             << "clean";
        if (!fileInfo.completeSuffix().isEmpty())
        {
            name << fileInfo.completeSuffix();
        }
        outputFile = fileInfo.dir().filePath(name.join('.'));
    }

    Cleaner *cleaner = new Cleaner(&app, inputFile, outputFile);
    QObject::connect(cleaner, SIGNAL(finished()), &app, SLOT(quit()));
    QTimer::singleShot(0, cleaner, SLOT(run()));
    return app.exec();
}
