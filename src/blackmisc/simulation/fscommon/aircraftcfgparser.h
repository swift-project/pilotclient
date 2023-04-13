/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_FSCOMMON_AIRCRAFTCFGPARSER_H
#define BLACKMISC_SIMULATION_FSCOMMON_AIRCRAFTCFGPARSER_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/aircraftmodelloader.h"
#include "blackmisc/simulation/fscommon/aircraftcfgentrieslist.h"
#include "blackmisc/simulation/simulatorinfo.h"

#include <QObject>
#include <QPointer>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <memory>

class QSettings;

namespace BlackMisc
{
    class CWorker;
    namespace Simulation::FsCommon
    {
        //! Utility, parsing the aircraft.cfg files
        class BLACKMISC_EXPORT CAircraftCfgParser : public IAircraftModelLoader
        {
            Q_OBJECT

        public:
            //! Constructor
            CAircraftCfgParser(const CSimulatorInfo &simInfo, QObject *parent = nullptr);

            //! Virtual destructor
            virtual ~CAircraftCfgParser() override;

            //! Get parsed aircraft cfg entries list
            const CAircraftCfgEntriesList &getAircraftCfgEntriesList() const { return m_parsedCfgEntriesList; }

            //! \name Interface functions
            //! @{
            virtual bool isLoadingFinished() const override;
            //! @}

            //! Parse a single file
            static CAircraftCfgEntriesList performParsingOfSingleFile(const QString &fileName, bool &ok, CStatusMessageList &msgs);

            //! Create an parser object for given simulator
            static CAircraftCfgParser *createModelLoader(const CSimulatorInfo &simInfo, QObject *parent = nullptr);

        protected:
            //! \name Interface functions
            //! @{
            virtual void startLoadingFromDisk(LoadMode mode, const ModelConsolidationCallback &modelConsolidation, const QStringList &modelDirectories) override;
            //! @}

        private:
            //! Section within file
            enum FileSection
            {
                General,
                Fltsim,
                Unknown
            };

            //! Perform the parsing for all directories
            //! \threadsafe
            CAircraftCfgEntriesList performParsing(
                const QStringList &directories, const QStringList &excludeDirectories,
                BlackMisc::CStatusMessageList &messages);

            //! Perform the parsing for one directory
            //! \threadsafe
            CAircraftCfgEntriesList performParsing(
                const QString &directory, const QStringList &excludeDirectories,
                BlackMisc::CStatusMessageList &messages);

            //! Fix the content read
            static QString fixedStringContent(const QVariant &qv);

            //! Value from settings, fixed string
            static QString fixedStringContent(const QSettings &settings, const QString &key);

            //! Content after "="
            static QString getFixedIniLineContent(const QString &line);

            //! Files to be used
            static const QStringList &fileNameFilters();

            //! Exclude the sub directories not to be parsed
            static bool isExcludedSubDirectory(const QString &excludeDirectory);

            CAircraftCfgEntriesList m_parsedCfgEntriesList; //!< parsed entries
            QPointer<BlackMisc::CWorker> m_parserWorker; //!< worker will destroy itself, so weak pointer
        };
    } // ns
} // ns

#endif // guard
