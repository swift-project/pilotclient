// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_FSCOMMON_AIRCRAFTCFGPARSER_H
#define SWIFT_MISC_SIMULATION_FSCOMMON_AIRCRAFTCFGPARSER_H

#include <memory>

#include <QObject>
#include <QPointer>
#include <QString>
#include <QStringList>
#include <QVariant>

#include "misc/simulation/aircraftmodellist.h"
#include "misc/simulation/aircraftmodelloader.h"
#include "misc/simulation/fscommon/aircraftcfgentrieslist.h"
#include "misc/simulation/simulatorinfo.h"
#include "misc/swiftmiscexport.h"

class QSettings;

namespace swift::misc
{
    class CWorker;
    namespace simulation::fscommon
    {
        //! Utility, parsing the aircraft.cfg files
        class SWIFT_MISC_EXPORT CAircraftCfgParser : public IAircraftModelLoader
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
                swift::misc::CStatusMessageList &messages);

            //! Perform the parsing for one directory
            //! \threadsafe
            CAircraftCfgEntriesList performParsing(
                const QString &directory, const QStringList &excludeDirectories,
                swift::misc::CStatusMessageList &messages);

            //! Fix the content read
            static QString fixedStringContent(const QVariant &qv);

            //! Value from settings, fixed string
            static QString fixedStringContent(const QSettings &settings, const QString &key);

            //! Content after "="
            static QString getFixedIniLineContent(const QString &line);

            //! Files to be used
            static const QStringList &fileNameFilters(bool isMSFS);

            //! Exclude the sub directories not to be parsed
            static bool isExcludedSubDirectory(const QString &excludeDirectory);

            CAircraftCfgEntriesList m_parsedCfgEntriesList; //!< parsed entries
            QPointer<swift::misc::CWorker> m_parserWorker; //!< worker will destroy itself, so weak pointer
        };
    } // namespace simulation::fscommon
} // namespace swift::misc

#endif // guard
