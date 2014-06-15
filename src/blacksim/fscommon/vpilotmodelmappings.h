/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKSIM_FSCOMMON_VPILOTMODELMAPPINGS_H
#define BLACKSIM_FSCOMMON_VPILOTMODELMAPPINGS_H

#include "../simulatormodelmappings.h"
#include <QStringList>

namespace BlackSim
{
    namespace FsCommon
    {
        /*!
         * Model mappings
         */
        class CVPilotModelMappings : public ISimulatorModelMappings
        {
        public:
            //! Constructor
            CVPilotModelMappings(QObject *parent = nullptr) : ISimulatorModelMappings(parent) {}

            //! Destructor
            virtual ~CVPilotModelMappings() {}

            //! File names
            void addFilename(const QString &fileName);

            //! Directory with .vmr files
            void addDirectory(const QString &directory);

            //! The standard directory for vPilot mappings
            static const QString &standardMappingsDirectory();

        public slots:
            //! Load data
            virtual bool load() override;

        private:
            QStringList m_fileList; //!< list of file names

            //! Single file read and parsing
            bool loadFile(const QString &fileName);
        };
    } // namespace
} // namespace
#endif // guard
