#include "tool.h"
#include "blackcore/context_all_interfaces.h"
#include "blackmisc/propertyindexallclasses.h"
#include "blackmisc/avallclasses.h"
#include "blackmisc/pqallquantities.h"
#include "blackmisc/loghandler.h"
#include "blackmisc/worker.h"
#include "blacksound/soundgenerator.h"

#include <QTextStream>
#include <QString>
#include <QFuture>
#include <QMetaObject>
#include <QGenericReturnArgument>
#include <QGenericArgument>
#include <QtConcurrent/QtConcurrent>

using namespace BlackCore;
using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Audio;

namespace BlackMiscTest
{

    /*
     * Start a new process
     */
    QProcess *Tool::startNewProcess(const QString &executable, const QStringList &arguments, QObject *parent)
    {
        QProcess *process = new QProcess(parent);
        process->startDetached(executable, arguments);
        return process;
    }

    /*
     * Send data to testservice, this sends data to the slots on the server
     */
    void Tool::serverLoop(CRuntime *runtime)
    {
        QMetaObject::invokeMethod(CLogHandler::instance(), "enableConsoleOutput", Q_ARG(bool, false));

        Q_ASSERT(runtime);
        QThread::sleep(3); // let the DBus server startup

        // log
        CLogSubscriber applicationMessageSubscriber, audioMessageSubscriber, networkMessageSubscriber, ownAircraftMessageSubscriber, settingsMessageSubscriber, simulatorMessageSubscriber;
        CStatusMessage::StatusSeverity messageSeverity = CStatusMessage::SeverityInfo;
        auto refreshSubscriptionSeverities = [ & ]()
        {
            applicationMessageSubscriber.changeSubscription(CLogPattern::allOf(runtime->getIContextApplication()).withSeverityAtOrAbove(messageSeverity));
            audioMessageSubscriber.changeSubscription(CLogPattern::allOf(runtime->getIContextAudio()).withSeverityAtOrAbove(messageSeverity));
            networkMessageSubscriber.changeSubscription(CLogPattern::allOf(runtime->getIContextNetwork()).withSeverityAtOrAbove(messageSeverity));
            ownAircraftMessageSubscriber.changeSubscription(CLogPattern::allOf(runtime->getIContextOwnAircraft()).withSeverityAtOrAbove(messageSeverity));
            settingsMessageSubscriber.changeSubscription(CLogPattern::allOf(runtime->getIContextSettings()).withSeverityAtOrAbove(messageSeverity));
            simulatorMessageSubscriber.changeSubscription(CLogPattern::allOf(runtime->getIContextSimulator()).withSeverityAtOrAbove(messageSeverity));
        };
        refreshSubscriptionSeverities();

        QTextStream qtout(stdout);
        qtout << "Running on server here " << Tool::getPid() << " thread: " << QThread::currentThreadId() << endl;

        //
        // Server loop
        //
        QTextStream qtin(stdin);
        QString line;
        while (line != "x" && runtime)
        {
            qtout << "-------------" << endl;
            qtout << "Connected with network: " << (runtime->getIContextNetwork()->isConnected() ? "yes" : "no") << endl;
            qtout << "Thread id: " << QThread::currentThreadId() << endl;
            qtout << "-------------" << endl;
            qtout << "x .. to exit" << endl;
            qtout << "0 .. settings     1 .. vatlib audio devices    2 .. Qt audio devices" << endl;
            qtout << "h .. further commands" << endl;
            qtout << "-------------" << endl;

            line = qtin.readLine().toLower().trimmed();
            if (line.startsWith("0"))
            {
                qtout << "-------------" << endl;
                QString ret1;
                QMetaObject::invokeMethod(runtime->getIContextSettings(), "getSettingsFileName",
                                          Qt::BlockingQueuedConnection,
                                          Q_RETURN_ARG(QString, ret1));
                qtout << "Settings: " << ret1 << endl;


                QMetaObject::invokeMethod(runtime->getIContextSettings(), "getSettingsAsJsonString",
                                          Qt::BlockingQueuedConnection,
                                          Q_RETURN_ARG(QString, ret1));
                qtout << "JSON " << ret1 << endl;
            }
            else if (line.startsWith("1"))
            {
                qtout << "-------------" << endl;
                qtout << "vatlib audio devices" << endl;
                CAudioDeviceInfoList devices;
                QMetaObject::invokeMethod(runtime->getIContextAudio(), "getAudioDevices",
                                          Qt::BlockingQueuedConnection,
                                          Q_RETURN_ARG(BlackMisc::Audio::CAudioDeviceInfoList, devices));
                qtout << devices << endl;
            }
            else if (line.startsWith("2"))
            {
                qtout << "-------------" << endl;
                qtout << "Qt audio devices" << endl;
                BlackSound::CSoundGenerator::printAllQtSoundDevices();
            }
            else if (line.startsWith("level"))
            {
                line = line.replace("level", "").trimmed();
                messageSeverity = CStatusMessage::stringToSeverity(line);
                refreshSubscriptionSeverities();
                qtout << "Changed level to " << CStatusMessage::severityToString(messageSeverity) << endl;
            }
            else if (line.startsWith("log"))
            {
                line.replace("log", "");
                bool enable = line.endsWith("e");
                if (line.startsWith("app") || line.startsWith("all")) { applicationMessageSubscriber.enableConsoleOutput(enable); }
                if (line.startsWith("aud") || line.startsWith("all")) { audioMessageSubscriber.enableConsoleOutput(enable); }
                if (line.startsWith("net") || line.startsWith("all")) { networkMessageSubscriber.enableConsoleOutput(enable); }
                if (line.startsWith("own") || line.startsWith("all")) { ownAircraftMessageSubscriber.enableConsoleOutput(enable); }
                if (line.startsWith("set") || line.startsWith("all")) { settingsMessageSubscriber.enableConsoleOutput(enable); }
                if (line.startsWith("sim") || line.startsWith("all")) { simulatorMessageSubscriber.enableConsoleOutput(enable); }
            }
            else if (line.startsWith("h"))
            {
                qtout << "1) logging severity levels:" << endl;
                qtout << "   d, i, w, e  example: level d for debug level" << endl;
                qtout << "2) logging for contexts:" << endl;
                qtout << "   log + context + [e]nabled / [d]isabled" << endl;
                qtout << "   contexts: app / aud / net / own (aircraft) / set / sim / all" << endl;
                qtout << "   examples: logappd, lognete, logsimd, logalle" << endl;
                qtout << "3) all . commands can be used, e.g. .com1 127.35" << endl;
                qtout << endl;
            }
            else if (line.startsWith("."))
            {
                // handle dot commands
                bool c = runtime->parseCommandLine(line);
                if (c) { qtout << "Handled command " << line; }
                else   { qtout << "Not handled " <<  line; }
                qtout << endl;
            }
        }
        QCoreApplication::quit();
    }
} // namespace
