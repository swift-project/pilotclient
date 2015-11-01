/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_FSCOMMON_VPILOTRULESREADER_H
#define BLACKMISC_SIMULATION_FSCOMMON_VPILOTRULESREADER_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/worker.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/fscommon/vpilotmodelruleset.h"
#include <QStringList>
#include <QObject>
#include <QReadWriteLock>

namespace BlackMisc
{
    namespace Simulation
    {
        namespace FsCommon
        {
            //! Model mappings
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
                BlackMisc::Simulation::CAircraftModelList getAsModels() const;

                //! Get model count
                //! \threadsafe
                int getModelsCount() const;

                //! Loaded rules
                //! \threadsafe
                int countRulesLoaded() const;

                //! Graceful shutdown
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
                BlackMisc::CWorker *readASync(bool convertToModels);

            private slots:
                //! Asyncronous read finished
                void ps_readASyncFinished();

            private:
                QStringList m_fileList;              //!< list of file names
                QStringList m_fileListWithProblems;  //!< problems during parsing
                int m_loadedFiles = 0;               //!< loaded files
                CVPilotModelRuleSet m_rules;         //!< rules list
                bool m_asyncLoadInProgress = false;  //!< Asynchronous load in progress
                bool m_shutdown            = false;  //!< Shutdown
                mutable BlackMisc::Simulation::CAircraftModelList m_models; //!< converted to models
                mutable QReadWriteLock m_lockData;

                //! Read single file and do parsing
                //! \threadsafe
                bool loadFile(const QString &fileName, CVPilotModelRuleSet &ruleSet);

            };
        } // namespace
    } // namespace
} // namespace

#endif // guard
