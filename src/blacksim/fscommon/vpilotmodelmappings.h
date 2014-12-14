/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIM_FSCOMMON_VPILOTMODELMAPPINGS_H
#define BLACKSIM_FSCOMMON_VPILOTMODELMAPPINGS_H

#include "../simulatormodelmappings.h"
#include <QStringList>

namespace BlackSim
{
    namespace FsCommon
    {
        //! Model mappings
        class CVPilotModelMappings : public ISimulatorModelMappings
        {
        public:
            //! Constructor
            CVPilotModelMappings(bool standardDirectory, QObject *parent = nullptr);

            //! Destructor
            virtual ~CVPilotModelMappings() {}

            //! File names
            void addFilename(const QString &fileName);

            //! Directory with .vmr files
            void addDirectory(const QString &directory);

            //! Loaded files (number)
            int countFilesLoaded() const { return m_loadedFiles; }

            //! The standard directory for vPilot mappings
            static const QString &standardMappingsDirectory();

        public slots:
            //! Load data
            virtual bool read() override;

        private:
            QStringList m_fileList;             //!< list of file names
            QStringList m_fileListWithProblems; //!< problems during parsing
            int m_loadedFiles = 0;              //!< loaded files

            //! Single file read and parsing
            bool loadFile(const QString &fileName);
        };
    } // namespace
} // namespace
#endif // guard
