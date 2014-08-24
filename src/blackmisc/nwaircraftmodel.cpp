/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

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
            QString s = this->m_modelString;
            if (!s.isEmpty()) s.append(' ');
            s.append(this->m_queriedModelStringFlag ? "queried" : "mapped");
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
        void CAircraftModel::convertFromJson(const QJsonObject &json)
        {
            BlackMisc::deserializeJson(json, CAircraftModel::jsonMembers(), TupleConverter<CAircraftModel>::toTuple(*this));
        }

        /*
         * Property by index
         */
        QVariant CAircraftModel::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return this->toQVariant(); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexModelString:
                return QVariant(this->m_modelString);
                break;
            case IndexIsQueriedModelString:
                return QVariant(this->m_queriedModelStringFlag);
                break;
            default:
                return CValueObject::propertyByIndex(index);
                break;
            }
        }

        /*
         * Set property as index
         */
        void CAircraftModel::setPropertyByIndex(const QVariant &variant, const BlackMisc::CPropertyIndex &index)
        {
            if (index.isMyself())
            {
                this->convertFromQVariant(variant);
                return;
            }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexModelString:
                this->m_modelString = variant.toString();
                break;
            case IndexIsQueriedModelString:
                this->m_queriedModelStringFlag = variant.toBool();
                break;
            default:
                CValueObject::setPropertyByIndex(variant, index);
                break;
            }
        }

        /*
         * Matches string?
         */
        bool CAircraftModel::matchesModelString(const QString &modelString, Qt::CaseSensitivity sensitivity) const
        {
            if (sensitivity == Qt::CaseSensitive)
                return modelString == this->m_modelString;
            else
                return this->m_modelString.indexOf(modelString) == 0;
        }

    } // namespace
} // namespace
