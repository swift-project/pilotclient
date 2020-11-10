/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "simulationenvironmentprovider.h"
#include "blackmisc/aviation/aircraftsituationchange.h"

#include "blackmisc/logmessage.h"
#include "blackmisc/verify.h"
#include "blackconfig/buildconfig.h"

#include <QStringBuilder>

using namespace BlackConfig;
using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Simulation::Settings;

namespace BlackMisc
{
    namespace Simulation
    {
        bool ISimulationEnvironmentProvider::rememberGroundElevation(const CCallsign &requestedForCallsign, bool likelyOnGroundElevation, const ICoordinateGeodetic &elevationCoordinate, const CLength &epsilon)
        {
            if (!elevationCoordinate.hasMSLGeodeticHeight())
            {
                BLACK_AUDIT_X(false, Q_FUNC_INFO, "Elevation needs to be MSL NON NULL");
                return false;
            }

            const CLength minRange = ISimulationEnvironmentProvider::minRange(epsilon);
            const double elvFt = elevationCoordinate.geodeticHeight().value(CLengthUnit::ft());

            CCoordinateGeodetic alreadyInRange;
            CCoordinateGeodetic alreadyInRangeGnd;
            {
                QReadLocker l(&m_lockElvCoordinates);
                if (!m_enableElevation) { return false; }

                // check if we have already an elevation within range
                alreadyInRangeGnd = m_elvCoordinatesGnd.findFirstWithinRangeOrDefault(elevationCoordinate, minRange);
                alreadyInRange    = m_elvCoordinates.findFirstWithinRangeOrDefault(elevationCoordinate, minRange);
            }

            constexpr double maxDistFt = 30.0;

            // here we deal with gnd situation and do not expect a lot of variance
            if (!alreadyInRangeGnd.isNull())
            {
                // found
                const double distFt = qAbs(alreadyInRangeGnd.geodeticHeight().value(CLengthUnit::ft()) - elvFt);
                if (distFt > maxDistFt)
                {
                    // such a huge distance to existing value
                    CLogMessage(this).debug(u"Suspicious GND elevation distance '%1': %2ft at %3") << requestedForCallsign.asString() << distFt << elevationCoordinate.geodeticHeight().valueRoundedAsString(CLengthUnit::ft(), 1);
                    BLACK_AUDIT_X(!CBuildConfig::isLocalDeveloperDebugBuild(), Q_FUNC_INFO, "Suspicious gnd. elevation distance");
                }
                return false;
            }

            // here we deal with all kind of values, so it can be that
            // values vary in a much larger range
            if (!alreadyInRange.isNull())
            {
                // found
                const double distFt = qAbs(alreadyInRange.geodeticHeight().value(CLengthUnit::ft()) - elvFt);
                if (distFt > maxDistFt)
                {
                    // such a huge distance to existing value
                    CLogMessage(this).debug(u"Suspicious NON GND elevation distance for '%1': %2ft at %3") << requestedForCallsign.asString() << distFt << elevationCoordinate.geodeticHeight().valueRoundedAsString(CLengthUnit::ft(), 1);
                    // BLACK_AUDIT_X(!CBuildConfig::isLocalDeveloperDebugBuild(), Q_FUNC_INFO, "Suspicious elevation distance");
                }
                return false;
            }

            const qint64 now = QDateTime::currentMSecsSinceEpoch();
            {
                // we keep latest at front
                // * we assume we find them faster
                // * and need them more frequently (the recent ones)
                QWriteLocker l(&m_lockElvCoordinates);
                if (likelyOnGroundElevation)
                {
                    if (m_elvCoordinatesGnd.size() > m_maxElevationsGnd) { m_elvCoordinatesGnd.pop_back(); }
                    m_elvCoordinatesGnd.push_front(elevationCoordinate);
                }
                else
                {
                    if (m_elvCoordinates.size() > m_maxElevations) { m_elvCoordinates.pop_back(); }
                    m_elvCoordinates.push_front(elevationCoordinate);
                }

                // statistics
                if (m_pendingElevationRequests.contains(requestedForCallsign))
                {
                    const qint64 startedMs = m_pendingElevationRequests.value(requestedForCallsign);
                    const qint64 deltaMs   = now - startedMs;
                    m_pendingElevationRequests.remove(requestedForCallsign);
                    m_statsCurrentElevRequestTimeMs = deltaMs;
                    if (m_statsMaxElevRequestTimeMs < deltaMs) { m_statsMaxElevRequestTimeMs = deltaMs; }
                }
            }
            return true;
        }

        bool ISimulationEnvironmentProvider::rememberGroundElevation(const CCallsign &requestedForCallsign, bool likelyOnGroundElevation, const CElevationPlane &elevationPlane)
        {
            if (!elevationPlane.hasMSLGeodeticHeight())
            {
                BLACK_AUDIT_X(false, Q_FUNC_INFO, "Elevation plane needs to be MSL NON NULL");
                return false;
            }
            return this->rememberGroundElevation(requestedForCallsign, likelyOnGroundElevation, elevationPlane, elevationPlane.getRadius());
        }

        bool ISimulationEnvironmentProvider::insertCG(const CLength &cg, const CCallsign &cs)
        {
            if (cs.isEmpty()) { return false; }

            const bool remove = cg.isNull();
            {
                QWriteLocker l(&m_lockCG);
                if (remove) { m_cgsPerCallsign.remove(cs); }
                else        { m_cgsPerCallsign[cs] = cg;   }
            }
            return true;
        }

        bool ISimulationEnvironmentProvider::insertCG(const CLength &cg, const QString &modelString, const CCallsign &cs)
        {
            bool stored = false;
            QWriteLocker l(&m_lockCG);
            if (!m_enableCG)   { return false; }
            if (!cs.isEmpty())
            {
                if (m_cgsPerCallsignOverridden.contains(cs))
                {
                    // only keep as overridden value
                    m_cgsPerCallsignOverridden[cs] = cg;
                }
                else
                {
                    m_cgsPerCallsign[cs] = cg; stored = true;
                }
            }
            if (!modelString.isEmpty())
            {
                const QString ms = modelString.toUpper();
                if (m_cgsPerModelOverridden.contains(ms))
                {
                    // only keep as overridden value
                    m_cgsPerModelOverridden[ms] = cg;
                }
                else
                {
                    m_cgsPerModel[ms] = cg;
                    stored = true;
                }
            }
            return stored;
        }

        bool ISimulationEnvironmentProvider::insertCGOverridden(const CLength &cg, const CCallsign &cs)
        {
            if (cs.isEmpty()) { return false; }

            QWriteLocker l(&m_lockCG);
            if (!m_enableCG) { return false; }
            if (cg.isNull())
            {
                m_cgsPerCallsignOverridden.remove(cs);
                return false;
            }

            m_cgsPerCallsignOverridden[cs] = cg;
            return true;
        }

        bool ISimulationEnvironmentProvider::insertCGOverridden(const CLength &cg, const CCallsignSet &callsigns)
        {
            if (callsigns.isEmpty()) { return false; }

            QWriteLocker l(&m_lockCG);
            if (!m_enableCG) { return false; }
            for (const CCallsign &cs : callsigns)
            {
                if (cg.isNull())
                {
                    m_cgsPerCallsignOverridden.remove(cs);
                }
                else
                {
                    m_cgsPerCallsignOverridden[cs] = cg;
                }
            }
            return true;
        }

        bool ISimulationEnvironmentProvider::insertCGForModelString(const CLength &cg, const QString &modelString)
        {
            if (modelString.isEmpty()) { return false; }

            QWriteLocker l(&m_lockCG);
            if (!m_enableCG) { return false; }
            if (cg.isNull())
            {
                m_cgsPerModel.remove(modelString.toUpper());
                return false;
            }

            m_cgsPerModel[modelString.toUpper()] = cg;
            return true;
        }

        bool ISimulationEnvironmentProvider::insertCGForModelStringOverridden(const CLength &cg, const QString &modelString)
        {
            if (modelString.isEmpty()) { return false; }
            QWriteLocker l(&m_lockCG);
            if (cg.isNull())
            {
                m_cgsPerModelOverridden.remove(modelString.toUpper());
                return false;
            }
            m_cgsPerModelOverridden[modelString.toUpper()] = cg;
            return true;
        }

        CLengthPerCallsign ISimulationEnvironmentProvider::clearCGOverrides()
        {
            QWriteLocker l(&m_lockCG);
            m_cgsPerModelOverridden.clear();
            m_cgsPerCallsignOverridden.clear();
            return m_cgsPerCallsign; // all remaining CGs
        }

        CLength ISimulationEnvironmentProvider::overriddenCGorDefault(const CLength &defaultCG, const QString &modelString) const
        {
            if (modelString.isEmpty()) { return defaultCG; }
            const QString ms = modelString.toUpper();
            QReadLocker l(&m_lockCG);
            if (!m_cgsPerModelOverridden.contains(ms)) { return defaultCG; }
            return m_cgsPerModelOverridden[ms];
        }

        int ISimulationEnvironmentProvider::removeSimulatorCG(const CCallsign &cs)
        {
            if (cs.isEmpty()) { return 0; }

            QWriteLocker l(&m_lockCG);
            m_cgsPerCallsignOverridden.remove(cs);
            return m_cgsPerCallsign.remove(cs);
        }

        void ISimulationEnvironmentProvider::removePendingElevationRequest(const CCallsign &cs)
        {
            QWriteLocker l(&m_lockElvCoordinates);
            m_pendingElevationRequests.remove(cs);
        }

        CLength ISimulationEnvironmentProvider::minRange(const CLength &range)
        {
            return (range.isNull() || range < CElevationPlane::singlePointRadius()) ?
                   CElevationPlane::singlePointRadius() :
                   range;
        }

        CCoordinateGeodeticList ISimulationEnvironmentProvider::getAllElevationCoordinates() const
        {
            QReadLocker l(&m_lockElvCoordinates);
            CCoordinateGeodeticList cl(m_elvCoordinatesGnd);
            cl.push_back(m_elvCoordinates);
            return cl;
        }

        CCoordinateGeodeticList ISimulationEnvironmentProvider::getElevationCoordinatesOnGround() const
        {
            QReadLocker l(&m_lockElvCoordinates);
            return m_elvCoordinatesGnd;
        }

        CElevationPlane ISimulationEnvironmentProvider::averageElevationOfOnGroundAircraft(const CAircraftSituation &reference, const CLength &range, int minValues, int sufficientValues) const
        {
            const CCoordinateGeodeticList coordinates = this->getElevationCoordinatesOnGround();
            return coordinates.averageGeodeticHeight(reference, range, CAircraftSituation::allowedAltitudeDeviation(), minValues, sufficientValues);
        }

        CAltitude ISimulationEnvironmentProvider::highestElevation() const
        {
            const CCoordinateGeodeticList coordinates = this->getElevationCoordinatesOnGround();
            if (coordinates.isEmpty()) { return CAltitude::null(); }
            return coordinates.findMaxHeight();
        }

        CCoordinateGeodeticList ISimulationEnvironmentProvider::getAllElevationCoordinates(int &maxRemembered) const
        {
            QReadLocker l(&m_lockElvCoordinates);
            maxRemembered = m_maxElevations;
            CCoordinateGeodeticList cl(m_elvCoordinatesGnd);
            cl.push_back(m_elvCoordinates);
            return cl;
        }

        int ISimulationEnvironmentProvider::cleanUpElevations(const ICoordinateGeodetic &referenceCoordinate, int maxNumber)
        {
            int currentMax;
            CCoordinateGeodeticList coordinates(this->getAllElevationCoordinates(currentMax));
            if (maxNumber < 0) { maxNumber = currentMax; }
            const int size = coordinates.size();
            if (size <= maxNumber) { return 0; }
            coordinates.sortByEuclideanDistanceSquared(referenceCoordinate);
            coordinates.truncate(maxNumber);
            const int delta = size - coordinates.size();
            {
                QWriteLocker l(&m_lockElvCoordinates);
                m_elvCoordinates = coordinates;
            }
            return delta;
        }

        CElevationPlane ISimulationEnvironmentProvider::findClosestElevationWithinRange(const ICoordinateGeodetic &reference, const CLength &range) const
        {
            if (!this->isElevationProviderEnabled()) { return CElevationPlane::null(); }

            // for single point we use a slightly optimized version
            const bool singlePoint = (&range == &CElevationPlane::singlePointRadius() || range.isNull() || range <= CElevationPlane::singlePointRadius());
            const CCoordinateGeodeticList elevations = this->getAllElevationCoordinates();
            const CCoordinateGeodetic coordinate = singlePoint ?
                                                   elevations.findFirstWithinRangeOrDefault(reference, CElevationPlane::singlePointRadius()) :
                                                   elevations.findClosestWithinRange(reference, range);
            const bool found = !coordinate.isNull();

            {
                QWriteLocker l{&m_lockElvCoordinates };
                if (found)
                {
                    m_elvFound++;
                    return CElevationPlane(coordinate, reference); // plane with radius = distance to reference
                }
                else
                {
                    m_elvMissed++;
                    return CElevationPlane::null();
                }
            }
        }

        CElevationPlane ISimulationEnvironmentProvider::findClosestElevationWithinRangeOrRequest(const ICoordinateGeodetic &reference, const CLength &range, const CCallsign &callsign)
        {
            if (!this->isElevationProviderEnabled()) { return CElevationPlane::null(); }
            const CElevationPlane ep = ISimulationEnvironmentProvider::findClosestElevationWithinRange(reference, range);
            if (ep.isNull())
            {
                const qint64 now = QDateTime::currentMSecsSinceEpoch();
                this->requestElevation(reference, callsign);
                QWriteLocker l(&m_lockElvCoordinates);
                m_pendingElevationRequests[callsign] = now;
            }
            return ep;
        }

        bool ISimulationEnvironmentProvider::requestElevationBySituation(const CAircraftSituation &situation)
        {
            if (!this->isElevationProviderEnabled()) { return false; }
            return this->requestElevation(situation, situation.getCallsign());
        }

        QPair<int, int> ISimulationEnvironmentProvider::getElevationsFoundMissed() const
        {
            QReadLocker l(&m_lockElvCoordinates);
            return QPair<int, int>(m_elvFound, m_elvMissed);
        }

        QString ISimulationEnvironmentProvider::getElevationsFoundMissedInfo() const
        {
            static const QString info("%1/%2 %3% in %4 (all)/%5 (gnd)");
            const QPair<int, int> foundMissed = this->getElevationsFoundMissed();
            const int f = foundMissed.first;
            const int m = foundMissed.second;
            const double hitRatioPercent = 100.0 * static_cast<double>(f) / static_cast<double>(f + m);

            int elvGnd;
            int elv;
            {
                QReadLocker l(&m_lockElvCoordinates);
                elvGnd = m_elvCoordinatesGnd.sizeInt();
                elv    = m_elvCoordinates.sizeInt();
            }
            return info.arg(f).arg(m).arg(QString::number(hitRatioPercent, 'f', 1)).arg(elv).arg(elvGnd);
        }

        QPair<qint64, qint64> ISimulationEnvironmentProvider::getElevationRequestTimes() const
        {
            QReadLocker l(&m_lockElvCoordinates);
            return QPair<qint64, qint64>(m_statsCurrentElevRequestTimeMs, m_statsMaxElevRequestTimeMs);
        }

        QString ISimulationEnvironmentProvider::getElevationRequestTimesInfo() const
        {
            if (!this->isElevationProviderEnabled()) { return QStringLiteral("Elevation provider disabled"); }

            static const QString info("%1ms/%2ms");
            QPair<qint64, qint64> times = this->getElevationRequestTimes();
            if (times.first < 0 || times.second < 0) { return QStringLiteral("no req. times"); }
            return info.arg(times.first).arg(times.second);
        }

        CSimulatorPluginInfo ISimulationEnvironmentProvider::getSimulatorPluginInfo() const
        {
            QReadLocker l(&m_lockSimInfo);
            return m_simulatorPluginInfo;
        }

        CSimulatorInfo ISimulationEnvironmentProvider::getSimulatorInfo() const
        {
            return this->getSimulatorPluginInfo().getSimulatorInfo();
        }

        QString ISimulationEnvironmentProvider::getSimulatorNameAndVersion() const
        {
            QString n;
            QString v;
            {
                QReadLocker l(&m_lockSimInfo);
                n = m_simulatorName;
                v = m_simulatorVersion;
            }

            if (!n.isEmpty() && !v.isEmpty()) { return n % u' ' % v; }
            if (!n.isEmpty()) { return n; }

            const CSimulatorInfo simInfo = this->getSimulatorInfo();
            if (!simInfo.isUnspecified()) { return simInfo.toQString(true); }
            return QStringLiteral("not available");
        }

        CAircraftModel ISimulationEnvironmentProvider::getDefaultModel() const
        {
            QReadLocker l(&m_lockModel);
            return m_defaultModel;
        }

        CLengthPerCallsign ISimulationEnvironmentProvider::getSimulatorCGsPerCallsign() const
        {
            QReadLocker l(&m_lockCG);
            return m_cgsPerCallsign;
        }

        QHash<QString, CLength> ISimulationEnvironmentProvider::getSimulatorCGsPerModelString() const
        {
            QReadLocker l(&m_lockCG);
            return m_cgsPerModel;
        }

        CLength ISimulationEnvironmentProvider::getSimulatorCG(const Aviation::CCallsign &callsign) const
        {
            if (callsign.isEmpty()) { return CLength::null(); }
            QReadLocker l(&m_lockCG);
            if (!m_enableCG) { return CLength::null(); }
            if (m_cgsPerCallsignOverridden.contains(callsign))
            {
                return m_cgsPerCallsignOverridden[callsign];
            }
            if (!m_cgsPerCallsign.contains(callsign)) { return CLength::null(); }
            return m_cgsPerCallsign.value(callsign);
        }

        CLength ISimulationEnvironmentProvider::getSimulatorOrDbCG(const CCallsign &callsign, const CLength &dbCG) const
        {
            if (callsign.isEmpty()) { return CLength::null(); }
            const CSimulatorSettings::CGSource source = m_settings.getCGSource();
            if (source == CSimulatorSettings::CGFromDBOnly || (source == CSimulatorSettings::CGFromDBFirst && !dbCG.isNull())) { return dbCG; }
            const CLength simCG = this->getSimulatorCG(callsign);
            if (source == CSimulatorSettings::CGFromSimulatorOnly || (source == CSimulatorSettings::CGFromSimulatorFirst && !simCG.isNull())) { return simCG; }
            return dbCG;
        }

        CLength ISimulationEnvironmentProvider::getSimulatorCGPerModelString(const QString &modelString) const
        {
            if (modelString.isEmpty()) { return CLength::null(); }
            const QString ms = modelString.toUpper();
            QReadLocker l(&m_lockCG);
            if (!m_enableCG) { return CLength::null(); }
            if (m_cgsPerModelOverridden.contains(ms))
            {
                return m_cgsPerModelOverridden.value(ms);
            }
            if (!m_cgsPerModel.contains(ms)) { return CLength::null(); }
            return m_cgsPerModel.value(ms);
        }

        CLength ISimulationEnvironmentProvider::getSimulatorOrDbCGPerModelString(const QString &modelString, const CLength &dbCG) const
        {
            if (modelString.isEmpty()) { return CLength::null(); }
            const CSimulatorSettings::CGSource source = m_settings.getCGSource();
            const QString ms = modelString.toUpper();
            {
                QReadLocker l(&m_lockCG);
                if (m_cgsPerModelOverridden.contains(ms)) { return m_cgsPerModelOverridden.value(ms); }
            }
            if (source == CSimulatorSettings::CGFromDBOnly || (!dbCG.isNull() && source == CSimulatorSettings::CGFromDBFirst)) { return dbCG; }
            const CLength simCG = this->getSimulatorCGPerModelString(modelString);
            if (source == CSimulatorSettings::CGFromSimulatorOnly || (source == CSimulatorSettings::CGFromSimulatorFirst && simCG.isNull())) { return simCG; }
            return dbCG;
        }

        bool ISimulationEnvironmentProvider::hasSimulatorCG(const Aviation::CCallsign &callsign) const
        {
            if (callsign.isEmpty()) { return false; }
            QReadLocker l(&m_lockCG);
            return m_enableCG && (m_cgsPerCallsign.contains(callsign) || m_cgsPerCallsignOverridden.contains(callsign));
        }

        bool ISimulationEnvironmentProvider::hasSameSimulatorCG(const CLength &cg, const CCallsign &callsign) const
        {
            if (callsign.isEmpty()) { return false; }
            QReadLocker l(&m_lockCG);
            if (m_cgsPerCallsignOverridden.contains(callsign))
            {
                return m_cgsPerCallsignOverridden[callsign] == cg;
            }

            // normal values
            if (!m_cgsPerCallsign.contains(callsign)) { return false; }
            return m_cgsPerCallsign[callsign] == cg;
        }

        int ISimulationEnvironmentProvider::setMaxElevationsRemembered(int max)
        {
            QWriteLocker l(&m_lockElvCoordinates);
            m_maxElevations = qMax(max, 50);
            return m_maxElevations;
        }

        int ISimulationEnvironmentProvider::getMaxElevationsRemembered() const
        {
            QReadLocker l(&m_lockElvCoordinates);
            return m_maxElevations;
        }

        void ISimulationEnvironmentProvider::resetSimulationEnvironmentStatistics()
        {
            QWriteLocker l(&m_lockElvCoordinates);
            m_statsCurrentElevRequestTimeMs = -1;
            m_statsMaxElevRequestTimeMs     = -1;
            m_elvFound = m_elvMissed        =  0;
        }

        int ISimulationEnvironmentProvider::removeElevationValues(const CAircraftSituation &reference, const CLength &removeRange)
        {
            QWriteLocker l(&m_lockElvCoordinates);
            const int r = m_elvCoordinatesGnd.removeInsideRange(reference, removeRange);
            return r;
        }

        bool ISimulationEnvironmentProvider::cleanElevationValues(const CAircraftSituation &reference, const CLength &keptRange, bool forced)
        {
            if (reference.isNull() || keptRange.isNull()) { return false; }
            const CLength r = minRange(keptRange);

            CCoordinateGeodeticList cleanedKeptElvs;
            bool maxReached = false;
            bool cleaned    = false;

            {
                QReadLocker l(&m_lockElvCoordinates);
                cleanedKeptElvs = m_elvCoordinates;
                maxReached = m_elvCoordinates.size() >= m_maxElevations;
            }
            if (!cleanedKeptElvs.isEmpty() && (forced || maxReached))
            {
                const int removed = cleanedKeptElvs.removeOutsideRange(reference, r);
                if (removed > 0)
                {
                    cleaned = true;
                    QWriteLocker l(&m_lockElvCoordinates);
                    m_elvCoordinates = cleanedKeptElvs;
                }
            }

            cleanedKeptElvs.clear();
            {
                QReadLocker l(&m_lockElvCoordinates);
                cleanedKeptElvs = m_elvCoordinatesGnd;
                maxReached = m_elvCoordinatesGnd.size() >= m_maxElevationsGnd;
            }
            if (!cleanedKeptElvs.isEmpty() && (forced || maxReached))
            {
                const int removed = cleanedKeptElvs.removeOutsideRange(reference, r);
                if (removed > 0)
                {
                    cleaned = true;
                    QWriteLocker l(&m_lockElvCoordinates);
                    m_elvCoordinatesGnd = cleanedKeptElvs;
                }
            }

            return cleaned;
        }

        ISimulationEnvironmentProvider::ISimulationEnvironmentProvider(const CSimulatorPluginInfo &pluginInfo) :
            m_simulatorPluginInfo(pluginInfo)
        { }

        ISimulationEnvironmentProvider::ISimulationEnvironmentProvider(const CSimulatorPluginInfo &pluginInfo, const CSimulatorSettings &settings, bool supportElevation, bool supportCG) :
            m_simulatorPluginInfo(pluginInfo), m_settings(settings), m_enableElevation(supportElevation), m_enableCG(supportCG)
        { }

        bool ISimulationEnvironmentProvider::isCgProviderEnabled() const
        {
            QReadLocker l(&m_lockCG);
            return m_enableCG;
        }

        bool ISimulationEnvironmentProvider::isElevationProviderEnabled() const
        {
            QReadLocker l(&m_lockElvCoordinates);
            return m_enableElevation;
        }

        void ISimulationEnvironmentProvider::setCgProviderEnabled(bool enabled)
        {
            QWriteLocker l(&m_lockCG);
            m_enableCG = enabled;
        }

        void ISimulationEnvironmentProvider::setElevationProviderEnabled(bool enabled)
        {
            QWriteLocker l(&m_lockElvCoordinates);
            m_enableElevation = enabled;
        }

        void ISimulationEnvironmentProvider::setSimulationProviderEnabled(bool elvEnabled, bool cgEnabled)
        {
            setElevationProviderEnabled(elvEnabled);
            setCgProviderEnabled(cgEnabled);
        }

        void ISimulationEnvironmentProvider::setNewPluginInfo(const CSimulatorPluginInfo &info, const CSimulatorSettings &settings, const CAircraftModel &defaultModel)
        {
            this->setNewPluginInfo(info, settings);
            this->setDefaultModel(defaultModel);
        }

        void ISimulationEnvironmentProvider::setNewPluginInfo(const CSimulatorPluginInfo &info, const CSimulatorSettings &settings)
        {
            QWriteLocker l(&m_lockSimInfo);
            m_simulatorPluginInfo = info;
            m_settings = settings;
        }

        void ISimulationEnvironmentProvider::setSimulatorDetails(const QString &name, const QString &details, const QString &version)
        {
            QWriteLocker l(&m_lockSimInfo);
            m_simulatorName    = name;
            m_simulatorDetails = details;
            m_simulatorVersion = version;
        }

        QString ISimulationEnvironmentProvider::getSimulatorName() const
        {
            QReadLocker l(&m_lockSimInfo);
            return m_simulatorName;
        }

        QString ISimulationEnvironmentProvider::getSimulatorVersion() const
        {
            QReadLocker l(&m_lockSimInfo);
            return m_simulatorVersion;
        }

        QString ISimulationEnvironmentProvider::getSimulatorDetails() const
        {
            QReadLocker l(&m_lockSimInfo);
            return m_simulatorDetails;
        }

        void ISimulationEnvironmentProvider::setDefaultModel(const CAircraftModel &defaultModel)
        {
            QWriteLocker l(&m_lockModel);
            m_defaultModel = defaultModel;
        }

        void ISimulationEnvironmentProvider::clearDefaultModel()
        {
            QWriteLocker l(&m_lockModel);
            m_defaultModel = CAircraftModel();
        }

        void ISimulationEnvironmentProvider::clearElevations()
        {
            QWriteLocker l(&m_lockElvCoordinates);
            m_elvCoordinates.clear();
            m_elvCoordinatesGnd.clear();
            m_pendingElevationRequests.clear();
            m_statsCurrentElevRequestTimeMs = -1;
            m_statsMaxElevRequestTimeMs     = -1;
            m_elvFound = m_elvMissed        =  0;
        }

        void ISimulationEnvironmentProvider::clearCGs()
        {
            QWriteLocker l(&m_lockCG);
            m_cgsPerCallsign.clear();
            m_cgsPerCallsignOverridden.clear();
            // intentionally not cleaning CGs per model, as models will not change, callsign do!
        }

        void ISimulationEnvironmentProvider::clearSimulationEnvironmentData()
        {
            this->clearDefaultModel();
            this->clearElevations();
            this->clearCGs();
            this->resetSimulationEnvironmentStatistics();
        }

        // pin vtables to this file
        void CSimulationEnvironmentAware::anchor()
        { }

        CElevationPlane CSimulationEnvironmentAware::findClosestElevationWithinRange(const ICoordinateGeodetic &reference, const PhysicalQuantities::CLength &range) const
        {
            if (!this->hasProvider()) { return CElevationPlane::null(); }
            return this->provider()->findClosestElevationWithinRange(reference, range);
        }

        CElevationPlane CSimulationEnvironmentAware::findClosestElevationWithinRangeOrRequest(const ICoordinateGeodetic &reference, const CLength &range, const CCallsign &callsign)
        {
            if (!this->hasProvider()) { return CElevationPlane::null(); }
            return this->provider()->findClosestElevationWithinRangeOrRequest(reference, range, callsign);
        }

        CElevationPlane CSimulationEnvironmentAware::averageElevationOfOnGroundAircraft(const CAircraftSituation &reference, const CLength &range, int minValues, int sufficientValues) const
        {
            if (!this->hasProvider()) { return CElevationPlane::null(); }
            return this->provider()->averageElevationOfOnGroundAircraft(reference, range, minValues, sufficientValues);
        }

        CAltitude CSimulationEnvironmentAware::highestElevation() const
        {
            if (!this->hasProvider()) { return CAltitude::null(); }
            return this->provider()->highestElevation();
        }

        bool CSimulationEnvironmentAware::requestElevation(const ICoordinateGeodetic &reference, const CCallsign &callsign)
        {
            if (!this->hasProvider()) { return false; }
            return this->provider()->requestElevation(reference, callsign);
        }

        bool CSimulationEnvironmentAware::requestElevation(const CAircraftSituation &situation)
        {
            return this->requestElevation(situation, situation.getCallsign());
        }

        QPair<int, int> CSimulationEnvironmentAware::getElevationsFoundMissed() const
        {
            if (!this->hasProvider()) { return QPair<int, int>(0, 0); }
            return this->provider()->getElevationsFoundMissed();
        }

        QString CSimulationEnvironmentAware::getElevationsFoundMissedInfo() const
        {
            if (!this->hasProvider()) { return QString(); }
            return this->provider()->getElevationsFoundMissedInfo();
        }

        QPair<qint64, qint64> CSimulationEnvironmentAware::getElevationRequestTimes() const
        {
            if (!this->hasProvider()) { return QPair<qint64, qint64>(-1, -1); }
            return this->provider()->getElevationRequestTimes();
        }

        QString CSimulationEnvironmentAware::getElevationRequestTimesInfo() const
        {
            if (!this->hasProvider()) { return QString(); }
            return this->provider()->getElevationRequestTimesInfo();
        }

        CSimulatorPluginInfo CSimulationEnvironmentAware::getSimulatorPluginInfo() const
        {
            if (!this->hasProvider()) { return CSimulatorPluginInfo(); }
            return this->provider()->getSimulatorPluginInfo();
        }

        CSimulatorInfo CSimulationEnvironmentAware::getSimulatorInfo() const
        {
            if (!this->hasProvider()) { return CSimulatorInfo(); }
            return this->provider()->getSimulatorInfo();
        }

        QString CSimulationEnvironmentAware::getSimulatorNameAndVersion() const
        {
            if (!this->hasProvider()) { return "not available"; }
            return this->provider()->getSimulatorNameAndVersion();
        }

        CAircraftModel CSimulationEnvironmentAware::getDefaultModel() const
        {
            if (!this->hasProvider()) { return CAircraftModel(); }
            return this->provider()->getDefaultModel();
        }

        CLength CSimulationEnvironmentAware::getSimulatorCG(const CCallsign &callsign) const
        {
            if (!this->hasProvider()) { return CLength::null(); }
            return this->provider()->getSimulatorCG(callsign);
        }

        CLength CSimulationEnvironmentAware::getSimulatorOrDbCG(const CCallsign &callsign, const CLength &dbCG) const
        {
            if (!this->hasProvider()) { return CLength::null(); }
            return this->provider()->getSimulatorOrDbCG(callsign, dbCG);
        }

        bool CSimulationEnvironmentAware::hasSimulatorCG(const CCallsign &callsign) const
        {
            if (!this->hasProvider()) { return false; }
            return this->provider()->hasSimulatorCG(callsign);
        }

        bool CSimulationEnvironmentAware::cleanElevationValues(const CAircraftSituation &reference, const CLength &range, bool forced)
        {
            if (!this->hasProvider()) { return false; }
            return this->provider()->cleanElevationValues(reference, range, forced);
        }
    } // namespace
} // namespace
