// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_FSCOMMON_VPILOTRULESREADER_H
#define SWIFT_MISC_SIMULATION_FSCOMMON_VPILOTRULESREADER_H

#include <QObject>
#include <QReadWriteLock>
#include <QString>
#include <QStringList>

#include "misc/datacache.h"
#include "misc/simulation/aircraftmodellist.h"
#include "misc/simulation/data/modelcaches.h"
#include "misc/simulation/fscommon/vpilotmodelruleset.h"
#include "misc/swiftmiscexport.h"

namespace swift::misc
{
    class CWorker;

    namespace simulation::fscommon
    {
        //! Model mappings
        //! \deprecated vPilot rules might be removed in future
        class SWIFT_MISC_EXPORT CVPilotRulesReader : public QObject
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
            swift::misc::simulation::CAircraftModelList getAsModels();

            //! Get as models from cache
            //! \threadsafe
            swift::misc::simulation::CAircraftModelList getAsModelsFromCache() const;

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
            swift::misc::CWorker *readInBackground(bool convertToModels);

        private slots:
            //! Asyncronous read finished
            //! \threadsafe
            void ps_readInBackgroundFinished();

            //! Cache changed
            void ps_onVPilotCacheChanged();

            //! Set cache (in main thread)
            //! \threadsafe
            void ps_setCache(const swift::misc::simulation::CAircraftModelList &models);

        private:
            QStringList m_fileList; //!< list of file names
            QStringList m_fileListWithProblems; //!< problems during parsing
            int m_loadedFiles = 0; //!< loaded files
            CVPilotModelRuleSet m_rules; //!< rules list
            bool m_asyncLoadInProgress = false; //!< Asynchronous load in progress
            bool m_shutdown = false; //!< Shutdown
            swift::misc::CData<data::TVPilotAircraftModels> m_cachedVPilotModels {
                this, &CVPilotRulesReader::ps_onVPilotCacheChanged
            }; //!< cache for latest vPilot rules
            mutable QReadWriteLock m_lockData;

            //! Read single file and do parsing
            //! \threadsafe
            bool loadFile(const QString &fileName, CVPilotModelRuleSet &ruleSet);
        };
    } // namespace simulation::fscommon
} // namespace swift::misc

#endif // SWIFT_MISC_SIMULATION_FSCOMMON_VPILOTRULESREADER_H
