/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "avaircraftsituation.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/propertyindex.h"

using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;

namespace BlackMisc
{
    namespace Aviation
    {
        /*
         * Convert to string
         */
        QString CAircraftSituation::convertToQString(bool i18n) const
        {
            QString s(this->m_position.toQString(i18n));
            s.append(" altitude: ").append(this->m_altitude.toQString(i18n));
            s.append(" bank: ").append(this->m_bank.toQString(i18n));
            s.append(" pitch: ").append(this->m_pitch.toQString(i18n));
            s.append(" gs: ").append(this->m_groundspeed.toQString(i18n));
            s.append(" heading: ").append(this->m_heading.toQString(i18n));
            s.append(" timestamp: ").append(this->m_timestamp.toString("dd hh:mm:ss"));
            return s;
        }

        /*
         * metaTypeId
         */
        int CAircraftSituation::getMetaTypeId() const
        {
            return qMetaTypeId<CAircraftSituation>();
        }

        /*
         * is a
         */
        bool CAircraftSituation::isA(int metaTypeId) const
        {
            if (metaTypeId == qMetaTypeId<CAircraftSituation>()) { return true; }

            return this->CValueObject::isA(metaTypeId);
        }

        /*
         * Compare
         */
        int CAircraftSituation::compareImpl(const CValueObject &otherBase) const
        {
            const auto &other = static_cast<const CAircraftSituation &>(otherBase);

            return compare(TupleConverter<CAircraftSituation>::toTuple(*this), TupleConverter<CAircraftSituation>::toTuple(other));
        }

        /*
         * Marshall to DBus
         */
        void CAircraftSituation::marshallToDbus(QDBusArgument &argument) const
        {
            argument << TupleConverter<CAircraftSituation>::toTuple(*this);
        }

        /*
         * Unmarshall from DBus
         */
        void CAircraftSituation::unmarshallFromDbus(const QDBusArgument &argument)
        {
            argument >> TupleConverter<CAircraftSituation>::toTuple(*this);
        }

        /*
         * Equal?
         */
        bool CAircraftSituation::operator ==(const CAircraftSituation &other) const
        {
            if (this == &other) return true;
            return TupleConverter<CAircraftSituation>::toTuple(*this) == TupleConverter<CAircraftSituation>::toTuple(other);
        }

        /*
         * Unequal?
         */
        bool CAircraftSituation::operator !=(const CAircraftSituation &other) const
        {
            return !((*this) == other);
        }

        /*
         * Hash
         */
        uint CAircraftSituation::getValueHash() const
        {
            return qHash(TupleConverter<CAircraftSituation>::toTuple(*this));
        }

        /*
         * Property by index
         */
        QVariant CAircraftSituation::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return this->toQVariant(); }
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
                return this->m_groundspeed.propertyByIndex(index.copyFrontRemoved());
            case IndexTimeStamp:
                return QVariant::fromValue(this->m_timestamp);
            case IndexTimeStampFormatted:
                return QVariant::fromValue(this->m_groundspeed.toQString("dd hh:mm:ss"));
            default:
                break;
            }

            Q_ASSERT_X(false, "CAircraftSituation", "index unknown");
            QString m = QString("no property, index ").append(index.toQString());
            return QVariant::fromValue(m);
        }

        /*
         * Property by index
         */
        void CAircraftSituation::setPropertyByIndex(const QVariant &variant, const BlackMisc::CPropertyIndex &index)
        {
            if (index.isMyself())
            {
                this->convertFromQVariant(variant);
                return;
            }

            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexPosition:
                this->m_position.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexAltitude:
                this->m_altitude.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexPitch:
                this->m_pitch.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexBank:
                this->m_bank.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexGroundspeed:
                this->m_groundspeed.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            default:
                Q_ASSERT_X(false, "CAircraftSituation", "index unknown (setter)");
                break;
            }
        }

        /*
         * Register metadata
         */
        void CAircraftSituation::registerMetadata()
        {
            qRegisterMetaType<CAircraftSituation>();
            qDBusRegisterMetaType<CAircraftSituation>();
        }

    } // namespace
} // namespace
