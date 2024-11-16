// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/aviation/atcstation.h"

#include <QCoreApplication>
#include <QStringBuilder>
#include <QtGlobal>

#include "misc/aviation/comsystem.h"
#include "misc/comparefunctions.h"
#include "misc/mixin/mixincompare.h"
#include "misc/pq/physicalquantity.h"
#include "misc/pq/units.h"
#include "misc/propertyindexref.h"
#include "misc/propertyindexvariantmap.h" // needed for mixin::Index::apply
#include "misc/stringutils.h"

using namespace swift::misc;
using namespace swift::misc::physical_quantities;
using namespace swift::misc::geo;
using namespace swift::misc::network;

SWIFT_DEFINE_VALUEOBJECT_MIXINS(swift::misc::aviation, CAtcStation)

namespace swift::misc::aviation
{
    CAtcStation::CAtcStation()
    {}

    CAtcStation::CAtcStation(const QString &callsign) : m_callsign(callsign)
    {
        m_callsign.setTypeHint(CCallsign::Atc);
    }

    CAtcStation::CAtcStation(const CCallsign &callsign, const CUser &controller, const CFrequency &frequency,
                             const CCoordinateGeodetic &pos, const CLength &range, bool isOnline,
                             const QDateTime &logoffTimeUtc,
                             const CInformationMessage &atis, const CInformationMessage &metar) : m_callsign(callsign), m_controller(controller), m_frequency(frequency), m_position(pos),
                                                                                                  m_range(range), m_isOnline(isOnline), m_logoffTimeUtc(logoffTimeUtc),
                                                                                                  m_atis(atis), m_metar(metar)
    {
        // sync callsigns
        m_callsign.setTypeHint(CCallsign::Atc);
        if (!m_controller.hasCallsign() && !callsign.isEmpty())
        {
            m_controller.setCallsign(m_callsign);
        }
    }

    bool CAtcStation::hasLogoffTimeUtc() const
    {
        return !m_logoffTimeUtc.isNull();
    }

    bool CAtcStation::hasMetar() const
    {
        return m_metar.hasMessage();
    }

    QString CAtcStation::getCallsignAsStringCrossCoupled() const
    {
        if (!this->isAfvCrossCoupled()) { return this->getCallsignAsString(); }
        return QStringLiteral("*") % this->getCallsignAsString();
    }

    QString CAtcStation::getCallsignSuffix() const
    {
        return m_callsign.getSuffix();
    }

    int CAtcStation::getSuffixSortOrder() const
    {
        return m_callsign.getSuffixSortOrder();
    }

    void CAtcStation::setCallsign(const CCallsign &callsign)
    {
        m_callsign = callsign;
        m_controller.setCallsign(callsign);
        m_isAfvCrossCoupled = callsign.getStringAsSet().startsWith('*');
    }

    QString CAtcStation::getCallsignAndControllerRealName() const
    {
        if (m_callsign.isEmpty()) { return this->getControllerRealName(); }
        if (!m_controller.hasRealName()) { return m_callsign.asString(); }
        return m_callsign.asString() % u' ' % this->getControllerRealName();
    }

    void CAtcStation::setController(const CUser &controller)
    {
        m_controller = controller;
        m_controller.setCallsign(m_callsign);
    }

    QString CAtcStation::convertToQString(bool i18n) const
    {
        static const QString atcI18n(QCoreApplication::translate("Aviation", "ATC station"));
        static const QString rangeI18n(QCoreApplication::translate("Aviation", "range"));
        static const QString fromUtcI18n(QCoreApplication::translate("Aviation", "from(UTC)"));
        static const QString untilUtcI18n(QCoreApplication::translate("Aviation", "until(UTC)"));

        const QString s = (i18n ? atcI18n : QStringLiteral("ATC station")) %
                          u' ' % m_callsign.toQString(i18n) %
                          u' ' % m_position.toQString(i18n) %
                          u" online: " % boolToYesNo(m_isOnline) %

                          // controller
                          (m_controller.isNull() ? QString() :
                                                   u' ' % m_controller.toQString(i18n)) %

                          // frequency
                          u' ' % m_frequency.valueRoundedWithUnit(3, i18n) %

                          // ATIS
                          (!this->hasAtis() ? QString() :
                                              u' ' % m_atis.toQString(i18n)) %

                          // METAR
                          (!this->hasMetar() ? QString() :
                                               u' ' % m_metar.toQString(i18n)) %

                          // range
                          u' ' % (i18n ? rangeI18n : QStringLiteral("range")) %
                          u' ' % m_range.toQString(i18n) %

                          // distance / bearing
                          u' ' % ICoordinateWithRelativePosition::convertToQString(i18n);

        return s;

        // force strings for translation in resource files
        (void)QT_TRANSLATE_NOOP("Aviation", "ATC station");
        (void)QT_TRANSLATE_NOOP("Aviation", "online");
        (void)QT_TRANSLATE_NOOP("Aviation", "offline");
        (void)QT_TRANSLATE_NOOP("Aviation", "from(UTC)");
        (void)QT_TRANSLATE_NOOP("Aviation", "until(UTC)");
        (void)QT_TRANSLATE_NOOP("Aviation", "range");
        (void)QT_TRANSLATE_NOOP("Aviation", "distance");
    }

    void CAtcStation::setFrequency(const CFrequency &frequency)
    {
        m_frequency = frequency;
        m_frequency.setUnit(CFrequencyUnit::MHz());
    }

    bool CAtcStation::isInRange() const
    {
        if (m_range.isNull() || !hasValidRelativeDistance()) { return false; }
        return (this->getRelativeDistance() <= m_range);
    }

    bool CAtcStation::setOnline(bool online)
    {
        if (online == m_isOnline) { return false; }
        m_isOnline = online;
        return true;
    }

    bool CAtcStation::isComUnitTunedToFrequency(const CComSystem &comUnit) const
    {
        return comUnit.isActiveFrequencySameFrequency(this->getFrequency());
    }

    bool CAtcStation::isAtcStationFrequency(const CFrequency &frequency) const
    {
        return CComSystem::isSameFrequency(frequency, this->getFrequency());
    }

    const CInformationMessage &CAtcStation::getInformationMessage(CInformationMessage::InformationType type) const
    {
        switch (type)
        {
        case CInformationMessage::ATIS: return this->getAtis();
        case CInformationMessage::METAR: return this->getMetar();
        default: break;
        }
        return CInformationMessage::unspecified();
    }

    bool CAtcStation::setMessage(const CInformationMessage &message)
    {
        switch (message.getType())
        {
        case CInformationMessage::ATIS: this->setAtis(message); return true;
        case CInformationMessage::METAR: this->setMetar(message); return true;
        default: break;
        }
        return false;
    }

    CLatitude CAtcStation::latitude() const
    {
        return this->getPosition().latitude();
    }

    CLongitude CAtcStation::longitude() const
    {
        return this->getPosition().longitude();
    }

    const CAltitude &CAtcStation::geodeticHeight() const
    {
        return m_position.geodeticHeight();
    }

    QVector3D CAtcStation::normalVector() const
    {
        return m_position.normalVector();
    }

    std::array<double, 3> CAtcStation::normalVectorDouble() const
    {
        return m_position.normalVectorDouble();
    }

    QVariant CAtcStation::propertyByIndex(CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexLogoffTime: return QVariant::fromValue(m_logoffTimeUtc);
        case IndexCallsign: return m_callsign.propertyByIndex(index.copyFrontRemoved());
        case IndexCallsignString: return this->getCallsignAsString();
        case IndexCallsignStringCrossCopuled: return this->getCallsignAsStringCrossCoupled();
        case IndexController: return m_controller.propertyByIndex(index.copyFrontRemoved());
        case IndexFrequency: return m_frequency.propertyByIndex(index.copyFrontRemoved());
        case IndexIsOnline: return QVariant::fromValue(m_isOnline);
        case IndexLatitude: return this->latitude().propertyByIndex(index.copyFrontRemoved());
        case IndexLongitude: return this->longitude().propertyByIndex(index.copyFrontRemoved());
        case IndexPosition: return m_position.propertyByIndex(index.copyFrontRemoved());
        case IndexRange: return m_range.propertyByIndex(index.copyFrontRemoved());
        case IndexIsInRange: return QVariant::fromValue(isInRange());
        case IndexAtis: return m_atis.propertyByIndex(index.copyFrontRemoved());
        case IndexMetar: return m_metar.propertyByIndex(index.copyFrontRemoved());
        case IndexIsAfvCrossCoupled: return QVariant::fromValue(m_isAfvCrossCoupled);
        default:
            return (ICoordinateWithRelativePosition::canHandleIndex(index)) ?
                       ICoordinateWithRelativePosition::propertyByIndex(index) :
                       CValueObject::propertyByIndex(index);
        }
    }

    void CAtcStation::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<CAtcStation>();
            return;
        }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexLogoffTime: this->setLogoffTimeUtc(variant.value<QDateTime>()); break;
        case IndexCallsign: m_callsign.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
        case IndexController: m_controller.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
        case IndexFrequency: m_frequency.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
        case IndexIsOnline: this->setOnline(variant.value<bool>()); break;
        case IndexPosition: m_position.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
        case IndexRange: m_range.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
        case IndexAtis: m_atis.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
        case IndexMetar: m_metar.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
        case IndexIsAfvCrossCoupled: this->setAfvCrossCoupled(variant.value<bool>()); break;
        case IndexCallsignString:
        case IndexCallsignStringCrossCopuled:
        {
            const QString cs = variant.toString();
            *this = CAtcStation();
            this->setAfvCrossCoupled(cs.startsWith('*'));
        }
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

    int CAtcStation::comparePropertyByIndex(CPropertyIndexRef index, const CAtcStation &compareValue) const
    {
        if (index.isMyself()) { return this->getCallsign().comparePropertyByIndex(CPropertyIndexRef::empty(), compareValue.getCallsign()); }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexLogoffTime: return Compare::compare(this->getLogoffTimeUtc(), compareValue.getLogoffTimeUtc());
        case IndexCallsignString:
        case IndexCallsignStringCrossCopuled:
            return m_callsign.comparePropertyByIndex(CPropertyIndexRef::empty(), compareValue.getCallsign());
        case IndexCallsign: return m_callsign.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getCallsign());
        case IndexController: return m_controller.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getController());
        case IndexFrequency: return m_frequency.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getFrequency());
        case IndexIsOnline: return Compare::compare(this->isOnline(), compareValue.isOnline());
        case IndexLatitude: return this->latitude().comparePropertyByIndex(index.copyFrontRemoved(), compareValue.latitude());
        case IndexLongitude: return this->longitude().comparePropertyByIndex(index.copyFrontRemoved(), compareValue.longitude());
        case IndexPosition: return m_position.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getPosition());
        case IndexRange: return m_range.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getRange());
        case IndexIsInRange: return Compare::compare(this->isInRange(), compareValue.isInRange());
        case IndexAtis: return m_atis.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getAtis());
        case IndexMetar: return m_metar.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getMetar());
        case IndexIsAfvCrossCoupled: return Compare::compare(this->isAfvCrossCoupled(), compareValue.isAfvCrossCoupled());
        default:
            if (ICoordinateWithRelativePosition::canHandleIndex(index))
            {
                return ICoordinateWithRelativePosition::comparePropertyByIndex(index, compareValue);
            }
            return CValueObject::comparePropertyByIndex(index, *this);
        }
        Q_ASSERT_X(false, Q_FUNC_INFO, "Compare failed");
        return 0;
    }
} // namespace swift::misc::aviation
