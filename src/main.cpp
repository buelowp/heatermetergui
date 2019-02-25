/**
 * HeaterMeter
 * main.cpp
 * Copyright Peter Buelow (goballstate@gmail.com)
 * 
 * MIT License
 *
 * Copyright (c) 2019 Peter Buelow
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#include "heatermeter.h"
#include <QApplication>

struct CommandArguments {
    QString hostName;
    int minGraphTemp;
    int maxGraphTemp;
};

enum CommandLineParseResult
{
    CommandLineOk,
    CommandLineError,
    CommandLineVersionRequested,
    CommandLineHelpRequested
};

CommandLineParseResult parseCommandLine(QCommandLineParser &parser, struct CommandArguments *ca, QString *errorMessage)
{
    parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
    const QCommandLineOption hostNameOption(QStringList() << "d" << "destination", "Destination hostname or IP of the HeaterMeter device.", "hostname");
    parser.addOption(hostNameOption);
    const QCommandLineOption minimumOption(QStringList() << "n" << "min", "Minimum temp to show on the graph, default is 50", "min");
    parser.addOption(minimumOption);
    const QCommandLineOption maximumOption(QStringList() << "x" << "max", "Maximum temp to show on the graph, default is 400", "max");
    parser.addOption(maximumOption);
    const QCommandLineOption helpOption = parser.addHelpOption();
    const QCommandLineOption versionOption = parser.addVersionOption();

    if (!parser.parse(QApplication::arguments())) {
        *errorMessage = parser.errorText();
        return CommandLineError;
    }

    if (parser.optionNames().size() < 1) {
        return CommandLineError;
    }
    
    if (parser.isSet(versionOption)) {
        return CommandLineVersionRequested;
    }

    if (parser.isSet(helpOption)) {
        return CommandLineHelpRequested;
    }

    if (!parser.isSet(hostNameOption)) {
        ca->hostName = "localhost";
        return CommandLineError;
    }
    else {
        ca->hostName = parser.value(hostNameOption);
        qDebug() << __PRETTY_FUNCTION__ << ": Found hostname" << ca->hostName;
    }
    if (!parser.isSet(maximumOption)) {
        ca->maxGraphTemp = 400;
    }
    else {
        qDebug() << __PRETTY_FUNCTION__ << ": setting graph maximum to" << ca->maxGraphTemp;
        ca->maxGraphTemp = parser.value(maximumOption).toInt();
    }
    if (!parser.isSet(minimumOption)) {
        ca->minGraphTemp = 50;
    }
    else {
        qDebug() << __PRETTY_FUNCTION__ << ": setting graph minimum to" << ca->minGraphTemp;
        ca->minGraphTemp = parser.value(minimumOption).toInt();
    }
    return CommandLineOk;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName("heatermeter");
    QCoreApplication::setApplicationVersion("0.1");
    QApplication::setOverrideCursor(Qt::BlankCursor);
    QCommandLineParser parser;
    parser.setApplicationDescription(QCoreApplication::translate("HeaterMeter", "Provide GUI frontend for HeaterMeter"));
    QString errorMessage;
    struct CommandArguments ca;

    switch (parseCommandLine(parser, &ca, &errorMessage)) {
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

    if (ca.hostName.size() == 0) {
        qWarning() << __PRETTY_FUNCTION__ << ": Error, no hostname captured, exiting...";
        exit(10);
    }
    
    qDebug() << __PRETTY_FUNCTION__ << ": Connecting to hostname" << ca.hostName;
    HeaterMeter w(ca.hostName);
    
    w.setMinGraphTemp(ca.minGraphTemp);
    w.setMaxGraphTemp(ca.maxGraphTemp);
    w.showFullScreen();
    w.getVersion();
    w.getConfig();
    w.show();

    return app.exec();
}

