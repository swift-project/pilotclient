/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "nwaircraftmapping.h"
#include "propertyindex.h"

using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;

namespace BlackMisc
{
    namespace Network
    {

        /*
         * Constructor
         */
        CAircraftMapping::CAircraftMapping(const QString &aircraftDesignator, const QString &airlineDesignator, const QString &model) :
            m_icao(CAircraftIcao(aircraftDesignator, airlineDesignator)), m_model(CAircraftModel(model, false))
        { }

        /*
         * Convert to string
         */
        QString CAircraftMapping::convertToQString(bool i18n) const
        {
            QString s = QString(this->m_model.toQString(i18n)).append(' ').append(this->m_icao.toQString(i18n));
            return s;
        }

        /*
         * Compare
         */
        int CAircraftMapping::compareImpl(const CValueObject &otherBase) const
        {
            const auto &other = static_cast<const CAircraftMapping &>(otherBase);
            return compare(TupleConverter<CAircraftMapping>::toTuple(*this), TupleConverter<CAircraftMapping>::toTuple(other));
        }

        /*
         * Marshall to DBus
         */
        void CAircraftMapping::marshallToDbus(QDBusArgument &argument) const
        {
            argument << TupleConverter<CAircraftMapping>::toTuple(*this);
        }

        /*
         * Unmarshall from DBus
         */
        void CAircraftMapping::unmarshallFromDbus(const QDBusArgument &argument)
        {
            argument >> TupleConverter<CAircraftMapping>::toTuple(*this);
        }

        /*
         * Hash
         */
        uint CAircraftMapping::getValueHash() const
        {
            return qHash(TupleConverter<CAircraftMapping>::toTuple(*this));
        }

        /*
         * Model string?
         */
        bool CAircraftMapping::matchesModelString(const QString &modelString, Qt::CaseSensitivity sensitivity) const
        {
            return this->m_model.matchesModelString(modelString, sensitivity);
        }

        /*
         * Equal?
         */
        bool CAircraftMapping::operator ==(const CAircraftMapping &other) const
        {
            if (this == &other) return true;
            return TupleConverter<CAircraftMapping>::toTuple(*this) == TupleConverter<CAircraftMapping>::toTuple(other);
        }

        /*
         * Unequal?
         */
        bool CAircraftMapping::operator !=(const CAircraftMapping &other) const
        {
            return !((*this) == other);
        }

        /*
         * metaTypeId
         */
        int CAircraftMapping::getMetaTypeId() const
        {
            return qMetaTypeId<CAircraftMapping>();
        }

        /*
         * is a
         */
        bool CAircraftMapping::isA(int metaTypeId) const
        {
            if (metaTypeId == qMetaTypeId<CAircraftMapping>()) { return true; }
            return this->CValueObject::isA(metaTypeId);
        }

        /*
         * Register metadata
         */
        void CAircraftMapping::registerMetadata()
        {
            qRegisterMetaType<CAircraftMapping>();
            qDBusRegisterMetaType<CAircraftMapping>();
        }

        /*
         * Members
         */
        const QStringList &CAircraftMapping::jsonMembers()
        {
            return TupleConverter<CAircraftMapping>::jsonMembers();
        }

        /*
         * To JSON
         */
        QJsonObject CAircraftMapping::toJson() const
        {
            return BlackMisc::serializeJson(CAircraftMapping::jsonMembers(), TupleConverter<CAircraftMapping>::toTuple(*this));
        }

        /*
         * From Json
         */
        void CAircraftMapping::convertFromJson(const QJsonObject &json)
        {
            BlackMisc::deserializeJson(json, CAircraftMapping::jsonMembers(), TupleConverter<CAircraftMapping>::toTuple(*this));
        }

        /*
         * Property by index
         */
        QVariant CAircraftMapping::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return this->toQVariant(); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexModel:
                return this->m_model.propertyByIndex(index.copyFrontRemoved());
                break;
            case IndexIcaoCode:
                return this->m_model.propertyByIndex(index.copyFrontRemoved());
                break;
            default:
                break;
            }
            Q_ASSERT_X(false, "CAircraftMapping", "index unknown");
            QString m = QString("no property, index ").append(index.toQString());
            return QVariant::fromValue(m);
        }

        /*
         * Set property as index
         */
        void CAircraftMapping::setPropertyByIndex(const QVariant &variant, const BlackMisc::CPropertyIndex &index)
        {
            if (index.isMyself())
            {
                this->convertFromQVariant(variant);
                return;
            }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexModel:
                this->m_model.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexIcaoCode:
                this->m_icao.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            default:
                Q_ASSERT_X(false, "CAircraftMapping", "index unknown");
                break;
            }
        }

    } // namespace
} // namespace
