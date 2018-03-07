/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "simulationenvironmentprovider.h"

using namespace BlackMisc::Geo;

namespace BlackMisc
{
    namespace Simulation
    {
        bool ISimulationEnvironmentProvider::rememberGroundElevation(const ICoordinateGeodetic &elevationCoordinate, const PhysicalQuantities::CLength &epsilon)
        {
            {
                QReadLocker l(&m_lockElvCoordinates);
                if (m_elvCoordinates.containsObjectInRange(elevationCoordinate, epsilon)) { return false; }
            }
            {
                QWriteLocker l(&m_lockElvCoordinates);
                m_elvCoordinates.push_back(elevationCoordinate);
            }
            return true;
        }

        bool ISimulationEnvironmentProvider::rememberGroundElevation(const CElevationPlane &elevationPlane)
        {
            if (!elevationPlane.hasMSLGeodeticHeight()) { return false; }
            return this->rememberGroundElevation(elevationPlane, elevationPlane.getRadius());
        }

        CCoordinateGeodeticList ISimulationEnvironmentProvider::getElevationCoordinates() const
        {
            QReadLocker l(&m_lockElvCoordinates);
            return m_elvCoordinates;
        }

        int ISimulationEnvironmentProvider::cleanUpElevations(const ICoordinateGeodetic &referenceCoordinate, int maxNumber)
        {
            CCoordinateGeodeticList coordinates(this->getElevationCoordinates());
            const int size = coordinates.size();
            if (size <= maxNumber) { return 0; }
            coordinates.sortByEuclideanDistanceSquared(referenceCoordinate);
            coordinates.truncate(maxNumber);
            const int delta = size - coordinates.size();
            QWriteLocker l(&m_lockElvCoordinates);
            m_elvCoordinates = coordinates;
            return delta;
        }

        CElevationPlane ISimulationEnvironmentProvider::findClosestElevationWithinRange(const ICoordinateGeodetic &reference, const PhysicalQuantities::CLength &range)
        {
            return this->getElevationCoordinates().findClosestWithinRange(reference, range);
        }

        void ISimulationEnvironmentProvider::clearSimulationEnvironmentData()
        {
            QWriteLocker l(&m_lockElvCoordinates);
            m_elvCoordinates.clear();
        }

        CElevationPlane CSimulationEnvironmentAware::findClosestElevationWithinRange(const ICoordinateGeodetic &reference, const PhysicalQuantities::CLength &range)
        {
            if (!m_simEnvironmentProvider) { return CElevationPlane::null(); }
            return m_simEnvironmentProvider->findClosestElevationWithinRange(reference, range);
        }
    } // namespace
} // namespace
