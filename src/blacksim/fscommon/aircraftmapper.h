/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIM_FSCOMMON_AIRCRAFTMAPPER_H
#define BLACKSIM_FSCOMMON_AIRCRAFTMAPPER_H

#include "aircraftcfgentrieslist.h"
#include "../simulatormodelmappings.h"
#include <QObject>
#include <QScopedPointer>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
#include <QReadWriteLock>
#include <memory>

namespace BlackSim
{
    namespace FsCommon
    {
        //! Mapper for all models (works for FS9/FSX).
        //! \details Reads all the mapping rules and all the available flight simulator models.
        //!          Then all rules for models not existing are eliminated ( \sa synchronize ).
        //!          Thereafter all existing models and mappings can be obtained from here.
        //! \sa CAircraftCfgEntries
        //! \sa CAircraftCfgEntriesList
        class CAircraftMapper : public QObject
        {
            Q_OBJECT

        signals:
            //! Number of entries read
            void entriesRead(int number);

            //! Full init completed
            void initCompleted(bool success);

        public:
            //! Default constructor
            CAircraftMapper(QObject *parent = nullptr);

            //! Constructor, handing over ownership or mappings object
            CAircraftMapper(std::unique_ptr<ISimulatorModelMappings> mappings, QObject *parent = nullptr);

            //! Constructor, handing over ownership or mappings object
            CAircraftMapper(std::unique_ptr<ISimulatorModelMappings> mappings, const QString &simObjectsDir, QObject *parent = nullptr);

            //! Destructor
            ~CAircraftMapper();

            //! Completely init
            bool initCompletely(QString simObjectDir);

            //! Read for directory or re-read
            int readSimObjects(const QString &simObjectDir = "");

            //! Read in background
            QFuture<int> &readInBackground(const QString &simObjectDir = "");

            //! Init in background
            QFuture<bool> &initCompletelyInBackground(const QString &simObjectDir = "");

            //! Running in background
            bool isRunningInBackground() const;

            //! Init completed?
            bool isInitialized() const;

            //! Get all aircraft entries (aka models available)
            const CAircraftCfgEntriesList &getAircraftCfgEntriesList() const { return m_entries; }

            //! Get all mappings
            const BlackMisc::Network::CAircraftMappingList &getAircraftMappingList() const { return m_mappings->getMappingList(); }

            //! Number of aircraft entries
            int countAircraftCfgEntries() const { return m_entries.size(); }

            //! Number of mapping definitions
            int countMappingRules() const { return m_mappings ? m_mappings->size() : 0; }

            //! Set the directory
            bool changeCAircraftCfgEntriesDirectory(const QString &directory) { return this->m_entries.changeDirectory(directory); }

            //! Contains model with title?
            bool containsModelWithTitle(const QString &title, Qt::CaseSensitivity caseSensitivity = Qt::CaseInsensitive);

            //! Model with title
            BlackMisc::Network::CAircraftModel getModelWithTitle(const QString &title, Qt::CaseSensitivity caseSensitivity = Qt::CaseInsensitive) const;

            //! Synchronize models and mappings
            //! \remarks after this step, we only have mappings for which we have models
            int synchronize();

            //! Shutdown
            void gracefulShutdown();

        private:
            QScopedPointer<BlackSim::ISimulatorModelMappings> m_mappings; //!< all mapping definitions
            CAircraftCfgEntriesList                           m_entries;  //!< all entries
            QFuture<bool>                                     m_backgroundInit;
            QFuture<int>                                      m_backgroundRead;
            bool                                              m_init = false;

        };
    } // namespace
} // namespace
#endif // guard
