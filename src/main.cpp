#include "heatermeter.h"
#include <QApplication>

enum CommandLineParseResult
{
    CommandLineOk,
    CommandLineError,
    CommandLineVersionRequested,
    CommandLineHelpRequested
};

CommandLineParseResult parseCommandLine(QCommandLineParser &parser, QString *hostname, QString *errorMessage)
{
    parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
    const QCommandLineOption hostNameOption(QStringList() << "h" << "hostname", "Hostname or IP of the HeaterMeter device.");
    parser.addOption(hostNameOption);
    const QCommandLineOption helpOption = parser.addHelpOption();
    const QCommandLineOption versionOption = parser.addVersionOption();

    if (!parser.parse(QCoreApplication::arguments())) {
        *errorMessage = parser.errorText();
        return CommandLineError;
    }

    if (parser.isSet(versionOption))
        return CommandLineVersionRequested;

    if (parser.isSet(helpOption))
        return CommandLineHelpRequested;

    if (!parser.isSet(hostNameOption)) {
        return CommandLineError;
    }
    else {
        *hostname = parser.value(hostNameOption);
    }
    return CommandLineOk;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName("heatermeter");
    QCoreApplication::setApplicationVersion("0.1");
    QString hostname;
    QCommandLineParser parser;
    parser.setApplicationDescription(QCoreApplication::translate("HeaterMeter", "Provide GUI frontend for HeaterMeter"));
    QString errorMessage;

    switch (parseCommandLine(parser, &hostname, &errorMessage)) {
    case CommandLineOk:
        break;
    case CommandLineError:
        fputs(qPrintable(errorMessage), stderr);
        fputs("\n\n", stderr);
        fputs(qPrintable(parser.helpText()), stderr);
        return 1;
    case CommandLineVersionRequested:
        printf("%s %s\n", qPrintable(QCoreApplication::applicationName()),
               qPrintable(QCoreApplication::applicationVersion()));
        return 0;
    case CommandLineHelpRequested:
        parser.showHelp();
        Q_UNREACHABLE();
    }

    HeaterMeter w(hostname);

    w.setGeometry(0,0, 800, 480);
    w.getVersion();
    w.show();

    return app.exec();
}

