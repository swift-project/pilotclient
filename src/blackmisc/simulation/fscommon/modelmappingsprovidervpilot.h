/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_FSCOMMON_VPILOTMODELMAPPINGS_H
#define BLACKMISC_SIMULATION_FSCOMMON_VPILOTMODELMAPPINGS_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/simulation/modelmappingsprovider.h"
#include "blackmisc/simulation/fscommon/vpilotrulesreader.h"
#include <QStringList>
#include <QScopedPointer>

namespace BlackMisc
{
    namespace Simulation
    {
        namespace FsCommon
        {
            //! Model mappings
            class BLACKMISC_EXPORT CModelMappingsProviderVPilot : public IModelMappingsProvider
            {
            public:
                //! Constructor
                CModelMappingsProviderVPilot(bool standardDirectory, QObject *parent = nullptr);

                //! Destructor
                virtual ~CModelMappingsProviderVPilot() {}

                //! \copydoc IModelMappingsProvider::getMatchingModels
                virtual CAircraftModelList getMatchingModels() const override { return this->m_mappingModels; }

            public slots:
                //! Load data
                virtual bool read() override;

            private:
                QScopedPointer<CVPilotRulesReader>        m_vPilotReader;  //!< used vPilot model reader
                BlackMisc::Simulation::CAircraftModelList m_mappingModels; //!< models
            };
        } // namespace
    } // namespace
} // namespace
#endif // guard
