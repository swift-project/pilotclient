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
    void Tool::serverLoop(BlackCore::CRuntime *runtime)
    {
        QMetaObject::invokeMethod(CLogHandler::instance(), "enableConsoleOutput", Q_ARG(bool, false));

        Q_ASSERT(runtime);
        QThread::sleep(3); // let the DBus server startup

        // log
        //! \todo make thread safe or ..?
//        CLogSubscriber messageSubscriber;
//        messageSubscriber.enableConsoleOutput(true);
//        messageSubscriber.changeSubscription(CLogPattern().withSeverityAtOrAbove(CStatusMessage::SeverityInfo));

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
                CAudioDeviceList devices;
                QMetaObject::invokeMethod(runtime->getIContextAudio(), "getAudioDevices",
                                          Qt::BlockingQueuedConnection,
                                          Q_RETURN_ARG(BlackMisc::Audio::CAudioDeviceList, devices));
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
                CStatusMessage::StatusSeverity severity = CStatusMessage::stringToSeverity(line);
                // messageSubscriber.changeSubscription(CLogPattern().withSeverityAtOrAbove(severity));
                qtout << "Changed level to" << CStatusMessage::severityToString(severity);
            }
            else if (line.startsWith("log"))
            {
                line.replace("log", "");
                bool enable = line.endsWith("e");
                if (line.startsWith("all"))
                {
                    QMetaObject::invokeMethod(CLogHandler::instance(), "enableConsoleOutput", Q_ARG(bool, enable));
                }
                else
                {
                    CLogCategoryList categories;
                    if (line.startsWith("app"))      { categories = runtime->getIContextApplication(); }
                    else if (line.startsWith("aud")) { categories = runtime->getIContextAudio(); }
                    else if (line.startsWith("net")) { categories = runtime->getIContextNetwork(); }
                    else if (line.startsWith("own")) { categories = runtime->getIContextOwnAircraft(); }
                    else if (line.startsWith("set")) { categories = runtime->getIContextSettings(); }
                    else if (line.startsWith("sim")) { categories = runtime->getIContextSimulator(); }
                    if (! categories.isEmpty())
                    {
                        BlackMisc::singleShot(0, BlackMisc::CLogHandler::instance()->thread(), [ = ]()
                        {
                            BlackMisc::CLogHandler::instance()->handlerForPattern(CLogPattern::allOf(categories))->enableConsoleOutput(enable);
                        });
                    }
                }
            }
            else if (line.startsWith("h"))
            {
                qtout << "1) global logging:" << endl;
                qtout << "   levels: d, i, w, e  example: level d for debug level" << endl;
                qtout << "2) logging for contexts:" << endl;
                qtout << "   log + context + [e]nabled / [d]isabled" << endl;
                qtout << "   contexts: app / aud / net / own (aircraft) / set / sim / all" << endl;
                qtout << "   examples: logappd, lognete, logsimd, logalle" << endl;
                qtout << "3) all . commands can be used, e.g. .com1 127.35" << endl;
                qtout << endl;
            }
            else if (line.startsWith("."))
            {
                bool c = runtime->parseCommandLine(line);
                if (c) { qtout << "Handled command " << line; }
                else   { qtout << "Not handled " <<  line; }
                qtout << endl;
            }
        }
        QCoreApplication::quit();
    }
} // namespace
