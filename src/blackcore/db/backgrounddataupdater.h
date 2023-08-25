// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_DB_BACKGROUNDDATAUPDATER_H
#define BLACKCORE_DB_BACKGROUNDDATAUPDATER_H

#include "blackcore/data/dbcaches.h"
#include "blackcore/blackcoreexport.h"
#include "blackmisc/simulation/data/modelcaches.h"
#include "blackmisc/network/entityflags.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/worker.h"

#include <QMap>
#include <QReadWriteLock>
#include <atomic>

namespace BlackCore::Db
{
    //! Update and consolidation of DB data
    class BLACKCORE_EXPORT CBackgroundDataUpdater : public BlackMisc::CContinuousWorker
    {
        Q_OBJECT

    public:
        //! Log categories
        static const QStringList &getLogCategories();

        //! Constructor
        CBackgroundDataUpdater(QObject *owner);

        //! The message history
        //! \threadsafe
        BlackMisc::CStatusMessageList getMessageHistory() const;

    signals:
        //! Consolidation
        void consolidating(bool running);

    private:
        mutable QReadWriteLock m_lockMsg; //!< lock history messages
        std::atomic_int m_cycle { 0 }; //!< cycle
        std::atomic_bool m_inWork { false }; //!< indicates a running update
        std::atomic_bool m_updatePublishedModels { true }; //!< update when models have been updated
        QMap<QString, QDateTime> m_syncedModelsLatestChange; //! timestamp per cache when last synced
        BlackMisc::CStatusMessageList m_messageHistory;

        // set/caches as member as we are in own thread, central instance will not work
        BlackMisc::Simulation::Data::CModelCaches m_modelCaches { false, this };
        BlackMisc::Simulation::Data::CModelSetCaches m_modelSets { false, this };

        //! Do the update checks
        void doWork();

        //! Read of new DB data
        void triggerInfoReads();

        //! Sync the model cache, normally model set or simulator models cache
        //! \param modelSetFlag true means model set, false means model cach
        //! \param dbModelsConsidered if no DB models are passed all DB models are used
        void syncModelOrModelSetCacheWithDbData(bool modelSetFlag, const BlackMisc::Simulation::CAircraftModelList &dbModelsConsidered = {});

        //! Sync DB entity
        void syncDbEntity(BlackMisc::Network::CEntityFlags::Entity entity);

        //! Still enabled etc.
        bool doWorkCheck() const;

        //! Models have been published
        void onModelsPublished(const BlackMisc::Simulation::CAircraftModelList &modelsPublished, bool directWrite);

        //! Model or model set instance
        BlackMisc::Simulation::Data::IMultiSimulatorModelCaches &modelCaches(bool modelSetFlag);

        //! Add history message
        //! \threadsafe
        void addHistory(const BlackMisc::CStatusMessage &msg);
    };
} // ns
#endif // guard
