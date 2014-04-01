#include "tool.h"
#include "blackcore/coreruntime.h"
#include "blackcore/context_network.h"
#include "blackcore/context_audio.h"
#include "blackmisc/valuemap.h"
#include "blackmisc/avallclasses.h"
#include "blackmisc/pqallquantities.h"
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
    void Tool::serverLoop(const BlackCore::CRuntime *runtime)
    {
        QThread::sleep(3); // let the client connect
        qDebug() << "Running on server here" << Tool::getPid();

        //
        // Server loop
        //
        QTextStream qtin(stdin);
        QString line;
        while (line != "x")
        {
            const BlackCore::IContextNetwork *networkContext = core->getIContextNetwork();
            const BlackCore::IContextAudio *audioContext = core->getIContextAudio();

            // display current status
            qDebug() << "-------------";
            qDebug() << "ATC booked";
            qDebug() << networkContext->getAtcStationsBooked().toQString();
            qDebug() << "-------------";
            qDebug() << "ATC online";
            qDebug() << networkContext->getAtcStationsOnline().toQString();

            // next round? Server
            qDebug() << "-------------";
            qDebug() << "Connected with network: " << networkContext->isConnected();

            qDebug() << "-------------";
            qDebug() << "Key  x to exit";
            qDebug() << "1 .. ATC booked";
            qDebug() << "2 .. ATC online";
            qDebug() << "3 .. Aircrafts in range";
            qDebug() << "4 .. my aircraft";
            qDebug() << "5 .. voice rooms";

            line = qtin.readLine();

            if (line.startsWith("1"))
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
                qDebug() << networkContext->getOwnAircraft();
            }
            else if (line.startsWith("5"))
            {
                qDebug() << "-------------";
                qDebug() << "voice rooms";
                qDebug() << audioContext->getComVoiceRooms();
            }
        }
        QCoreApplication::quit();
    }
} // namespace
