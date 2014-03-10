#include "avaircraftsituation.h"
#include "blackmisc/blackmiscfreefunctions.h"

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
            s.append(" ts: ").append(this->m_timestamp.toString("dd hh:mm:ss"));
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
        QVariant CAircraftSituation::propertyByIndex(int index) const
        {
            switch (index)
            {
            case IndexPosition:
                return QVariant::fromValue(this->m_position);
            case IndexPositionLatitude:
                return QVariant::fromValue(this->latitude());
            case IndexPositionLongitude:
                return QVariant::fromValue(this->longitude());
            case IndexPositionHeight:
                return QVariant::fromValue(this->getHeight());
            case IndexAltitude:
                return QVariant::fromValue(this->m_altitude);
            case IndexHeading:
                return QVariant::fromValue(this->m_heading);
            case IndexPitch:
                return QVariant::fromValue(this->m_pitch);
            case IndexBank:
                return QVariant::fromValue(this->m_bank);
            case IndexGroundspeed:
                return QVariant::fromValue(this->m_groundspeed);
            case IndexTimeStamp:
                return QVariant::fromValue(this->m_timestamp);
            case IndexTimeStampFormatted:
                return QVariant::fromValue(this->m_groundspeed.toQString("dd hh:mm:ss"));
            default:
                break;
            }

            Q_ASSERT_X(false, "CAircraftSituation", "index unknown");
            QString m = QString("no property, index ").append(QString::number(index));
            return QVariant::fromValue(m);
        }

        /*
         * Property as string by index
         */
        QString CAircraftSituation::propertyByIndexAsString(int index, bool i18n) const
        {
            QVariant qv = this->propertyByIndex(index);
            return BlackMisc::qVariantToString(qv, i18n);
        }

        /*
         * Property by index
         */
        void CAircraftSituation::setPropertyByIndex(const QVariant &variant, int index)
        {
            switch (index)
            {
            case IndexPosition:
                this->setPosition(variant.value<CCoordinateGeodetic>());
                break;
            case IndexAltitude:
                this->setAltitude(variant.value<CAltitude>());
                break;
            case IndexPitch:
                this->setPitch(variant.value<CAngle>());
                break;
            case IndexBank:
                this->setBank(variant.value<CAngle>());
                break;
            case IndexGroundspeed:
                this->setGroundspeed(variant.value<CSpeed>());
                break;
            case IndexPositionHeight:
                this->setHeight(variant.value<CLength>());
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
