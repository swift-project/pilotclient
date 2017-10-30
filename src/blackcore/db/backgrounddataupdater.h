/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_DB_BACKGROUNDDATAUPDATER_H
#define BLACKCORE_DB_BACKGROUNDDATAUPDATER_H

#include "blackcore/data/dbcaches.h"
#include "blackcore/blackcoreexport.h"
#include "blackmisc/simulation/data/modelcaches.h"
#include "blackmisc/network/entityflags.h"
#include "blackmisc/worker.h"
#include <QMap>

namespace BlackCore
{
    namespace Db
    {
        /**
         * Update and consolidation of DB data
         */
        class BLACKCORE_EXPORT CBackgroundDataUpdater : public BlackMisc::CContinuousWorker
        {
            Q_OBJECT

        public:
            //! Log categories
            static const BlackMisc::CLogCategoryList &getLogCategories();

            //! Constructor
            CBackgroundDataUpdater(QObject *owner);

        signals:
            //! Consolidation
            void consolidating(bool running);

        private:
            int  m_cycle = 0;      //!< cycle
            bool m_inWork = false; //!< indicates a running update
            bool m_updatePublishedModels = true; //!< update when models have been updated

            BlackMisc::Simulation::Data::CModelCaches    m_modelCaches { false, this };    //!< caches
            BlackMisc::Simulation::Data::CModelSetCaches m_modelSetCaches { false, this }; //!< caches
            QMap<QString, QDateTime> m_syncedModelsLatestChange; //! timestamp per cache

            //! Do the update checks
            void doWork();

            //! Read of new DB data
            void triggerInfoReads();

            //! Sync the model cache, normally model set or simulator models cache
            void syncModelOrModelSetCacheWithDbData(BlackMisc::Simulation::Data::IMultiSimulatorModelCaches &cache,
                                                    const BlackMisc::Simulation::CAircraftModelList &dbModelsConsidered = {});

            //! Sync DB entity
            void syncDbEntity(BlackMisc::Network::CEntityFlags::Entity entity) const;

            //! Still enabled etc.
            bool doWorkCheck() const;

            //! Models have been published
            void onModelsPublished(const BlackMisc::Simulation::CAircraftModelList &modelsPublished, bool directWrite);
        };
    } // ns
} // ns
#endif // guard
