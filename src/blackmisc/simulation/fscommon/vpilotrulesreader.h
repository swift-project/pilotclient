/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_FSCOMMON_VPILOTRULESREADER_H
#define BLACKMISC_SIMULATION_FSCOMMON_VPILOTRULESREADER_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/datacache.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/data/modelcaches.h"
#include "blackmisc/simulation/fscommon/vpilotmodelruleset.h"

#include <QObject>
#include <QReadWriteLock>
#include <QString>
#include <QStringList>

namespace BlackMisc
{
    class CWorker;

    namespace Simulation::FsCommon
    {
        //! Model mappings
        //! \deprecated vPilot rules might be removed in future
        class BLACKMISC_EXPORT CVPilotRulesReader : public QObject
        {
            Q_OBJECT

        public:
            //! Constructor
            CVPilotRulesReader(bool standardDirectory = true, QObject *parent = nullptr);

            //! Destructor
            virtual ~CVPilotRulesReader();

            //! Files
            //! \threadsafe
            QStringList getFiles() const;

            //! Has files
            //! \threadsafe
            bool hasFiles() const;

            //! File names
            //! \threadsafe
            void addFilename(const QString &fileName);

            //! Directory with .vmr files
            //! \threadsafe
            void addDirectory(const QString &directory);

            //! Loaded files (number)
            //! \threadsafe
            int countFilesLoaded() const;

            //! Loaded rules
            //! \threadsafe
            CVPilotModelRuleSet getRules() const;

            //! Get as models
            //! \threadsafe
            BlackMisc::Simulation::CAircraftModelList getAsModels();

            //! Get as models from cache
            //! \threadsafe
            BlackMisc::Simulation::CAircraftModelList getAsModelsFromCache() const;

            //! Get model count
            //! \threadsafe
            int getModelsCount() const;

            //! Loaded rules
            //! \threadsafe
            int countRulesLoaded() const;

            //! Graceful shutdown
            //! \threadsafe
            void gracefulShutdown();

            //! The standard directory for vPilot mappings
            static const QString &standardMappingsDirectory();

        signals:
            //! Rules read
            void readFinished(bool success);

        public slots:
            //! Load data
            //! \threadsafe
            bool read(bool convertToModels);

            //! Load data in background thread
            //! \threadsafe
            BlackMisc::CWorker *readInBackground(bool convertToModels);

        private slots:
            //! Asyncronous read finished
            //! \threadsafe
            void ps_readInBackgroundFinished();

            //! Cache changed
            void ps_onVPilotCacheChanged();

            //! Set cache (in main thread)
            //! \threadsafe
            void ps_setCache(const BlackMisc::Simulation::CAircraftModelList &models);

        private:
            QStringList m_fileList; //!< list of file names
            QStringList m_fileListWithProblems; //!< problems during parsing
            int m_loadedFiles = 0; //!< loaded files
            CVPilotModelRuleSet m_rules; //!< rules list
            bool m_asyncLoadInProgress = false; //!< Asynchronous load in progress
            bool m_shutdown = false; //!< Shutdown
            BlackMisc::CData<BlackMisc::Simulation::Data::TVPilotAircraftModels> m_cachedVPilotModels { this, &CVPilotRulesReader::ps_onVPilotCacheChanged }; //!< cache for latest vPilot rules
            mutable QReadWriteLock m_lockData;

            //! Read single file and do parsing
            //! \threadsafe
            bool loadFile(const QString &fileName, CVPilotModelRuleSet &ruleSet);
        };
    } // namespace
} // namespace

#endif // guard
