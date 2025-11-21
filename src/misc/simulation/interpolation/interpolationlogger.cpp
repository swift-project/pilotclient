// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/simulation/interpolation/interpolationlogger.h"

#include <QDateTime>
#include <QStringBuilder>

#include "config/buildconfig.h"
#include "misc/aviation/callsign.h"
#include "misc/directoryutils.h"
#include "misc/geo/kmlutils.h"
#include "misc/logmessage.h"
#include "misc/pq/angle.h"
#include "misc/pq/length.h"
#include "misc/pq/units.h"
#include "misc/stringutils.h"
#include "misc/swiftdirectories.h"
#include "misc/worker.h"

using namespace swift::config;
using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::geo;
using namespace swift::misc::math;
using namespace swift::misc::physical_quantities;
using namespace swift::misc::simulation;

namespace swift::misc::simulation
{
    CInterpolationLogger::CInterpolationLogger(QObject *parent) : QObject(parent)
    {
        this->setObjectName("CInterpolationLogger");
    }

    const QStringList &CInterpolationLogger::getLogCategories()
    {
        static const QStringList cats { CLogCategories::interpolator() };
        return cats;
    }

    CWorker *CInterpolationLogger::writeLogInBackground(bool clearLog)
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

        QPointer<CInterpolationLogger> myself(this);
        CWorker *worker = CWorker::fromTask(this, "WriteInterpolationLog", [situations, parts, myself, clearLog]() {
            const CStatusMessageList msg = CInterpolationLogger::writeLogFiles(situations, parts);
            CLogMessage::preformatted(msg);

            if (clearLog && myself) { myself->clearLog(); }
        });
        return worker;
    }

    QStringList CInterpolationLogger::getLatestLogFiles()
    {
        QStringList files({ "", "" });
        const QString logDir = CSwiftDirectories::logDirectory();
        QDir logs(logDir);
        if (!logs.exists()) { return files; }
        logs.setNameFilters(filePatterns());
        const QStringList interpolations =
            logs.entryList(QStringList({ filePatternInterpolationLog() }), QDir::NoFilter, QDir::Time);
        if (!interpolations.isEmpty()) { files[0] = CFileUtils::appendFilePaths(logDir, interpolations.first()); }
        const QStringList parts = logs.entryList(QStringList({ filePatternPartsLog() }), QDir::NoFilter, QDir::Time);
        if (!parts.isEmpty()) { files[1] = CFileUtils::appendFilePaths(logDir, parts.first()); }
        return files;
    }

    QString CInterpolationLogger::getLogDirectory() { return CSwiftDirectories::logDirectory(); }

    CStatusMessageList CInterpolationLogger::writeLogFiles(const QList<SituationLog> &interpolation,
                                                           const QList<PartsLog> &parts)
    {
        if (parts.isEmpty() && interpolation.isEmpty())
        {
            return CStatusMessage(static_cast<CInterpolationLogger *>(nullptr)).warning(u"No data for log");
        }
        static const QString html = QStringLiteral("Entries: %1\n\n%2");
        const QString htmlTemplate = CFileUtils::readFileToString(CSwiftDirectories::htmlTemplateFilePath());

        CStatusMessageList msgs;
        const QString ts = QDateTime::currentDateTimeUtc().toString("yyyyMMddhhmmss");

        const QString htmlInterpolation = CInterpolationLogger::getHtmlInterpolationLog(interpolation);
        if (!htmlInterpolation.isEmpty())
        {
            QString file = filePatternInterpolationLog();
            file.remove('*');
            const QString fn =
                CFileUtils::appendFilePaths(CSwiftDirectories::logDirectory(), QStringLiteral("%1 %2").arg(ts, file));
            const bool s = CFileUtils::writeStringToFile(
                htmlTemplate.arg(html.arg(interpolation.size()).arg(htmlInterpolation)), fn);
            msgs.push_back(CInterpolationLogger::logStatusFileWriting(s, fn));
        }

        const QString htmlParts = CInterpolationLogger::getHtmlPartsLog(parts);
        if (!htmlParts.isEmpty())
        {
            QString file = filePatternPartsLog();
            file.remove('*');
            const QString fn =
                CFileUtils::appendFilePaths(CSwiftDirectories::logDirectory(), QStringLiteral("%1 %2").arg(ts, file));
            const bool s = CFileUtils::writeStringToFile(htmlTemplate.arg(html.arg(parts.size()).arg(htmlParts)), fn);
            msgs.push_back(CInterpolationLogger::logStatusFileWriting(s, fn));
        }

        QString kml = CKmlUtils::wrapAsKmlDocument(CInterpolationLogger::getKmlChangedSituations(interpolation));
        if (!kml.isEmpty())
        {
            const QString fn = CFileUtils::appendFilePaths(CSwiftDirectories::logDirectory(),
                                                           QStringLiteral("%1_changedSituations.kml").arg(ts));
            const bool s = CFileUtils::writeStringToFile(kml, fn);
            msgs.push_back(CInterpolationLogger::logStatusFileWriting(s, fn));
        }

        kml = CKmlUtils::wrapAsKmlDocument(CInterpolationLogger::getKmlInterpolatedSituations(interpolation));
        if (!kml.isEmpty())
        {
            const QString fn = CFileUtils::appendFilePaths(CSwiftDirectories::logDirectory(),
                                                           QStringLiteral("%1_interpolatedSituations.kml").arg(ts));
            const bool s = CFileUtils::writeStringToFile(kml, fn);
            msgs.push_back(CInterpolationLogger::logStatusFileWriting(s, fn));
        }

        kml = CKmlUtils::wrapAsKmlDocument(CInterpolationLogger::getKmlElevations(interpolation));
        if (!kml.isEmpty())
        {
            const QString fn = CFileUtils::appendFilePaths(CSwiftDirectories::logDirectory(),
                                                           QStringLiteral("%1_elevations.kml").arg(ts));
            const bool s = CFileUtils::writeStringToFile(kml, fn);
            msgs.push_back(CInterpolationLogger::logStatusFileWriting(s, fn));
        }

        return msgs;
    }

    CStatusMessage CInterpolationLogger::logStatusFileWriting(bool success, const QString &fileName)
    {
        return success ?
                   CStatusMessage(static_cast<CInterpolationLogger *>(nullptr)).info(u"Written log file '%1'")
                       << fileName :
                   CStatusMessage(static_cast<CInterpolationLogger *>(nullptr)).error(u"Failed to write log file '%1'")
                       << fileName;
    }

    void CInterpolationLogger::logInterpolation(const SituationLog &log)
    {
        QWriteLocker l(&m_lockSituations);
        m_situationLogs.push_back(log);
        if (m_situationLogs.size() > m_maxSituations) { m_situationLogs.removeFirst(); }
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
        if (m_situationLogs.isEmpty()) { return {}; }
        return m_situationLogs.last();
    }

    SituationLog CInterpolationLogger::getLastSituationLog(const CCallsign &cs) const
    {
        const QList<SituationLog> copy(this->getSituationsLog(cs));
        if (copy.isEmpty()) { return {}; }
        return copy.last();
    }

    CAircraftSituation CInterpolationLogger::getLastSituation() const
    {
        QReadLocker l(&m_lockSituations);
        if (m_situationLogs.isEmpty()) { return {}; }
        return m_situationLogs.last().situationCurrent;
    }

    CAircraftSituation CInterpolationLogger::getLastSituation(const CCallsign &cs) const
    {
        const QList<SituationLog> copy(this->getSituationsLog(cs));
        if (copy.isEmpty()) { return {}; }
        return copy.last().situationCurrent;
    }

    CAircraftParts CInterpolationLogger::getLastParts() const
    {
        QReadLocker l(&m_lockParts);
        if (m_partsLogs.isEmpty()) { return {}; }
        return m_partsLogs.last().parts;
    }

    CAircraftParts CInterpolationLogger::getLastParts(const CCallsign &cs) const
    {
        const QList<PartsLog> copy(this->getPartsLog(cs));
        if (copy.isEmpty()) { return {}; }
        return copy.last().parts;
    }

    PartsLog CInterpolationLogger::getLastPartsLog() const
    {
        QReadLocker l(&m_lockParts);
        if (m_partsLogs.isEmpty()) { return {}; }
        return m_partsLogs.last();
    }

    PartsLog CInterpolationLogger::getLastPartsLog(const CCallsign &cs) const
    {
        const QList<PartsLog> copy(this->getPartsLog(cs));
        if (copy.isEmpty()) { return {}; }
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
            QStringLiteral(u"<thead><tr>"
                           u"<th title=\"changed situation\">cs.</th><th>Int</th>"
                           u"<th title=\"recalculated interpolant\">recalc</th>"
                           u"<th>CS</th><th>timestamp</th><th>since</th>"
                           u"<th>ts old</th><th>ts new</th><th>ts cur</th>"
                           u"<th>Interpolation ts.</th><th>Sample &Delta;t</th><th>fraction</th>"
                           u"<th>lat.old</th><th>lat.new</th><th>lat.cur</th>"
                           u"<th>lng.old</th><th>lng.new</th><th>lng.cur</th>"
                           u"<th>alt.old</th><th>alt.2nd</th><th>alt.new</th><th>alt.cur</th>"
                           u"<th>elv.old</th><th>elv.2nd</th><th>elv.new</th><th>elv.cur</th>"
                           u"<th>gnd.factor</th>"
                           u"<th>onGnd.old</th><th>onGnd.new</th><th>onGnd.cur</th>"
                           u"<th>CG</th>"
                           u"<th>parts</th><th title=\"changed parts\">cp.</th><th>parts details</th>"
                           u"</tr></thead>\n");

        static const CLengthUnit ft = CLengthUnit::ft();
        const SituationLog firstLog = logs.first();
        qint64 newPosTs = firstLog.newestInterpolationSituation().getMSecsSinceEpoch();
        CAircraftParts lastParts; // default, so shown if parts are different from default

        QString tableRows("<tbody>\n");
        for (const SituationLog &log : logs)
        {
            const CAircraftSituation situationOld = log.oldestInterpolationSituation();
            const CAircraftSituation situationNew = log.newestInterpolationSituation();
            const CAircraftSituation situation2nd = log.secondInterpolationSituation();
            const bool changedNewPosition = (newPosTs != situationNew.getMSecsSinceEpoch());
            const bool changedParts = (lastParts != log.parts);
            newPosTs = situationNew.getMSecsSinceEpoch();
            lastParts = log.parts;

            // fixme KB 2018/12 comment and tableRows can be removed if there are no further issues
            // concatenating in multiple steps, otherwise C4503 warnings
            tableRows +=
                u"<tr>" %
                (changedNewPosition ? QStringLiteral("<td class=\"changed\">*</td>") : QStringLiteral("<td></td>")) %
                u"<td>" % log.interpolator % u"</td>" % u"<td>" % boolToYesNo(log.interpolantRecalc) %
                u"</td>"
                u"<td>" %
                log.callsign.asString() % u"</td>" % u"<td>" % msSinceEpochToTime(log.tsCurrent) % u"</td>" % u"<td>" %
                QString::number(log.tsCurrent - firstLog.tsCurrent) % u"</td>" %

                u"<td class=\"old\">" % situationOld.getTimestampAndOffset(true) % u"</td>" % u"<td class=\"new\">" %
                situationNew.getTimestampAndOffset(true) % u"</td>" % u"<td class=\"cur\">" %
                log.situationCurrent.getTimestampAndOffset(true) % u"</td>" %

                u"<td>" % msSinceEpochToTime(log.tsInterpolated) % u"</td>" % u"<td>" %
                QString::number(log.deltaSampleTimesMs) % u"ms</td>" % u"<td>" % QString::number(log.simTimeFraction) %
                u"</td>" %

                // tableRows +=
                u"<td class=\"old\">" % situationOld.latitudeAsString() % u"</td>" % u"<td class=\"new\">" %
                situationNew.latitudeAsString() % u"</td>" % u"<td class=\"cur\">" %
                log.situationCurrent.latitudeAsString() % u"</td>" %

                u"<td class=\"old\">" % situationOld.longitudeAsString() % u"</td>" % u"<td class=\"new\">" %
                situationNew.longitudeAsString() % u"</td>" % u"<td class=\"cur\">" %
                log.situationCurrent.longitudeAsString() % u"</td>" %

                // tableRows +=
                u"<td class=\"old\">" % situationOld.getAltitude().valueRoundedWithUnit(ft, 1) % u"</td>" %
                u"<td class=\"old\">" % situation2nd.getAltitude().valueRoundedWithUnit(ft, 1) % u"</td>" %
                u"<td class=\"new\">" % situationNew.getAltitude().valueRoundedWithUnit(ft, 1) % u"</td>" %
                u"<td class=\"cur\">" % log.situationCurrent.getAltitude().valueRoundedWithUnit(ft, 1) % u"</td>" %

                u"<td class=\"old\">" % situationOld.getGroundElevation().valueRoundedWithUnit(ft, 1) % u" " %
                situationOld.getGroundElevationInfoAsString() % u"</td>" % u"<td class=\"old\">" %
                situation2nd.getGroundElevation().valueRoundedWithUnit(ft, 1) % u" " %
                situation2nd.getGroundElevationInfoAsString() % u"</td>" % u"<td class=\"new\">" %
                situationNew.getGroundElevation().valueRoundedWithUnit(ft, 1) % u" " %
                situationNew.getGroundElevationInfoAsString() % u"</td>" % u"<td class=\"cur\">" %
                log.situationCurrent.getGroundElevation().valueRoundedWithUnit(ft, 1) % u" " %
                log.situationCurrent.getGroundElevationInfoAsString() % u"</td>" %

                u"<td>" % QString::number(log.groundFactor) % u"</td>" % u"<td class=\"old\">" %
                situationOld.getOnGroundInfo().toQString() % u"</td>" % u"<td class=\"new\">" %
                situationNew.getOnGroundInfo().toQString() % u"</td>" % u"<td class=\"cur\">" %
                log.situationCurrent.getOnGroundInfo().toQString() % u"</td>" %

                // tableRows +=
                u"<td>" % log.cgAboveGround.valueRoundedWithUnit(ft, 0) % u"</td>" % u"<td>" %
                boolToYesNo(log.useParts) % u"</td>" % (changedParts ? u"<td class=\"changed\">*</td>" : u"<td></td>") %
                u"<td>" %
                (!log.useParts || log.parts.isNull() ? QString() : log.parts.toQString(true).toHtmlEscaped()) %
                u"</td>" % u"</tr>\n";
        }

        tableRows += QStringLiteral("</tbody>\n");
        return u"<table class=\"small\">\n" % tableHeader % tableRows % u"</table>\n";
    }

    QString CInterpolationLogger::getKmlChangedSituations(const QList<SituationLog> &logs)
    {
        if (logs.isEmpty()) { return {}; }

        QString kml;
        qint64 newPosTs = -1;
        int n = 1;
        const CKmlUtils::KMLSettings s(true, true);

        for (const SituationLog &log : logs)
        {
            const CAircraftSituation situationNew = log.newestInterpolationSituation();
            const bool changedNewPosition = (newPosTs != situationNew.getMSecsSinceEpoch());
            const bool recalc = log.interpolantRecalc;
            if (!changedNewPosition && !recalc) { continue; }
            newPosTs = situationNew.getMSecsSinceEpoch();
            kml += CKmlUtils::asPlacemark(QStringLiteral("%1: %2 new pos: %3 recalc: %4")
                                              .arg(n++)
                                              .arg(situationNew.getFormattedUtcTimestampHmsz(),
                                                   boolToYesNo(changedNewPosition), boolToYesNo(recalc)),
                                          situationNew.toQString(true), situationNew, s) %
                   u"\n";
        }

        return kml;
    }

    QString CInterpolationLogger::getKmlElevations(const QList<SituationLog> &logs)
    {
        if (logs.isEmpty()) { return {}; }

        QString kml;
        qint64 newPosTs = -1;
        int n = 1;
        const CKmlUtils::KMLSettings s(true, true);

        for (const SituationLog &log : logs)
        {
            const CAircraftSituation situationNew = log.newestInterpolationSituation();
            const bool changedNewPosition = (newPosTs != situationNew.getMSecsSinceEpoch());
            const bool recalc = log.interpolantRecalc;
            if (!changedNewPosition && !recalc) { continue; }
            if (!situationNew.hasGroundElevation()) { continue; }
            newPosTs = situationNew.getMSecsSinceEpoch();
            kml += CKmlUtils::asPlacemark(QStringLiteral("%1: %2 %3 info: %4 alt.cor: %5")
                                              .arg(n++)
                                              .arg(situationNew.getFormattedUtcTimestampHmsz(),
                                                   situationNew.getGroundElevationAndInfo(), log.elevationInfo,
                                                   log.altCorrection),
                                          situationNew.getGroundElevationPlane().toQString(true),
                                          situationNew.getGroundElevationPlane(), s) %
                   u"\n";
        }

        return kml;
    }

    QString CInterpolationLogger::getKmlInterpolatedSituations(const QList<SituationLog> &logs)
    {
        if (logs.isEmpty()) { return {}; }

        const CKmlUtils::KMLSettings s(true, false);
        QString coordinates;
        for (const SituationLog &log : logs)
        {
            const CAircraftSituation situation = log.situationCurrent;
            coordinates += CKmlUtils::asRawCoordinates(situation, s.withAltitude) % u"\n";
        }

        return u"<Placemark>\n"
               u"<name>Interpolation " %
               QString::number(logs.size()) % u"entries</name>\n" % CKmlUtils::asLineString(coordinates, s) %
               u"</Placemark>\n";
    }

    QString CInterpolationLogger::getHtmlPartsLog(const QList<PartsLog> &logs)
    {
        if (logs.isEmpty()) { return {}; }
        static const QString tableHeader = QStringLiteral(u"<thead><tr>"
                                                          u"<th>CS</th><th>timestamp</th>"
                                                          u"<th>c.</th>"
                                                          u"<th>parts</th>"
                                                          u"</tr></thead>\n");

        CAircraftParts lastParts; // default, so shown if parts are different from default
        QString tableRows("<tbody>\n");
        for (const PartsLog &log : logs)
        {
            const bool changedParts = (lastParts != log.parts);
            lastParts = log.parts;
            tableRows += u"<tr><td>" % log.callsign.asString() % u"</td>" % u"<td>" %
                         msSinceEpochToTime(log.tsCurrent) % u"</td>" %
                         (changedParts ? u"<td class=\"changed\">*</td>" : u"<td></td>") % u"<td>" %
                         (log.empty ? QStringLiteral("empty") : log.parts.toQString()) % u"</td></tr>";
        }
        tableRows += "</tbody>\n";
        return QStringLiteral("<table class=\"small\">\n") % tableHeader % tableRows % QStringLiteral("</table>\n");
    }

    void CInterpolationLogger::clearLog()
    {
        {
            QWriteLocker l(&m_lockSituations);
            m_situationLogs.clear();
        }
        {
            QWriteLocker l(&m_lockParts);
            m_partsLogs.clear();
        }
    }

    QString CInterpolationLogger::msSinceEpochToTime(qint64 ms)
    {
        static const QString dateFormat("hh:mm:ss.zzz");
        return QDateTime::fromMSecsSinceEpoch(ms).toString(dateFormat);
    }

    QString CInterpolationLogger::msSinceEpochToTimeAndTimestamp(qint64 ms)
    {
        return CInterpolationLogger::msSinceEpochToTime(ms) % u"/" % QString::number(ms);
    }

    QString CInterpolationLogger::msSinceEpochToTime(qint64 t1, qint64 t2, qint64 t3)
    {
        if (t3 < 0) { return QStringLiteral("%1 %2").arg(msSinceEpochToTime(t1), msSinceEpochToTime(t2)); }

        return QStringLiteral("%1 %2 %3").arg(msSinceEpochToTime(t1), msSinceEpochToTime(t2), msSinceEpochToTime(t3));
    }

    QString SituationLog::toQString(bool withSetup, bool withCurrentSituation, bool withElevation,
                                    bool withOtherPositions, bool withDeltaTimes, const QString &separator) const
    {
        const CAircraftSituation situationOldInterpolation = this->oldestInterpolationSituation();
        const CAircraftSituation situationNewInterpolation = this->newestInterpolationSituation();

        return (withSetup ? u"setup: " % usedSetup.toQString(true) % separator : QString()) %
               (withElevation ? u"Elev.info: " % elevationInfo % u" scenery os: " %
                                    sceneryOffset.valueRoundedWithUnit(1) % separator :
                                QString()) %
               u"change: " % change.toQString(true) % separator % u"Interpolation CS: " % callsign.asString() %
               separator % u"ts: " % CInterpolationLogger::msSinceEpochToTimeAndTimestamp(tsCurrent) % u" | type: " %
               this->interpolationType() % u" | gnd.fa.: " % QString::number(groundFactor) % u" | CG: " %
               cgAboveGround.valueRoundedWithUnit(CLengthUnit::m(), 1) % u" " %
               cgAboveGround.valueRoundedWithUnit(CLengthUnit::ft(), 1) % u" | alt.cor.: " % altCorrection %
               u" | #nw.sit.: " % QString::number(noNetworkSituations) % u" | #invalid: " %
               QString::number(noInvalidSituations) %
               (withDeltaTimes ?
                    separator % u"cur.time: " % CInterpolationLogger::msSinceEpochToTimeAndTimestamp(tsCurrent) %
                        u" | int.time: " % CInterpolationLogger::msSinceEpochToTimeAndTimestamp(tsInterpolated) %
                        u" | dt.cur.int.: " % QString::number(deltaCurrentToInterpolatedTime()) % u"ms" %
                        u" | sample dt: " % QString::number(deltaSampleTimesMs) % u"ms" % u" | fr.[0-1]: " %
                        QString::number(simTimeFraction) % u" | old int.pos.: " %
                        situationOldInterpolation.getTimestampAndOffset(true) % u" | new int.pos.: " %
                        situationNewInterpolation.getTimestampAndOffset(true) % u" | #int.pos.: " %
                        QString::number(interpolationSituations.size()) :
                    QString()) %
               (withCurrentSituation ? separator % u"cur.sit.(interpolated): " % situationCurrent.toQString(true) :
                                       QString()) %
               (withOtherPositions ? separator % u"old: " % situationOldInterpolation.toQString(true) % separator %
                                         u"new: " % situationNewInterpolation.toQString(true) :
                                     QString());
    }

    QString PartsLog::toQString(const QString &separator) const
    {
        return u"CS: " % callsign.asString() % separator % u"ts: " %
               CInterpolationLogger::msSinceEpochToTimeAndTimestamp(tsCurrent) % u" | #nw.parts: " %
               QString::number(noNetworkParts) % separator % u"parts: " % parts.toQString(true);
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

#ifdef __GNUC__
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wstrict-overflow"
#endif
    const CAircraftSituation &SituationLog::secondInterpolationSituation() const
    {
        if (interpolationSituations.size() < 2) { return aviation::CAircraftSituation::null(); }
        const aviation::CAircraftSituationList::size_type i =
            interpolationSituations.size() - 2; // 2nd latest, latest at end
        return interpolationSituations[i];
    }
#ifdef __GNUC__
#    pragma GCC diagnostic pop
#endif

} // namespace swift::misc::simulation
