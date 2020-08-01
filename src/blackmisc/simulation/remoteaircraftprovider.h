/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_REMOTEAIRCRAFTPROVIDER_H
#define BLACKMISC_SIMULATION_REMOTEAIRCRAFTPROVIDER_H

#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/airspaceaircraftsnapshot.h"
#include "blackmisc/simulation/reverselookup.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/aviation/aircraftpartslist.h"
#include "blackmisc/aviation/aircraftsituationlist.h"
#include "blackmisc/aviation/aircraftsituationchangelist.h"
#include "blackmisc/aviation/percallsign.h"
#include "blackmisc/aviation/callsignset.h"
#include "blackmisc/provider.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/identifiable.h"

#include <QHash>
#include <QList>
#include <QMetaObject>
#include <QObject>
#include <QJsonObject>
#include <QtGlobal>
#include <QReadWriteLock>
#include <functional>

namespace BlackMisc
{
    namespace Geo { class CElevationPlane; }
    namespace Simulation
    {
        //! Direct thread safe in memory access to remote aircraft
        //! \note Can not be derived from QObject (as for the signals), as this would create multiple
        //!       inheritance. Hence Q_DECLARE_INTERFACE is used.
        //! \ingroup remoteaircraftprovider
        class BLACKMISC_EXPORT IRemoteAircraftProvider : public IProvider
        {
        public:
            static constexpr int MaxSituationsPerCallsign   = 6;  //!< How many situations we keep per callsign
            static constexpr int MaxPartsPerCallsign        = 20; //!< How many parts we keep per callsign (we keep more parts than situations as parts can just come in)
            static constexpr int MaxPartsAgePerCallsignSecs = 60; //!< How many seconds to keep parts for interpolation

            //! Destructor
            virtual ~IRemoteAircraftProvider() override;

            //! Copy constructor
            IRemoteAircraftProvider(const IRemoteAircraftProvider &) = delete;

            //! Copy assignment operator
            IRemoteAircraftProvider &operator =(const IRemoteAircraftProvider &) = delete;

            //! All remote aircraft
            //! \threadsafe
            virtual CSimulatedAircraftList getAircraftInRange() const = 0;

            //! Count remote aircraft
            //! \threadsafe
            virtual int getAircraftInRangeCount() const = 0;

            //! Unique callsigns for aircraft in range
            //! \threadsafe
            virtual Aviation::CCallsignSet getAircraftInRangeCallsigns() const = 0;

            //! Is aircraft in range?
            //! \threadsafe
            virtual bool isAircraftInRange(const Aviation::CCallsign &callsign) const = 0;

            //! Is VTOL aircraft?
            //! \threadsafe
            virtual bool isVtolAircraft(const Aviation::CCallsign &callsign) const = 0;

            //! Current snapshot
            //! \threadsafe
            virtual CAirspaceAircraftSnapshot getLatestAirspaceAircraftSnapshot() const = 0;

            //! Aircraft for callsign
            //! \threadsafe
            virtual CSimulatedAircraft getAircraftInRangeForCallsign(const Aviation::CCallsign &callsign) const = 0;

            //! Aircraft model for callsign
            //! \threadsafe
            virtual CAircraftModel getAircraftInRangeModelForCallsign(const Aviation::CCallsign &callsign) const = 0;

            //! Rendered aircraft situations (per callsign, time history)
            //! \threadsafe
            virtual Aviation::CAircraftSituationList remoteAircraftSituations(const Aviation::CCallsign &callsign) const = 0;

            //! Average update time
            //! \threadsafe
            virtual MillisecondsMinMaxMean remoteAircraftSituationsTimestampDifferenceMinMaxMean(const Aviation::CCallsign &callsign) const = 0;

            //! Rendered aircraft situations (per callsign and index)
            //! \remark if situation does not exist, an NULL situation is returned
            //! \param callsign
            //! \param index 0..latest, 1..next older, ...
            //! \threadsafe
            virtual Aviation::CAircraftSituation remoteAircraftSituation(const Aviation::CCallsign &callsign, int index) const = 0;

            //! Latest aircraft situation for all callsigns
            //! \threadsafe
            virtual Aviation::CAircraftSituationList latestRemoteAircraftSituations() const = 0;

            //! Latest aircraft situation "on ground" having a provider elevation
            //! \threadsafe
            virtual Aviation::CAircraftSituationList latestOnGroundProviderElevations() const = 0;

            //! Number of remote aircraft situations for callsign
            //! \remark latest situations first
            //! \threadsafe
            virtual int remoteAircraftSituationsCount(const Aviation::CCallsign &callsign) const = 0;

            //! All parts (per callsign, time history)
            //! \remark latest parts first
            //! \threadsafe
            virtual Aviation::CAircraftPartsList remoteAircraftParts(const Aviation::CCallsign &callsign) const = 0;

            //! All parts (per callsign, time history)
            //! \threadsafe
            virtual int remoteAircraftPartsCount(const Aviation::CCallsign &callsign) const = 0;

            //! Is remote aircraft supporting parts?
            //! \threadsafe
            virtual bool isRemoteAircraftSupportingParts(const Aviation::CCallsign &callsign) const = 0;

            //! Get the latest aircraft parts (if any, otherwise default)
            //! \threadsafe
            Aviation::CAircraftParts getLatestAircraftParts(const Aviation::CCallsign &callsign) const;

            //! Number of aircraft supporting parts
            //! \threadsafe
            virtual int getRemoteAircraftSupportingPartsCount() const = 0;

            //! Remote aircraft supporting parts.
            //! \threadsafe
            virtual Aviation::CCallsignSet remoteAircraftSupportingParts() const = 0;

            //! Aircraft changes.
            //! \threadsafe
            virtual Aviation::CAircraftSituationChangeList remoteAircraftSituationChanges(const Aviation::CCallsign &callsign) const = 0;

            //! Aircraft changes count.
            //! \threadsafe
            virtual int remoteAircraftSituationChangesCount(const Aviation::CCallsign &callsign) const = 0;

            //! Enable/disable aircraft and follow up logic like sending signals
            //! \threadsafe
            //! \remark depending on implementation similar or more sophisticated as setEnabledFlag
            virtual bool updateAircraftEnabled(const Aviation::CCallsign &callsign, bool enabledForRendering) = 0;

            //! Just set enable/disable aircraft flag, no further logic
            //! \threadsafe
            virtual bool setAircraftEnabledFlag(const BlackMisc::Aviation::CCallsign &callsign, bool enabledForRendering) = 0;

            //! Enable/disable aircraft
            //! \threadsafe
            virtual int updateMultipleAircraftEnabled(const Aviation::CCallsignSet &callsigns, bool enabledForRendering) = 0;

            //! Set aircraft rendered
            //! \threadsafe
            virtual bool updateAircraftRendered(const Aviation::CCallsign &callsign, bool rendered) = 0;

            //! Set aircraft rendered
            //! \threadsafe
            virtual int updateMultipleAircraftRendered(const Aviation::CCallsignSet &callsigns, bool rendered) = 0;

            //! Mark all as not rendered
            //! \threadsafe
            virtual void updateMarkAllAsNotRendered() = 0;

            //! Change model
            //! \threadsafe
            virtual bool updateAircraftModel(const Aviation::CCallsign &callsign, const CAircraftModel &model, const CIdentifier &originator) = 0;

            //! Change network model
            //! \threadsafe
            virtual bool updateAircraftNetworkModel(const Aviation::CCallsign &callsign, const CAircraftModel &model, const CIdentifier &originator) = 0;

            //! Change fast position updates
            //! \threadsafe
            virtual bool updateFastPositionEnabled(const Aviation::CCallsign &callsign, bool enableFastPositonUpdates) = 0;

            //! Update the ground elevation
            //! \threadsafe
            virtual int updateAircraftGroundElevation(const Aviation::CCallsign &callsign, const Geo::CElevationPlane &elevation, Aviation::CAircraftSituation::GndElevationInfo info, bool *updateAircraftGroundElevation) = 0;

            //! Update the CG
            //! \threadsafe
            virtual bool updateCG(const Aviation::CCallsign &callsign, const PhysicalQuantities::CLength &cg) = 0;

            //! Update the CG for this model string
            //! \threadsafe
            virtual Aviation::CCallsignSet updateCGForModel(const QString &modelString, const PhysicalQuantities::CLength &cg) = 0;

            //! Update the CG and model string
            //! \threadsafe
            virtual bool updateCGAndModelString(const Aviation::CCallsign &callsign, const PhysicalQuantities::CLength &cg, const QString &modelString) = 0;

            //! CG values from DB
            //! \threadsafe
            //! @{
            virtual PhysicalQuantities::CLength getCGFromDB(const Aviation::CCallsign &callsign) const = 0;
            virtual PhysicalQuantities::CLength getCGFromDB(const QString &modelString) const = 0;
            virtual void rememberCGFromDB(const PhysicalQuantities::CLength &cgFromDB, const Aviation::CCallsign &callsign) = 0;
            virtual void rememberCGFromDB(const PhysicalQuantities::CLength &cgFromDB, const QString &modelString) = 0;
            //! @}

            //! Get reverse lookup meesages
            //! \threadsafe
            virtual CStatusMessageList getReverseLookupMessages(const Aviation::CCallsign &callsign) const = 0;

            //! Enabled reverse lookup logging?
            //! \threadsafe
            virtual ReverseLookupLogging isReverseLookupMessagesEnabled() const = 0;

            //! Enable reverse lookup logging
            //! \threadsafe
            virtual void enableReverseLookupMessages(ReverseLookupLogging enable) = 0;

            //! Get aircraft parts history
            //! \threadsafe
            virtual CStatusMessageList getAircraftPartsHistory(const Aviation::CCallsign &callsign) const = 0;

            //! Is storing aircraft parts history enabled?
            //! \threadsafe
            virtual bool isAircraftPartsHistoryEnabled() const = 0;

            //! Enable storing of aircraft parts history
            //! \threadsafe
            virtual void enableAircraftPartsHistory(bool enabled) = 0;

            //! Number of situations added
            //! \threadsafe
            virtual int aircraftSituationsAdded() const = 0;

            //! Number of parts added
            //! \threadsafe
            virtual int aircraftPartsAdded() const = 0;

            //! When last modified
            //! \threadsafe
            virtual qint64 situationsLastModified(const Aviation::CCallsign &callsign) const = 0;

            //! When last modified
            //! \threadsafe
            virtual qint64 partsLastModified(const Aviation::CCallsign &callsign) const = 0;

            //! Average elevation of aircraft in given range, which are NOT moving
            //! \remark can be used to anticipate field elevation
            //! \threadsafe
            virtual Geo::CElevationPlane averageElevationOfNonMovingAircraft(const Aviation::CAircraftSituation &reference, const PhysicalQuantities::CLength &range, int minValues = 1, int sufficientValues = 2) const = 0;

            //! Connect signals to slot receiver. As the interface is no QObject, slots can not be connected directly.
            //! In order to disconnect a list of connections is provided, which have to be disconnected manually.
            //! \note all connections are normally Qt::QueuedConnection receiver is required for connection type
            virtual QList<QMetaObject::Connection> connectRemoteAircraftProviderSignals(
                QObject *receiver,
                std::function<void(const Aviation::CAircraftSituation &)> addedSituationSlot,
                std::function<void(const Aviation::CCallsign &, const Aviation::CAircraftParts &)> addedPartsSlot,
                std::function<void(const Aviation::CCallsign &)> removedAircraftSlot,
                std::function<void(const CAirspaceAircraftSnapshot &)> aircraftSnapshot
            ) = 0;

            //! Remove outdated aircraft parts, but never the most recent one
            static void removeOutdatedParts(Aviation::CAircraftPartsList &partsList);

            //! Wildcard callsign
            static const Aviation::CCallsign &testAltitudeOffsetCallsign();

        protected:
            //! Constructor
            IRemoteAircraftProvider();
        };
    } //s
} // ns

Q_DECLARE_INTERFACE(BlackMisc::Simulation::IRemoteAircraftProvider, "org.swift-project.iremoteaircraftprovider")

namespace BlackMisc
{
    namespace Simulation
    {
        //! Implementaion of the interface, which can also be used for testing
        class BLACKMISC_EXPORT CRemoteAircraftProvider :
            public QObject,
            public IRemoteAircraftProvider,
            public CIdentifiable
        {
            Q_OBJECT
            Q_INTERFACES(BlackMisc::Simulation::IRemoteAircraftProvider)

        public:
            //! Log categories
            static const CLogCategoryList &getLogCategories();

            //! Constructor
            CRemoteAircraftProvider(QObject *parent);

            //! \ingroup remoteaircraftprovider
            //! @{
            virtual CSimulatedAircraftList getAircraftInRange() const override;
            virtual Aviation::CCallsignSet getAircraftInRangeCallsigns() const override;
            virtual CSimulatedAircraft getAircraftInRangeForCallsign(const Aviation::CCallsign &callsign) const override;
            virtual CAircraftModel getAircraftInRangeModelForCallsign(const Aviation::CCallsign &callsign) const override;
            virtual int getAircraftInRangeCount() const override;
            virtual bool isAircraftInRange(const Aviation::CCallsign &callsign) const override;
            virtual bool isVtolAircraft(const Aviation::CCallsign &callsign) const override;
            virtual Aviation::CAircraftSituationList remoteAircraftSituations(const Aviation::CCallsign &callsign) const override;
            virtual Aviation::CAircraftSituation remoteAircraftSituation(const Aviation::CCallsign &callsign, int index) const override;
            virtual MillisecondsMinMaxMean remoteAircraftSituationsTimestampDifferenceMinMaxMean(const Aviation::CCallsign &callsign) const override;
            virtual Aviation::CAircraftSituationList latestRemoteAircraftSituations() const override;
            virtual Aviation::CAircraftSituationList latestOnGroundProviderElevations() const override;
            virtual int remoteAircraftSituationsCount(const Aviation::CCallsign &callsign) const override;
            virtual Aviation::CAircraftPartsList remoteAircraftParts(const Aviation::CCallsign &callsign) const override;
            virtual int remoteAircraftPartsCount(const Aviation::CCallsign &callsign) const override;
            virtual bool isRemoteAircraftSupportingParts(const Aviation::CCallsign &callsign) const override;
            virtual int getRemoteAircraftSupportingPartsCount() const override;
            virtual Aviation::CCallsignSet remoteAircraftSupportingParts() const override;
            virtual Aviation::CAircraftSituationChangeList remoteAircraftSituationChanges(const Aviation::CCallsign &callsign) const override;
            virtual int remoteAircraftSituationChangesCount(const Aviation::CCallsign &callsign) const override;
            virtual bool updateAircraftEnabled(const Aviation::CCallsign &callsign, bool enabledForRendering) override;
            virtual bool setAircraftEnabledFlag(const BlackMisc::Aviation::CCallsign &callsign, bool enabledForRendering) override;
            virtual int updateMultipleAircraftEnabled(const Aviation::CCallsignSet &callsigns, bool enabledForRendering) override;
            virtual bool updateAircraftModel(const Aviation::CCallsign &callsign, const CAircraftModel &model, const CIdentifier &originator) override;
            virtual bool updateAircraftNetworkModel(const Aviation::CCallsign &callsign, const CAircraftModel &model, const CIdentifier &originator) override;
            virtual bool updateFastPositionEnabled(const Aviation::CCallsign &callsign, bool enableFastPositonUpdates) override;
            virtual bool updateAircraftRendered(const Aviation::CCallsign &callsign, bool rendered) override;
            virtual int updateMultipleAircraftRendered(const Aviation::CCallsignSet &callsigns, bool rendered) override;
            virtual int updateAircraftGroundElevation(const Aviation::CCallsign &callsign, const Geo::CElevationPlane &elevation, Aviation::CAircraftSituation::GndElevationInfo info, bool *setForOnGroundPosition) override;
            virtual bool updateCG(const Aviation::CCallsign &callsign, const PhysicalQuantities::CLength &cg) override;
            virtual bool updateCGAndModelString(const Aviation::CCallsign &callsign, const PhysicalQuantities::CLength &cg, const QString &modelString) override;
            virtual Aviation::CCallsignSet updateCGForModel(const QString &modelString, const PhysicalQuantities::CLength &cg) override;
            virtual PhysicalQuantities::CLength getCGFromDB(const Aviation::CCallsign &callsign) const override;
            virtual PhysicalQuantities::CLength getCGFromDB(const QString &modelString) const override;
            virtual void rememberCGFromDB(const PhysicalQuantities::CLength &cgFromDB, const Aviation::CCallsign &callsign) override;
            virtual void rememberCGFromDB(const PhysicalQuantities::CLength &cgFromDB, const QString &modelString) override;
            virtual void updateMarkAllAsNotRendered() override;
            virtual CStatusMessageList getAircraftPartsHistory(const Aviation::CCallsign &callsign) const override;
            virtual bool isAircraftPartsHistoryEnabled() const override;
            virtual void enableAircraftPartsHistory(bool enabled) override;
            virtual int aircraftSituationsAdded() const override;
            virtual int aircraftPartsAdded() const override;
            virtual qint64 situationsLastModified(const Aviation::CCallsign &callsign) const override;
            virtual qint64 partsLastModified(const Aviation::CCallsign &callsign) const override;
            virtual Geo::CElevationPlane averageElevationOfNonMovingAircraft(const Aviation::CAircraftSituation &reference, const PhysicalQuantities::CLength &range, int minValues = 1, int sufficientValues = 2) const override;
            virtual QList<QMetaObject::Connection> connectRemoteAircraftProviderSignals(
                QObject *receiver,
                std::function<void(const Aviation::CAircraftSituation &)> addedSituationSlot,
                std::function<void(const Aviation::CCallsign &, const Aviation::CAircraftParts &)> addedPartsSlot,
                std::function<void(const Aviation::CCallsign &)> removedAircraftSlot,
                std::function<void(const CAirspaceAircraftSnapshot &)> aircraftSnapshotSlot
            ) override;
            //! @}

            //! \ingroup remoteaircraftprovider
            //! \ingroup reverselookup
            //! @{
            virtual void enableReverseLookupMessages(ReverseLookupLogging enable) override;
            virtual ReverseLookupLogging isReverseLookupMessagesEnabled() const override;
            virtual BlackMisc::CStatusMessageList getReverseLookupMessages(const BlackMisc::Aviation::CCallsign &callsign) const override;
            //! @}

            //! Reverse lookup messages
            //! \threadsafe
            //! \ingroup reverselookup
            //! @{
            void addReverseLookupMessages(const Aviation::CCallsign &callsign, const CStatusMessageList &messages);
            void addReverseLookupMessage(const Aviation::CCallsign &callsign, const CStatusMessage &message);
            void addReverseLookupMessage(
                const Aviation::CCallsign &callsign, const QString &message,
                CStatusMessage::StatusSeverity severity = CStatusMessage::SeverityInfo);
            //! @}

            //! \copydoc BlackMisc::IProvider::asQObject
            virtual QObject *asQObject() override { return this; }

            //! Clear all data
            void clear();

            // ------------------- testing ---------------

            //! Has test offset value?
            bool hasTestAltitudeOffset(const Aviation::CCallsign &callsign) const;

            //! Has test offset value?
            bool hasTestAltitudeOffsetGlobalValue() const;

            //! Offset for callsign
            bool testAddAltitudeOffset(const Aviation::CCallsign &callsign, const PhysicalQuantities::CLength &offset);

        signals:
            //! A new aircraft appeared
            void addedAircraft(const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft);

            //! Parts added
            void addedAircraftParts(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftParts &parts);

            //! Situation added
            void addedAircraftSituation(const BlackMisc::Aviation::CAircraftSituation &situation);

            //! Aircraft were changed
            void changedAircraftInRange();

            //! An aircraft disappeared
            void removedAircraft(const BlackMisc::Aviation::CCallsign &callsign);

            //! \copydoc BlackCore::CAirspaceAnalyzer::airspaceAircraftSnapshot
            void airspaceAircraftSnapshot(const BlackMisc::Simulation::CAirspaceAircraftSnapshot &snapshot);

        protected:
            //! Remove all aircraft in range
            //! \threadsafe
            bool removeAircraft(const Aviation::CCallsign &callsign);

            //! Remove all aircraft in range
            //! \threadsafe
            void removeAllAircraft();

            //! Remove the lookup messages
            //! \threadsafe
            void removeReverseLookupMessages(const Aviation::CCallsign &callsign);

            //! Add new aircraft, ignored if aircraft already exists
            //! \threadsafe
            bool addNewAircraftInRange(const CSimulatedAircraft &aircraft);

            //! Init a new aircraft and add it or update model of existing aircraft
            //! \threadsafe
            CSimulatedAircraft addOrUpdateAircraftInRange(
                const Aviation::CCallsign &callsign,
                const QString &aircraftIcao, const QString &airlineIcao, const QString &livery, const QString &modelString,
                CAircraftModel::ModelType modelType,
                CStatusMessageList *log = nullptr);

            //! Update aircraft
            //! \threadsafe
            int updateAircraftInRange(const Aviation::CCallsign &callsign, const CPropertyIndexVariantMap &vm, bool skipEqualValues = true);

            //! Update aircraft bearing, distance and situation
            //! \threadsafe
            //! \remark does NOT emit signals
            bool updateAircraftInRangeDistanceBearing(const Aviation::CCallsign &callsign, const Aviation::CAircraftSituation &situation, const PhysicalQuantities::CLength &distance, const PhysicalQuantities::CAngle &bearing);

            //! Store an aircraft situation
            //! \remark latest situations are kept first
            //! \threadsafe
            virtual Aviation::CAircraftSituation storeAircraftSituation(const Aviation::CAircraftSituation &situation, bool allowTestAltitudeOffset = true);

            //! Store an aircraft part
            //! \remark latest parts are kept first
            //! \threadsafe
            //! @{
            void storeAircraftParts(const Aviation::CCallsign &callsign, const Aviation::CAircraftParts &parts, bool removeOutdated);
            void storeAircraftParts(const Aviation::CCallsign &callsign, const QJsonObject &jsonObject, qint64 currentOffsetMs);
            //! @}

            //! Guess situation "on ground" and update model's CG if applicable
            //! \remark updates CG and ground flag in situation
            bool guessOnGroundAndUpdateModelCG(Aviation::CAircraftSituation &situation, const Aviation::CAircraftSituationChange &change, const CAircraftModel &aircraftModel);

            //! Add an offset for testing
            Aviation::CAircraftSituation addTestAltitudeOffsetToSituation(const Aviation::CAircraftSituation &situation) const;

            //! What to log?
            //! \threadsafe
            ReverseLookupLogging whatToReverseLog() const;

        private:
            //! Store the latest changes
            //! \remark latest first
            //! \threadsafe
            void storeChange(const Aviation::CAircraftSituationChange &change);

            Aviation::CAircraftSituationListPerCallsign m_situationsByCallsign;        //!< situations, for performance reasons per callsign, thread safe access required
            Aviation::CAircraftSituationPerCallsign m_latestSituationByCallsign;       //!< latest situations, for performance reasons per callsign, thread safe access required
            Aviation::CAircraftSituationPerCallsign m_latestOnGroundProviderElevation; //!< situations on ground with elevation from provider
            Aviation::CAircraftPartsListPerCallsign m_partsByCallsign;                 //!< parts, for performance reasons per callsign, thread safe access required
            Aviation::CAircraftSituationChangeListPerCallsign m_changesByCallsign;     //!< changes, for performance reasons per callsign, thread safe access required (same timestamps as corresponding situations)
            Aviation::CCallsignSet m_aircraftWithParts;                                //!< aircraft supporting parts, thread safe access required
            int m_situationsAdded = 0; //!< total number of situations added, thread safe access required
            int m_partsAdded      = 0; //!< total number of parts added, thread safe access required

            ReverseLookupLogging m_enableReverseLookupMsgs = RevLogSimplifiedInfo;     //!< shall we log. information about the matching process
            Simulation::CSimulatedAircraftPerCallsign m_aircraftInRange;      //!< aircraft, thread safe access required
            Aviation::CStatusMessageListPerCallsign m_reverseLookupMessages;  //!< reverse lookup messages
            Aviation::CStatusMessageListPerCallsign m_aircraftPartsMessages;  //!< status messages for parts history
            Aviation::CTimestampPerCallsign m_situationsLastModified;         //!< when situations last modified
            Aviation::CTimestampPerCallsign m_partsLastModified;              //!< when parts last modified
            Aviation::CLengthPerCallsign    m_testOffset;                     //!< offsets
            Aviation::CLengthPerCallsign    m_dbCGPerCallsign;                //!< DB CG per callsign
            QHash<QString, PhysicalQuantities::CLength> m_dbCGPerModelString; //!< DB CG per model string

            bool m_enableAircraftPartsHistory = true;  //!< shall we keep a history of aircraft parts

            // locks
            mutable QReadWriteLock m_lockSituations;   //!< lock for situations: m_situationsByCallsign
            mutable QReadWriteLock m_lockParts;        //!< lock for parts: m_partsByCallsign, m_aircraftSupportingParts
            mutable QReadWriteLock m_lockChanges;      //!< lock for changes: m_changesByCallsign
            mutable QReadWriteLock m_lockAircraft;     //!< lock aircraft: m_aircraftInRange, m_dbCGPerCallsign
            mutable QReadWriteLock m_lockMessages;     //!< lock for messages
            mutable QReadWriteLock m_lockPartsHistory; //!< lock for aircraft parts
        };

        //! Class which can be directly used to access an \sa IRemoteAircraftProvider object
        class BLACKMISC_EXPORT CRemoteAircraftAware : public IProviderAware<IRemoteAircraftProvider>
        {
        public:
            //! Destructor
            virtual ~CRemoteAircraftAware() override;

            //! \copydoc IRemoteAircraftProvider::getAircraftInRange
            CSimulatedAircraftList getAircraftInRange() const;

            //! \copydoc IRemoteAircraftProvider::isAircraftInRange
            bool isAircraftInRange(const Aviation::CCallsign &callsign) const;

            //! \copydoc IRemoteAircraftProvider::isVtolAircraft
            bool isVtolAircraft(const Aviation::CCallsign &callsign) const;

            //! \copydoc IRemoteAircraftProvider::getAircraftInRangeCount
            int getAircraftInRangeCount() const;

            //! \copydoc IRemoteAircraftProvider::getAircraftInRangeCallsigns
            Aviation::CCallsignSet getAircraftInRangeCallsigns() const;

            //! \copydoc IRemoteAircraftProvider::getAircraftInRangeForCallsign
            CSimulatedAircraft getAircraftInRangeForCallsign(const Aviation::CCallsign &callsign) const;

            //! \copydoc IRemoteAircraftProvider::getAircraftInRangeModelForCallsign
            CAircraftModel getAircraftInRangeModelForCallsign(const Aviation::CCallsign &callsign) const;

            //! \copydoc IRemoteAircraftProvider::getLatestAirspaceAircraftSnapshot
            CAirspaceAircraftSnapshot getLatestAirspaceAircraftSnapshot() const;

            //! \copydoc IRemoteAircraftProvider::remoteAircraftSituations
            Aviation::CAircraftSituationList remoteAircraftSituations(const Aviation::CCallsign &callsign) const;

            //! \copydoc IRemoteAircraftProvider::remoteAircraftSituation
            Aviation::CAircraftSituation remoteAircraftSituation(const Aviation::CCallsign &callsign, int index) const;

            //! \copydoc IRemoteAircraftProvider::latestRemoteAircraftSituations
            Aviation::CAircraftSituationList latestRemoteAircraftSituations() const;

            //! \copydoc IRemoteAircraftProvider::latestOnGroundProviderElevations
            Aviation::CAircraftSituationList latestOnGroundProviderElevations() const;

            //! \copydoc IRemoteAircraftProvider::remoteAircraftSituationsCount
            int remoteAircraftSituationsCount(const Aviation::CCallsign &callsign) const;

            //! \copydoc IRemoteAircraftProvider::remoteAircraftParts
            Aviation::CAircraftPartsList remoteAircraftParts(const Aviation::CCallsign &callsign) const;

            //! \copydoc IRemoteAircraftProvider::remoteAircraftPartsCount
            int remoteAircraftPartsCount(const Aviation::CCallsign &callsign) const;

            //! \copydoc IRemoteAircraftProvider::remoteAircraftSituationChanges
            Aviation::CAircraftSituationChangeList remoteAircraftSituationChanges(const Aviation::CCallsign &callsign) const;

            //! \copydoc IRemoteAircraftProvider::remoteAircraftSupportingParts
            Aviation::CCallsignSet remoteAircraftSupportingParts() const;

            //! \copydoc IRemoteAircraftProvider::isRemoteAircraftSupportingParts
            bool isRemoteAircraftSupportingParts(const Aviation::CCallsign &callsign) const;

            //! \copydoc IRemoteAircraftProvider::getRemoteAircraftSupportingPartsCount
            int getRemoteAircraftSupportingPartsCount() const;

            //! \copydoc IRemoteAircraftProvider::updateAircraftEnabled
            bool updateAircraftEnabled(const Aviation::CCallsign &callsign, bool enabledForRendering);

            //! \copydoc IRemoteAircraftProvider::setAircraftEnabledFlag
            bool setAircraftEnabledFlag(const Aviation::CCallsign &callsign, bool enabledForRendering);

            //! \copydoc IRemoteAircraftProvider::updateMultipleAircraftEnabled
            bool updateMultipleAircraftEnabled(const Aviation::CCallsignSet &callsigns, bool enabledForRendering);

            //! \copydoc IRemoteAircraftProvider::updateAircraftModel
            bool updateAircraftModel(const Aviation::CCallsign &callsign, const CAircraftModel &model, const CIdentifier &originator);

            //! \copydoc IRemoteAircraftProvider::updateAircraftNetworkModel
            bool updateAircraftNetworkModel(const Aviation::CCallsign &callsign, const CAircraftModel &model, const CIdentifier &originator);

            //! \copydoc IRemoteAircraftProvider::updateAircraftRendered
            bool updateAircraftRendered(const Aviation::CCallsign &callsign, bool rendered);

            //! \copydoc IRemoteAircraftProvider::updateMultipleAircraftRendered
            bool updateMultipleAircraftRendered(const Aviation::CCallsignSet &callsigns, bool rendered);

            //! \copydoc IRemoteAircraftProvider::updateAircraftGroundElevation
            int updateAircraftGroundElevation(const Aviation::CCallsign &callsign, const Geo::CElevationPlane &elevation, Aviation::CAircraftSituation::GndElevationInfo info, bool *updateAircraftGroundElevation);

            //! \copydoc IRemoteAircraftProvider::updateCG
            bool updateCG(const Aviation::CCallsign &callsign, const PhysicalQuantities::CLength &cg);

            //! \copydoc IRemoteAircraftProvider::updateCGForModel
            Aviation::CCallsignSet updateCGForModel(const QString &modelString, const PhysicalQuantities::CLength &cg);

            //! \copydoc IRemoteAircraftProvider::updateCGAndModelString
            bool updateCGAndModelString(const Aviation::CCallsign &callsign, const PhysicalQuantities::CLength &cg, const QString &modelString);

            //! \copydoc IRemoteAircraftProvider::updateMarkAllAsNotRendered
            void updateMarkAllAsNotRendered();

            //! \copydoc IRemoteAircraftProvider::aircraftSituationsAdded
            int aircraftSituationsAdded() const;

            //! \copydoc IRemoteAircraftProvider::aircraftPartsAdded
            int aircraftPartsAdded() const;

            //! \copydoc IRemoteAircraftProvider::situationsLastModified
            qint64 situationsLastModified(const Aviation::CCallsign &callsign) const;

            //! \copydoc IRemoteAircraftProvider::partsLastModified
            qint64 partsLastModified(const Aviation::CCallsign &callsign) const;

            //! \copydoc IRemoteAircraftProvider::averageElevationOfNonMovingAircraft
            Geo::CElevationPlane averageElevationOfNonMovingAircraft(const Aviation::CAircraftSituation &reference, const PhysicalQuantities::CLength &range, int minValues = 1) const;

            //! Set remote aircraft provider
            void setRemoteAircraftProvider(IRemoteAircraftProvider *remoteAircraftProvider) { this->setProvider(remoteAircraftProvider); }

            //! Get the aircraft provider
            IRemoteAircraftProvider *getRemoteAircraftProvider() const { return this->getProvider(); }

        protected:
            //! Default constructor
            CRemoteAircraftAware() {}

            //! Constructor
            CRemoteAircraftAware(IRemoteAircraftProvider *remoteAircraftProvider) : IProviderAware(remoteAircraftProvider) { Q_ASSERT(remoteAircraftProvider); }
        };
    } // namespace
} // namespace

#endif // guard
