/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "interpolator.h"
#include "blackconfig/buildconfig.h"
#include "blackmisc/simulation/interpolationhints.h"
#include "blackmisc/simulation/interpolatorlinear.h"
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
        template <typename Derived>
        CInterpolator<Derived>::CInterpolator(const QString &objectName, QObject *parent) :
            QObject(parent)
        {
            this->setObjectName(objectName);
        }

        template <typename Derived>
        CAircraftSituation CInterpolator<Derived>::getInterpolatedSituation(const CCallsign &callsign, qint64 currentTimeMsSinceEpoc,
            const CInterpolationAndRenderingSetup &setup, const CInterpolationHints &hints, CInterpolationStatus &status) const
        {
            status.reset();
            InterpolationLog log;

            // any data at all?
            if (m_aircraftSituations.isEmpty()) { return {}; }
            CAircraftSituation currentSituation = m_aircraftSituations.front();

            // data, split situations by time
            if (currentTimeMsSinceEpoc < 0) { currentTimeMsSinceEpoc = QDateTime::currentMSecsSinceEpoch(); }

            // interpolant function from derived class
            auto interpolant = derived()->getInterpolant(currentTimeMsSinceEpoc, setup, hints, status, log);

            // succeeded so far?
            if (!status.didInterpolationSucceed()) { return currentSituation; }

            // use derived interpolant function
            currentSituation.setPosition(interpolant.interpolatePosition(setup, hints));
            currentSituation.setAltitude(interpolant.interpolateAltitude(setup, hints));

            // Update current position by hints' elevation
            // * for XP provided by hints.getElevationProvider at current position
            // * for FSX/P3D provided as hints.getElevation which is set to current position of remote aircraft in simulator
            // As XP uses lazy init we will call getGroundElevation only when needed, so default here via getElevationPlane
            CAltitude currentGroundElevation(hints.getElevationPlane().getAltitudeIfWithinRadius(currentSituation));

            // Interpolate between altitude and ground elevation, with proportions weighted according to interpolated onGround flag
            if (hints.hasAircraftParts())
            {
                const double groundFactor = hints.getAircraftParts().isOnGroundInterpolated();
                log.groundFactor = groundFactor;
                if (groundFactor > 0.0)
                {
                    currentGroundElevation = hints.getGroundElevation(currentSituation);
                    if (!currentGroundElevation.isNull())
                    {
                        Q_ASSERT_X(currentGroundElevation.getReferenceDatum() == CAltitude::MeanSeaLevel, Q_FUNC_INFO, "Need MSL value");
                        currentSituation.setAltitude(CAltitude(currentSituation.getAltitude() * (1.0 - groundFactor)
                                                               + currentGroundElevation * groundFactor,
                                                               currentSituation.getAltitude().getReferenceDatum()));
                    }
                }
                currentSituation.setGroundElevation(currentGroundElevation);
                CInterpolator::setGroundFlagFromInterpolator(hints, groundFactor, currentSituation);
            }
            else
            {
                // guess ground flag
                constexpr double NoGroundFactor = -1;
                currentSituation.setGroundElevation(currentGroundElevation);
                CInterpolator::setGroundFlagFromInterpolator(hints, NoGroundFactor, currentSituation);
            }

            if (setup.isForcingFullInterpolation() || hints.isVtolAircraft() || status.hasChangedPosition())
            {
                const auto pbh = interpolant.pbh();
                currentSituation.setHeading(pbh.getHeading());
                currentSituation.setPitch(pbh.getPitch());
                currentSituation.setBank(pbh.getBank());
                currentSituation.setGroundSpeed(pbh.getGroundSpeed());
                status.setChangedPosition(true);
            }
            status.setInterpolationSucceeded(true);
            if (hints.isLoggingInterpolation())
            {
                log.timestamp = currentTimeMsSinceEpoc;
                log.callsign = callsign;
                log.vtolAircraft = hints.isVtolAircraft();
                log.currentSituation = currentSituation;
                log.useParts = hints.hasAircraftParts();
                log.parts = hints.getAircraftParts();
                this->logInterpolation(log);
            }

            return currentSituation;
        }

        CHeading CInterpolatorPbh::getHeading() const
        {
            // HINT: VTOL aircraft can change pitch/bank without changing position, planes cannot
            // Interpolate heading: HDG = (HdgB - HdgA) * t + HdgA
            const CHeading headingBegin = oldSituation.getHeading();
            CHeading headingEnd = newSituation.getHeading();

            if ((headingEnd - headingBegin).value(CAngleUnit::deg()) < -180)
            {
                headingEnd += CHeading(360, CHeading::Magnetic, CAngleUnit::deg());
            }

            if ((headingEnd - headingBegin).value(CAngleUnit::deg()) > 180)
            {
                headingEnd -= CHeading(360, CHeading::Magnetic, CAngleUnit::deg());
            }

            return CHeading((headingEnd - headingBegin)
                            * simulationTimeFraction
                            + headingBegin,
                            headingBegin.getReferenceNorth());
        }

        CAngle CInterpolatorPbh::getPitch() const
        {
            // Interpolate Pitch: Pitch = (PitchB - PitchA) * t + PitchA
            const CAngle pitchBegin = oldSituation.getPitch();
            const CAngle pitchEnd = newSituation.getPitch();
            const CAngle pitch = (pitchEnd - pitchBegin) * simulationTimeFraction + pitchBegin;
            return pitch;
        }

        CAngle CInterpolatorPbh::getBank() const
        {
            // Interpolate bank: Bank = (BankB - BankA) * t + BankA
            const CAngle bankBegin = oldSituation.getBank();
            const CAngle bankEnd = newSituation.getBank();
            const CAngle bank = (bankEnd - bankBegin) * simulationTimeFraction + bankBegin;
            return bank;
        }

        CSpeed CInterpolatorPbh::getGroundSpeed() const
        {
            return (newSituation.getGroundSpeed() - oldSituation.getGroundSpeed())
                   * simulationTimeFraction
                   + oldSituation.getGroundSpeed();
        }

        template <typename Derived>
        CAircraftParts CInterpolator<Derived>::getInterpolatedParts(const CCallsign &callsign, qint64 currentTimeMsSinceEpoch,
            const CInterpolationAndRenderingSetup &setup, CPartsStatus &partsStatus, bool log) const
        {
            Q_UNUSED(setup);
            partsStatus.reset();
            if (currentTimeMsSinceEpoch < 0) { currentTimeMsSinceEpoch = QDateTime::currentMSecsSinceEpoch(); }

            // find the first parts not in the correct order, keep only the parts before that one
            const auto end = std::is_sorted_until(m_aircraftParts.begin(), m_aircraftParts.end(), [](auto && a, auto && b) { return b.getAdjustedMSecsSinceEpoch() < a.getAdjustedMSecsSinceEpoch(); });
            const auto validParts = makeRange(m_aircraftParts.begin(), end);

            // stop if we don't have any parts
            if (validParts.isEmpty()) { return {}; }
            partsStatus.setSupportsParts(true);

            CAircraftParts currentParts;
            do
            {
                // find the first parts earlier than the current time
                const auto pivot = std::partition_point(validParts.begin(), validParts.end(), [ = ](auto && p) { return p.getAdjustedMSecsSinceEpoch() > currentTimeMsSinceEpoch; });
                const auto partsNewer = makeRange(validParts.begin(), pivot).reverse();
                const auto partsOlder = makeRange(pivot, validParts.end());

                if (partsOlder.isEmpty()) { currentParts = *(partsNewer.end() - 1); break; }
                currentParts = partsOlder.front();
                if (currentParts.isOnGround()) { break; }

                // here we know aircraft is not on ground, and we check if it was recently on ground or if it will be on ground soon
                const auto latestTakeoff = std::adjacent_find(partsOlder.begin(), partsOlder.end(), [](auto &&, auto && p) { return p.isOnGround(); });
                const auto soonestLanding = std::find_if(partsNewer.begin(), partsNewer.end(), [](auto && p) { return p.isOnGround(); });

                // maxSecs is the maximum effective value of `secondsSinceTakeoff` and `secondsUntilLanding`. If `secondsSinceTakeoff > significantPast` then `takeoffFactor > 1`
                //         and if `secondsUntilLanding > predictableFuture` then `landingFactor > 1`, and `std::min(std::min(takeoffFactor, landingFactor), 1.0)` ensures `>1` is ignored.
                //         but if the offset < 5s then we must use a smaller value for the landing, hence `std::min(max, static_cast<double>(soonestLanding->getTimeOffsetMs()) / 1000.0)`.
                const double maxSecs = 5.0; // preferred length of time over which to blend the onground flag, when possible

                // our clairvoyance is limited by the time offset (all times here in seconds)
                const double significantPastSecs = maxSecs;
                const double predictableFutureSecs = soonestLanding == partsNewer.end() ? maxSecs : std::min(maxSecs, static_cast<double>(soonestLanding->getTimeOffsetMs()) / 1000.0);
                const double secondsSinceTakeoff = latestTakeoff == partsOlder.end() ? maxSecs : (currentTimeMsSinceEpoch - latestTakeoff->getAdjustedMSecsSinceEpoch()) / 1000.0;
                const double secondsUntilLanding = soonestLanding == partsNewer.end() ? maxSecs : (soonestLanding->getAdjustedMSecsSinceEpoch() - currentTimeMsSinceEpoch) / 1000.0;
                Q_ASSERT(secondsSinceTakeoff >= 0.0);
                Q_ASSERT(secondsUntilLanding >= 0.0);

                const double takeoffFactor = secondsSinceTakeoff / significantPastSecs;
                const double landingFactor = secondsUntilLanding / predictableFutureSecs;
                const double airborneFactor = std::min(std::min(takeoffFactor, landingFactor), 1.0);
                currentParts.setOnGroundInterpolated(1.0 - smootherStep(airborneFactor));
            }
            while (false);

            if (log)
            {
                PartsLog log;
                log.callsign = callsign;
                log.timestamp = currentTimeMsSinceEpoch;
                log.parts = currentParts;
                CInterpolator::logParts(log);
            }

            return currentParts;
        }

        template <typename Derived>
        void CInterpolator<Derived>::addAircraftSituation(const CAircraftSituation &situation)
        {
            m_aircraftSituations.push_frontMaxElements(situation, IRemoteAircraftProvider::MaxSituationsPerCallsign);
        }

        template <typename Derived>
        void CInterpolator<Derived>::addAircraftParts(const CAircraftParts &parts)
        {
            m_aircraftParts.push_front(parts);
            IRemoteAircraftProvider::removeOutdatedParts(m_aircraftParts);
        }

        template <typename Derived>
        CWorker *CInterpolator<Derived>::writeLogInBackground()
        {
            QList<InterpolationLog> interpolation;
            QList<PartsLog> parts;
            {
                QReadLocker l(&m_lockLogs);
                interpolation = m_interpolationLogs;
                parts = m_partsLogs;
            }

            CWorker *worker = CWorker::fromTask(this, "WriteInterpolationLog", [interpolation, parts]()
            {
                const CStatusMessageList msg = CInterpolator::writeLogFile(interpolation, parts);
                CLogMessage::preformatted(msg);
            });
            return worker;
        }

        QStringList IInterpolator::getLatestLogFiles()
        {
            QStringList files({ "", ""});
            const QString logDir = CDirectoryUtils::getLogDirectory();
            QDir logs(logDir);
            if (!logs.exists()) { return files; }
            logs.setNameFilters(QStringList() << "*interpolation.html" << "*parts.html");
            const QStringList interpolations = logs.entryList(QStringList({"*interpolation.html"}), QDir::NoFilter, QDir::Time);
            if (!interpolations.isEmpty())
            {
                files[0] = CFileUtils::appendFilePaths(logDir, interpolations.first());
            }
            const QStringList parts = logs.entryList(QStringList({"*parts.html"}), QDir::NoFilter, QDir::Time);
            if (!parts.isEmpty())
            {
                files[1] = CFileUtils::appendFilePaths(logDir, parts.first());
            }
            return files;
        }

        template <typename Derived>
        CStatusMessageList CInterpolator<Derived>::writeLogFile(const QList<InterpolationLog> &interpolation, const QList<PartsLog> &parts)
        {
            if (parts.isEmpty() && interpolation.isEmpty()) { return CStatusMessage(static_cast<CInterpolator *>(nullptr)).warning("No data for log"); }
            static const QString html = QLatin1String("Entries: %1\n\n%2");
            const QString htmlTemplate = CFileUtils::readFileToString(CBuildConfig::getHtmlTemplateFileName());

            CStatusMessageList msgs;
            const QString ts = QDateTime::currentDateTimeUtc().toString("yyyyMMddhhmmss");
            const QString htmlInterpolation = CInterpolator::getHtmlInterpolationLog(interpolation);
            if (!htmlInterpolation.isEmpty())
            {
                const QString fn = CFileUtils::appendFilePaths(CDirectoryUtils::getLogDirectory(), QString("%1 interpolation.html").arg(ts));
                const bool s = CFileUtils::writeStringToFile(htmlTemplate.arg(html.arg(interpolation.size()).arg(htmlInterpolation)), fn);
                msgs.push_back(CInterpolator::logStatusFileWriting(s, fn));
            }

            const QString htmlParts = CInterpolator::getHtmlPartsLog(parts);
            if (!htmlParts.isEmpty())
            {
                const QString fn = CFileUtils::appendFilePaths(CDirectoryUtils::getLogDirectory(), QString("%1 parts.html").arg(ts));
                const bool s = CFileUtils::writeStringToFile(htmlTemplate.arg(html.arg(parts.size()).arg(htmlParts)), fn);
                msgs.push_back(CInterpolator::logStatusFileWriting(s, fn));
            }
            return msgs;
        }

        template <typename Derived>
        CStatusMessage CInterpolator<Derived>::logStatusFileWriting(bool success, const QString &fileName)
        {
            if (success)
            {
                return CStatusMessage(static_cast<CInterpolator *>(nullptr)).info("Written log file '%1'") << fileName;
            }
            else
            {
                return CStatusMessage(static_cast<CInterpolator *>(nullptr)).error("Failed to write log file '%1'") << fileName;
            }
        }

        template <typename Derived>
        void CInterpolator<Derived>::logInterpolation(const typename CInterpolator<Derived>::InterpolationLog &log) const
        {
            QWriteLocker l(&m_lockLogs);
            m_interpolationLogs.append(log);
        }

        template <typename Derived>
        void CInterpolator<Derived>::logParts(const typename CInterpolator<Derived>::PartsLog &parts) const
        {
            QWriteLocker l(&m_lockLogs);
            m_partsLogs.append(parts);
        }

        template <typename Derived>
        QString CInterpolator<Derived>::getHtmlInterpolationLog(const QList<InterpolationLog> &logs)
        {
            if (logs.isEmpty()) { return {}; }
            const QString tableHeader =
                QLatin1String("<thead><tr>") %
                QLatin1String("<th>c.</th><th>CS</th><th>VTOL</th><th>timestamp</th><th>since</th>") %
                QLatin1String("<th>ts old</th><th>ts new</th><th>ts cur</th>") %
                QLatin1String("<th>&Delta;t</th><th>&Delta;t fr.</th><th>fraction</th>") %
                QLatin1String("<th>lat.old</th><th>lat.new</th><th>lat.cur</th>") %
                QLatin1String("<th>lng.old</th><th>lng.new</th><th>lng.cur</th>") %
                QLatin1String("<th>alt.old</th><th>alt.new</th><th>alt.cur</th>") %
                QLatin1String("<th>elv.old</th><th>elv.new</th><th>elv.cur</th>") %
                QLatin1String("<th>gnd.factor</th>") %
                QLatin1String("<th>onGnd.old</th><th>onGnd.new</th><th>onGnd.cur</th>") %
                QLatin1String("<th>parts</th><th>c.</th><th>parts details</th>") %
                QLatin1String("</tr></thead>\n");

            static const CLengthUnit ft = CLengthUnit::ft();
            const InterpolationLog firstLog = logs.first();
            qint64 newPosTs = firstLog.newSituation.getMSecsSinceEpoch();
            CAircraftParts lastParts; // default, so shown if parts are different from default

            QString tableRows("<tbody>\n");
            for (const InterpolationLog &log : logs)
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
                    QLatin1String("<td>") % boolToYesNo(log.useParts) % QLatin1String("</td>") %
                    (changedParts ? QLatin1String("<td>*</td>") : QLatin1String("<td></td>")) %
                    QLatin1String("<td>") % (log.useParts ? log.parts.toQString(true) : QLatin1String("")) % QLatin1String("</td>") %
                    QLatin1String("</tr>\n");
            }
            tableRows += QLatin1String("</tbody>\n");
            return QLatin1String("<table class=\"small\">\n") % tableHeader % tableRows % QLatin1String("</table>\n");
        }

        template <typename Derived>
        QString CInterpolator<Derived>::getHtmlPartsLog(const QList<PartsLog> &logs)
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
                    QLatin1String("<td>") % log.parts.toQString() % QLatin1String("</td>");
            }
            tableRows += QLatin1String("</tbody>\n");
            return QLatin1String("<table class=\"small\">\n") % tableHeader % tableRows % QLatin1String("</table>\n");
        }

        template <typename Derived>
        void CInterpolator<Derived>::clearLog()
        {
            QWriteLocker l(&m_lockLogs);
            this->m_partsLogs.clear();
            this->m_interpolationLogs.clear();
        }

        template <typename Derived>
        void CInterpolator<Derived>::setGroundElevationFromHint(const CInterpolationHints &hints, CAircraftSituation &situation, bool override)
        {
            if (!override && situation.hasGroundElevation()) { return; }
            const CAltitude elevation = hints.getGroundElevation(situation);
            if (elevation.isNull()) { return; }
            situation.setGroundElevation(elevation);
        }

        template <typename Derived>
        void CInterpolator<Derived>::setGroundFlagFromInterpolator(const CInterpolationHints &hints, double groundFactor, CAircraftSituation &situation)
        {
            // by interpolation
            if (groundFactor >= 1.0)
            {
                situation.setOnGround(CAircraftSituation::OnGround, CAircraftSituation::OnGroundByInterpolation);
                return;
            }
            if (groundFactor < 1.0 && groundFactor >= 0.0)
            {
                situation.setOnGround(CAircraftSituation::NotOnGround, CAircraftSituation::OnGroundByInterpolation);
                return;
            }

            // on elevation and CG
            if (!situation.getGroundElevation().isNull())
            {
                CLength offset(hints.isVtolAircraft() ? 0.0 : 1.0, CLengthUnit::m()); // offset from ground
                CAircraftSituation::OnGroundReliability reliability = CAircraftSituation::OnGroundByElevation;
                if (!hints.isVtolAircraft() && !hints.getCGAboveGround().isNull())
                {
                    offset = hints.getCGAboveGround();
                    reliability = CAircraftSituation::OnGroundByElevationAndCG;
                }

                Q_ASSERT_X(situation.getGroundElevation().getReferenceDatum() == CAltitude::MeanSeaLevel, Q_FUNC_INFO, "Need MSL elevation");
                const CAircraftSituation::IsOnGround og =
                    situation.getHeightAboveGround() <= offset ?
                    CAircraftSituation::OnGround : CAircraftSituation::NotOnGround;
                situation.setOnGround(og, reliability);
                return;
            }

            // for VTOL aircraft we give up
            if (hints.isVtolAircraft())
            {
                situation.setOnGround(CAircraftSituation::OnGroundSituationUnknown, CAircraftSituation::OnGroundReliabilityNoSet);
                return;
            }

            // we guess on speed, pitch and bank by excluding situations
            situation.setOnGround(CAircraftSituation::NotOnGround, CAircraftSituation::OnGroundByGuessing);
            if (qAbs(situation.getPitch().value(CAngleUnit::deg())) > 10) { return; }
            if (qAbs(situation.getBank().value(CAngleUnit::deg())) > 10)  { return; }
            if (situation.getGroundSpeed().value(CSpeedUnit::km_h()) > 50) { return; }

            // not sure, but this is a guess
            situation.setOnGround(CAircraftSituation::OnGround, CAircraftSituation::OnGroundByGuessing);
        }

        template <typename Derived>
        QString CInterpolator<Derived>::msSinceEpochToTime(qint64 ms)
        {
            static const QString dateFormat("hh:mm:ss.zzz");
            return QDateTime::fromMSecsSinceEpoch(ms).toString(dateFormat);
        }

        template <typename Derived>
        QString CInterpolator<Derived>::msSinceEpochToTime(qint64 t1, qint64 t2, qint64 t3)
        {
            if (t3 < 0) return QString("%1 %2").arg(msSinceEpochToTime(t1), msSinceEpochToTime(t2));
            return QString("%1 %2 %3").arg(msSinceEpochToTime(t1), msSinceEpochToTime(t2), msSinceEpochToTime(t3));
        }

        bool CInterpolationStatus::allTrue() const
        {
            return m_interpolationSucceeded && m_changedPosition;
        }

        void CInterpolationStatus::reset()
        {
            m_changedPosition = false;
            m_interpolationSucceeded = false;
        }

        bool CPartsStatus::allTrue() const
        {
            return m_supportsParts;
        }

        void CPartsStatus::reset()
        {
            m_supportsParts = false;
        }

        // see here for the reason of thess forward instantiations
        // https://isocpp.org/wiki/faq/templates#separate-template-fn-defn-from-decl
        //! \cond PRIVATE
        template class BLACKMISC_EXPORT_DEFINE_TEMPLATE CInterpolator<CInterpolatorLinear>;
        //! \endcond
    } // namespace
} // namespace
