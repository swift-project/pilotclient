#include "nwaircraftmodel.h"
#include <QString>

namespace BlackMisc
{
    namespace Network
    {
        /*
         * Convert to string
         */
        QString CAircraftModel::convertToQString(bool /** i18n **/) const
        {
            QString s = this->m_queriedModelString;
            return s;
        }

        /*
         * Compare
         */
        int CAircraftModel::compareImpl(const CValueObject &otherBase) const
        {
            const auto &other = static_cast<const CAircraftModel &>(otherBase);
            return compare(TupleConverter<CAircraftModel>::toTuple(*this), TupleConverter<CAircraftModel>::toTuple(other));
        }

        /*
         * Marshall to DBus
         */
        void CAircraftModel::marshallToDbus(QDBusArgument &argument) const
        {
            argument << TupleConverter<CAircraftModel>::toTuple(*this);
        }

        /*
         * Unmarshall from DBus
         */
        void CAircraftModel::unmarshallFromDbus(const QDBusArgument &argument)
        {
            argument >> TupleConverter<CAircraftModel>::toTuple(*this);
        }

        /*
         * Hash
         */
        uint CAircraftModel::getValueHash() const
        {
            return qHash(TupleConverter<CAircraftModel>::toTuple(*this));
        }

        /*
         * Equal?
         */
        bool CAircraftModel::operator ==(const CAircraftModel &other) const
        {
            if (this == &other) return true;
            return TupleConverter<CAircraftModel>::toTuple(*this) == TupleConverter<CAircraftModel>::toTuple(other);
        }

        /*
         * Unequal?
         */
        bool CAircraftModel::operator !=(const CAircraftModel &other) const
        {
            return !((*this) == other);
        }

        /*
         * metaTypeId
         */
        int CAircraftModel::getMetaTypeId() const
        {
            return qMetaTypeId<CAircraftModel>();
        }

        /*
         * is a
         */
        bool CAircraftModel::isA(int metaTypeId) const
        {
            if (metaTypeId == qMetaTypeId<CAircraftModel>()) { return true; }
            return this->CValueObject::isA(metaTypeId);
        }

        /*
         * Register metadata
         */
        void CAircraftModel::registerMetadata()
        {
            qRegisterMetaType<CAircraftModel>();
            qDBusRegisterMetaType<CAircraftModel>();
        }

        /*
         * Members
         */
        const QStringList &CAircraftModel::jsonMembers()
        {
            return TupleConverter<CAircraftModel>::jsonMembers();
        }

        /*
         * To JSON
         */
        QJsonObject CAircraftModel::toJson() const
        {
            return BlackMisc::serializeJson(CAircraftModel::jsonMembers(), TupleConverter<CAircraftModel>::toTuple(*this));
        }

        /*
         * From Json
         */
        void CAircraftModel::fromJson(const QJsonObject &json)
        {
            BlackMisc::deserializeJson(json, CAircraftModel::jsonMembers(), TupleConverter<CAircraftModel>::toTuple(*this));
        }

        /*
         * Property by index
         */
        QVariant CAircraftModel::propertyByIndex(int index) const
        {
            switch (index)
            {
            case IndexQueriedModelString:
                return QVariant(this->m_queriedModelString);
                break;
            default:
                break;
            }
            Q_ASSERT_X(false, "CAircraftModel", "index unknown");
            QString m = QString("no property, index ").append(QString::number(index));
            return QVariant::fromValue(m);
        }

        /*
         * Set property as index
         */
        void CAircraftModel::setPropertyByIndex(const QVariant &variant, int index)
        {
            switch (index)
            {
            case IndexQueriedModelString:
                this->m_queriedModelString = variant.toString();
                break;
            default:
                Q_ASSERT_X(false, "CAircraftModel", "index unknown");
                break;
            }
        }

    } // namespace
} // namespace
