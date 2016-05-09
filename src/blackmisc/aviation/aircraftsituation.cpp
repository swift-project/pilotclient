/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/variant.h"

using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;

namespace BlackMisc
{
    namespace Aviation
    {

        CAircraftSituation::CAircraftSituation(const CCoordinateGeodetic &position, const CAltitude &altitude, const CHeading &heading, const CAngle &pitch, const CAngle &bank, const CSpeed &gs)
            : m_position(position), m_altitude(altitude), m_heading(heading), m_pitch(pitch),
              m_bank(bank), m_groundSpeed(gs) {}

        CAircraftSituation::CAircraftSituation(const CCallsign &correspondingCallsign, const CCoordinateGeodetic &position, const CAltitude &altitude, const CHeading &heading, const CAngle &pitch, const CAngle &bank, const CSpeed &gs)
            : m_correspondingCallsign(correspondingCallsign),
              m_position(position), m_altitude(altitude), m_heading(heading), m_pitch(pitch),
              m_bank(bank), m_groundSpeed(gs)
        {
            m_correspondingCallsign.setTypeHint(CCallsign::Aircraft);
        }

        QString CAircraftSituation::convertToQString(bool i18n) const
        {
            QString s(this->m_position.toQString(i18n));
            s.append(" altitude: ").append(this->m_altitude.toQString(i18n));
            s.append(" bank: ").append(this->m_bank.toQString(i18n));
            s.append(" pitch: ").append(this->m_pitch.toQString(i18n));
            s.append(" gs: ").append(this->m_groundSpeed.toQString(i18n));
            s.append(" heading: ").append(this->m_heading.toQString(i18n));
            s.append(" timestamp: ").append(this->getFormattedUtcTimestampDhms());
            return s;
        }

        CVariant CAircraftSituation::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            if (ITimestampBased::canHandleIndex(index))
            {
                return ITimestampBased::propertyByIndex(index);
            }

            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexPosition:
                return this->m_position.propertyByIndex(index.copyFrontRemoved());
            case IndexLatitude:
                return this->latitude().propertyByIndex(index.copyFrontRemoved());
            case IndexLongitude:
                return this->longitude().propertyByIndex(index.copyFrontRemoved());
            case IndexAltitude:
                return this->m_altitude.propertyByIndex(index.copyFrontRemoved());
            case IndexHeading:
                return this->m_heading.propertyByIndex(index.copyFrontRemoved());
            case IndexPitch:
                return this->m_pitch.propertyByIndex(index.copyFrontRemoved());
            case IndexBank:
                return this->m_bank.propertyByIndex(index.copyFrontRemoved());
            case IndexGroundspeed:
                return this->m_groundSpeed.propertyByIndex(index.copyFrontRemoved());
            case IndexCallsign:
                return this->m_correspondingCallsign.propertyByIndex(index.copyFrontRemoved());
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        void CAircraftSituation::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CAircraftSituation>(); return; }
            if (ITimestampBased::canHandleIndex(index))
            {
                ITimestampBased::setPropertyByIndex(index, variant);
                return;
            }

            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexPosition:
                this->m_position.setPropertyByIndex(index.copyFrontRemoved(), variant);
                break;
            case IndexAltitude:
                this->m_altitude.setPropertyByIndex(index.copyFrontRemoved(), variant);
                break;
            case IndexPitch:
                this->m_pitch.setPropertyByIndex(index.copyFrontRemoved(), variant);
                break;
            case IndexBank:
                this->m_bank.setPropertyByIndex(index.copyFrontRemoved(), variant);
                break;
            case IndexGroundspeed:
                this->m_groundSpeed.setPropertyByIndex(index.copyFrontRemoved(), variant);
                break;
            case IndexCallsign:
                this->m_correspondingCallsign.setPropertyByIndex(index.copyFrontRemoved(), variant);
                break;
            default:
                CValueObject::setPropertyByIndex(index, variant);
                break;
            }
        }

        bool CAircraftSituation::isOnGroundGuessed() const
        {
            CLength heightAboveGround(this->getHeightAboveGround());
            if (!heightAboveGround.isNull())
            {
                return heightAboveGround.value(CLengthUnit::m()) < 1.0;
            }

            // we guess on pitch an bank
            if (qAbs(this->getPitch().value(CAngleUnit::deg())) > 10) { return false; }
            if (qAbs(this->getBank().value(CAngleUnit::deg())) > 10)  { return false; }

            if (this->getGroundSpeed().value(CSpeedUnit::km_h()) > 80) { return false; }

            // not sure, but this is a guess
            return true;
        }

        CLength CAircraftSituation::getHeightAboveGround() const
        {
            static const CLength notAvialable(0, CLengthUnit::nullUnit());
            if (this->m_altitude.getReferenceDatum() == CAltitude::AboveGround)
            {
                // we have a sure value
                return this->getAltitude();
            }
            if (!m_position.geodeticHeight().isNull() && !m_altitude.isNull())
            {
                return m_altitude - m_position.geodeticHeight();
            }
            return notAvialable;
        }

        void CAircraftSituation::setCallsign(const CCallsign &callsign)
        {
            this->m_correspondingCallsign = callsign;
            this->m_correspondingCallsign.setTypeHint(CCallsign::Aircraft);
        }

    } // namespace
} // namespace
