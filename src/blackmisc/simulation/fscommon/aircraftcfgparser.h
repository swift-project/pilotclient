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
#include "blackmisc/simulation/fscommon/aircraftcfgentrieslist.h"

#include <atomic>

namespace BlackMisc
{
    namespace Simulation
    {
        namespace FsCommon
        {

            //! Utility, parsing the aircraft.cfg files
            class BLACKMISC_EXPORT CAircraftCfgParser : public QObject
            {
                Q_OBJECT

            public:

                //! Parser mode
                enum ParserMode
                {
                    ModeBlocking,
                    ModeAsync
                };

                CAircraftCfgParser() { }

                //! Constructor
                CAircraftCfgParser(const QString &rootDirectory, const QStringList &exludes = {}) :
                    m_rootDirectory(rootDirectory),
                    m_excludedDirectories(exludes)
                { }

                //! Virtual destructor
                virtual ~CAircraftCfgParser() {}

                //! Change the directory
                bool changeRootDirectory(const QString &directory);

                //! Parse all cfg files
                void parse(ParserMode mode = ModeAsync);

                //! Has current directory been parsed?
                bool isParsingFinished() const { return m_parserWorker->isFinished(); }

                //! Cancel read
                void cancelParsing() { m_cancelParsing = true; }

                //! Current root directory
                QString getRootDirectory() const {  return this->m_rootDirectory; }

                //! Get parsed aircraft cfg entries list
                CAircraftCfgEntriesList getAircraftCfgEntriesList() const { return m_parsedCfgEntriesList; }

            signals:

                //! Parsing is finished
                void parsingFinished();

            private slots:

                void ps_updateCfgEntriesList(const BlackMisc::Simulation::FsCommon::CAircraftCfgEntriesList &cfgEntriesList);

                CAircraftCfgEntriesList parseImpl(const QString &directory, const QStringList &excludeDirectories = {});

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

                //! Fix the content read
                static QString fixedStringContent(const QVariant &qv);

                //! Value from settings, fixed string
                static QString fixedStringContent(const QSettings &settings, const QString &key);

                //! Content after "="
                static QString getFixedIniLineContent(const QString &line);

                QString m_rootDirectory;    //!< root directory parsing aircraft.cfg files
                QStringList m_excludedDirectories;

                CAircraftCfgEntriesList m_parsedCfgEntriesList;
                QPointer<BlackMisc::CWorker> m_parserWorker;

                std::atomic<bool> m_cancelParsing = { false };

            };
        } // namespace
    } // namespace
} // namespace


#endif // guard
