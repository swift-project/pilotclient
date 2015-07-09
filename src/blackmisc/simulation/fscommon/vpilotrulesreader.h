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
#include "blackmisc/simulation/fscommon/vpilotmodelruleset.h"
#include <QStringList>
#include <QObject>

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
                virtual ~CVPilotRulesReader() {}

                //! File names
                void addFilename(const QString &fileName);

                //! Directory with .vmr files
                void addDirectory(const QString &directory);

                //! Loaded files (number)
                int countFilesLoaded() const { return m_loadedFiles; }

                //! Loaded rules
                const CVPilotModelRuleSet &getRules() const { return m_rules; }

                //! Loaded rules
                int countRulesLoaded() const;

                //! The standard directory for vPilot mappings
                static const QString &standardMappingsDirectory();

            signals:
                //! Rules read
                void readFinished(bool success);

            public slots:
                //! Load data
                bool read();

            private:
                QStringList m_fileList;             //!< list of file names
                QStringList m_fileListWithProblems; //!< problems during parsing
                int m_loadedFiles = 0;              //!< loaded files
                CVPilotModelRuleSet m_rules;        //!< rules list

                //! Read single file and do parsing
                bool loadFile(const QString &fileName);

           };
        } // namespace
    } // namespace
} // namespace

#endif // guard
