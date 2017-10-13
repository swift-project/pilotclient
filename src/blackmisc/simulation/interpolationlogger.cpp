/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "interpolationlogger.h"
#include "blackconfig/buildconfig.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/aviation/heading.h"
#include "blackmisc/pq/angle.h"
#include "blackmisc/pq/speed.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/worker.h"
#include "blackmisc/directoryutils.h"
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

        CWorker *CInterpolationLogger::writeLogInBackground()
        {
            QList<SituationLog> interpolation;
            QList<PartsLog> parts;
            {
                QReadLocker l(&m_lockLogs);
                interpolation = m_situationLogs;
                parts = m_partsLogs;
            }

            CWorker *worker = CWorker::fromTask(this, "WriteInterpolationLog", [interpolation, parts]()
            {
                const CStatusMessageList msg = CInterpolationLogger::writeLogFile(interpolation, parts);
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

        void CInterpolationLogger::logInterpolation(const CInterpolationLogger::SituationLog &log)
        {
            QWriteLocker l(&m_lockLogs);
            m_situationLogs.append(log);
        }

        void CInterpolationLogger::logParts(const CInterpolationLogger::PartsLog &parts)
        {
            QWriteLocker l(&m_lockLogs);
            m_partsLogs.append(parts);
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
            const QString tableHeader =
                QLatin1String("<thead><tr>") %
                QLatin1String("<th title=\"changed situation\">cs.</th><th>CS</th><th>VTOL</th><th>timestamp</th><th>since</th>") %
                QLatin1String("<th>ts old</th><th>ts new</th><th>ts cur</th>") %
                QLatin1String("<th>&Delta;t</th><th>&Delta;t fr.</th><th>fraction</th>") %
                QLatin1String("<th>lat.old</th><th>lat.new</th><th>lat.cur</th>") %
                QLatin1String("<th>lng.old</th><th>lng.new</th><th>lng.cur</th>") %
                QLatin1String("<th>alt.old</th><th>alt.new</th><th>alt.cur</th>") %
                QLatin1String("<th>elv.old</th><th>elv.new</th><th>elv.cur</th>") %
                QLatin1String("<th>gnd.factor</th>") %
                QLatin1String("<th>onGnd.old</th><th>onGnd.new</th><th>onGnd.cur</th>") %
                QLatin1String("<th>CG</th>") %
                QLatin1String("<th>parts</th><th title=\"changed parts\">cp.</th><th>parts details</th>") %
                QLatin1String("</tr></thead>\n");

            static const CLengthUnit ft = CLengthUnit::ft();
            const SituationLog firstLog = logs.first();
            qint64 newPosTs = firstLog.newSituation.getMSecsSinceEpoch();
            CAircraftParts lastParts; // default, so shown if parts are different from default

            QString tableRows("<tbody>\n");
            for (const SituationLog &log : logs)
            {
                const bool changedNewPosition = newPosTs != log.newSituation.getMSecsSinceEpoch();
                const bool changedParts = lastParts != log.parts;
                newPosTs = log.newSituation.getMSecsSinceEpoch();
                lastParts = log.parts;

                // concatenating in multiple steps, otherwise C4503 warnings
                tableRows +=
                    QLatin1String("<tr>") %
                    (changedNewPosition ? QLatin1String("<td class=\"changed\">*</td>") : QLatin1String("<td></td>")) %
                    QLatin1String("<td>") % log.callsign.asString() % QLatin1String("</td>") %
                    QLatin1String("<td>") % boolToYesNo(log.vtolAircraft) % QLatin1String("</td>") %
                    QLatin1String("<td>") % msSinceEpochToTime(log.timestamp) % QLatin1String("</td>") %
                    QLatin1String("<td>") % QString::number(log.timestamp - firstLog.timestamp) % QLatin1String("</td>") %

                    QLatin1String("<td class=\"old\">") % msSinceEpochToTime(log.oldSituation.getAdjustedMSecsSinceEpoch()) % QLatin1Char('-') % QString::number(log.oldSituation.getTimeOffsetMs()) % QLatin1String("</td>") %
                    QLatin1String("<td class=\"new\">") % msSinceEpochToTime(log.newSituation.getAdjustedMSecsSinceEpoch()) % QLatin1Char('-') % QString::number(log.newSituation.getTimeOffsetMs()) % QLatin1String("</td>") %
                    QLatin1String("<td class=\"cur\">") % msSinceEpochToTime(log.currentSituation.getAdjustedMSecsSinceEpoch()) % QLatin1Char('-') % QString::number(log.currentSituation.getTimeOffsetMs()) % QLatin1String("</td>") %

                    QLatin1String("<td>") % QString::number(log.deltaTimeMs) % QLatin1String("</td>") %
                    QLatin1String("<td>") % QString::number(log.deltaTimeFractionMs) % QLatin1String("</td>") %
                    QLatin1String("<td>") % QString::number(log.simulationTimeFraction) % QLatin1String("</td>");

                tableRows +=
                    QLatin1String("<td class=\"old\">") % log.oldSituation.latitudeAsString() % QLatin1String("</td>") %
                    QLatin1String("<td class=\"new\">") % log.newSituation.latitudeAsString() % QLatin1String("</td>") %
                    QLatin1String("<td class=\"cur\">") % log.currentSituation.latitudeAsString() % QLatin1String("</td>") %

                    QLatin1String("<td class=\"old\">") % log.oldSituation.longitudeAsString() % QLatin1String("</td>") %
                    QLatin1String("<td class=\"new\">") % log.newSituation.longitudeAsString() % QLatin1String("</td>") %
                    QLatin1String("<td class=\"cur\">") % log.currentSituation.longitudeAsString() % QLatin1String("</td>");

                tableRows +=
                    QLatin1String("<td class=\"old\">") % log.oldSituation.getAltitude().valueRoundedWithUnit(ft, 1) % QLatin1String("</td>") %
                    QLatin1String("<td class=\"new\">") % log.newSituation.getAltitude().valueRoundedWithUnit(ft, 1) % QLatin1String("</td>") %
                    QLatin1String("<td class=\"cur\">") % log.currentSituation.getAltitude().valueRoundedWithUnit(ft, 1) % QLatin1String("</td>") %

                    QLatin1String("<td class=\"old\">") % log.oldSituation.getGroundElevation().valueRoundedWithUnit(ft, 1) % QLatin1String("</td>") %
                    QLatin1String("<td class=\"new\">") % log.newSituation.getGroundElevation().valueRoundedWithUnit(ft, 1) % QLatin1String("</td>") %
                    QLatin1String("<td class=\"cur\">") % log.currentSituation.getGroundElevation().valueRoundedWithUnit(ft, 1) % QLatin1String("</td>") %

                    QLatin1String("<td>") % QString::number(log.groundFactor) % QLatin1String("</td>") %
                    QLatin1String("<td class=\"old\">") % log.oldSituation.getOnGroundInfo() % QLatin1String("</td>") %
                    QLatin1String("<td class=\"new\">") % log.newSituation.getOnGroundInfo() % QLatin1String("</td>") %
                    QLatin1String("<td class=\"cur\">") % log.currentSituation.getOnGroundInfo() % QLatin1String("</td>");

                tableRows +=
                    QLatin1String("<td>") % log.cgAboveGround.valueRoundedWithUnit(CLengthUnit::ft(), 0) % QLatin1String("</td>") %
                    QLatin1String("<td>") % boolToYesNo(log.useParts) % QLatin1String("</td>") %
                    (changedParts ? QLatin1String("<td class=\"changed\">*</td>") : QLatin1String("<td></td>")) %
                    QLatin1String("<td>") % (log.useParts ? log.parts.toQString(true) : QLatin1String("")) % QLatin1String("</td>") %
                    QLatin1String("</tr>\n");
            }

            tableRows += QLatin1String("</tbody>\n");
            return QLatin1String("<table class=\"small\">\n") % tableHeader % tableRows % QLatin1String("</table>\n");
        }

        QString CInterpolationLogger::getHtmlPartsLog(const QList<PartsLog> &logs)
        {
            if (logs.isEmpty()) { return {}; }
            const QString tableHeader =
                QLatin1String("<thead><tr>") %
                QLatin1String("<th>CS</th><th>timestamp</th>") %
                QLatin1String("<th>c.</th>") %
                QLatin1String("<th>parts</th>") %
                QLatin1String("</tr></thead>\n");

            CAircraftParts lastParts; // default, so shown if parts are different from default
            QString tableRows("<tbody>\n");
            for (const PartsLog &log : logs)
            {
                const bool changedParts = lastParts != log.parts;
                lastParts = log.parts;
                tableRows +=
                    QLatin1String("<tr>") %
                    QLatin1String("<td>") % log.callsign.asString() % QLatin1String("</td>") %
                    QLatin1String("<td>") % msSinceEpochToTime(log.timestamp) % QLatin1String("</td>") %
                    (changedParts ? QLatin1String("<td class=\"changed\">*</td>") : QLatin1String("<td></td>")) %
                    QLatin1String("<td>") % (log.empty ? QLatin1String("empty") : log.parts.toQString()) % QLatin1String("</td>");
            }
            tableRows += QLatin1String("</tbody>\n");
            return QLatin1String("<table class=\"small\">\n") % tableHeader % tableRows % QLatin1String("</table>\n");
        }

        void CInterpolationLogger::clearLog()
        {
            QWriteLocker l(&m_lockLogs);
            m_partsLogs.clear();
            m_situationLogs.clear();
        }

        QString CInterpolationLogger::msSinceEpochToTime(qint64 ms)
        {
            static const QString dateFormat("hh:mm:ss.zzz");
            return QDateTime::fromMSecsSinceEpoch(ms).toString(dateFormat);
        }

        QString CInterpolationLogger::msSinceEpochToTime(qint64 t1, qint64 t2, qint64 t3)
        {
            if (t3 < 0) return QString("%1 %2").arg(msSinceEpochToTime(t1), msSinceEpochToTime(t2));
            return QString("%1 %2 %3").arg(msSinceEpochToTime(t1), msSinceEpochToTime(t2), msSinceEpochToTime(t3));
        }
    } // namespace
} // namespace
