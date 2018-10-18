/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "interpolationlogger.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/aviation/heading.h"
#include "blackmisc/pq/angle.h"
#include "blackmisc/pq/speed.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/worker.h"
#include "blackmisc/directoryutils.h"
#include "blackconfig/buildconfig.h"
#include <QDateTime>
#include <QStringBuilder>

using namespace BlackConfig;
using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Math;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Simulation;

namespace BlackMisc
{
    namespace Simulation
    {
        CInterpolationLogger::CInterpolationLogger(QObject *parent) :
            QObject(parent)
        {
            this->setObjectName("CInterpolationLogger");
        }

        const CLogCategoryList &CInterpolationLogger::getLogCategories()
        {
            static const CLogCategoryList cats { CLogCategory::interpolator() };
            return cats;
        }

        CWorker *CInterpolationLogger::writeLogInBackground()
        {
            QList<SituationLog> situations;
            QList<PartsLog> parts;
            {
                QReadLocker l(&m_lockSituations);
                situations = m_situationLogs;
            }
            {
                QReadLocker l(&m_lockParts);
                parts = m_partsLogs;
            }

            CWorker *worker = CWorker::fromTask(this, "WriteInterpolationLog", [situations, parts]()
            {
                const CStatusMessageList msg = CInterpolationLogger::writeLogFile(situations, parts);
                CLogMessage::preformatted(msg);
            });
            return worker;
        }

        QStringList CInterpolationLogger::getLatestLogFiles()
        {
            QStringList files({ "", ""});
            const QString logDir = CDirectoryUtils::logDirectory();
            QDir logs(logDir);
            if (!logs.exists()) { return files; }
            logs.setNameFilters(filePatterns());
            const QStringList interpolations = logs.entryList(QStringList({filePatternInterpolationLog()}), QDir::NoFilter, QDir::Time);
            if (!interpolations.isEmpty())
            {
                files[0] = CFileUtils::appendFilePaths(logDir, interpolations.first());
            }
            const QStringList parts = logs.entryList(QStringList({filePatternPartsLog()}), QDir::NoFilter, QDir::Time);
            if (!parts.isEmpty())
            {
                files[1] = CFileUtils::appendFilePaths(logDir, parts.first());
            }
            return files;
        }

        QString CInterpolationLogger::getLogDirectory()
        {
            return CDirectoryUtils::logDirectory();
        }

        CStatusMessageList CInterpolationLogger::writeLogFile(const QList<SituationLog> &interpolation, const QList<PartsLog> &parts)
        {
            if (parts.isEmpty() && interpolation.isEmpty()) { return CStatusMessage(static_cast<CInterpolationLogger *>(nullptr)).warning("No data for log"); }
            static const QString html = QStringLiteral("Entries: %1\n\n%2");
            const QString htmlTemplate = CFileUtils::readFileToString(CDirectoryUtils::htmlTemplateFilePath());

            CStatusMessageList msgs;
            const QString ts = QDateTime::currentDateTimeUtc().toString("yyyyMMddhhmmss");
            const QString htmlInterpolation = CInterpolationLogger::getHtmlInterpolationLog(interpolation);
            if (!htmlInterpolation.isEmpty())
            {
                QString file = filePatternInterpolationLog();
                file.remove('*');
                const QString fn = CFileUtils::appendFilePaths(CDirectoryUtils::logDirectory(), QString("%1 %2").arg(ts, file));
                const bool s = CFileUtils::writeStringToFile(htmlTemplate.arg(html.arg(interpolation.size()).arg(htmlInterpolation)), fn);
                msgs.push_back(CInterpolationLogger::logStatusFileWriting(s, fn));
            }

            const QString htmlParts = CInterpolationLogger::getHtmlPartsLog(parts);
            if (!htmlParts.isEmpty())
            {
                QString file = filePatternPartsLog();
                file.remove('*');
                const QString fn = CFileUtils::appendFilePaths(CDirectoryUtils::logDirectory(), QString("%1 %2").arg(ts, file));
                const bool s = CFileUtils::writeStringToFile(htmlTemplate.arg(html.arg(parts.size()).arg(htmlParts)), fn);
                msgs.push_back(CInterpolationLogger::logStatusFileWriting(s, fn));
            }
            return msgs;
        }

        CStatusMessage CInterpolationLogger::logStatusFileWriting(bool success, const QString &fileName)
        {
            return success ?
                   CStatusMessage(static_cast<CInterpolationLogger *>(nullptr)).info("Written log file '%1'") << fileName :
                   CStatusMessage(static_cast<CInterpolationLogger *>(nullptr)).error("Failed to write log file '%1'") << fileName;
        }

        void CInterpolationLogger::logInterpolation(const SituationLog &log)
        {
            QWriteLocker l(&m_lockSituations);
            m_situationLogs.push_back(log);
            if (m_situationLogs.size() > m_maxSituations)
            {
                m_situationLogs.removeFirst();
            }
        }

        void CInterpolationLogger::logParts(const PartsLog &log)
        {
            QWriteLocker l(&m_lockParts);
            m_partsLogs.push_back(log);
        }

        void CInterpolationLogger::setMaxSituations(int max)
        {
            QReadLocker l(&m_lockSituations);
            m_maxSituations = max;
        }

        QList<SituationLog> CInterpolationLogger::getSituationsLog() const
        {
            QReadLocker l(&m_lockSituations);
            return m_situationLogs;
        }

        QList<PartsLog> CInterpolationLogger::getPartsLog() const
        {
            QReadLocker l(&m_lockParts);
            return m_partsLogs;
        }

        QList<SituationLog> CInterpolationLogger::getSituationsLog(const CCallsign &cs) const
        {
            const QList<SituationLog> copy(this->getSituationsLog());
            QList<SituationLog> logs;
            for (const SituationLog &log : copy)
            {
                if (log.callsign != cs) { continue; }
                logs.push_back(log);
            }
            return logs;
        }

        QList<PartsLog> CInterpolationLogger::getPartsLog(const CCallsign &cs) const
        {
            const QList<PartsLog> copy(this->getPartsLog());
            QList<PartsLog> logs;
            for (const PartsLog &log : copy)
            {
                if (log.callsign != cs) { continue; }
                logs.push_back(log);
            }
            return logs;
        }

        SituationLog CInterpolationLogger::getLastSituationLog() const
        {
            QReadLocker l(&m_lockSituations);
            if (m_situationLogs.isEmpty()) { return SituationLog(); }
            return m_situationLogs.last();
        }

        SituationLog CInterpolationLogger::getLastSituationLog(const CCallsign &cs) const
        {
            const QList<SituationLog> copy(this->getSituationsLog(cs));
            if (copy.isEmpty()) { return SituationLog(); }
            return copy.last();
        }

        CAircraftSituation CInterpolationLogger::getLastSituation() const
        {
            QReadLocker l(&m_lockSituations);
            if (m_situationLogs.isEmpty()) { return CAircraftSituation(); }
            return m_situationLogs.last().situationCurrent;
        }

        CAircraftSituation CInterpolationLogger::getLastSituation(const CCallsign &cs) const
        {
            const QList<SituationLog> copy(this->getSituationsLog(cs));
            if (copy.isEmpty()) { return CAircraftSituation(); }
            return copy.last().situationCurrent;
        }

        CAircraftParts CInterpolationLogger::getLastParts() const
        {
            QReadLocker l(&m_lockParts);
            if (m_partsLogs.isEmpty()) { return CAircraftParts(); }
            return m_partsLogs.last().parts;
        }

        CAircraftParts CInterpolationLogger::getLastParts(const CCallsign &cs) const
        {
            const QList<PartsLog> copy(this->getPartsLog(cs));
            if (copy.isEmpty()) { return CAircraftParts(); }
            return copy.last().parts;
        }

        PartsLog CInterpolationLogger::getLastPartsLog() const
        {
            QReadLocker l(&m_lockParts);
            if (m_partsLogs.isEmpty()) { return PartsLog(); }
            return m_partsLogs.last();
        }

        PartsLog CInterpolationLogger::getLastPartsLog(const CCallsign &cs) const
        {
            const QList<PartsLog> copy(this->getPartsLog(cs));
            if (copy.isEmpty()) { return PartsLog(); }
            return copy.last();
        }

        const QString &CInterpolationLogger::filePatternInterpolationLog()
        {
            static const QString p("*interpolation.html");
            return p;
        }

        const QString &CInterpolationLogger::filePatternPartsLog()
        {
            static const QString p("*parts.html");
            return p;
        }

        const QStringList &CInterpolationLogger::filePatterns()
        {
            static const QStringList l({ filePatternInterpolationLog(), filePatternPartsLog() });
            return l;
        }

        QString CInterpolationLogger::getHtmlInterpolationLog(const QList<SituationLog> &logs)
        {
            if (logs.isEmpty()) { return {}; }
            static const QString tableHeader =
                QStringLiteral("<thead><tr>") %
                QStringLiteral("<th title=\"changed situation\">cs.</th><th>Int</th>") %
                QStringLiteral("<th>CS</th><th>VTOL</th><th>timestamp</th><th>since</th>") %
                QStringLiteral("<th>ts old</th><th>ts new</th><th>ts cur</th>") %
                QStringLiteral("<th>Interpolation ts.</th><th>Sample &Delta;t</th><th>fraction</th>") %
                QStringLiteral("<th>lat.old</th><th>lat.new</th><th>lat.cur</th>") %
                QStringLiteral("<th>lng.old</th><th>lng.new</th><th>lng.cur</th>") %
                QStringLiteral("<th>alt.old</th><th>alt.new</th><th>alt.cur</th>") %
                QStringLiteral("<th>elv.old</th><th>elv.new</th><th>elv.cur</th>") %
                QStringLiteral("<th>gnd.factor</th>") %
                QStringLiteral("<th>onGnd.old</th><th>onGnd.new</th><th>onGnd.cur</th>") %
                QStringLiteral("<th>CG</th>") %
                QStringLiteral("<th>parts</th><th title=\"changed parts\">cp.</th><th>parts details</th>") %
                QStringLiteral("</tr></thead>\n");

            static const CLengthUnit ft = CLengthUnit::ft();
            const SituationLog firstLog = logs.first();
            qint64 newPosTs = firstLog.newestInterpolationSituation().getMSecsSinceEpoch();
            CAircraftParts lastParts; // default, so shown if parts are different from default

            QString tableRows("<tbody>\n");
            for (const SituationLog &log : logs)
            {
                const CAircraftSituation situationOld = log.oldestInterpolationSituation();
                const CAircraftSituation situationNew = log.newestInterpolationSituation();
                const bool changedNewPosition = newPosTs != situationNew.getMSecsSinceEpoch();
                const bool changedParts = (lastParts != log.parts);
                newPosTs = situationNew.getMSecsSinceEpoch();
                lastParts = log.parts;

                // concatenating in multiple steps, otherwise C4503 warnings
                tableRows +=
                    QStringLiteral("<tr>") %
                    (changedNewPosition ? QStringLiteral("<td class=\"changed\">*</td>") : QStringLiteral("<td></td>")) %
                    QStringLiteral("<td>") % log.interpolator % QStringLiteral("</td>") %
                    QStringLiteral("<td>") % log.callsign.asString() % QStringLiteral("</td>") %
                    QStringLiteral("<td>") % boolToYesNo(log.vtolAircraft) % QStringLiteral("</td>") %
                    QStringLiteral("<td>") % msSinceEpochToTime(log.tsCurrent) % QStringLiteral("</td>") %
                    QStringLiteral("<td>") % QString::number(log.tsCurrent - firstLog.tsCurrent) % QStringLiteral("</td>") %

                    QStringLiteral("<td class=\"old\">") % situationOld.getTimestampAndOffset(true) % QStringLiteral("</td>") %
                    QStringLiteral("<td class=\"new\">") % situationNew.getTimestampAndOffset(true) % QStringLiteral("</td>") %
                    QStringLiteral("<td class=\"cur\">") % log.situationCurrent.getTimestampAndOffset(true) % QStringLiteral("</td>") %

                    QStringLiteral("<td>") % msSinceEpochToTime(log.tsInterpolated) % QStringLiteral("</td>") %
                    QStringLiteral("<td>") % QString::number(log.deltaSampleTimesMs) % QStringLiteral("ms</td>") %
                    QStringLiteral("<td>") % QString::number(log.simTimeFraction) % QStringLiteral("</td>");

                tableRows +=
                    QStringLiteral("<td class=\"old\">") % situationOld.latitudeAsString() % QStringLiteral("</td>") %
                    QStringLiteral("<td class=\"new\">") % situationNew.latitudeAsString() % QStringLiteral("</td>") %
                    QStringLiteral("<td class=\"cur\">") % log.situationCurrent.latitudeAsString() % QStringLiteral("</td>") %

                    QStringLiteral("<td class=\"old\">") % situationOld.longitudeAsString() % QStringLiteral("</td>") %
                    QStringLiteral("<td class=\"new\">") % situationNew.longitudeAsString() % QStringLiteral("</td>") %
                    QStringLiteral("<td class=\"cur\">") % log.situationCurrent.longitudeAsString() % QStringLiteral("</td>");

                tableRows +=
                    QStringLiteral("<td class=\"old\">") % situationOld.getAltitude().valueRoundedWithUnit(ft, 1) % QStringLiteral("</td>") %
                    QStringLiteral("<td class=\"new\">") % situationNew.getAltitude().valueRoundedWithUnit(ft, 1) % QStringLiteral("</td>") %
                    QStringLiteral("<td class=\"cur\">") % log.situationCurrent.getAltitude().valueRoundedWithUnit(ft, 1) % QStringLiteral("</td>") %

                    QStringLiteral("<td class=\"old\">") % situationOld.getGroundElevation().valueRoundedWithUnit(ft, 1) % QStringLiteral("</td>") %
                    QStringLiteral("<td class=\"new\">") % situationNew.getGroundElevation().valueRoundedWithUnit(ft, 1) % QStringLiteral("</td>") %
                    QStringLiteral("<td class=\"cur\">") % log.situationCurrent.getGroundElevation().valueRoundedWithUnit(ft, 1) % QStringLiteral("</td>") %

                    QStringLiteral("<td>") % QString::number(log.groundFactor) % QStringLiteral("</td>") %
                    QStringLiteral("<td class=\"old\">") % situationOld.getOnGroundInfo() % QStringLiteral("</td>") %
                    QStringLiteral("<td class=\"new\">") % situationNew.getOnGroundInfo() % QStringLiteral("</td>") %
                    QStringLiteral("<td class=\"cur\">") % log.situationCurrent.getOnGroundInfo() % QStringLiteral("</td>");

                tableRows +=
                    QStringLiteral("<td>") % log.cgAboveGround.valueRoundedWithUnit(ft, 0) % QStringLiteral("</td>") %
                    QStringLiteral("<td>") % boolToYesNo(log.useParts) % QStringLiteral("</td>") %
                    (changedParts ? QStringLiteral("<td class=\"changed\">*</td>") : QStringLiteral("<td></td>")) %
                    QStringLiteral("<td>") % (log.useParts ? log.parts.toQString(true) : QStringLiteral("")) % QStringLiteral("</td>") %
                    QStringLiteral("</tr>\n");
            }

            tableRows += QStringLiteral("</tbody>\n");
            return QStringLiteral("<table class=\"small\">\n") % tableHeader % tableRows % QStringLiteral("</table>\n");
        }

        QString CInterpolationLogger::getHtmlPartsLog(const QList<PartsLog> &logs)
        {
            if (logs.isEmpty()) { return {}; }
            static const QString tableHeader =
                QStringLiteral("<thead><tr>") %
                QStringLiteral("<th>CS</th><th>timestamp</th>") %
                QStringLiteral("<th>c.</th>") %
                QStringLiteral("<th>parts</th>") %
                QStringLiteral("</tr></thead>\n");

            CAircraftParts lastParts; // default, so shown if parts are different from default
            QString tableRows("<tbody>\n");
            for (const PartsLog &log : logs)
            {
                const bool changedParts = (lastParts != log.parts);
                lastParts = log.parts;
                tableRows +=
                    QStringLiteral("<tr>") %
                    QStringLiteral("<td>") % log.callsign.asString() % QStringLiteral("</td>") %
                    QStringLiteral("<td>") % msSinceEpochToTime(log.tsCurrent) % QStringLiteral("</td>") %
                    (changedParts ? QStringLiteral("<td class=\"changed\">*</td>") : QStringLiteral("<td></td>")) %
                    QStringLiteral("<td>") % (log.empty ? QStringLiteral("empty") : log.parts.toQString()) % QStringLiteral("</td>");
            }
            tableRows += QStringLiteral("</tbody>\n");
            return QStringLiteral("<table class=\"small\">\n") % tableHeader % tableRows % QStringLiteral("</table>\n");
        }

        void CInterpolationLogger::clearLog()
        {
            { QWriteLocker l(&m_lockSituations); m_situationLogs.clear(); }
            { QWriteLocker l(&m_lockParts); m_partsLogs.clear(); }
        }

        QString CInterpolationLogger::msSinceEpochToTime(qint64 ms)
        {
            static const QString dateFormat("hh:mm:ss.zzz");
            return QDateTime::fromMSecsSinceEpoch(ms).toString(dateFormat);
        }

        QString CInterpolationLogger::msSinceEpochToTimeAndTimestamp(qint64 ms)
        {
            return CInterpolationLogger::msSinceEpochToTime(ms) % QStringLiteral("/") % QString::number(ms);
        }

        QString CInterpolationLogger::msSinceEpochToTime(qint64 t1, qint64 t2, qint64 t3)
        {
            static const QString s2("%1 %2");
            if (t3 < 0) { return s2.arg(msSinceEpochToTime(t1), msSinceEpochToTime(t2)); }

            static const QString s3("%1 %2 %3");
            return s3.arg(msSinceEpochToTime(t1), msSinceEpochToTime(t2), msSinceEpochToTime(t3));
        }

        QString SituationLog::toQString(
            bool withSetup,
            bool withCurrentSituation,
            bool withElevation, bool withOtherPositions, bool withDeltaTimes, const QString &separator) const
        {
            const CAircraftSituation situationOldInterpolation = this->oldestInterpolationSituation();
            const CAircraftSituation situationNewInterpolation = this->newestInterpolationSituation();

            return (
                       withSetup ?
                       QStringLiteral("setup: ") % usedSetup.toQString(true) % separator :
                       QStringLiteral("")
                   ) %
                   (
                       withElevation ?
                       QStringLiteral("Elev.info: ") % elevationInfo %
                       QStringLiteral(" scenery os: ") % sceneryOffset.valueRoundedWithUnit(1) % separator :
                       QStringLiteral("")
                   ) %
                   QStringLiteral("change: ") % change.toQString(true) %
                   separator %
                   QStringLiteral("Interpolation CS: ") % callsign.asString() % separator %
                   QStringLiteral("ts: ") % CInterpolationLogger::msSinceEpochToTimeAndTimestamp(tsCurrent) %
                   QStringLiteral(" | type: ") % this->interpolationType() %
                   QStringLiteral(" | gnd.fa.: ") % QString::number(groundFactor) %
                   QStringLiteral(" | CG: ") % cgAboveGround.valueRoundedWithUnit(CLengthUnit::m(), 1) %
                   QStringLiteral(" ") % cgAboveGround.valueRoundedWithUnit(CLengthUnit::ft(), 1) %
                   QStringLiteral(" | alt.cor.: ") % altCorrection %
                   QStringLiteral(" | #nw.sit.: ") % QString::number(noNetworkSituations) %
                   QStringLiteral(" | #invalid: ") % QString::number(noInvalidSituations) %
                   (
                       withDeltaTimes ?
                       separator %
                       QStringLiteral("cur.time: ") % CInterpolationLogger::msSinceEpochToTimeAndTimestamp(tsCurrent) %
                       QStringLiteral(" | int.time: ") % CInterpolationLogger::msSinceEpochToTimeAndTimestamp(tsInterpolated) %
                       QStringLiteral(" | dt.cur.int.: ") %  QString::number(deltaCurrentToInterpolatedTime()) % QStringLiteral("ms") %
                       QStringLiteral(" | sample dt: ") % QString::number(deltaSampleTimesMs) % QStringLiteral("ms") %
                       QStringLiteral(" | fr.[0-1]: ") % QString::number(simTimeFraction) %
                       QStringLiteral(" | old int.pos.: ") % situationOldInterpolation.getTimestampAndOffset(true) %
                       QStringLiteral(" | new int.pos.: ") % situationNewInterpolation.getTimestampAndOffset(true) %
                       QStringLiteral(" | #int.pos.: ") % QString::number(interpolationSituations.size()) :
                       QStringLiteral("")
                   ) %
                   (
                       withCurrentSituation ?
                       separator %
                       QStringLiteral("cur.sit.(interpolated): ") % situationCurrent.toQString(true) :
                       QStringLiteral("")
                   ) %
                   (
                       withOtherPositions ?
                       separator %
                       QStringLiteral("old: ") % situationOldInterpolation.toQString(true) %
                       separator %
                       QStringLiteral("new: ") % situationNewInterpolation.toQString(true) :
                       QStringLiteral("")
                   );
        }

        QString PartsLog::toQString(const QString &separator) const
        {
            return QStringLiteral("CS: ") % callsign.asString() % separator %
                   QStringLiteral("ts: ") % CInterpolationLogger::msSinceEpochToTimeAndTimestamp(tsCurrent) %
                   QStringLiteral(" | #nw.parts: ") % QString::number(noNetworkParts) %
                   separator %
                   QStringLiteral("parts: ") % parts.toQString(true);
        }

        const QString &SituationLog::interpolationType() const
        {
            static const QString s("spline");
            static const QString l("linear");
            static const QString u("unknown");
            if (interpolator == 's') { return s; }
            if (interpolator == 'l') { return l; }
            return u;
        }
    } // namespace
} // namespace
