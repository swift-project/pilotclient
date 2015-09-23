/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "modelmappingsprovider.h"

using namespace BlackMisc;
using namespace BlackMisc::Simulation;

namespace BlackMisc
{
    namespace Simulation
    {
        IModelMappingsProvider::IModelMappingsProvider(QObject *parent) : QObject(parent) {}

        const CAircraftModelList &IModelMappingsProvider::getDatastoreModels() const
        {
            return m_datastoreModels;
        }
    } // namespace
} // namespace
