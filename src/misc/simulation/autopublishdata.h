// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_AUTOPUBLISH_H
#define SWIFT_MISC_SIMULATION_AUTOPUBLISH_H

#include "aircraftmodellist.h"
#include "simulatorinfo.h"
#include "misc/pq/length.h"
#include "misc/datacache.h"
#include "misc/statusmessagelist.h"
#include "misc/swiftdirectories.h"
#include "misc/directoryutils.h"
#include "misc/swiftmiscexport.h"

#include <QMap>
#include <QSet>

namespace swift::misc::simulation
{
    namespace data
    {
        //! Last selection
        struct TLastAutoPublish : public TDataTrait<qint64>
        {
            //! First load is synchronous
            static constexpr bool isPinned() { return true; }

            //! Default simulator
            static const qint64 &defaultValue()
            {
                static constexpr qint64 d = -1;
                return d;
            }

            //! Key
            static const char *key() { return "autopublishlast"; }
        };
    }

    //! Which data have changed
    struct ChangedAutoPublishData
    {
        bool modelKnown = false; //!< model known in DB
        bool changedCG = false; //!< CG changed
        bool changedSim = false; //!< simulator changed

        //! Set all to true;
        void setAllTrue()
        {
            modelKnown = true;
            changedCG = true;
            changedSim = true;
        }
    };

    //! Objects that can be use for auto-publishing.
    //! Auto publishing means we sent those data to the DB.
    class SWIFT_MISC_EXPORT CAutoPublishData
    {
    public:
        //! @{
        //! Insert values we might want to update in the DB
        void insert(const QString &modelString, const physical_quantities::CLength &cg);
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

        //! @{
        //! Write to file
        bool writeJsonToFile() const;
        bool writeJsonToFile(const QString &pathAndFile) const;
        //! @}

        //! Read from JSON file
        bool readFromJsonFile(const QString &fileAndPath, bool clear = true);

        //! Read all JSON files matching the base name
        int readFromJsonFiles(const QString &dirPath = CSwiftDirectories::logDirectory());

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
        static bool existAutoPublishFiles(const QString &dirPath = CSwiftDirectories::logDirectory());

        //! Delete any existing auto publish files
        static int deleteAutoPublishFiles(const QString &dirPath = CSwiftDirectories::logDirectory());

        // ----------------- testing only ---------------

        //! Add some test data
        //! \private testing only
        void testData();

    private:
        //! All files matching the pattern
        static QStringList findAndCleanupPublishFiles(const QString &dirPath);

        QMap<QString, physical_quantities::CLength> m_modelStringVsCG;
        QMap<QString, CSimulatorInfo> m_modelStringVsSimulatorInfo;
    };
} // namespace

#endif // guard
