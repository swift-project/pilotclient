// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_DB_BACKGROUNDDATAUPDATER_H
#define SWIFT_CORE_DB_BACKGROUNDDATAUPDATER_H

#include "core/data/dbcaches.h"
#include "core/swiftcoreexport.h"
#include "misc/simulation/data/modelcaches.h"
#include "misc/network/entityflags.h"
#include "misc/statusmessagelist.h"
#include "misc/worker.h"

#include <QMap>
#include <QReadWriteLock>
#include <atomic>

namespace swift::core::db
{
    //! Update and consolidation of DB data
    class SWIFT_CORE_EXPORT CBackgroundDataUpdater : public swift::misc::CContinuousWorker
    {
        Q_OBJECT

    public:
        //! Log categories
        static const QStringList &getLogCategories();

        //! Constructor
        CBackgroundDataUpdater(QObject *owner);

        //! The message history
        //! \threadsafe
        swift::misc::CStatusMessageList getMessageHistory() const;

    signals:
        //! Consolidation
        void consolidating(bool running);

    private:
        mutable QReadWriteLock m_lockMsg; //!< lock history messages
        std::atomic_int m_cycle { 0 }; //!< cycle
        std::atomic_bool m_inWork { false }; //!< indicates a running update
        std::atomic_bool m_updatePublishedModels { true }; //!< update when models have been updated
        QMap<QString, QDateTime> m_syncedModelsLatestChange; //! timestamp per cache when last synced
        swift::misc::CStatusMessageList m_messageHistory;

        // set/caches as member as we are in own thread, central instance will not work
        swift::misc::simulation::data::CModelCaches m_modelCaches { false, this };
        swift::misc::simulation::data::CModelSetCaches m_modelSets { false, this };

        //! Do the update checks
        void doWork();

        //! Read of new DB data
        void triggerInfoReads();

        //! Sync the model cache, normally model set or simulator models cache
        //! \param modelSetFlag true means model set, false means model cach
        //! \param dbModelsConsidered if no DB models are passed all DB models are used
        void syncModelOrModelSetCacheWithDbData(bool modelSetFlag, const swift::misc::simulation::CAircraftModelList &dbModelsConsidered = {});

        //! Sync DB entity
        void syncDbEntity(swift::misc::network::CEntityFlags::Entity entity);

        //! Still enabled etc.
        bool doWorkCheck() const;

        //! Models have been published
        void onModelsPublished(const swift::misc::simulation::CAircraftModelList &modelsPublished, bool directWrite);

        //! Model or model set instance
        swift::misc::simulation::data::IMultiSimulatorModelCaches &modelCaches(bool modelSetFlag);

        //! Add history message
        //! \threadsafe
        void addHistory(const swift::misc::CStatusMessage &msg);
    };
} // ns
#endif // guard
