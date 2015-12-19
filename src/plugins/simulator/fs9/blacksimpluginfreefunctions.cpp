/* Copyright (C) 2014
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blacksimpluginfreefunctions.h"
#include "fs9host.h"
#include "fs9client.h"
#include "blackmisc/math/mathutils.h"
#include "blackmisc/logmessage.h"

using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackSimPlugin
{
    namespace Fs9
    {
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

        MPPositionVelocity aircraftSituationToFS9(const CAircraftSituation &oldSituation, const CAircraftSituation &newSituation, double updateInterval)
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
            CLength distanceLatitudeObj = calculateGreatCircleDistance(oldPosition, helperPosition);


            // Now we want the Longitude distance. Latitude must be equal for old and new position.
            helperPosition.setLatitude(oldPosition.latitude());
            helperPosition.setLongitude(newSituation.longitude());
            CLength distanceLongitudeObj = calculateGreatCircleDistance(oldPosition, helperPosition);

            // Latitude and Longitude velocity
            positionVelocity.lat_velocity = distanceLatitudeObj.value(CLengthUnit::ft()) * 65536.0 / updateInterval;
            if (oldPosition.latitude().value() > newSituation.latitude().value()) positionVelocity.lat_velocity *= -1;
            positionVelocity.lon_velocity = distanceLongitudeObj.value(CLengthUnit::ft()) * 65536.0 / updateInterval;
            if (oldPosition.longitude().value() > newSituation.longitude().value()) positionVelocity.lon_velocity *= -1;

            // TODO: Altitude velocity

            return positionVelocity;
        }

        MPPositionSlewMode aircraftSituationToFS9(const CAircraftSituation &situation)
        {
            MPPositionSlewMode positionSlewMode;

            // Latitude - integer and decimal places
            double latitude = situation.getPosition().latitude().value(CAngleUnit::deg()) * 10001750.0 / 90.0;
            positionSlewMode.lat_i = static_cast<qint32>(latitude);
            positionSlewMode.lat_f = qAbs((latitude - positionSlewMode.lat_i) * 65536);

            // Longitude - integer and decimal places
            double longitude = situation.getPosition().longitude().value(CAngleUnit::deg()) * ( 65536.0 * 65536.0) / 360.0;
            positionSlewMode.lon_hi = static_cast<qint32>(longitude);
            positionSlewMode.lon_lo = qAbs((longitude - positionSlewMode.lon_hi) * 65536);

            // Altitude - integer and decimal places
            double altitude = situation.getAltitude().value(CLengthUnit::m());
            positionSlewMode.alt_i = static_cast<qint32>(altitude);
            positionSlewMode.alt_f = (altitude - positionSlewMode.alt_i) * 65536;

            // Pitch, Bank and Heading
            FS_PBH pbhstrct;
            pbhstrct.hdg = situation.getHeading().value(CAngleUnit::deg()) * CFs9Sdk::headingMultiplier();
            pbhstrct.pitch = situation.getPitch().value(CAngleUnit::deg()) * CFs9Sdk::pitchMultiplier();
            pbhstrct.bank = situation.getBank().value(CAngleUnit::deg()) * CFs9Sdk::bankMultiplier();
            positionSlewMode.pbh = pbhstrct.pbh;

            return positionSlewMode;
        }

        HRESULT logDirectPlayError(HRESULT error)
        {
            QString errorMessage;
            switch(error)
            {
            case DPNERR_BUFFERTOOSMALL:
                errorMessage = "The supplied buffer is not large enough to contain the requested data.";
                break;
            case DPNERR_DOESNOTEXIST:
                errorMessage = "Requested element is not part of the address.";
                break;
            case DPNERR_INVALIDFLAGS:
                errorMessage = "The flags passed to this method are invalid.";
                break;
            case DPNERR_INVALIDPARAM:
                errorMessage = "One or more of the parameters passed to the method are invalid.";
                break;
            case DPNERR_INVALIDPOINTER:
                errorMessage = "Pointer specified as a parameter is invalid.";
                break;
            case DPNERR_INVALIDURL:
                errorMessage = "Specified string is not a valid DirectPlayURL.";
                break;
            case DPNERR_NOTALLOWED:
                errorMessage = "This function is not allowed on this object.";
                break;
            case DPNERR_INVALIDOBJECT:
                errorMessage = "The Microsoft DirectPlay object pointer is invalid.";
                break;
            case DPNERR_UNINITIALIZED:
                errorMessage = "This function is not allowed on this object.";
                break;
            case DPNERR_UNSUPPORTED:
                errorMessage = "The function or feature is not available in this implementation or on this service provider.";
                break;
            case DPNERR_NOTHOST:
                errorMessage = "The client attempted to connect to a nonhost computer. Additionally, this error value may be returned by a nonhost that tried to set the application description.";
                break;
            default:
                errorMessage = QString("Unknown error code %1").arg(error);
                break;
            }

            errorMessage = "DirectPlay: " + errorMessage;
            BlackMisc::CLogMessage("swift.fs9.freefunctions").error(errorMessage);
            return error;
        }
    }
}
