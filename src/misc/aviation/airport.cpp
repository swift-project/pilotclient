// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/aviation/airport.h"
#include "misc/pq/angle.h"
#include "misc/propertyindexref.h"
#include "misc/stringutils.h"

#include <QCoreApplication>
#include <Qt>
#include <QtGlobal>
#include <QStringBuilder>

using namespace swift::misc;
using namespace swift::misc::physical_quantities;
using namespace swift::misc::geo;

SWIFT_DEFINE_VALUEOBJECT_MIXINS(swift::misc::aviation, CAirport)

namespace swift::misc::aviation
{
    CAirport::CAirport(const QString &icao) : m_icao(icao)
    {}

    CAirport::CAirport(const CAirportIcaoCode &icao, const swift::misc::geo::CCoordinateGeodetic &position) : m_icao(icao), m_position(position)
    {}

    CAirport::CAirport(const CAirportIcaoCode &icao, const swift::misc::geo::CCoordinateGeodetic &position, const QString &descriptiveName) : m_descriptiveName(descriptiveName), m_icao(icao), m_position(position)
    {}

    QString CAirport::getLocationPlusOptionalName() const
    {
        if (m_location.isEmpty()) { return this->getDescriptiveName(); }
        if (m_descriptiveName.isEmpty()) { return this->getLocation(); }
        if (this->getDescriptiveName() == this->getLocation()) { return this->getLocation(); }
        return this->getLocation() % u" (" % this->getDescriptiveName() % u')';
    }

    bool CAirport::matchesLocation(const QString &location) const
    {
        if (location.isEmpty()) { return false; }
        return caseInsensitiveStringCompare(location, this->getLocation());
    }

    bool CAirport::matchesDescriptiveName(const QString &name) const
    {
        if (name.isEmpty()) { return false; }
        return caseInsensitiveStringCompare(name, this->getDescriptiveName());
    }

    void CAirport::updateMissingParts(const CAirport &airport)
    {
        if (!m_country.hasIsoCode() && airport.getCountry().hasIsoCode()) { m_country = airport.getCountry(); }
        if (m_descriptiveName.isEmpty()) { m_descriptiveName = airport.getDescriptiveName(); }
    }

    bool CAirport::isNull() const
    {
        return (m_icao.isEmpty() && m_descriptiveName.isEmpty());
    }

    QString CAirport::convertToQString(bool i18n) const
    {
        QString s = i18n ? QCoreApplication::translate("Aviation", "Airport") : "Airport";
        if (!m_icao.isEmpty()) { s.append(' ').append(m_icao.toQString(i18n)); }

        // position
        s.append(' ').append(m_position.toQString(i18n));
        return s;

        // force strings for translation in resource files
        (void)QT_TRANSLATE_NOOP("Aviation", "Airport");
    }

    CAirport CAirport::fromDatabaseJson(const QJsonObject &json, const QString &prefix)
    {
        CAirport airport(json.value(prefix + "icao").toString());
        airport.setDescriptiveName(json.value(prefix + "name").toString());
        airport.setLocation(json.value(prefix + "location").toString());
        const CCoordinateGeodetic pos(
            json.value(prefix + "latitude").toDouble(),
            json.value(prefix + "longitude").toDouble(),
            json.value(prefix + "altitude").toDouble());
        airport.setPosition(pos);
        airport.setOperating(json.value(prefix + "operating").toString() == QStringLiteral("Y"));

        const CCountry country = CCountry::fromDatabaseJson(json, "ctry_");
        airport.setCountry(country);

        airport.setKeyVersionTimestampFromDatabaseJson(json, prefix);
        return airport;
    }

    QVariant CAirport::propertyByIndex(swift::misc::CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexIcao: return m_icao.propertyByIndex(index.copyFrontRemoved());
        case IndexLocation: return QVariant(m_location);
        case IndexDescriptiveName: return QVariant(m_descriptiveName);
        case IndexPosition: return m_position.propertyByIndex(index.copyFrontRemoved());
        case IndexElevation: return this->getElevation().propertyByIndex(index.copyFrontRemoved());
        case IndexOperating: return QVariant::fromValue(this->isOperating());
        default:
            return (ICoordinateWithRelativePosition::canHandleIndex(index)) ?
                       ICoordinateWithRelativePosition::propertyByIndex(index) :
                       CValueObject::propertyByIndex(index);
        }
    }

    void CAirport::setPropertyByIndex(swift::misc::CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<CAirport>();
            return;
        }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexIcao:
            m_icao.setPropertyByIndex(index.copyFrontRemoved(), variant);
            break;
        case IndexLocation:
            this->setLocation(variant.toString());
            break;
        case IndexDescriptiveName:
            this->setDescriptiveName(variant.toString());
            break;
        case IndexPosition:
            m_position.setPropertyByIndex(index.copyFrontRemoved(), variant);
            break;
        case IndexOperating:
            this->setOperating(variant.toBool());
            break;
        default:
            if (ICoordinateWithRelativePosition::canHandleIndex(index))
            {
                ICoordinateWithRelativePosition::setPropertyByIndex(index, variant);
            }
            else
            {
                CValueObject::setPropertyByIndex(index, variant);
            }
            break;
        }
    }

    int CAirport::comparePropertyByIndex(CPropertyIndexRef index, const CAirport &compareValue) const
    {
        if (index.isMyself()) { return m_icao.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getIcao()); }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexIcao: return m_icao.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getIcao());
        case IndexLocation: return m_location.compare(compareValue.getLocation(), Qt::CaseInsensitive);
        case IndexDescriptiveName: return m_descriptiveName.compare(compareValue.getDescriptiveName(), Qt::CaseInsensitive);
        default:
            if (ICoordinateWithRelativePosition::canHandleIndex(index))
            {
                return ICoordinateWithRelativePosition::comparePropertyByIndex(index, compareValue);
            }
            break;
        }
        Q_ASSERT_X(false, Q_FUNC_INFO, "Compare failed");
        return 0;
    }
} // namespace
