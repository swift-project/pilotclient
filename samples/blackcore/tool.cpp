#include "tool.h"
#include "blackcore/context_all_interfaces.h"
#include "blackmisc/indexvariantmap.h"
#include "blackmisc/avallclasses.h"
#include "blackmisc/pqallquantities.h"
#include "blacksound/soundgenerator.h"

#include <QTextStream>
#include <QString>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>

using namespace BlackCore;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;

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
        QThread::sleep(3); // let the client connect
        qDebug() << "Running on server here" << Tool::getPid();

        //
        // Server loop
        //
        QTextStream qtin(stdin);
        QString line;
        while (line != "x" && runtime)
        {
            const BlackCore::IContextNetwork *networkContext = runtime->getIContextNetwork();
            const BlackCore::IContextAudio *audioContext = runtime->getIContextAudio();
            const BlackCore::IContextSettings *settingsContext = runtime->getIContextSettings();
            const BlackCore::IContextOwnAircraft *ownAircraftContext = runtime->getIContextOwnAircraft();

            BlackCore::IContextApplication *applicationContext = runtime->getIContextApplication();

            qDebug() << "-------------";
            qDebug() << "Connected with network: " << networkContext->isConnected();
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
                qDebug() << "Settings:" << settingsContext->getSettingsFileName();
                qDebug() << settingsContext->getSettingsAsJsonString();
            }
            else if (line.startsWith("1"))
            {
                qDebug() << "-------------";
                qDebug() << "ATC booked";
                qDebug() << networkContext->getAtcStationsBooked().toFormattedQString();
            }
            else if (line.startsWith("2"))
            {
                qDebug() << "-------------";
                qDebug() << "ATC online";
                qDebug() << networkContext->getAtcStationsOnline().toFormattedQString();
            }
            else if (line.startsWith("3"))
            {
                qDebug() << "-------------";
                qDebug() << "aircrafts in range";
                qDebug() << networkContext->getAircraftsInRange().toFormattedQString();
            }
            else if (line.startsWith("4"))
            {
                qDebug() << "-------------";
                qDebug() << "my aircraft";
                qDebug() << ownAircraftContext->getOwnAircraft();
            }
            else if (line.startsWith("5"))
            {
                qDebug() << "-------------";
                qDebug() << "voice rooms";
                qDebug() << audioContext->getComVoiceRooms();
            }
            else if (line.startsWith("6"))
            {
                qDebug() << "-------------";
                qDebug() << "vatlib audio devices";
                qDebug() << audioContext->getAudioDevices();
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
