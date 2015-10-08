/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_FSCOMMON_AIRCRAFTCFGPARSER_H
#define BLACKMISC_SIMULATION_FSCOMMON_AIRCRAFTCFGPARSER_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/worker.h"
#include "blackmisc/pixmap.h"
#include "blackmisc/simulation/aircraftmodelloader.h"
#include "blackmisc/simulation/fscommon/aircraftcfgentrieslist.h"

#include <QPointer>

namespace BlackMisc
{
    namespace Simulation
    {
        namespace FsCommon
        {
            //! Utility, parsing the aircraft.cfg files
            class BLACKMISC_EXPORT CAircraftCfgParser : public BlackMisc::Simulation::IAircraftModelLoader
            {
                Q_OBJECT

            public:
                //! Destructor
                CAircraftCfgParser();

                //! Constructor
                CAircraftCfgParser(const BlackMisc::Simulation::CSimulatorInfo &simInfo, const QString &rootDirectory, const QStringList &exludes = {});

                //! Virtual destructor
                virtual ~CAircraftCfgParser();

                //! Change the directory
                bool changeRootDirectory(const QString &directory);

                //! Current root directory
                QString getRootDirectory() const { return this->m_rootDirectory; }

                //! Get parsed aircraft cfg entries list
                const CAircraftCfgEntriesList &getAircraftCfgEntriesList() const { return m_parsedCfgEntriesList; }

                //! \copydoc IAircraftModelLoader::getPixmapForModel
                virtual BlackMisc::CPixmap iconForModel(const QString &modelName, BlackMisc::CStatusMessage &statusMessage) const override;

                //! \copydoc IAircraftModelLoader::startLoading
                virtual void startLoading(LoadMode mode = ModeBackground) override;

                //! \copydoc IAircraftModelLoader::isLoadingFinished
                virtual bool isLoadingFinished() const override;

                //! \copydoc IAircraftModelLoader::getAircraftModels
                virtual BlackMisc::Simulation::CAircraftModelList getAircraftModels() const override;

                //! Create an parser object for given simulator
                static std::unique_ptr<CAircraftCfgParser> createModelLoader(const BlackMisc::Simulation::CSimulatorInfo &simInfo);

            public slots:
                //! Parsed or injected entires
                void updateCfgEntriesList(const BlackMisc::Simulation::FsCommon::CAircraftCfgEntriesList &cfgEntriesList);

            private:
                //! Section within file
                enum FileSection
                {
                    General,
                    Fltsim,
                    Unknown
                };

                //! Does the directory exist?
                bool existsDir(const QString &directory = "") const;

                //! Perform the parsing
                //! \threadsafe
                CAircraftCfgEntriesList performParsing(const QString &directory, const QStringList &excludeDirectories, bool *ok);

                //! Fix the content read
                static QString fixedStringContent(const QVariant &qv);

                //! Value from settings, fixed string
                static QString fixedStringContent(const QSettings &settings, const QString &key);

                //! Content after "="
                static QString getFixedIniLineContent(const QString &line);

                QString m_rootDirectory;                        //!< root directory parsing aircraft.cfg files
                QStringList m_excludedDirectories;              //!< directories not to be parsed
                CAircraftCfgEntriesList m_parsedCfgEntriesList; //!< parsed entries
                QPointer<BlackMisc::CWorker> m_parserWorker;    //!< worker will destroy itself, so weak pointer
            };
        } // namespace
    } // namespace
} // namespace

#endif // guard
