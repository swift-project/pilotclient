/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "weather.h"
#include <QDebug>

namespace XBus
{

    template <class T>
    void setCloudLayerImpl(T &layer, int base, int tops, int type, int coverage)
    {
        layer.base.set(base);
        layer.tops.set(tops);
        layer.type.set(type);
        layer.coverage.set(coverage);
    }

    void CWeather::setCloudLayer(int layer, int base, int tops, int type, int coverage)
    {
        switch (layer)
        {
        case 0: setCloudLayerImpl(m_cloudLayer0, base, tops, type, coverage); break;
        case 1: setCloudLayerImpl(m_cloudLayer1, base, tops, type, coverage); break;
        case 2: setCloudLayerImpl(m_cloudLayer2, base, tops, type, coverage); break;
        default: qDebug() << "Invalid cloud layer" << layer; break;
        }
    }

    template <class T>
    void setWindLayerImpl(T &layer, int altitude, float direction, int speed, int shearDirection, int shearSpeed, int turbulence)
    {
        layer.altitude.set(altitude);
        layer.direction.set(direction);
        layer.speed.set(speed);
        layer.shearDirection.set(shearDirection);
        layer.shearSpeed.set(shearSpeed);
        layer.turbulence.set(turbulence);
    }

    void CWeather::setWindLayer(int layer, int altitude, float direction, int speed, int shearDirection, int shearSpeed, int turbulence)
    {
        switch (layer)
        {
        case 0: setWindLayerImpl(m_windLayer0, altitude, direction, speed, shearDirection, shearSpeed, turbulence); break;
        case 1: setWindLayerImpl(m_windLayer1, altitude, direction, speed, shearDirection, shearSpeed, turbulence); break;
        case 2: setWindLayerImpl(m_windLayer2, altitude, direction, speed, shearDirection, shearSpeed, turbulence); break;
        default: qDebug() << "Invalid wind layer" << layer; break;
        }
    }

}
