/* Copyright (C) 2014
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blacksimplugin_freefunctions.h"
#include "fs9_host.h"
#include "fs9_client.h"
#include "blackmisc/mathematics.h"

using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackSimPlugin
{
    namespace Fs9
    {
        void registerMetadata()
        {
            qRegisterMetaType<CFs9Host::HostStatus>();
            qRegisterMetaType<CFs9Host::HostStatus>("CFs9Host::HostStatus");
            qRegisterMetaType<CFs9Client::ClientStatus>();
        }

        CAircraftSituation aircraftSituationfromFS9(const MPPositionVelocity &positionVelocity)
        {
            CAircraftSituation situation;

            double dHigh = positionVelocity.lat_i;
            double dLow = positionVelocity.lat_f;

            dLow = dLow / 65536.0;
            if (dHigh > 0)
                dHigh = dHigh + dLow;
            else
                dHigh = dHigh - dLow;

            CCoordinateGeodetic position;
            position.setLatitude(CLatitude(dHigh * 90.0 / 10001750.0, CAngleUnit::deg()));

            dHigh = positionVelocity.lon_hi;
            dLow = positionVelocity.lon_lo;

            dLow = dLow / 65536.0;
            if (dHigh > 0)
                dHigh = dHigh + dLow;
            else
                dHigh = dHigh - dLow;

            position.setLongitude(CLongitude(dHigh * 360.0 / ( 65536.0 * 65536.0), CAngleUnit::deg()));

            dHigh = positionVelocity.alt_i;
            dLow = positionVelocity.alt_f;

            dLow = dLow / 65536.0;

            situation.setPosition(position);
            situation.setAltitude(CAltitude(dHigh + dLow, CAltitude::MeanSeaLevel, CLengthUnit::m()));
            double groundSpeed = positionVelocity.ground_velocity / 65536.0;
            situation.setGroundspeed(CSpeed(groundSpeed, CSpeedUnit::m_s()));

            FS_PBH pbhstrct;
            pbhstrct.pbh = positionVelocity.pbh;
            double pitch = pbhstrct.pitch / CFs9Sdk::pitchMultiplier();
            if (pitch > 180.0)
                pitch -= 360;

            double bank = pbhstrct.bank / CFs9Sdk::bankMultiplier();
            if (bank > 180.0)
                bank -= 360;

            situation.setPitch(CAngle(pitch, CAngleUnit::deg()));
            situation.setBank(CAngle(bank, CAngleUnit::deg()));
            situation.setHeading(CHeading(pbhstrct.hdg / CFs9Sdk::headingMultiplier(), CHeading::Magnetic, CAngleUnit::deg()));

            return situation;
        }

        MPPositionVelocity aircraftSituationtoFS9(const CAircraftSituation &oldSituation, const CAircraftSituation &newSituation, double updateInterval)
        {
            MPPositionVelocity positionVelocity;

            // Latitude - integer and decimal places
            double latitude = newSituation.getPosition().latitude().value(CAngleUnit::deg()) * 10001750.0 / 90.0;
            positionVelocity.lat_i = static_cast<qint32>(latitude);
            positionVelocity.lat_f = qAbs((latitude - positionVelocity.lat_i) * 65536);

            // Longitude - integer and decimal places
            double longitude = newSituation.getPosition().longitude().value(CAngleUnit::deg()) * ( 65536.0 * 65536.0) / 360.0;
            positionVelocity.lon_hi = static_cast<qint32>(longitude);
            positionVelocity.lon_lo = qAbs((longitude - positionVelocity.lon_hi) * 65536);

            // Altitude - integer and decimal places
            double altitude = newSituation.getAltitude().value(CLengthUnit::m());
            positionVelocity.alt_i = static_cast<qint32>(altitude);
            positionVelocity.alt_f = (altitude - positionVelocity.alt_i) * 65536;

            // Pitch, Bank and Heading
            FS_PBH pbhstrct;
            pbhstrct.hdg = newSituation.getHeading().value(CAngleUnit::deg()) * CFs9Sdk::headingMultiplier();
            pbhstrct.pitch = newSituation.getPitch().value(CAngleUnit::deg()) * CFs9Sdk::pitchMultiplier();
            pbhstrct.bank = newSituation.getBank().value(CAngleUnit::deg()) * CFs9Sdk::bankMultiplier();
            positionVelocity.pbh = pbhstrct.pbh;

            // Ground velocity
            positionVelocity.ground_velocity = newSituation.getGroundSpeed().value(CSpeedUnit::m_s());

            // Altitude velocity
            CCoordinateGeodetic oldPosition = oldSituation.getPosition();
            CCoordinateGeodetic newPosition = newSituation.getPosition();
            CCoordinateGeodetic helperPosition;

            // We want the distance in Latitude direction. Longitude must be equal for old and new position.
            helperPosition.setLatitude(newPosition.latitude());
            helperPosition.setLongitude(oldPosition.longitude());
            CLength distanceLatitudeObj = greatCircleDistance(oldPosition, helperPosition);


            // Now we want the Longitude distance. Latitude must be equal for old and new position.
            helperPosition.setLatitude(oldPosition.latitude());
            helperPosition.setLongitude(newSituation.longitude());
            CLength distanceLongitudeObj = greatCircleDistance(oldPosition, helperPosition);

            // Latitude and Longitude velocity
            positionVelocity.lat_velocity = distanceLatitudeObj.value(CLengthUnit::ft()) * 65536.0 / updateInterval;
            if (oldPosition.latitude().value() > newSituation.latitude().value()) positionVelocity.lat_velocity *= -1;
            positionVelocity.lon_velocity = distanceLongitudeObj.value(CLengthUnit::ft()) * 65536.0 / updateInterval;
            if (oldPosition.longitude().value() > newSituation.longitude().value()) positionVelocity.lon_velocity *= -1;

            // TODO: Altitude velocity

            return positionVelocity;
        }
    }
}
