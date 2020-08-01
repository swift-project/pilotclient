/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_AUTOPUBLISH_H
#define BLACKMISC_SIMULATION_AUTOPUBLISH_H

#include "aircraftmodellist.h"
#include "simulatorinfo.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/datacache.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/directoryutils.h"
#include "blackmisc/blackmiscexport.h"

#include <QMap>
#include <QSet>

namespace BlackMisc
{
    namespace Simulation
    {
        namespace Data
        {
            //! Last selection
            struct TLastAutoPublish : public TDataTrait<qint64>
            {
                //! First load is synchronous
                static constexpr bool isPinned() { return true; }

                //! Default simulator
                static const qint64 &defaultValue() { static constexpr qint64 d = -1; return d; }

                //! Key
                static const char *key() { return "autopublishlast"; }
            };
        }

        //! Which data have changed
        struct ChangedAutoPublishData
        {
            bool modelKnown = false; //!< model known in DB
            bool changedCG  = false; //!< CG changed
            bool changedSim = false; //!< simulator changed

            //! Set all to true;
            void setAllTrue()
            {
                modelKnown = true;
                changedCG  = true;
                changedSim = true;
            }
        };

        //! Objects that can be use for auto-publishing.
        //! Auto publishing means we sent those data to the DB.
        class BLACKMISC_EXPORT CAutoPublishData
        {
        public:
            //! Insert values we might want to update in the DB
            //! @{
            void insert(const QString &modelString, const PhysicalQuantities::CLength &cg);
            void insert(const QString &modelString, const CSimulatorInfo &simulator);
            //! @}

            //! Clear all
            void clear();

            //! Any data?
            bool isEmpty() const;

            //! Simple database JSON
            QString toDatabaseJson() const;

            //! Read from database JSON
            int fromDatabaseJson(const QString &jsonData, bool clear = true);

            //! Write to file
            //! @{
            bool writeJsonToFile() const;
            bool writeJsonToFile(const QString &pathAndFile) const;
            //! @}

            //! Read from JSON file
            bool readFromJsonFile(const QString &fileAndPath, bool clear = true);

            //! Read all JSON files matching the base name
            int readFromJsonFiles(const QString &dirPath = CDirectoryUtils::logDirectory());

            //! Analyze against DB data
            CStatusMessageList analyzeAgainstDBData(const CAircraftModelList &dbModels);

            //! Summary
            QString getSummary() const;

            //! All affected model strings
            QSet<QString> allModelStrings() const;

            //! File base name
            static const QString &fileBaseName();

            //! File appendix
            static const QString &fileAppendix();

            //! Do any auto pubish files exist?
            static bool existAutoPublishFiles(const QString &dirPath = CDirectoryUtils::logDirectory());

            //! Delete any existing auto publish files
            static int deleteAutoPublishFiles(const QString &dirPath = CDirectoryUtils::logDirectory());

            // ----------------- testing only ---------------

            //! Add some test data
            //! \private testing only
            void testData();

        private:
            //! All files matching the pattern
            static QStringList findAndCleanupPublishFiles(const QString &dirPath);

            QMap<QString, PhysicalQuantities::CLength> m_modelStringVsCG;
            QMap<QString, CSimulatorInfo> m_modelStringVsSimulatorInfo;
        };
    } // namespace
} // namespace

#endif // guard
