/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIM_FSCOMMON_AIRCRAFTINDEXER_H
#define BLACKSIM_FSCOMMON_AIRCRAFTINDEXER_H

#include "aircraftcfgentrieslist.h"
#include <QObject>
#include <QtConcurrent/QtConcurrent>

namespace BlackSim
{
    namespace FsCommon
    {
        //! Indexer for all models
        //! \sa CAircraftCfgEntries
        //! \sa CAircraftCfgEntriesList
        class CAircraftIndexer : QObject
        {
            Q_OBJECT

        signals:
            //! Number of entries read
            void entriesRead(int number);

        public:
            //! Single entity indexer
            static CAircraftIndexer &indexer()
            {
                static CAircraftIndexer indexer;
                return indexer;
            }

            //! Read for directory or re-read
            static int read(const QString directory = "");

            //! Read in background
            static QFuture<int> readInBackground(const QString directory = "");

        private:
            //! Constructor
            CAircraftIndexer(QObject *parent = nullptr);

            CAircraftCfgEntriesList m_entries; //!< all entries
        };
    } // namespace
} // namespace
#endif // guard
