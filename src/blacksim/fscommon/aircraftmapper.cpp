/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "aircraftmapper.h"
#include "blackmisc/logmessage.h"
#include <utility>

using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Network;

namespace BlackSim
{
    namespace FsCommon
    {
        CAircraftMapper::CAircraftMapper(QObject *parent) : QObject(parent) { }

        CAircraftMapper::CAircraftMapper(std::unique_ptr<ISimulatorModelMappings> mappings, QObject *parent) :
            QObject(parent), m_mappings(mappings.release())
        {  }

        CAircraftMapper::CAircraftMapper(std::unique_ptr<ISimulatorModelMappings> mappings, const QString &simObjectsDir, QObject *parent) :
            QObject(parent), m_mappings(mappings.release())
        {
            this->m_entries.changeDirectory(simObjectsDir);
        }

        CAircraftMapper::~CAircraftMapper()
        {
            this->gracefulShutdown();
        }

        int CAircraftMapper::readSimObjects(const QString &simObjectDir)
        {
            if (!simObjectDir.isEmpty()) { m_entries.changeDirectory(simObjectDir); }
            int n = m_entries.read();
            emit entriesRead(n);
            return n;
        }

        QFuture<int> &CAircraftMapper::readInBackground(const QString &simObjectDir)
        {
            if (!isRunningInBackground())
            {
                this->m_backgroundRead = QtConcurrent::run(this, &CAircraftMapper::readSimObjects, simObjectDir);
            }
            return this->m_backgroundRead;
        }

        QFuture<bool> &CAircraftMapper::initCompletelyInBackground(const QString &simObjectDir)
        {
            if (!isRunningInBackground())
            {
                this->m_backgroundInit = QtConcurrent::run(this, &CAircraftMapper::initCompletely, simObjectDir);
            }
            return this->m_backgroundInit;
        }

        bool CAircraftMapper::isRunningInBackground() const
        {
            return this->m_backgroundInit.isRunning() || this->m_backgroundRead.isRunning();
        }

        bool CAircraftMapper::isInitialized() const
        {
            return m_init;
        }

        bool CAircraftMapper::containsModelWithTitle(const QString &title, Qt::CaseSensitivity caseSensitivity)
        {
            return this->m_entries.containsModelWithTitle(title, caseSensitivity);
        }

        CAircraftModel CAircraftMapper::getModelWithTitle(const QString &title, Qt::CaseSensitivity caseSensitivity) const
        {
            CAircraftCfgEntriesList el = this->m_entries.findByTitle(title, caseSensitivity);
            if (el.isEmpty()) { return CAircraftModel(); }
            return el.front().toAircraftModel();
        }

        int CAircraftMapper::synchronize()
        {
            Q_ASSERT(this->m_mappings);
            if (!this->m_mappings) { return 0; }
            return this->m_mappings->synchronizeWithExistingModels(this->m_entries.getTitles(true));
        }

        void CAircraftMapper::gracefulShutdown()
        {
            // when running, force re-init
            if (isRunningInBackground())
            {
                m_init = false;
                this->m_entries.cancelRead();
                this->m_backgroundInit.cancel();
                this->m_backgroundRead.cancel();
            }
        }

        bool CAircraftMapper::initCompletely(QString simObjectDir)
        {
            if (this->m_init) { return true; }
            if (!this->m_mappings)
            {
                CLogMessage(this).error("Missing mapping defintions");
                emit initCompleted(false);
                return false;
            }

            if (!m_entries.existsDir(simObjectDir))
            {
                CLogMessage(this).error("Mapping engine, cannot read Flight Simulator directory: %1") << simObjectDir;
                emit initCompleted(false);
                return false;
            }

            // read the defintions (if required)
            int mappingSize = this->m_mappings->size();
            if (mappingSize < 1)
            {
                this->m_mappings->read();
                mappingSize = this->m_mappings->size();
                if (mappingSize < 1)
                {
                    CLogMessage(this).error("Reading mapping rules failed or empty");
                    emit initCompleted(false);
                    return false;
                }
            }
            CLogMessage(this).debug() << "Mapping definitions" << mappingSize;

            // read sim objects, can take a while
            int simObjectsSize = this->m_entries.size();
            if (simObjectsSize < 1)
            {
                simObjectsSize = this->readSimObjects(simObjectDir);
                if (simObjectsSize < 1)
                {
                    CLogMessage(this).error("No SimObjects found in %1") << simObjectDir;
                    emit initCompleted(false);
                    return false;
                }
            }
            simObjectsSize = this->m_entries.size();
            CLogMessage(this).info("Read %1 SimObjects from %2") << simObjectsSize << m_entries.getRootDirectory();

            // sync
            this->synchronize();
            CLogMessage(this).debug() << "Mapping definitions after sync" << this->m_mappings->size();

            // finish
            CLogMessage(this).info("Mapping system: %1 definitions for %2 entries") << this->m_mappings->size() << this->m_entries.size();
            emit initCompleted(true);
            this->m_init = true;
            return true;
        }

    } // namespace
} // namespace
