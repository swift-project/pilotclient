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
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/worker.h"
#include "blackmisc/directoryutils.h"
#include <QDateTime>
#include <QStringBuilder>

using namespace BlackConfig;
using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackMisc
{
    namespace Simulation
    {
        IInterpolator::IInterpolator(IRemoteAircraftProvider *provider, const QString &objectName, QObject *parent) :
            QObject(parent),
            CRemoteAircraftAware(provider)
        {
            Q_ASSERT_X(provider, Q_FUNC_INFO, "missing provider");
            this->setObjectName(objectName);
        }

        IInterpolator::~IInterpolator()
        { }

        BlackMisc::Aviation::CAircraftSituation IInterpolator::getInterpolatedSituation(
            const CCallsign &callsign, qint64 currentTimeSinceEpoc,
            const CInterpolationHints &hints, InterpolationStatus &status) const
        {
            // has to be thread safe

            status.reset();
            Q_ASSERT_X(!callsign.isEmpty(), Q_FUNC_INFO, "empty callsign");

            auto currentSituation = this->getInterpolatedSituation(callsign, this->remoteAircraftSituations(callsign), currentTimeSinceEpoc, hints, status);
            currentSituation.setCallsign(callsign); // make sure callsign is correct
            return currentSituation;
        }

        CAircraftParts IInterpolator::getInterpolatedParts(const CCallsign &callsign, const CAircraftPartsList &parts, qint64 currentTimeMsSinceEpoch, IInterpolator::PartsStatus &partsStatus) const
        {
            partsStatus.reset();
            if (currentTimeMsSinceEpoch < 0) { currentTimeMsSinceEpoch = QDateTime::currentMSecsSinceEpoch(); }

            // find the first parts not in the correct order, keep only the parts before that one
            const auto end = std::is_sorted_until(parts.begin(), parts.end(), [](auto && a, auto && b) { return b.getAdjustedMSecsSinceEpoch() < a.getAdjustedMSecsSinceEpoch(); });
            const auto validParts = makeRange(parts.begin(), end);

            // stop if we don't have any parts
            if (validParts.isEmpty()) { return {}; }
            partsStatus.setSupportsParts(true);

            // find the first parts earlier than the current time
            const auto pivot = std::partition_point(validParts.begin(), validParts.end(), [ = ](auto && p) { return p.getAdjustedMSecsSinceEpoch() > currentTimeMsSinceEpoch; });
            const auto partsNewer = makeRange(validParts.begin(), pivot).reverse();
            const auto partsOlder = makeRange(pivot, validParts.end());

            if (partsOlder.isEmpty()) { return *(partsNewer.end() - 1); }
            CAircraftParts currentParts = partsOlder.front();
            if (currentParts.isOnGround()) { return currentParts; }

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


            const CInterpolationAndRenderingSetup setup = this->getInterpolatorSetup();
            if (setup.getLogCallsigns().contains(callsign))
            {
                PartsLog log;
                log.timestamp = currentTimeMsSinceEpoch;
                log.parts = currentParts;
                IInterpolator::logParts(log);
            }

            return currentParts;
        }

        CAircraftParts IInterpolator::getInterpolatedParts(const CCallsign &callsign, qint64 currentTimeMsSinceEpoch, IInterpolator::PartsStatus &partsStatus) const
        {
            Q_ASSERT_X(!callsign.isEmpty(), Q_FUNC_INFO, "empty callsign");
            partsStatus.reset();

            partsStatus.setSupportsParts(this->isRemoteAircraftSupportingParts(callsign));
            if (!partsStatus.isSupportingParts()) { return {}; }
            return this->getInterpolatedParts(callsign, this->remoteAircraftParts(callsign, -1), currentTimeMsSinceEpoch, partsStatus);
        }

        void IInterpolator::setInterpolatorSetup(const CInterpolationAndRenderingSetup &setup)
        {
            QWriteLocker l(&m_lockSetup);
            m_setup = setup;
        }

        CWorker *IInterpolator::writeLogInBackground()
        {
            // make sure logging is stopped
            {
                QWriteLocker l(&m_lockSetup);
                m_setup.clearInterpolatorLogCallsigns();
            }

            QList<InterpolationLog> interpolation;
            QList<PartsLog> parts;
            {
                QReadLocker l(&m_lockLogs);
                interpolation = m_interpolationLogs;
                parts = m_partsLogs;
            }

            CWorker *worker = CWorker::fromTask(this, "WriteInterpolationLog", [interpolation, parts]()
            {
                const CStatusMessage msg = IInterpolator::writeLogFile(interpolation, parts);
                CLogMessage::preformatted(msg);
            });
            return worker;
        }

        CStatusMessage IInterpolator::writeLogFile(const QList<InterpolationLog> &interpolation, const QList<PartsLog> &parts)
        {
            if (parts.isEmpty() && interpolation.isEmpty()) { return CStatusMessage(static_cast<IInterpolator *>(nullptr)).warning("No data for log"); }
            const QString htmlInterpolation = IInterpolator::getHtmlInterpolationLog(interpolation);
            const QString htmlParts = IInterpolator::getHtmlPartsLog(parts);
            const QString html = htmlParts % QLatin1Literal("\n\n") % htmlInterpolation;
            const QString htmlTemplate = CFileUtils::readFileToString(CBuildConfig::getHtmlTemplateFileName());

            const QString ts = QDateTime::currentDateTimeUtc().toString("yyyyMMddhhmmss");
            const QString fn = CFileUtils::appendFilePaths(CDirectoryUtils::getLogDirectory(), QString("%1 interpolation.html").arg(ts));
            const bool s = CFileUtils::writeStringToFile(htmlTemplate.arg(html), fn);
            if (s)
            {
                return CStatusMessage(static_cast<IInterpolator *>(nullptr)).info("Written log file '%1'") << fn;
            }
            else
            {
                return CStatusMessage(static_cast<IInterpolator *>(nullptr)).error("Failed to write log file '%1'") << fn;
            }
        }

        CInterpolationAndRenderingSetup IInterpolator::getInterpolatorSetup() const
        {
            QReadLocker l(&m_lockSetup);
            return m_setup;
        }

        void IInterpolator::logInterpolation(const IInterpolator::InterpolationLog &log) const
        {
            QWriteLocker l(&m_lockLogs);
            m_interpolationLogs.append(log);
        }

        void IInterpolator::logParts(const IInterpolator::PartsLog &parts) const
        {
            QWriteLocker l(&m_lockLogs);
            m_partsLogs.append(parts);
        }

        QString IInterpolator::getHtmlInterpolationLog(const QList<InterpolationLog> &logs)
        {
            if (logs.isEmpty()) { return {}; }
            QString tableRows;
            const QString tableHeader =
                QLatin1Literal("<tr>") %
                QLatin1Literal("<th>CS</th><th>VTOL</th><th>timestamp</th>") %
                QLatin1Literal("<th>ts old</th><th>ts new</th><th>ts cur</th>") %
                QLatin1Literal("<th>&Delta;t</th><th>&Delta;t fr.</th><th>fraction</th>") %
                QLatin1Literal("<th>lat.old</th><th>lat.new</th><th>lat.cur</th>") %
                QLatin1Literal("<th>lng.old</th><th>lng.new</th><th>lng.cur</th>") %
                QLatin1Literal("<th>alt.old</th><th>alt.new</th><th>alt.cur</th>") %
                QLatin1Literal("<th>elv.old</th><th>elv.new</th><th>elv.cur</th>") %
                QLatin1Literal("<th>gnd.factor</th>") %
                QLatin1Literal("<th>onGnd.old</th><th>onGnd.new</th><th>onGnd.cur</th>") %
                QLatin1Literal("</tr>\n");

            static const CLengthUnit ft = CLengthUnit::ft();
            for (const InterpolationLog &log : logs)
            {
                // concatenating in multiple steps, otherwise C4503 warnings
                tableRows +=
                    QLatin1Literal("<tr>") %
                    QLatin1Literal("<td>") % log.callsign.asString() % QLatin1Literal("</td>") %
                    QLatin1Literal("<td>") % boolToYesNo(log.vtolAircraft) % QLatin1Literal("</td>") %
                    QLatin1Literal("<td>") % msSinceEpochToTime(log.timestamp) % QLatin1Literal("</td>") %

                    QLatin1Literal("<td class=\"old\">") % msSinceEpochToTime(log.oldSituation.getAdjustedMSecsSinceEpoch()) % QLatin1Char('-') % QString::number(log.oldSituation.getTimeOffsetMs()) % QLatin1Literal("</td>") %
                    QLatin1Literal("<td class=\"new\">") % msSinceEpochToTime(log.newSituation.getAdjustedMSecsSinceEpoch()) % QLatin1Char('-') % QString::number(log.newSituation.getTimeOffsetMs()) % QLatin1Literal("</td>") %
                    QLatin1Literal("<td class=\"cur\">") % msSinceEpochToTime(log.currentSituation.getAdjustedMSecsSinceEpoch()) % QLatin1Char('-') % QString::number(log.currentSituation.getTimeOffsetMs()) % QLatin1Literal("</td>") %

                    QLatin1Literal("<td>") % QString::number(log.deltaTimeMs) % QLatin1Literal("</td>") %
                    QLatin1Literal("<td>") % QString::number(log.deltaTimeFractionMs) % QLatin1Literal("</td>") %
                    QLatin1Literal("<td>") % QString::number(log.simulationTimeFraction) % QLatin1Literal("</td>");

                tableRows +=
                    QLatin1Literal("<td class=\"old\">") % log.oldSituation.latitudeAsString() % QLatin1Literal("</td>") %
                    QLatin1Literal("<td class=\"new\">") % log.newSituation.latitudeAsString() % QLatin1Literal("</td>") %
                    QLatin1Literal("<td class=\"cur\">") % log.currentSituation.latitudeAsString() % QLatin1Literal("</td>") %

                    QLatin1Literal("<td class=\"old\">") % log.oldSituation.longitudeAsString() % QLatin1Literal("</td>") %
                    QLatin1Literal("<td class=\"new\">") % log.newSituation.longitudeAsString() % QLatin1Literal("</td>") %
                    QLatin1Literal("<td class=\"cur\">") % log.currentSituation.longitudeAsString() % QLatin1Literal("</td>");

                tableRows +=
                    QLatin1Literal("<td class=\"old\">") % log.oldSituation.getAltitude().valueRoundedWithUnit(ft, 1) % QLatin1Literal("</td>") %
                    QLatin1Literal("<td class=\"new\">") % log.newSituation.getAltitude().valueRoundedWithUnit(ft, 1) % QLatin1Literal("</td>") %
                    QLatin1Literal("<td class=\"cur\">") % log.currentSituation.getAltitude().valueRoundedWithUnit(ft, 1) % QLatin1Literal("</td>") %

                    QLatin1Literal("<td class=\"old\">") % log.oldSituation.getGroundElevation().valueRoundedWithUnit(ft, 1) % QLatin1Literal("</td>") %
                    QLatin1Literal("<td class=\"new\">") % log.newSituation.getGroundElevation().valueRoundedWithUnit(ft, 1) % QLatin1Literal("</td>") %
                    QLatin1Literal("<td class=\"cur\">") % log.currentSituation.getGroundElevation().valueRoundedWithUnit(ft, 1) % QLatin1Literal("</td>") %

                    QLatin1Literal("<td>") % QString::number(log.groundFactor) % QLatin1Literal("</td>") %
                    QLatin1Literal("<td class=\"old\">") % log.oldSituation.getOnGroundInfo() % QLatin1Literal("</td>") %
                    QLatin1Literal("<td class=\"new\">") % log.newSituation.getOnGroundInfo() % QLatin1Literal("</td>") %
                    QLatin1Literal("<td class=\"cur\">") % log.currentSituation.getOnGroundInfo() % QLatin1Literal("</td>") %
                    QLatin1Literal("</tr>\n");
            }

            return QLatin1Literal("<table class=\"small\">\n") % tableHeader % tableRows % QLatin1Literal("</table>\n");
        }

        QString IInterpolator::getHtmlPartsLog(const QList<PartsLog> &logs)
        {
            if (logs.isEmpty()) { return {}; }
            QString tableRows;
            const QString tableHeader =
                QLatin1Literal("<tr>") %
                QLatin1Literal("<th>CS</th><th>timestamp</th>") %
                QLatin1Literal("<th>parts</th>") %
                QLatin1Literal("</tr>\n");

            for (const PartsLog &log : logs)
            {
                // concatenating in multiple steps, otherwise C4503 warnings
                tableRows +=
                    QLatin1Literal("<tr>") %
                    QLatin1Literal("<td>") % log.callsign.asString() % QLatin1Literal("</td>") %
                    QLatin1Literal("<td>") % msSinceEpochToTime(log.timestamp) % QLatin1Literal("</td>") %
                    QLatin1Literal("<td>") % log.parts.toQString() % QLatin1Literal("</td>");
            }

            return QLatin1Literal("<table class=\"small\">\n") % tableHeader % tableRows % QLatin1Literal("</table>\n");
        }

        void IInterpolator::clearLog()
        {
            QWriteLocker l(&m_lockLogs);
            this->m_partsLogs.clear();
            this->m_interpolationLogs.clear();
        }

        void IInterpolator::setGroundElevationFromHint(const CInterpolationHints &hints, CAircraftSituation &situation)
        {
            if (situation.hasGroundElevation()) { return; }
            const CAltitude elevation = hints.getGroundElevation(situation);
            if (elevation.isNull()) { return; }
            situation.setGroundElevation(elevation);
        }

        void IInterpolator::setGroundFlagFromInterpolator(const CInterpolationHints &hints, double groundFactor, CAircraftSituation &situation)
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

        QString IInterpolator::msSinceEpochToTime(qint64 ms)
        {
            static const QString dateFormat("hh:mm:ss.zzz");
            return QDateTime::fromMSecsSinceEpoch(ms).toString(dateFormat);
        }

        QString IInterpolator::msSinceEpochToTime(qint64 t1, qint64 t2, qint64 t3)
        {
            if (t3 < 0) return QString("%1 %2").arg(msSinceEpochToTime(t1), msSinceEpochToTime(t2));
            return QString("%1 %2 %3").arg(msSinceEpochToTime(t1), msSinceEpochToTime(t2), msSinceEpochToTime(t3));
        }

        bool IInterpolator::InterpolationStatus::allTrue() const
        {
            return m_interpolationSucceeded && m_changedPosition;
        }

        void IInterpolator::InterpolationStatus::reset()
        {
            m_changedPosition = false;
            m_interpolationSucceeded = false;
        }

        bool IInterpolator::PartsStatus::allTrue() const
        {
            return m_supportsParts;
        }

        void IInterpolator::PartsStatus::reset()
        {
            m_supportsParts = false;
        }
    } // namespace
} // namespace
