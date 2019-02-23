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

enum CommandLineParseResult
{
    CommandLineOk,
    CommandLineError,
    CommandLineVersionRequested,
    CommandLineHelpRequested
};

CommandLineParseResult parseCommandLine(QCommandLineParser &parser, QString &hostname, QString *errorMessage)
{
    parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
    const QCommandLineOption hostNameOption(QStringList() << "d" << "destination", "Destination hostname or IP of the HeaterMeter device.", "hostname");
    parser.addOption(hostNameOption);
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
        return CommandLineError;
    }
    else {
        hostname = parser.value(hostNameOption);
        qDebug() << __PRETTY_FUNCTION__ << ": Found hostname" << hostname;
    }
    return CommandLineOk;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName("heatermeter");
    QCoreApplication::setApplicationVersion("0.1");
    QApplication::setOverrideCursor(Qt::BlankCursor);
    QString hostname;
    QCommandLineParser parser;
    parser.setApplicationDescription(QCoreApplication::translate("HeaterMeter", "Provide GUI frontend for HeaterMeter"));
    QString errorMessage;

    switch (parseCommandLine(parser, hostname, &errorMessage)) {
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

    qDebug() << __PRETTY_FUNCTION__ << ": Connecting to hostname" << hostname;
    HeaterMeter w(hostname);
    
    w.showFullScreen();
    w.getVersion();
    w.getConfig();
    w.show();

    return app.exec();
}

