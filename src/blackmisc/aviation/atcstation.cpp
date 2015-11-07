/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/aviation/atcstation.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/audio/voiceroom.h"
#include "blackmisc/icon.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/variant.h"

using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Network;
using namespace BlackMisc::Audio;

namespace BlackMisc
{
    namespace Aviation
    {
        CAtcStation::CAtcStation()
        { }

        CAtcStation::CAtcStation(const QString &callsign) : m_callsign(callsign)
        {
            this->m_callsign.setTypeHint(CCallsign::Atc);
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
            this->m_callsign.setTypeHint(CCallsign::Atc);
            if (!this->m_controller.hasValidCallsign() && !callsign.isEmpty())
            {
                this->m_controller.setCallsign(m_callsign);
            }
        }

        bool CAtcStation::hasBookingTimes() const
        {
            return !(this->m_bookedFromUtc.isNull() && this->m_bookedUntilUtc.isNull());
        }

        bool CAtcStation::hasMetar() const
        {
            return this->m_metar.hasMessage();
        }

        QString CAtcStation::getCallsignSuffix() const
        {
            return m_callsign.getSuffix();
        }

        void CAtcStation::setCallsign(const CCallsign &callsign)
        {
            this->m_callsign = callsign;
            this->m_controller.setCallsign(callsign);
        }

        QString CAtcStation::convertToQString(bool i18n) const
        {
            QString s = i18n ?
                        QCoreApplication::translate("Aviation", "ATC station") :
                        "ATC station";
            s.append(' ').append(this->m_callsign.toQString(i18n));

            // position
            s.append(' ').append(this->m_position.toQString(i18n));

            // Online?
            s.append(' ');
            if (this->m_isOnline)
            {
                i18n ? s.append(QCoreApplication::translate("Aviation", "online")) : s.append("online");
            }
            else
            {
                i18n ? s.append(QCoreApplication::translate("Aviation", "offline")) : s.append("offline");
            }

            // controller name
            if (this->m_controller.isValid())
            {
                s.append(' ');
                s.append(this->m_controller.toQString(i18n));
            }

            // frequency
            s.append(' ');
            s.append(this->m_frequency.valueRoundedWithUnit(3,  i18n));

            // ATIS
            if (this->hasAtis())
            {
                s.append(' ');
                s.append(this->m_atis.toQString(i18n));
            }

            // METAR
            if (this->hasMetar())
            {
                s.append(' ');
                s.append(this->m_metar.toQString(i18n));
            }

            // range
            s.append(' ');
            i18n ? s.append(s.append(QCoreApplication::translate("Aviation", "range"))) : s.append("range");
            s.append(": ");
            s.append(this->m_range.toQString(i18n));

            // distance to plane
            if (this->m_distanceToOwnAircraft.isPositiveWithEpsilonConsidered())
            {
                s.append(' ');
                i18n ? s.append(QCoreApplication::translate("Aviation", "distance")) : s.append("distance");
                s.append(' ');
                s.append(this->m_distanceToOwnAircraft.toQString(i18n));
            }

            // from / to
            if (!this->hasBookingTimes()) return s;

            // append from
            s.append(' ');
            i18n ? s.append(s.append(QCoreApplication::translate("Aviation", "from(UTC)"))) : s.append("from(UTC)");
            s.append(": ");
            if (this->m_bookedFromUtc.isNull())
            {
                s.append('-');
            }
            else
            {
                s.append(this->m_bookedFromUtc.toString("yy-MM-dd HH:mm"));
            }

            // append to
            s.append(' ');
            i18n ? s.append(s.append(QCoreApplication::translate("Aviation", "until(UTC)"))) : s.append("to(UTC)");
            s.append(": ");
            if (this->m_bookedFromUtc.isNull())
            {
                s.append('-');
            }
            else
            {
                s.append(this->m_bookedUntilUtc.toString("yy-MM-dd HH:mm"));
            }
            return s;

            // force strings for translation in resource files
            (void)QT_TRANSLATE_NOOP("Aviation", "ATC station");
            (void)QT_TRANSLATE_NOOP("Aviation", "online");
            (void)QT_TRANSLATE_NOOP("Aviation", "offline");
            (void)QT_TRANSLATE_NOOP("Aviation", "from(UTC)");
            (void)QT_TRANSLATE_NOOP("Aviation", "until(UTC)");
            (void)QT_TRANSLATE_NOOP("Aviation", "range");
            (void)QT_TRANSLATE_NOOP("Aviation", "distance");
            (void)QT_TRANSLATE_NOOP("Network", "voiceroom");
        }

        void CAtcStation::setFrequency(const CFrequency &frequency)
        {
            this->m_frequency = frequency;
            this->m_frequency.setUnit(CFrequencyUnit::MHz());
        }

        void CAtcStation::syncronizeControllerData(CAtcStation &otherStation)
        {
            if (this->m_controller == otherStation.getController()) { return; }
            CUser otherController = otherStation.getController();
            this->m_controller.syncronizeData(otherController);
            otherStation.setController(otherController);
        }

        void CAtcStation::syncronizeWithBookedStation(CAtcStation &bookedStation)
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
                        CTime timeDiffBooking(bookedStation.bookedWhen());
                        CTime timeDiffOnline(this->bookedWhen()); // diff to now
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
            this->syncronizeControllerData(bookedStation);
            if (this->hasValidDistance())
            {
                bookedStation.setDistanceToOwnAircraft(this->getDistanceToOwnAircraft());
                bookedStation.setBearingToOwnAircraft(this->getBearingToOwnAircraft());
            }
            else if (bookedStation.hasValidDistance())
            {
                this->setDistanceToOwnAircraft(bookedStation.getDistanceToOwnAircraft());
                this->setBearingToOwnAircraft(bookedStation.getBearingToOwnAircraft());
            }
        }

        bool CAtcStation::isInRange() const
        {
            if (m_range.isNull() || !hasValidDistance()) { return false; }
            return (this->getDistanceToOwnAircraft() <= m_range);
        }

        bool CAtcStation::hasValidBookingTimes() const
        {
            return !this->m_bookedFromUtc.isNull() && this->m_bookedFromUtc.isValid() &&
                   !this->m_bookedUntilUtc.isNull() && this->m_bookedUntilUtc.isValid();
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
            if (this->m_bookedFromUtc > now)  { return false; }
            if (now > this->m_bookedUntilUtc) { return false; }
            return true;
        }

        bool CAtcStation::isComUnitTunedIn25KHz(const CComSystem &comUnit) const
        {
            return comUnit.isActiveFrequencyWithin25kHzChannel(this->getFrequency());
        }

        CTime CAtcStation::bookedWhen() const
        {
            if (!this->hasValidBookingTimes()) { return CTime(0, CTimeUnit::nullUnit()); }
            QDateTime now = QDateTime::currentDateTimeUtc();
            qint64 diffMs;
            if (this->m_bookedFromUtc > now)
            {
                // future
                diffMs = now.msecsTo(this->m_bookedFromUtc);
                return CTime(diffMs / 1000.0, CTimeUnit::s());
            }
            else if (this->m_bookedUntilUtc > now)
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

        CLatitude CAtcStation::latitude() const
        {
            return this->getPosition().latitude();
        }

        CLongitude CAtcStation::longitude() const
        {
            return this->getPosition().longitude();
        }

        const CLength &CAtcStation::geodeticHeight() const
        {
            return this->m_position.geodeticHeight();
        }

        QVector3D CAtcStation::normalVector() const
        {
            return this->m_position.normalVector();
        }

        CVariant CAtcStation::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexBookedFrom:
                return CVariant::from(this->m_bookedFromUtc);
            case IndexBookedUntil:
                return CVariant::from(this->m_bookedUntilUtc);
            case IndexCallsign:
                return this->m_callsign.propertyByIndex(index.copyFrontRemoved());
            case IndexController:
                return this->m_controller.propertyByIndex(index.copyFrontRemoved());
            case IndexFrequency:
                return this->m_frequency.propertyByIndex(index.copyFrontRemoved());
            case IndexIsOnline:
                return CVariant::from(this->m_isOnline);
            case IndexLatitude:
                return this->latitude().propertyByIndex(index.copyFrontRemoved());
            case IndexDistanceToOwnAircraft:
                return this->m_distanceToOwnAircraft.propertyByIndex(index.copyFrontRemoved());
            case IndexLongitude:
                return this->longitude().propertyByIndex(index.copyFrontRemoved());
            case IndexPosition:
                return this->m_position.propertyByIndex(index.copyFrontRemoved());
            case IndexRange:
                return this->m_range.propertyByIndex(index.copyFrontRemoved());
            case IndexIsInRange:
                return CVariant::fromValue(isInRange());
            case IndexAtis:
                return this->m_atis.propertyByIndex(index.copyFrontRemoved());
            case IndexMetar:
                return this->m_metar.propertyByIndex(index.copyFrontRemoved());
            case IndexVoiceRoom:
                return this->m_voiceRoom.propertyByIndex(index.copyFrontRemoved());
            default:
                if (ICoordinateGeodetic::canHandleIndex(index)) { return ICoordinateGeodetic::propertyByIndex(index); }
                return CValueObject::propertyByIndex(index);
            }
        }

        void CAtcStation::setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index)
        {
            if (index.isMyself()) { (*this) = variant.to<CAtcStation>(); return; }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexBookedFrom:
                this->setBookedFromUtc(variant.value<QDateTime>());
                break;
            case IndexBookedUntil:
                this->setBookedUntilUtc(variant.value<QDateTime>());
                break;
            case IndexCallsign:
                this->m_callsign.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexController:
                this->m_controller.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexFrequency:
                this->m_frequency.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexIsOnline:
                this->setOnline(variant.value<bool>());
                break;
            case IndexPosition:
                this->m_position.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexRange:
                this->m_range.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexDistanceToOwnAircraft:
                this->m_distanceToOwnAircraft.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexAtis:
                this->m_atis.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexMetar:
                this->m_metar.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexVoiceRoom:
                this->m_voiceRoom.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            default:
                CValueObject::setPropertyByIndex(variant, index);
                break;
            }
        }

    } // namespace
} // namespace
