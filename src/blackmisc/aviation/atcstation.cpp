/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/aviation/atcstation.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/mixin/mixincompare.h"
#include "blackmisc/pq/physicalquantity.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/variant.h"
#include "blackmisc/comparefunctions.h"
#include "blackmisc/stringutils.h"

#include <QCoreApplication>
#include <QtGlobal>
#include <QStringBuilder>

using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Network;

namespace BlackMisc
{
    namespace Aviation
    {
        CAtcStation::CAtcStation()
        { }

        CAtcStation::CAtcStation(const QString &callsign) : m_callsign(callsign)
        {
            m_callsign.setTypeHint(CCallsign::Atc);
        }

        CAtcStation::CAtcStation(const CCallsign &callsign, const CUser &controller, const CFrequency &frequency,
                                 const CCoordinateGeodetic &pos, const CLength &range, bool isOnline,
                                 const QDateTime &bookedFromUtc, const QDateTime &bookedUntilUtc,
                                 const CInformationMessage &atis, const CInformationMessage &metar) :
            m_callsign(callsign), m_controller(controller), m_frequency(frequency), m_position(pos),
            m_range(range), m_isOnline(isOnline), m_bookedFromUtc(bookedFromUtc), m_bookedUntilUtc(bookedUntilUtc),
            m_atis(atis), m_metar(metar)
        {
            // sync callsigns
            m_callsign.setTypeHint(CCallsign::Atc);
            if (!m_controller.hasCallsign() && !callsign.isEmpty())
            {
                m_controller.setCallsign(m_callsign);
            }
        }

        bool CAtcStation::hasBookingTimes() const
        {
            return !(m_bookedFromUtc.isNull() && m_bookedUntilUtc.isNull());
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
                              u' ' % ICoordinateWithRelativePosition::convertToQString(i18n) %

                              // booking from/until
                              u' ' %
                              (i18n ? fromUtcI18n : QStringLiteral("from(UTC)")) %
                              u' ' %
                              (m_bookedFromUtc.isNull() ? QStringLiteral("-") : m_bookedFromUtc.toString("yy-MM-dd HH:mm")) %

                              u' ' %
                              (i18n ? untilUtcI18n : QStringLiteral("until(UTC)")) %
                              u' ' %
                              (m_bookedUntilUtc.isNull() ? QStringLiteral("-") : m_bookedUntilUtc.toString("yy-MM-dd HH:mm"));

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

        void CAtcStation::synchronizeControllerData(CAtcStation &otherStation)
        {
            if (m_controller == otherStation.getController()) { return; }
            CUser otherController = otherStation.getController();
            m_controller.synchronizeData(otherController);
            otherStation.setController(otherController);
        }

        void CAtcStation::synchronizeWithBookedStation(CAtcStation &bookedStation)
        {
            if (bookedStation.getCallsign() != this->getCallsign()) { return; }

            // from online to booking
            bookedStation.setOnline(true);
            bookedStation.setFrequency(this->getFrequency());

            // Logoff Zulu Time set?
            // comes directly from the online controller and is most likely more accurate
            if (!this->getBookedUntilUtc().isNull())
            {
                bookedStation.setBookedUntilUtc(this->getBookedUntilUtc());
            }

            // from booking to online
            // booked now stations have valid data and need no update
            if (!this->isBookedNow() && bookedStation.hasValidBookingTimes())
            {
                if (this->hasValidBookingTimes())
                {
                    if (bookedStation.isBookedNow())
                    {
                        // can't get any better, we just copy from / to over
                        this->setBookedFromUntil(bookedStation);
                    }
                    else
                    {
                        // we already have some booking dates, we will verify those now
                        // and will set the most appropriate booking dates
                        const CTime timeDiffBooking(bookedStation.bookedWhen());
                        const CTime timeDiffOnline(this->bookedWhen()); // diff to now
                        if (timeDiffBooking.isNegativeWithEpsilonConsidered() && timeDiffOnline.isNegativeWithEpsilonConsidered())
                        {
                            // both in past
                            if (timeDiffBooking > timeDiffOnline)
                            {
                                this->setBookedFromUntil(bookedStation);
                            }
                        }
                        else if (timeDiffBooking.isPositiveWithEpsilonConsidered() && timeDiffOnline.isPositiveWithEpsilonConsidered())
                        {
                            // both in future
                            if (timeDiffBooking < timeDiffOnline)
                            {
                                this->setBookedFromUntil(bookedStation);
                            }
                        }
                        else if (timeDiffBooking.isPositiveWithEpsilonConsidered() && timeDiffOnline.isNegativeWithEpsilonConsidered())
                        {
                            // future booking is better than past booking
                            this->setBookedFromUntil(bookedStation);
                        }
                    }
                }
                else
                {
                    // no booking info so far, so we just copy over
                    this->setBookedFromUntil(bookedStation);
                }
            }

            // both ways
            this->synchronizeControllerData(bookedStation);
            if (this->hasValidRelativeDistance())
            {
                bookedStation.setRelativeDistance(this->getRelativeDistance());
                bookedStation.setRelativeBearing(this->getRelativeBearing());
            }
            else if (bookedStation.hasValidRelativeDistance())
            {
                this->setRelativeDistance(bookedStation.getRelativeDistance());
                this->setRelativeBearing(bookedStation.getRelativeBearing());
            }
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

        bool CAtcStation::hasValidBookingTimes() const
        {
            return !m_bookedFromUtc.isNull() && m_bookedFromUtc.isValid() &&
                   !m_bookedUntilUtc.isNull() && m_bookedUntilUtc.isValid();
        }

        void CAtcStation::setBookedFromUntil(const CAtcStation &otherStation)
        {
            this->setBookedFromUtc(otherStation.getBookedFromUtc());
            this->setBookedUntilUtc(otherStation.getBookedUntilUtc());
        }

        bool CAtcStation::isBookedNow() const
        {
            if (!this->hasValidBookingTimes()) { return false; }
            QDateTime now = QDateTime::currentDateTimeUtc();
            if (m_bookedFromUtc > now)  { return false; }
            if (now > m_bookedUntilUtc) { return false; }
            return true;
        }

        bool CAtcStation::isComUnitTunedIn25KHz(const CComSystem &comUnit) const
        {
            return comUnit.isActiveFrequencyWithin25kHzChannel(this->getFrequency());
        }

        bool CAtcStation::isComUnitTunedInChannelSpacing(const CComSystem &comUnit) const
        {
            return comUnit.isActiveFrequencyWithinChannelSpacing(this->getFrequency());
        }

        bool CAtcStation::isFrequencyWithinChannelSpacing(const CFrequency &frequency, CComSystem::ChannelSpacing spacing) const
        {
            return CComSystem::isWithinChannelSpacing(frequency, this->getFrequency(), spacing);
        }

        CTime CAtcStation::bookedWhen() const
        {
            if (!this->hasValidBookingTimes()) { return CTime(0, nullptr); }
            QDateTime now = QDateTime::currentDateTimeUtc();
            qint64 diffMs;
            if (m_bookedFromUtc > now)
            {
                // future
                diffMs = now.msecsTo(m_bookedFromUtc);
                return CTime(diffMs / 1000.0, CTimeUnit::s());
            }
            else if (m_bookedUntilUtc > now)
            {
                // now
                return CTime(0.0, CTimeUnit::s());
            }
            else
            {
                // past
                diffMs = m_bookedUntilUtc.msecsTo(now);
                return CTime(-diffMs / 1000.0, CTimeUnit::s());
            }
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

        CVariant CAtcStation::propertyByIndex(const CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexBookedFrom:  return CVariant::from(m_bookedFromUtc);
            case IndexBookedUntil: return CVariant::from(m_bookedUntilUtc);
            case IndexCallsign:    return m_callsign.propertyByIndex(index.copyFrontRemoved());
            case IndexCallsignString:             return this->getCallsignAsString();
            case IndexCallsignStringCrossCopuled: return this->getCallsignAsStringCrossCoupled();
            case IndexController:  return m_controller.propertyByIndex(index.copyFrontRemoved());
            case IndexFrequency:   return m_frequency.propertyByIndex(index.copyFrontRemoved());
            case IndexIsOnline:    return CVariant::from(m_isOnline);
            case IndexLatitude:    return this->latitude().propertyByIndex(index.copyFrontRemoved());
            case IndexLongitude:   return this->longitude().propertyByIndex(index.copyFrontRemoved());
            case IndexPosition:    return m_position.propertyByIndex(index.copyFrontRemoved());
            case IndexRange:       return m_range.propertyByIndex(index.copyFrontRemoved());
            case IndexIsInRange:   return CVariant::fromValue(isInRange());
            case IndexAtis:        return m_atis.propertyByIndex(index.copyFrontRemoved());
            case IndexMetar:       return m_metar.propertyByIndex(index.copyFrontRemoved());
            case IndexIsAfvCrossCoupled: return CVariant::from(m_isAfvCrossCoupled);
            default:
                return (ICoordinateWithRelativePosition::canHandleIndex(index)) ?
                       ICoordinateWithRelativePosition::propertyByIndex(index) :
                       CValueObject::propertyByIndex(index);
            }
        }

        void CAtcStation::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CAtcStation>(); return; }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexBookedFrom:  this->setBookedFromUtc(variant.value<QDateTime>()); break;
            case IndexBookedUntil: this->setBookedUntilUtc(variant.value<QDateTime>()); break;
            case IndexCallsign:    m_callsign.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexController:  m_controller.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexFrequency:   m_frequency.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexIsOnline:    this->setOnline(variant.value<bool>()); break;
            case IndexPosition:    m_position.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexRange:       m_range.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexAtis:        m_atis.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexMetar:       m_metar.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexIsAfvCrossCoupled: this->setAfvCrossCoupled(variant.value<bool>()); break;
            case IndexCallsignString:
            case IndexCallsignStringCrossCopuled:
                {
                    const QString cs = variant.toQString();
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

        int CAtcStation::comparePropertyByIndex(const CPropertyIndex &index, const CAtcStation &compareValue) const
        {
            if (index.isMyself()) { return this->getCallsign().comparePropertyByIndex(CPropertyIndexRef::empty(), compareValue.getCallsign()); }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexBookedFrom:  return Compare::compare(this->getBookedFromUtc(), compareValue.getBookedFromUtc());
            case IndexBookedUntil: return Compare::compare(this->getBookedUntilUtc(), compareValue.getBookedUntilUtc());
            case IndexCallsignString:
            case IndexCallsignStringCrossCopuled:
                return m_callsign.comparePropertyByIndex(CPropertyIndexRef::empty(), compareValue.getCallsign());
            case IndexCallsign:    return m_callsign.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getCallsign());
            case IndexController:  return m_controller.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getController());
            case IndexFrequency:   return m_frequency.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getFrequency());
            case IndexIsOnline:    return Compare::compare(this->isOnline(), compareValue.isOnline());
            case IndexLatitude:    return this->latitude().comparePropertyByIndex(index.copyFrontRemoved(), compareValue.latitude());
            case IndexLongitude:   return this->longitude().comparePropertyByIndex(index.copyFrontRemoved(), compareValue.longitude());
            case IndexPosition:    return m_position.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getPosition());
            case IndexRange:       return m_range.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getRange());
            case IndexIsInRange:   return Compare::compare(this->isInRange(), compareValue.isInRange());
            case IndexAtis:        return m_atis.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getAtis());
            case IndexMetar:       return m_metar.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getMetar());
            case IndexIsAfvCrossCoupled:   return Compare::compare(this->isAfvCrossCoupled(), compareValue.isAfvCrossCoupled());
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
    } // namespace
} // namespace
