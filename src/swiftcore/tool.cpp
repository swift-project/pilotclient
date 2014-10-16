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

        QTextStream qtout(stdout);
        qtout << "Running on server here " << Tool::getPid() << " thread: " << QThread::currentThreadId() << endl;

        //
        // Server loop
        //
        QTextStream qtin(stdin);
        QString line;
        while (line != "x" && runtime)
        {
            BlackCore::IContextApplication *applicationContext = runtime->getIContextApplication();
            BlackCore::IContextNetwork *networkContext = runtime->getIContextNetwork();
            BlackCore::IContextAudio *audioContext = runtime->getIContextAudio();
            BlackCore::IContextSettings *settingsContext = runtime->getIContextSettings();
            BlackCore::IContextOwnAircraft *ownAircraftContext = runtime->getIContextOwnAircraft();
            BlackCore::IContextSimulator *simulatorContext = runtime->getIContextSimulator();

            qtout << "-------------" << endl;
            qtout << "Connected with network: " << networkContext->isConnected() << endl;
            qtout << "Thread id: " << QThread::currentThreadId() << endl;
            qtout << "-------------" << endl;
            qtout << "x .. to exit              0 .. settings" << endl;
            qtout << "1 .. ATC booked           2 .. ATC online" << endl;
            qtout << "3 .. Aircrafts in range   4 .. my aircraft     5 .. voice rooms" << endl;
            qtout << "6 .. vatlib audio devices 7 .. Qt audio devices" << endl;
            qtout << "-------------" << endl;
            qtout << "logging:" << endl;
            qtout << "log + context + [e]nabled/[d]isabled" << endl;
            qtout << "contexts: app / aud / net / own (aircraft) / set / sim / all" << endl;
            qtout << "examples: logappd, lognete, logsimd, logalle" << endl;
            qtout << "-------------" << endl;

            line = qtin.readLine().toLower().trimmed();
            if (line.startsWith("0"))
            {
                qtout << "-------------" << endl;
                QString ret1;
                QMetaObject::invokeMethod(settingsContext, "getSettingsFileName",
                                          Qt::BlockingQueuedConnection,
                                          Q_RETURN_ARG(QString, ret1));
                qtout << "Settings: " << ret1 << endl;


                QMetaObject::invokeMethod(settingsContext, "getSettingsAsJsonString",
                                          Qt::BlockingQueuedConnection,
                                          Q_RETURN_ARG(QString, ret1));
                qtout << "JSON " << ret1 << endl;
            }
            else if (line.startsWith("1"))
            {
                // remarks: use fully qualified name in Q_RETURN_ARG
                qtout << "-------------" << endl;
                qtout << "ATC booked" << endl;
                CAtcStationList stations;
                QMetaObject::invokeMethod(networkContext, "getAtcStationsBooked",
                                          Qt::BlockingQueuedConnection,
                                          Q_RETURN_ARG(BlackMisc::Aviation::CAtcStationList, stations));
                qtout << stations << endl;
            }
            else if (line.startsWith("2"))
            {
                qtout << "-------------" << endl;
                qtout << "ATC online" << endl;
                CAtcStationList stations;
                QMetaObject::invokeMethod(networkContext, "getAtcStationsOnline",
                                          Qt::BlockingQueuedConnection,
                                          Q_RETURN_ARG(BlackMisc::Aviation::CAtcStationList, stations));
                qtout << stations << endl;
            }
            else if (line.startsWith("3"))
            {
                qtout << "-------------" << endl;
                qtout << "aircrafts in range" << endl;
                CAircraftList aircrafts;
                QMetaObject::invokeMethod(networkContext, "getAircraftsInRange",
                                          Qt::BlockingQueuedConnection,
                                          Q_RETURN_ARG(BlackMisc::Aviation::CAircraftList, aircrafts));
                qtout << aircrafts << endl;
            }
            else if (line.startsWith("4"))
            {
                qtout << "-------------" << endl;
                qtout << "my aircraft" << endl;
                CAircraft aircraft;
                QMetaObject::invokeMethod(ownAircraftContext, "getOwnAircraft",
                                          Qt::BlockingQueuedConnection,
                                          Q_RETURN_ARG(BlackMisc::Aviation::CAircraft, aircraft));
                qtout << aircraft << endl;

            }
            else if (line.startsWith("5"))
            {
                qtout << "-------------" << endl;
                qtout << "voice rooms" << endl;
                CVoiceRoomList voiceRooms;
                QMetaObject::invokeMethod(audioContext, "getComVoiceRooms",
                                          Qt::BlockingQueuedConnection,
                                          Q_RETURN_ARG(BlackMisc::Audio::CVoiceRoomList, voiceRooms));
                qtout << voiceRooms << endl;
            }
            else if (line.startsWith("6"))
            {
                qtout << "-------------" << endl;
                qtout << "vatlib audio devices" << endl;
                CAudioDeviceList devices;
                QMetaObject::invokeMethod(audioContext, "getAudioDevices",
                                          Qt::BlockingQueuedConnection,
                                          Q_RETURN_ARG(BlackMisc::Audio::CAudioDeviceList, devices));
                qtout << devices << endl;
            }
            else if (line.startsWith("7"))
            {
                qtout << "-------------" << endl;
                qtout << "Qt audio devices" << endl;
                BlackSound::CSoundGenerator::printAllQtSoundDevices();
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
                    QString category;
                    if (line.startsWith("app")) category = CLogCategoryList(applicationContext).back().toQString();
                    else if (line.startsWith("aud")) category = CLogCategoryList(audioContext).back().toQString();
                    else if (line.startsWith("net")) category = CLogCategoryList(networkContext).back().toQString();
                    else if (line.startsWith("own")) category = CLogCategoryList(ownAircraftContext).back().toQString();
                    else if (line.startsWith("set")) category = CLogCategoryList(settingsContext).back().toQString();
                    else if (line.startsWith("sim")) category = CLogCategoryList(simulatorContext).back().toQString();
                    if (! category.isEmpty())
                    {
                        BlackMisc::singleShot(0, BlackMisc::CLogHandler::instance()->thread(), [ = ]()
                        {
                            BlackMisc::CLogHandler::instance()->handlerForCategoryPrefix(category)->enableConsoleOutput(enable);
                        });
                    }
                }
            }
        }
        QCoreApplication::quit();
    }
} // namespace
