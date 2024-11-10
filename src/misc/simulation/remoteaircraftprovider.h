// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_REMOTEAIRCRAFTPROVIDER_H
#define SWIFT_MISC_SIMULATION_REMOTEAIRCRAFTPROVIDER_H

#include "misc/simulation/aircraftmodel.h"
#include "misc/simulation/airspaceaircraftsnapshot.h"
#include "misc/simulation/reverselookup.h"
#include "misc/simulation/simulatedaircraftlist.h"
#include "misc/aviation/aircraftpartslist.h"
#include "misc/aviation/aircraftsituationlist.h"
#include "misc/aviation/aircraftsituationchangelist.h"
#include "misc/aviation/percallsign.h"
#include "misc/aviation/callsignset.h"
#include "misc/provider.h"
#include "misc/swiftmiscexport.h"
#include "misc/identifiable.h"

#include <QHash>
#include <QList>
#include <QMetaObject>
#include <QObject>
#include <QJsonObject>
#include <QtGlobal>
#include <QReadWriteLock>
#include <functional>

namespace swift::misc
{
    namespace geo
    {
        class CElevationPlane;
    }
    namespace simulation
    {
        //! Direct thread safe in memory access to remote aircraft
        //! \note Can not be derived from QObject (as for the signals), as this would create multiple
        //!       inheritance. Hence Q_DECLARE_INTERFACE is used.
        //! \ingroup remoteaircraftprovider
        class SWIFT_MISC_EXPORT IRemoteAircraftProvider : public IProvider
        {
        public:
            static constexpr int MaxSituationsPerCallsign = 50; //!< How many situations we keep per callsign
            static constexpr int MaxPartsPerCallsign = 50; //!< How many parts we keep per callsign
            static constexpr int MaxPartsAgePerCallsignSecs = 60; //!< How many seconds to keep parts for interpolation

            //! Destructor
            virtual ~IRemoteAircraftProvider() override;

            //! Copy constructor
            IRemoteAircraftProvider(const IRemoteAircraftProvider &) = delete;

            //! Copy assignment operator
            IRemoteAircraftProvider &operator=(const IRemoteAircraftProvider &) = delete;

            //! All remote aircraft
            //! \threadsafe
            virtual CSimulatedAircraftList getAircraftInRange() const = 0;

            //! Count remote aircraft
            //! \threadsafe
            virtual int getAircraftInRangeCount() const = 0;

            //! Unique callsigns for aircraft in range
            //! \threadsafe
            virtual aviation::CCallsignSet getAircraftInRangeCallsigns() const = 0;

            //! Is aircraft in range?
            //! \threadsafe
            virtual bool isAircraftInRange(const aviation::CCallsign &callsign) const = 0;

            //! Is VTOL aircraft?
            //! \threadsafe
            virtual bool isVtolAircraft(const aviation::CCallsign &callsign) const = 0;

            //! Current snapshot
            //! \threadsafe
            virtual CAirspaceAircraftSnapshot getLatestAirspaceAircraftSnapshot() const = 0;

            //! Aircraft for callsign
            //! \threadsafe
            virtual CSimulatedAircraft getAircraftInRangeForCallsign(const aviation::CCallsign &callsign) const = 0;

            //! Aircraft model for callsign
            //! \threadsafe
            virtual CAircraftModel getAircraftInRangeModelForCallsign(const aviation::CCallsign &callsign) const = 0;

            //! Rendered aircraft situations (per callsign, time history)
            //! \threadsafe
            virtual aviation::CAircraftSituationList remoteAircraftSituations(const aviation::CCallsign &callsign) const = 0;

            //! Average update time
            //! \threadsafe
            virtual MillisecondsMinMaxMean remoteAircraftSituationsTimestampDifferenceMinMaxMean(const aviation::CCallsign &callsign) const = 0;

            //! Rendered aircraft situations (per callsign and index)
            //! \remark if situation does not exist, an NULL situation is returned
            //! \param callsign
            //! \param index 0..latest, 1..next older, ...
            //! \threadsafe
            virtual aviation::CAircraftSituation remoteAircraftSituation(const aviation::CCallsign &callsign, int index) const = 0;

            //! Latest aircraft situation for all callsigns
            //! \threadsafe
            virtual aviation::CAircraftSituationList latestRemoteAircraftSituations() const = 0;

            //! Latest aircraft situation "on ground" having a provider elevation
            //! \threadsafe
            virtual aviation::CAircraftSituationList latestOnGroundProviderElevations() const = 0;

            //! Number of remote aircraft situations for callsign
            //! \remark latest situations first
            //! \threadsafe
            virtual int remoteAircraftSituationsCount(const aviation::CCallsign &callsign) const = 0;

            //! All parts (per callsign, time history)
            //! \remark latest parts first
            //! \threadsafe
            virtual aviation::CAircraftPartsList remoteAircraftParts(const aviation::CCallsign &callsign) const = 0;

            //! All parts (per callsign, time history)
            //! \threadsafe
            virtual int remoteAircraftPartsCount(const aviation::CCallsign &callsign) const = 0;

            //! Is remote aircraft supporting parts?
            //! \threadsafe
            virtual bool isRemoteAircraftSupportingParts(const aviation::CCallsign &callsign) const = 0;

            //! Get the latest aircraft parts (if any, otherwise default)
            //! \threadsafe
            aviation::CAircraftParts getLatestAircraftParts(const aviation::CCallsign &callsign) const;

            //! Number of aircraft supporting parts
            //! \threadsafe
            virtual int getRemoteAircraftSupportingPartsCount() const = 0;

            //! Remote aircraft supporting parts.
            //! \threadsafe
            virtual aviation::CCallsignSet remoteAircraftSupportingParts() const = 0;

            //! Aircraft changes.
            //! \threadsafe
            virtual aviation::CAircraftSituationChangeList remoteAircraftSituationChanges(const aviation::CCallsign &callsign) const = 0;

            //! Aircraft changes count.
            //! \threadsafe
            virtual int remoteAircraftSituationChangesCount(const aviation::CCallsign &callsign) const = 0;

            //! Enable/disable aircraft and follow up logic like sending signals
            //! \threadsafe
            //! \remark depending on implementation similar or more sophisticated as setEnabledFlag
            virtual bool updateAircraftEnabled(const aviation::CCallsign &callsign, bool enabledForRendering) = 0;

            //! Just set enable/disable aircraft flag, no further logic
            //! \threadsafe
            virtual bool setAircraftEnabledFlag(const swift::misc::aviation::CCallsign &callsign, bool enabledForRendering) = 0;

            //! Enable/disable aircraft
            //! \threadsafe
            virtual int updateMultipleAircraftEnabled(const aviation::CCallsignSet &callsigns, bool enabledForRendering) = 0;

            //! Set aircraft rendered
            //! \threadsafe
            virtual bool updateAircraftRendered(const aviation::CCallsign &callsign, bool rendered) = 0;

            //! Set aircraft rendered
            //! \threadsafe
            virtual int updateMultipleAircraftRendered(const aviation::CCallsignSet &callsigns, bool rendered) = 0;

            //! Mark all as not rendered
            //! \threadsafe
            virtual void updateMarkAllAsNotRendered() = 0;

            //! Change model
            //! \threadsafe
            virtual bool updateAircraftModel(const aviation::CCallsign &callsign, const CAircraftModel &model, const CIdentifier &originator) = 0;

            //! Change network model
            //! \threadsafe
            virtual bool updateAircraftNetworkModel(const aviation::CCallsign &callsign, const CAircraftModel &model, const CIdentifier &originator) = 0;

            //! Change fast position updates
            //! \threadsafe
            virtual bool updateFastPositionEnabled(const aviation::CCallsign &callsign, bool enableFastPositonUpdates) = 0;

            //! Update the ground elevation
            //! \threadsafe
            virtual int updateAircraftGroundElevation(const aviation::CCallsign &callsign, const geo::CElevationPlane &elevation, aviation::CAircraftSituation::GndElevationInfo info, bool *updateAircraftGroundElevation) = 0;

            //! Update the CG
            //! \threadsafe
            virtual bool updateCG(const aviation::CCallsign &callsign, const physical_quantities::CLength &cg) = 0;

            //! Update the CG for this model string
            //! \threadsafe
            virtual aviation::CCallsignSet updateCGForModel(const QString &modelString, const physical_quantities::CLength &cg) = 0;

            //! Update the CG and model string
            //! \threadsafe
            virtual bool updateCGAndModelString(const aviation::CCallsign &callsign, const physical_quantities::CLength &cg, const QString &modelString) = 0;

            //! @{
            //! CG values from DB
            //! \threadsafe
            virtual physical_quantities::CLength getCGFromDB(const aviation::CCallsign &callsign) const = 0;
            virtual physical_quantities::CLength getCGFromDB(const QString &modelString) const = 0;
            virtual void rememberCGFromDB(const physical_quantities::CLength &cgFromDB, const aviation::CCallsign &callsign) = 0;
            virtual void rememberCGFromDB(const physical_quantities::CLength &cgFromDB, const QString &modelString) = 0;
            //! @}

            //! Get reverse lookup meesages
            //! \threadsafe
            virtual CStatusMessageList getReverseLookupMessages(const aviation::CCallsign &callsign) const = 0;

            //! Enabled reverse lookup logging?
            //! \threadsafe
            virtual ReverseLookupLogging isReverseLookupMessagesEnabled() const = 0;

            //! Enable reverse lookup logging
            //! \threadsafe
            virtual void enableReverseLookupMessages(ReverseLookupLogging enable) = 0;

            //! Get aircraft parts history
            //! \threadsafe
            virtual CStatusMessageList getAircraftPartsHistory(const aviation::CCallsign &callsign) const = 0;

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
            virtual qint64 situationsLastModified(const aviation::CCallsign &callsign) const = 0;

            //! When last modified
            //! \threadsafe
            virtual qint64 partsLastModified(const aviation::CCallsign &callsign) const = 0;

            //! Average elevation of aircraft in given range, which are NOT moving
            //! \remark can be used to anticipate field elevation
            //! \threadsafe
            virtual geo::CElevationPlane averageElevationOfNonMovingAircraft(const aviation::CAircraftSituation &reference, const physical_quantities::CLength &range, int minValues = 1, int sufficientValues = 2) const = 0;

            //! Connect signals to slot receiver. As the interface is no QObject, slots can not be connected directly.
            //! In order to disconnect a list of connections is provided, which have to be disconnected manually.
            //! \note all connections are normally Qt::QueuedConnection receiver is required for connection type
            virtual QList<QMetaObject::Connection> connectRemoteAircraftProviderSignals(
                QObject *receiver,
                std::function<void(const aviation::CAircraftSituation &)> addedSituationSlot,
                std::function<void(const aviation::CCallsign &, const aviation::CAircraftParts &)> addedPartsSlot,
                std::function<void(const aviation::CCallsign &)> removedAircraftSlot,
                std::function<void(const CAirspaceAircraftSnapshot &)> aircraftSnapshot) = 0;

            //! Remove outdated aircraft parts, but never the most recent one
            static void removeOutdatedParts(aviation::CAircraftPartsList &partsList);

            //! Wildcard callsign
            static const aviation::CCallsign &testAltitudeOffsetCallsign();

        protected:
            //! Constructor
            IRemoteAircraftProvider();
        };
    } // s
} // ns

Q_DECLARE_INTERFACE(swift::misc::simulation::IRemoteAircraftProvider, "org.swift-project.iremoteaircraftprovider")

namespace swift::misc::simulation
{
    //! Implementaion of the interface, which can also be used for testing
    class SWIFT_MISC_EXPORT CRemoteAircraftProvider :
        public QObject,
        public IRemoteAircraftProvider,
        public CIdentifiable
    {
        Q_OBJECT
        Q_INTERFACES(swift::misc::simulation::IRemoteAircraftProvider)

    public:
        //! Log categories
        static const QStringList &getLogCategories();

        //! Constructor
        CRemoteAircraftProvider(QObject *parent);

        // remoteaircraftprovider
        virtual CSimulatedAircraftList getAircraftInRange() const override;
        virtual aviation::CCallsignSet getAircraftInRangeCallsigns() const override;
        virtual CSimulatedAircraft getAircraftInRangeForCallsign(const aviation::CCallsign &callsign) const override;
        virtual CAircraftModel getAircraftInRangeModelForCallsign(const aviation::CCallsign &callsign) const override;
        virtual int getAircraftInRangeCount() const override;
        virtual bool isAircraftInRange(const aviation::CCallsign &callsign) const override;
        virtual bool isVtolAircraft(const aviation::CCallsign &callsign) const override;
        virtual aviation::CAircraftSituationList remoteAircraftSituations(const aviation::CCallsign &callsign) const override;
        virtual aviation::CAircraftSituation remoteAircraftSituation(const aviation::CCallsign &callsign, int index) const override;
        virtual MillisecondsMinMaxMean remoteAircraftSituationsTimestampDifferenceMinMaxMean(const aviation::CCallsign &callsign) const override;
        virtual aviation::CAircraftSituationList latestRemoteAircraftSituations() const override;
        virtual aviation::CAircraftSituationList latestOnGroundProviderElevations() const override;
        virtual int remoteAircraftSituationsCount(const aviation::CCallsign &callsign) const override;
        virtual aviation::CAircraftPartsList remoteAircraftParts(const aviation::CCallsign &callsign) const override;
        virtual int remoteAircraftPartsCount(const aviation::CCallsign &callsign) const override;
        virtual bool isRemoteAircraftSupportingParts(const aviation::CCallsign &callsign) const override;
        virtual int getRemoteAircraftSupportingPartsCount() const override;
        virtual aviation::CCallsignSet remoteAircraftSupportingParts() const override;
        virtual aviation::CAircraftSituationChangeList remoteAircraftSituationChanges(const aviation::CCallsign &callsign) const override;
        virtual int remoteAircraftSituationChangesCount(const aviation::CCallsign &callsign) const override;
        virtual bool updateAircraftEnabled(const aviation::CCallsign &callsign, bool enabledForRendering) override;
        virtual bool setAircraftEnabledFlag(const swift::misc::aviation::CCallsign &callsign, bool enabledForRendering) override;
        virtual int updateMultipleAircraftEnabled(const aviation::CCallsignSet &callsigns, bool enabledForRendering) override;
        virtual bool updateAircraftModel(const aviation::CCallsign &callsign, const CAircraftModel &model, const CIdentifier &originator) override;
        virtual bool updateAircraftNetworkModel(const aviation::CCallsign &callsign, const CAircraftModel &model, const CIdentifier &originator) override;
        virtual bool updateFastPositionEnabled(const aviation::CCallsign &callsign, bool enableFastPositonUpdates) override;
        virtual bool updateAircraftRendered(const aviation::CCallsign &callsign, bool rendered) override;
        virtual int updateMultipleAircraftRendered(const aviation::CCallsignSet &callsigns, bool rendered) override;
        virtual int updateAircraftGroundElevation(const aviation::CCallsign &callsign, const geo::CElevationPlane &elevation, aviation::CAircraftSituation::GndElevationInfo info, bool *setForOnGroundPosition) override;
        virtual bool updateCG(const aviation::CCallsign &callsign, const physical_quantities::CLength &cg) override;
        virtual bool updateCGAndModelString(const aviation::CCallsign &callsign, const physical_quantities::CLength &cg, const QString &modelString) override;
        virtual aviation::CCallsignSet updateCGForModel(const QString &modelString, const physical_quantities::CLength &cg) override;
        virtual physical_quantities::CLength getCGFromDB(const aviation::CCallsign &callsign) const override;
        virtual physical_quantities::CLength getCGFromDB(const QString &modelString) const override;
        virtual void rememberCGFromDB(const physical_quantities::CLength &cgFromDB, const aviation::CCallsign &callsign) override;
        virtual void rememberCGFromDB(const physical_quantities::CLength &cgFromDB, const QString &modelString) override;
        virtual void updateMarkAllAsNotRendered() override;
        virtual CStatusMessageList getAircraftPartsHistory(const aviation::CCallsign &callsign) const override;
        virtual bool isAircraftPartsHistoryEnabled() const override;
        virtual void enableAircraftPartsHistory(bool enabled) override;
        virtual int aircraftSituationsAdded() const override;
        virtual int aircraftPartsAdded() const override;
        virtual qint64 situationsLastModified(const aviation::CCallsign &callsign) const override;
        virtual qint64 partsLastModified(const aviation::CCallsign &callsign) const override;
        virtual geo::CElevationPlane averageElevationOfNonMovingAircraft(const aviation::CAircraftSituation &reference, const physical_quantities::CLength &range, int minValues = 1, int sufficientValues = 2) const override;
        virtual QList<QMetaObject::Connection> connectRemoteAircraftProviderSignals(
            QObject *receiver,
            std::function<void(const aviation::CAircraftSituation &)> addedSituationSlot,
            std::function<void(const aviation::CCallsign &, const aviation::CAircraftParts &)> addedPartsSlot,
            std::function<void(const aviation::CCallsign &)> removedAircraftSlot,
            std::function<void(const CAirspaceAircraftSnapshot &)> aircraftSnapshotSlot) override;

        virtual void enableReverseLookupMessages(ReverseLookupLogging enable) override;
        virtual ReverseLookupLogging isReverseLookupMessagesEnabled() const override;
        virtual swift::misc::CStatusMessageList getReverseLookupMessages(const swift::misc::aviation::CCallsign &callsign) const override;

        //! @{
        //! Reverse lookup messages
        //! \threadsafe
        //! \ingroup reverselookup
        void addReverseLookupMessages(const aviation::CCallsign &callsign, const CStatusMessageList &messages);
        void addReverseLookupMessage(const aviation::CCallsign &callsign, const CStatusMessage &message);
        void addReverseLookupMessage(
            const aviation::CCallsign &callsign, const QString &message,
            CStatusMessage::StatusSeverity severity = CStatusMessage::SeverityInfo);
        //! @}

        //! \copydoc swift::misc::IProvider::asQObject
        virtual QObject *asQObject() override { return this; }

        //! Clear all data
        void clear();

        // ------------------- testing ---------------

        //! Has test offset value?
        bool hasTestAltitudeOffset(const aviation::CCallsign &callsign) const;

        //! Has test offset value?
        bool hasTestAltitudeOffsetGlobalValue() const;

        //! Offset for callsign
        bool testAddAltitudeOffset(const aviation::CCallsign &callsign, const physical_quantities::CLength &offset);

    signals:
        //! A new aircraft appeared
        void addedAircraft(const swift::misc::simulation::CSimulatedAircraft &remoteAircraft);

        //! Parts added
        void addedAircraftParts(const swift::misc::aviation::CCallsign &callsign, const swift::misc::aviation::CAircraftParts &parts);

        //! Situation added
        void addedAircraftSituation(const swift::misc::aviation::CAircraftSituation &situation);

        //! Aircraft were changed
        void changedAircraftInRange();

        //! An aircraft disappeared
        void removedAircraft(const swift::misc::aviation::CCallsign &callsign);

        //! \copydoc swift::core::CAirspaceAnalyzer::airspaceAircraftSnapshot
        void airspaceAircraftSnapshot(const swift::misc::simulation::CAirspaceAircraftSnapshot &snapshot);

    protected:
        //! Remove all aircraft in range
        //! \threadsafe
        bool removeAircraft(const aviation::CCallsign &callsign);

        //! Remove all aircraft in range
        //! \threadsafe
        void removeAllAircraft();

        //! Remove the lookup messages
        //! \threadsafe
        void removeReverseLookupMessages(const aviation::CCallsign &callsign);

        //! Add new aircraft, ignored if aircraft already exists
        //! \threadsafe
        bool addNewAircraftInRange(const CSimulatedAircraft &aircraft);

        //! Init a new aircraft and add it or update model of existing aircraft
        //! \threadsafe
        CSimulatedAircraft addOrUpdateAircraftInRange(
            const aviation::CCallsign &callsign,
            const QString &aircraftIcao, const QString &airlineIcao, const QString &livery, const QString &modelString,
            CAircraftModel::ModelType modelType,
            CStatusMessageList *log = nullptr);

        //! Update aircraft
        //! \threadsafe
        int updateAircraftInRange(const aviation::CCallsign &callsign, const CPropertyIndexVariantMap &vm, bool skipEqualValues = true);

        //! Update aircraft bearing, distance and situation
        //! \threadsafe
        //! \remark does NOT emit signals
        bool updateAircraftInRangeDistanceBearing(const aviation::CCallsign &callsign, const aviation::CAircraftSituation &situation, const physical_quantities::CLength &distance, const physical_quantities::CAngle &bearing);

        //! Store an aircraft situation
        //! \remark latest situations are kept first
        //! \threadsafe
        virtual aviation::CAircraftSituation storeAircraftSituation(const aviation::CAircraftSituation &situation, bool allowTestAltitudeOffset = true);

        //! @{
        //! Store an aircraft part
        //! \remark latest parts are kept first
        //! \threadsafe
        void storeAircraftParts(const aviation::CCallsign &callsign, const aviation::CAircraftParts &parts, bool removeOutdated);
        void storeAircraftParts(const aviation::CCallsign &callsign, const QJsonObject &jsonObject, qint64 currentOffsetMs);
        //! @}

        //! Guess situation "on ground" and update model's CG if applicable
        //! \remark updates CG and ground flag in situation
        bool guessOnGroundAndUpdateModelCG(aviation::CAircraftSituation &situation, const aviation::CAircraftSituationChange &change, const CAircraftModel &aircraftModel);

        //! Add an offset for testing
        aviation::CAircraftSituation addTestAltitudeOffsetToSituation(const aviation::CAircraftSituation &situation) const;

        //! What to log?
        //! \threadsafe
        ReverseLookupLogging whatToReverseLog() const;

        //! Set ground elevation from elevation plane and guess ground
        //! \note requires a sorted list latest first
        static int setGroundElevationCheckedAndGuessGround(aviation::CAircraftSituationList &situations, const geo::CElevationPlane &elevationPlane, aviation::CAircraftSituation::GndElevationInfo info, const simulation::CAircraftModel &model, aviation::CAircraftSituationChange *changeOut, bool *setForOnGroundPosition);

    private:
        //! Store the latest changes
        //! \remark latest first
        //! \threadsafe
        void storeChange(const aviation::CAircraftSituationChange &change);

        aviation::CAircraftSituationListPerCallsign m_situationsByCallsign; //!< situations, for performance reasons per callsign, thread safe access required
        aviation::CAircraftSituationPerCallsign m_latestSituationByCallsign; //!< latest situations, for performance reasons per callsign, thread safe access required
        aviation::CAircraftSituationPerCallsign m_latestOnGroundProviderElevation; //!< situations on ground with elevation from provider
        aviation::CAircraftPartsListPerCallsign m_partsByCallsign; //!< parts, for performance reasons per callsign, thread safe access required
        aviation::CAircraftSituationChangeListPerCallsign m_changesByCallsign; //!< changes, for performance reasons per callsign, thread safe access required (same timestamps as corresponding situations)
        aviation::CCallsignSet m_aircraftWithParts; //!< aircraft supporting parts, thread safe access required
        int m_situationsAdded = 0; //!< total number of situations added, thread safe access required
        int m_partsAdded = 0; //!< total number of parts added, thread safe access required

        ReverseLookupLogging m_enableReverseLookupMsgs = RevLogSimplifiedInfo; //!< shall we log. information about the matching process
        simulation::CSimulatedAircraftPerCallsign m_aircraftInRange; //!< aircraft, thread safe access required
        aviation::CStatusMessageListPerCallsign m_reverseLookupMessages; //!< reverse lookup messages
        aviation::CStatusMessageListPerCallsign m_aircraftPartsMessages; //!< status messages for parts history
        aviation::CTimestampPerCallsign m_situationsLastModified; //!< when situations last modified
        aviation::CTimestampPerCallsign m_partsLastModified; //!< when parts last modified
        aviation::CLengthPerCallsign m_testOffset; //!< offsets
        aviation::CLengthPerCallsign m_dbCGPerCallsign; //!< DB CG per callsign
        QHash<QString, physical_quantities::CLength> m_dbCGPerModelString; //!< DB CG per model string

        bool m_enableAircraftPartsHistory = true; //!< shall we keep a history of aircraft parts

        // locks
        mutable QReadWriteLock m_lockSituations; //!< lock for situations: m_situationsByCallsign
        mutable QReadWriteLock m_lockParts; //!< lock for parts: m_partsByCallsign, m_aircraftSupportingParts
        mutable QReadWriteLock m_lockChanges; //!< lock for changes: m_changesByCallsign
        mutable QReadWriteLock m_lockAircraft; //!< lock aircraft: m_aircraftInRange, m_dbCGPerCallsign
        mutable QReadWriteLock m_lockMessages; //!< lock for messages
        mutable QReadWriteLock m_lockPartsHistory; //!< lock for aircraft parts
    };

    //! Class which can be directly used to access an \sa IRemoteAircraftProvider object
    class SWIFT_MISC_EXPORT CRemoteAircraftAware : public IProviderAware<IRemoteAircraftProvider>
    {
    public:
        //! Destructor
        virtual ~CRemoteAircraftAware() override;

        //! \copydoc IRemoteAircraftProvider::getAircraftInRange
        CSimulatedAircraftList getAircraftInRange() const;

        //! \copydoc IRemoteAircraftProvider::isAircraftInRange
        bool isAircraftInRange(const aviation::CCallsign &callsign) const;

        //! \copydoc IRemoteAircraftProvider::isVtolAircraft
        bool isVtolAircraft(const aviation::CCallsign &callsign) const;

        //! \copydoc IRemoteAircraftProvider::getAircraftInRangeCount
        int getAircraftInRangeCount() const;

        //! \copydoc IRemoteAircraftProvider::getAircraftInRangeCallsigns
        aviation::CCallsignSet getAircraftInRangeCallsigns() const;

        //! \copydoc IRemoteAircraftProvider::getAircraftInRangeForCallsign
        CSimulatedAircraft getAircraftInRangeForCallsign(const aviation::CCallsign &callsign) const;

        //! \copydoc IRemoteAircraftProvider::getAircraftInRangeModelForCallsign
        CAircraftModel getAircraftInRangeModelForCallsign(const aviation::CCallsign &callsign) const;

        //! \copydoc IRemoteAircraftProvider::getLatestAirspaceAircraftSnapshot
        CAirspaceAircraftSnapshot getLatestAirspaceAircraftSnapshot() const;

        //! \copydoc IRemoteAircraftProvider::remoteAircraftSituations
        aviation::CAircraftSituationList remoteAircraftSituations(const aviation::CCallsign &callsign) const;

        //! \copydoc IRemoteAircraftProvider::remoteAircraftSituation
        aviation::CAircraftSituation remoteAircraftSituation(const aviation::CCallsign &callsign, int index) const;

        //! \copydoc IRemoteAircraftProvider::latestRemoteAircraftSituations
        aviation::CAircraftSituationList latestRemoteAircraftSituations() const;

        //! \copydoc IRemoteAircraftProvider::latestOnGroundProviderElevations
        aviation::CAircraftSituationList latestOnGroundProviderElevations() const;

        //! \copydoc IRemoteAircraftProvider::remoteAircraftSituationsCount
        int remoteAircraftSituationsCount(const aviation::CCallsign &callsign) const;

        //! \copydoc IRemoteAircraftProvider::remoteAircraftParts
        aviation::CAircraftPartsList remoteAircraftParts(const aviation::CCallsign &callsign) const;

        //! \copydoc IRemoteAircraftProvider::remoteAircraftPartsCount
        int remoteAircraftPartsCount(const aviation::CCallsign &callsign) const;

        //! \copydoc IRemoteAircraftProvider::remoteAircraftSituationChanges
        aviation::CAircraftSituationChangeList remoteAircraftSituationChanges(const aviation::CCallsign &callsign) const;

        //! \copydoc IRemoteAircraftProvider::remoteAircraftSupportingParts
        aviation::CCallsignSet remoteAircraftSupportingParts() const;

        //! \copydoc IRemoteAircraftProvider::isRemoteAircraftSupportingParts
        bool isRemoteAircraftSupportingParts(const aviation::CCallsign &callsign) const;

        //! \copydoc IRemoteAircraftProvider::getRemoteAircraftSupportingPartsCount
        int getRemoteAircraftSupportingPartsCount() const;

        //! \copydoc IRemoteAircraftProvider::updateAircraftEnabled
        bool updateAircraftEnabled(const aviation::CCallsign &callsign, bool enabledForRendering);

        //! \copydoc IRemoteAircraftProvider::setAircraftEnabledFlag
        bool setAircraftEnabledFlag(const aviation::CCallsign &callsign, bool enabledForRendering);

        //! \copydoc IRemoteAircraftProvider::updateMultipleAircraftEnabled
        bool updateMultipleAircraftEnabled(const aviation::CCallsignSet &callsigns, bool enabledForRendering);

        //! \copydoc IRemoteAircraftProvider::updateAircraftModel
        bool updateAircraftModel(const aviation::CCallsign &callsign, const CAircraftModel &model, const CIdentifier &originator);

        //! \copydoc IRemoteAircraftProvider::updateAircraftNetworkModel
        bool updateAircraftNetworkModel(const aviation::CCallsign &callsign, const CAircraftModel &model, const CIdentifier &originator);

        //! \copydoc IRemoteAircraftProvider::updateAircraftRendered
        bool updateAircraftRendered(const aviation::CCallsign &callsign, bool rendered);

        //! \copydoc IRemoteAircraftProvider::updateMultipleAircraftRendered
        bool updateMultipleAircraftRendered(const aviation::CCallsignSet &callsigns, bool rendered);

        //! \copydoc IRemoteAircraftProvider::updateAircraftGroundElevation
        int updateAircraftGroundElevation(const aviation::CCallsign &callsign, const geo::CElevationPlane &elevation, aviation::CAircraftSituation::GndElevationInfo info, bool *updateAircraftGroundElevation);

        //! \copydoc IRemoteAircraftProvider::updateCG
        bool updateCG(const aviation::CCallsign &callsign, const physical_quantities::CLength &cg);

        //! \copydoc IRemoteAircraftProvider::updateCGForModel
        aviation::CCallsignSet updateCGForModel(const QString &modelString, const physical_quantities::CLength &cg);

        //! \copydoc IRemoteAircraftProvider::updateCGAndModelString
        bool updateCGAndModelString(const aviation::CCallsign &callsign, const physical_quantities::CLength &cg, const QString &modelString);

        //! \copydoc IRemoteAircraftProvider::updateMarkAllAsNotRendered
        void updateMarkAllAsNotRendered();

        //! \copydoc IRemoteAircraftProvider::aircraftSituationsAdded
        int aircraftSituationsAdded() const;

        //! \copydoc IRemoteAircraftProvider::aircraftPartsAdded
        int aircraftPartsAdded() const;

        //! \copydoc IRemoteAircraftProvider::situationsLastModified
        qint64 situationsLastModified(const aviation::CCallsign &callsign) const;

        //! \copydoc IRemoteAircraftProvider::partsLastModified
        qint64 partsLastModified(const aviation::CCallsign &callsign) const;

        //! \copydoc IRemoteAircraftProvider::averageElevationOfNonMovingAircraft
        geo::CElevationPlane averageElevationOfNonMovingAircraft(const aviation::CAircraftSituation &reference, const physical_quantities::CLength &range, int minValues = 1) const;

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

#endif // guard
