#include "tool.h"
#include "blackcore/context_all_interfaces.h"
#include "blackmisc/indexvariantmap.h"
#include "blackmisc/avallclasses.h"
#include "blackmisc/pqallquantities.h"
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
        Q_ASSERT(runtime);
        QThread::sleep(3); // let the DBus server startup
        qDebug() << "Running on server here" << Tool::getPid() << "thread:" << QThread::currentThreadId();

        //
        // Server loop
        //
        QTextStream qtin(stdin);
        QString line;
        while (line != "x" && runtime)
        {
            BlackCore::IContextNetwork *networkContext = runtime->getIContextNetwork();
            BlackCore::IContextAudio *audioContext = runtime->getIContextAudio();
            BlackCore::IContextSettings *settingsContext = runtime->getIContextSettings();
            BlackCore::IContextOwnAircraft *ownAircraftContext = runtime->getIContextOwnAircraft();

            BlackCore::IContextApplication *applicationContext = runtime->getIContextApplication();

            qDebug() << "-------------";
            qDebug() << "Connected with network: " << networkContext->isConnected();
            qDebug() << "Thread id:" << QThread::currentThreadId();
            qDebug() << "-------------";
            qDebug() << "x .. to exit              0 .. settings";
            qDebug() << "1 .. ATC booked           2 .. ATC online";
            qDebug() << "3 .. Aircrafts in range   4 .. my aircraft     5 .. voice rooms";
            qDebug() << "6 .. vatlib audio devices 7 .. Qt audio devices";
            qDebug() << "-------------";
            qDebug() << "oe . redirect enabled     od . disable redirect";
            qDebug() << "-------------";
            qDebug() << "signal / slot logging:";
            qDebug() << "sig + context + [e]nabled/[d]isabled";
            qDebug() << "slo + context + [e]nabled/[d]isabled";
            qDebug() << "contexts: app / aud / net / own (aircraft) / set / sim / all";
            qDebug() << "examples: sigappd, slonete, slosimd, sloalle";
            qDebug() << "-------------";

            line = qtin.readLine().toLower().trimmed();
            if (line.startsWith("0"))
            {
                qDebug() << "-------------";
                QString ret1;
                QMetaObject::invokeMethod(settingsContext, "getSettingsFileName",
                                          Qt::BlockingQueuedConnection,
                                          Q_RETURN_ARG(QString, ret1));
                qDebug() << "Settings:" << ret1;


                QMetaObject::invokeMethod(settingsContext, "getSettingsAsJsonString",
                                          Qt::BlockingQueuedConnection,
                                          Q_RETURN_ARG(QString, ret1));
                qDebug() << "JSON" << ret1;
            }
            else if (line.startsWith("1"))
            {
                // remarks: use fully qualified name in Q_RETURN_ARG
                qDebug() << "-------------";
                qDebug() << "ATC booked";
                CAtcStationList stations;
                QMetaObject::invokeMethod(networkContext, "getAtcStationsBooked",
                                          Qt::BlockingQueuedConnection,
                                          Q_RETURN_ARG(BlackMisc::Aviation::CAtcStationList, stations));
                qDebug() << stations;
            }
            else if (line.startsWith("2"))
            {
                qDebug() << "-------------";
                qDebug() << "ATC online";
                CAtcStationList stations;
                QMetaObject::invokeMethod(networkContext, "getAtcStationsOnline",
                                          Qt::BlockingQueuedConnection,
                                          Q_RETURN_ARG(BlackMisc::Aviation::CAtcStationList, stations));
                qDebug() << stations;
            }
            else if (line.startsWith("3"))
            {
                qDebug() << "-------------";
                qDebug() << "aircrafts in range";
                CAircraftList aircrafts;
                QMetaObject::invokeMethod(networkContext, "getAircraftsInRange",
                                          Qt::BlockingQueuedConnection,
                                          Q_RETURN_ARG(BlackMisc::Aviation::CAircraftList, aircrafts));
                qDebug() << aircrafts;
            }
            else if (line.startsWith("4"))
            {
                qDebug() << "-------------";
                qDebug() << "my aircraft";
                CAircraft aircraft;
                QMetaObject::invokeMethod(ownAircraftContext, "getOwnAircraft",
                                          Qt::BlockingQueuedConnection,
                                          Q_RETURN_ARG(BlackMisc::Aviation::CAircraft, aircraft));
                qDebug() << aircraft;

            }
            else if (line.startsWith("5"))
            {
                qDebug() << "-------------";
                qDebug() << "voice rooms";
                CVoiceRoomList voiceRooms;
                QMetaObject::invokeMethod(audioContext, "getComVoiceRooms",
                                          Qt::BlockingQueuedConnection,
                                          Q_RETURN_ARG(BlackMisc::Audio::CVoiceRoomList, voiceRooms));
                qDebug() << voiceRooms;
            }
            else if (line.startsWith("6"))
            {
                qDebug() << "-------------";
                qDebug() << "vatlib audio devices";
                CAudioDeviceList devices;
                QMetaObject::invokeMethod(audioContext, "getAudioDevices",
                                          Qt::BlockingQueuedConnection,
                                          Q_RETURN_ARG(BlackMisc::Audio::CAudioDeviceList, devices));
                qDebug() << devices;
            }
            else if (line.startsWith("7"))
            {
                qDebug() << "-------------";
                qDebug() << "Qt audio devices";
                BlackSound::CSoundGenerator::printAllQtSoundDevices();
            }
            else if (line.startsWith("oe"))
            {
                applicationContext->setOutputRedirectionLevel(IContextApplication::RedirectAllOutput);
                applicationContext->setStreamingForRedirectedOutputLevel(IContextApplication::RedirectAllOutput);
            }
            else if (line.startsWith("od"))
            {
                applicationContext->setOutputRedirectionLevel(IContextApplication::RedirectNone);
                applicationContext->setStreamingForRedirectedOutputLevel(IContextApplication::RedirectNone);
            }
            else if (line.startsWith("sig"))
            {
                line.replace("signal", "");
                line.replace("sig", "");
                bool enable = line.endsWith("e");
                if (line.startsWith("app")) runtime->signalLogForApplication(enable);
                else if (line.startsWith("aud")) runtime->signalLogForAudio(enable);
                else if (line.startsWith("net")) runtime->signalLogForNetwork(enable);
                else if (line.startsWith("own")) runtime->signalLogForOwnAircraft(enable);
                else if (line.startsWith("set")) runtime->signalLogForSettings(enable);
                else if (line.startsWith("sim")) runtime->signalLogForSimulator(enable);
                else if (line.startsWith("all")) runtime->signalLog(enable);
            }
            else if (line.startsWith("slo"))
            {
                line.replace("slot", "");
                line.replace("slo", "");
                bool enable = line.endsWith("e");
                if (line.startsWith("app")) runtime->slotLogForApplication(enable);
                else if (line.startsWith("aud")) runtime->slotLogForAudio(enable);
                else if (line.startsWith("net")) runtime->slotLogForNetwork(enable);
                else if (line.startsWith("own")) runtime->slotLogForOwnAircraft(enable);
                else if (line.startsWith("set")) runtime->slotLogForSettings(enable);
                else if (line.startsWith("sim")) runtime->slotLogForSimulator(enable);
                else if (line.startsWith("all")) runtime->slotLog(enable);
            }
        }
        QCoreApplication::quit();
    }
} // namespace
