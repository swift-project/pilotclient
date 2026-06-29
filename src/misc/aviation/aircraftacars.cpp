// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/aviation/aircraftacars.h"

#include <QtGlobal>

#include "QStringBuilder"

#include "misc/comparefunctions.h"
#include "misc/propertyindex.h"
#include "misc/stringutils.h"

SWIFT_DEFINE_VALUEOBJECT_MIXINS(swift::misc::aviation, CAircraftAcars)

namespace swift::misc::aviation
{
    CAircraftAcars::CAircraftAcars(int simRate, int fuelTotalquantity)
        : m_simRate(simRate), m_fuelTotalquantity(fuelTotalquantity)
    {}

    // CAircraftAcars::CAircraftAcars(int simRate, qint64 timestamp) : m_simRate(simRate) {}

    CAircraftAcars::CAircraftAcars(int simRate, int fuelTotalquantity, qint64 timestamp)
        : m_simRate(simRate), m_fuelTotalquantity(fuelTotalquantity)
    {
        this->setMSecsSinceEpoch(timestamp);
    }

    QString CAircraftAcars::convertToQString(bool i18n) const
    {
        // return QStringLiteral("CAircraftAcars"); // Platzhalter, bitte anpassen!

        return u"ts: " % this->getFormattedTimestampAndOffset(true) % u" details: " % this->getAcarsDetailsAsString() %
               u" | simrate: " % QString::number(m_simRate);
    }

    QJsonObject CAircraftAcars::toIncrementalJson() const
    {
        QJsonObject json = this->toJson();
        json.remove(attributeNameIsFullJson());
        json.insert(attributeNameIsFullJson(), QJsonValue(false));
        return json;
    }

    QJsonObject CAircraftAcars::toFullJson() const
    {
        QJsonObject json = this->toJson();
        json.remove(attributeNameIsFullJson());
        json.insert(attributeNameIsFullJson(), QJsonValue(true));
        return json;
    }

    bool CAircraftAcars::isNull() const { return this->getAcarsDetails() == NotSet && m_simRate < 0; }

    bool CAircraftAcars::equalValues(const CAircraftAcars &other) const
    {
        // currently same as some values are diabled for comparison
        // but that could change in future
        return other == *this;
    }

    const CAircraftAcars &CAircraftAcars::null()
    {
        static const CAircraftAcars null(1, 100);
        return null;
    }

    const QString &CAircraftAcars::acarsDetailsToString(CAircraftAcars::AcarsDetails details)
    {
        static const QString guessed("guessed");
        static const QString notset("not set");
        static const QString fsd("FSD acars");

        switch (details)
        {
        case GuessedAcars: return guessed;
        case FSDAircraftAcars: return fsd;
        case NotSet: break;
        default: break;
        }
        return notset;
    }

    const QString &CAircraftAcars::attributeNameIsFullJson()
    {
        static const QString a("is_full_data");
        return a;
    }

    QVariant CAircraftAcars::propertyByIndex(swift::misc::CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        if (ITimestampWithOffsetBased::canHandleIndex(index))
        {
            return ITimestampWithOffsetBased::propertyByIndex(index);
        }

        const auto i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexSimrate: return QVariant::fromValue(m_simRate);
        case IndexFuelTotalQuantity: return QVariant::fromValue(m_fuelTotalquantity);
        default: return CValueObject::propertyByIndex(index);
        }
    }

    void CAircraftAcars::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<CAircraftAcars>();
            return;
        }
        if (ITimestampWithOffsetBased::canHandleIndex(index))
        {
            ITimestampWithOffsetBased::setPropertyByIndex(index, variant);
            return;
        }

        const auto i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexSimrate: m_simRate = variant.toInt(); break;
        case IndexFuelTotalQuantity: m_fuelTotalquantity = variant.toInt(); break;
        default: CValueObject::setPropertyByIndex(index, variant); break;
        }
    }

    int CAircraftAcars::comparePropertyByIndex(CPropertyIndexRef index, const CAircraftAcars &compareValue) const
    {
        if (index.isMyself())
        {
            return ITimestampWithOffsetBased::comparePropertyByIndex(CPropertyIndex(), compareValue);
        }
        if (ITimestampWithOffsetBased::canHandleIndex(index))
        {
            return ITimestampWithOffsetBased::comparePropertyByIndex(index, compareValue);
        }

        const auto i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexSimrate: return Compare::compare(m_simRate, compareValue.getSimrate());
        case IndexFuelTotalQuantity: return Compare::compare(m_fuelTotalquantity, compareValue.getFuelTotalquantity());
        default: break;
        }
        Q_ASSERT_X(false, Q_FUNC_INFO, "No comparison");
        return 0;
    }

} // namespace swift::misc::aviation
