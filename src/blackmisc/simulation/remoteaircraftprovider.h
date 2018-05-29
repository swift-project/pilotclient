/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_REMOTEAIRCRAFTPROVIDER_H
#define BLACKMISC_SIMULATION_REMOTEAIRCRAFTPROVIDER_H

#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/airspaceaircraftsnapshot.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/aviation/aircraftpartslist.h"
#include "blackmisc/aviation/aircraftsituationlist.h"
#include "blackmisc/aviation/aircraftsituationchange.h"
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
            static constexpr int MaxSituationsPerCallsign = 6;    //!< How many situations we keep per callsign
            static constexpr int MaxPartsPerCallsign = 20;        //!< How many parts we keep per callsign (we keep more parts than situations as parts can just come in)
            static constexpr int MaxPartsAgePerCallsignSecs = 60; //!< How many seconds to keep parts for interpolation
            static constexpr int DefaultOffsetTimeMs = 6000;      //!< \fixme copied from CNetworkVatlib::c_positionTimeOffsetMsec

            //! Situations per callsign
            using CSituationsPerCallsign = QHash<Aviation::CCallsign, Aviation::CAircraftSituationList>;

            //! Parts per callsign
            using CPartsPerCallsign = QHash<Aviation::CCallsign, Aviation::CAircraftPartsList>;

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

            //! Number of remote aircraft situations for callsign
            //! \remark latest situations first
            //! \threadsafe
            virtual int remoteAircraftSituationsCount(const Aviation::CCallsign &callsign) const = 0;

            //! All parts (per callsign, time history)
            //! \remark latest parts first
            //! \threadsafe
            virtual Aviation::CAircraftPartsList remoteAircraftParts(const Aviation::CCallsign &callsign, qint64 cutoffTimeBefore = -1) const = 0;

            //! All parts (per callsign, time history)
            //! \threadsafe
            virtual int remoteAircraftPartsCount(const Aviation::CCallsign &callsign, qint64 cutoffTimeBefore = -1) const = 0;

            //! Get the change object for callsign
            //! \threadsafe
            Aviation::CAircraftSituationChange remoteAircraftSituationChange(const Aviation::CCallsign &callsign) const;

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

            //! Enable/disable enabled aircraft
            //! \threadsafe
            virtual bool updateAircraftEnabled(const Aviation::CCallsign &callsign, bool enabledForRendering) = 0;

            //! Aircraft rendered?
            //! \threadsafe
            virtual bool updateAircraftRendered(const Aviation::CCallsign &callsign, bool rendered) = 0;

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
            virtual int updateAircraftGroundElevation(const Aviation::CCallsign &callsign, const Geo::CElevationPlane &elevation) = 0;

            //! Update the CG
            //! \threadsafe
            virtual bool updateCG(const Aviation::CCallsign &callsign, const PhysicalQuantities::CLength &cg) = 0;

            //! Get reverse lookup meesages
            //! \threadsafe
            virtual CStatusMessageList getReverseLookupMessages(const Aviation::CCallsign &callsign) const = 0;

            //! Enabled reverse lookup logging?
            //! \threadsafe
            virtual bool isReverseLookupMessagesEnabled() const = 0;

            //! Enable reverse lookup logging
            //! \threadsafe
            virtual void enableReverseLookupMessages(bool enabled) = 0;

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

            //! Destructor
            virtual ~IRemoteAircraftProvider() {}

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
            virtual int remoteAircraftSituationsCount(const Aviation::CCallsign &callsign) const override;
            virtual Aviation::CAircraftPartsList remoteAircraftParts(const Aviation::CCallsign &callsign, qint64 cutoffTimeValuesBefore = -1) const override;
            virtual int remoteAircraftPartsCount(const Aviation::CCallsign &callsign, qint64 cutoffTimeValuesBefore = -1) const override;
            virtual bool isRemoteAircraftSupportingParts(const Aviation::CCallsign &callsign) const override;
            virtual int getRemoteAircraftSupportingPartsCount() const override;
            virtual Aviation::CCallsignSet remoteAircraftSupportingParts() const override;
            virtual bool updateAircraftEnabled(const Aviation::CCallsign &callsign, bool enabledForRendering) override;
            virtual bool updateAircraftModel(const Aviation::CCallsign &callsign, const CAircraftModel &model, const CIdentifier &originator) override;
            virtual bool updateAircraftNetworkModel(const Aviation::CCallsign &callsign, const CAircraftModel &model, const CIdentifier &originator) override;
            virtual bool updateFastPositionEnabled(const Aviation::CCallsign &callsign, bool enableFastPositonUpdates) override;
            virtual bool updateAircraftRendered(const Aviation::CCallsign &callsign, bool rendered) override;
            virtual int updateAircraftGroundElevation(const Aviation::CCallsign &callsign, const Geo::CElevationPlane &elevation) override;
            virtual bool updateCG(const Aviation::CCallsign &callsign, const PhysicalQuantities::CLength &cg) override;
            virtual void updateMarkAllAsNotRendered() override;
            virtual CStatusMessageList getAircraftPartsHistory(const Aviation::CCallsign &callsign) const override;
            virtual bool isAircraftPartsHistoryEnabled() const override;
            virtual void enableAircraftPartsHistory(bool enabled) override;
            virtual int aircraftSituationsAdded() const override;
            virtual int aircraftPartsAdded() const override;
            virtual qint64 situationsLastModified(const Aviation::CCallsign &callsign) const override;
            virtual qint64 partsLastModified(const Aviation::CCallsign &callsign) const override;
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
            virtual void enableReverseLookupMessages(bool enabled) override;
            virtual bool isReverseLookupMessagesEnabled() const override;
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

            //! Store an aircraft situation
            //! \remark latest situations are kept first
            //! \threadsafe
            void storeAircraftSituation(const Aviation::CAircraftSituation &situation);

            //! Store an aircraft part
            //! \remark latest parts are kept first
            //! \threadsafe
            //! @{
            void storeAircraftParts(const Aviation::CCallsign &callsign, const Aviation::CAircraftParts &parts, bool removeOutdated);
            void storeAircraftParts(const Aviation::CCallsign &callsign, const QJsonObject &jsonObject, int currentOffset);
            //! @}

            //! Add an offset for testing
            Aviation::CAircraftSituation testAddAltitudeOffsetToSituation(const Aviation::CAircraftSituation &situation) const;

        private:
            // hashs, because not sorted by key but keeping order
            CSituationsPerCallsign m_situationsByCallsign; //!< situations, for performance reasons per callsign, thread safe access required
            CPartsPerCallsign      m_partsByCallsign;      //!< parts, for performance reasons per callsign, thread safe access required
            Aviation::CCallsignSet m_aircraftWithParts;    //!< aircraft supporting parts, thread safe access required
            int m_situationsAdded = 0; //!< total number of situations added, thread safe access required
            int m_partsAdded = 0;      //!< total number of parts added, thread safe access required

            CSimulatedAircraftList m_aircraftInRange; //!< aircraft, thread safe access required
            QMap<Aviation::CCallsign, CStatusMessageList> m_reverseLookupMessages;
            QMap<Aviation::CCallsign, CStatusMessageList> m_aircraftPartsHistory; //!< JSON aircraft parts history
            QMap<Aviation::CCallsign, qint64> m_situationsLastModified;
            QMap<Aviation::CCallsign, qint64> m_partsLastModified;
            QMap<Aviation::CCallsign, PhysicalQuantities::CLength> m_testOffset;

            bool m_enableReverseLookupMsgs = false;   //!< shall we log. information about the matching process
            bool m_enableAircraftPartsHistory = true; //!< shall we keep a history of aircraft parts

            // locks
            mutable QReadWriteLock m_lockSituations;   //!< lock for situations: m_situationsByCallsign
            mutable QReadWriteLock m_lockParts;        //!< lock for parts: m_partsByCallsign, m_aircraftSupportingParts
            mutable QReadWriteLock m_lockAircraft;     //!< lock aircraft: m_aircraftInRange
            mutable QReadWriteLock m_lockMessages;     //!< lock for messages
            mutable QReadWriteLock m_lockPartsHistory; //!< lock for aircraft parts
        };

        //! Class which can be directly used to access an \sa IRemoteAircraftProvider object
        class BLACKMISC_EXPORT CRemoteAircraftAware : public IProviderAware<IRemoteAircraftProvider>
        {
        public:
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

            //! \copydoc IRemoteAircraftProvider::remoteAircraftSituationsCount
            int remoteAircraftSituationsCount(const Aviation::CCallsign &callsign) const;

            //! \copydoc IRemoteAircraftProvider::remoteAircraftParts
            Aviation::CAircraftPartsList remoteAircraftParts(const Aviation::CCallsign &callsign, qint64 cutoffTimeBefore = -1) const;

            //! \copydoc IRemoteAircraftProvider::remoteAircraftPartsCount
            int remoteAircraftPartsCount(const Aviation::CCallsign &callsign, qint64 cutoffTimeBefore = -1) const;

            //! \copydoc IRemoteAircraftProvider::remoteAircraftSituationChange
            Aviation::CAircraftSituationChange remoteAircraftSituationChange(const Aviation::CCallsign &callsign) const;

            //! \copydoc IRemoteAircraftProvider::remoteAircraftSupportingParts
            Aviation::CCallsignSet remoteAircraftSupportingParts() const;

            //! \copydoc IRemoteAircraftProvider::isRemoteAircraftSupportingParts
            bool isRemoteAircraftSupportingParts(const Aviation::CCallsign &callsign) const;

            //! \copydoc IRemoteAircraftProvider::getRemoteAircraftSupportingPartsCount
            int getRemoteAircraftSupportingPartsCount() const;

            //! \copydoc IRemoteAircraftProvider::updateAircraftEnabled
            bool updateAircraftEnabled(const Aviation::CCallsign &callsign, bool enabledForRedering);

            //! \copydoc IRemoteAircraftProvider::updateAircraftModel
            bool updateAircraftModel(const Aviation::CCallsign &callsign, const CAircraftModel &model, const CIdentifier &originator);

            //! \copydoc IRemoteAircraftProvider::updateAircraftNetworkModel
            bool updateAircraftNetworkModel(const Aviation::CCallsign &callsign, const CAircraftModel &model, const CIdentifier &originator);

            //! \copydoc IRemoteAircraftProvider::updateAircraftRendered
            bool updateAircraftRendered(const Aviation::CCallsign &callsign, bool rendered);

            //! \copydoc IRemoteAircraftProvider::updateAircraftGroundElevation
            int updateAircraftGroundElevation(const Aviation::CCallsign &callsign, const Geo::CElevationPlane &elevation);

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
