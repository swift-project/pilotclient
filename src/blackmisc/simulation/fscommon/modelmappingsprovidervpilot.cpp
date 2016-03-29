/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "modelmappingsprovidervpilot.h"

#include <QFile>
#include <QDir>
#include <QStandardPaths>

using namespace BlackMisc::Network;

namespace BlackMisc
{
    namespace Simulation
    {
        namespace FsCommon
        {
            CModelMappingsProviderVPilot::CModelMappingsProviderVPilot(bool standardDirectory, QObject *parent) :
                IModelMappingsProvider(parent),
                m_vPilotReader(new CVPilotRulesReader(standardDirectory, this))
            {
                // void
            }

            bool CModelMappingsProviderVPilot::read()
            {
                Q_ASSERT_X(this->m_vPilotReader, Q_FUNC_INFO, "missing reader");
                bool success = this->m_vPilotReader->read(false);
                if (success)
                {
                    this->m_mappingModels = this->m_vPilotReader->getAsModels();
                }
                return success;
            }
        } // namespace
    } // namespace
} // namespace
