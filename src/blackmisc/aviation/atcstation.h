/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_ATCSTATION_H
#define BLACKMISC_AVIATION_ATCSTATION_H

#include "blackmisc/aviation/callsign.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/aviation/informationmessage.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/geo/latitude.h"
#include "blackmisc/geo/longitude.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/network/user.h"
#include "blackmisc/pq/frequency.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/time.h"
#include "blackmisc/propertyindexref.h"
#include "blackmisc/valueobject.h"

#include <QDateTime>
#include <QMetaType>
#include <QString>
#include <QVector3D>
#include <array>
#include <tuple>

BLACK_DECLARE_VALUEOBJECT_MIXINS(BlackMisc::Aviation, CAtcStation)

namespace BlackMisc::Aviation
{
    //! Value object encapsulating information about an ATC station.
    class BLACKMISC_EXPORT CAtcStation : public CValueObject<CAtcStation>, public Geo::ICoordinateWithRelativePosition
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexCallsign = CPropertyIndexRef::GlobalIndexCAtcStation,
            IndexCallsignString,
            IndexCallsignStringCrossCopuled,
            IndexController,
            IndexFrequency,
            IndexPosition,
            IndexRange,
            IndexIsInRange,
            IndexIsOnline,
            IndexIsAfvCrossCoupled,
            IndexBookedFrom,
            IndexBookedUntil,
            IndexLatitude,
            IndexLongitude,
            IndexAtis,
            IndexMetar
        };

        //! Default constructor.
        CAtcStation();

        //! Simplified constructor
        CAtcStation(const QString &callsign);

        //! ATC station constructor
        CAtcStation(const CCallsign &callsign, const Network::CUser &controller,
                    const PhysicalQuantities::CFrequency &frequency,
                    const Geo::CCoordinateGeodetic &pos, const PhysicalQuantities::CLength &range,
                    bool isOnline = false, const QDateTime &bookedFromUtc = QDateTime(), const QDateTime &bookedUntilUtc = QDateTime(),
                    const CInformationMessage &atis = CInformationMessage(CInformationMessage::ATIS), const CInformationMessage &metar = CInformationMessage(CInformationMessage::METAR));

        //! Has booking times?
        bool hasBookingTimes() const;

        //! Has ATIS?
        bool hasAtis() const { return m_atis.hasMessage(); }

        //! Has METAR?
        bool hasMetar() const;

        //! Get callsign.
        const CCallsign &getCallsign() const { return m_callsign; }

        //! Has callsign?
        bool hasCallsign() const { return !m_callsign.isEmpty(); }

        //! Get callsign as string.
        QString getCallsignAsString() const { return m_callsign.asString(); }

        //! Get callsign
        QString getCallsignAsStringCrossCoupled() const;

        //! Callsign suffix (e.g. TWR)
        QString getCallsignSuffix() const;

        //! Callsign suffix sort order
        int getSuffixSortOrder() const;

        //! Set callsign
        void setCallsign(const CCallsign &callsign);

        //! Get controller
        const BlackMisc::Network::CUser &getController() const { return m_controller; }

        //! Get controller name.
        QString getControllerRealName() const { return m_controller.getRealName(); }

        //! Callsign and controller's name if available
        QString getCallsignAndControllerRealName() const;

        //! Get controller name.
        QString getControllerId() const { return m_controller.getId(); }

        //! Set controller
        void setController(const Network::CUser &controller);

        //! Set controller's name
        void setControllerRealName(const QString &controllerName) { m_controller.setRealName(controllerName); }

        //! Set controller's id
        void setControllerId(const QString &controllerId) { m_controller.setId(controllerId); }

        //! Has valid realname?
        bool hasRealName() const { return m_controller.hasRealName(); }

        //! Has valid id?
        bool hasId() const { return m_controller.hasId(); }

        //! Valid COM frequency
        bool hasValidFrequency() const { return CComSystem::isValidCivilAviationFrequency(this->getFrequency()); }

        //! Get frequency.
        const PhysicalQuantities::CFrequency &getFrequency() const { return m_frequency; }

        //! Set frequency
        void setFrequency(const BlackMisc::PhysicalQuantities::CFrequency &frequency);

        //! Get the position of the center of the controller's area of visibility.
        const Geo::CCoordinateGeodetic &getPosition() const { return m_position; }

        //! Set position
        void setPosition(const BlackMisc::Geo::CCoordinateGeodetic &position) { m_position = position; }

        //! Synchronize controller data
        //! Updates two stations (normally a booked and online ATC station) with complementary data
        void synchronizeControllerData(CAtcStation &otherStation);

        //! Synchronize station data
        //! Updates the two stations (a booked and online ATC station) with complementary data
        //! \pre this object is the online station, the passed station the booked station
        void synchronizeWithBookedStation(CAtcStation &bookedStation);

        //! Get the radius of the controller's area of visibility.
        const PhysicalQuantities::CLength &getRange() const { return m_range; }

        //! Set range
        void setRange(const PhysicalQuantities::CLength &range) { m_range = range; }

        //! In range? If range and distance to own aircraft are not available false
        bool isInRange() const;

        //! Is station online (or just booked)?
        bool isOnline() const { return m_isOnline; }

        //! Set online
        bool setOnline(bool online);

        //! Is AFV cross coupled?
        bool isAfvCrossCoupled() const { return m_isAfvCrossCoupled; }

        //! Set AFV cross coupled
        void setAfvCrossCoupled(bool coupled) { m_isAfvCrossCoupled = coupled; }

        //! Booked date/time if any.
        //! This represents the closest booking within a time frame as there can be multiple bookings.
        const QDateTime &getBookedFromUtc() const { return m_bookedFromUtc; }

        //! Booked date/time if any.
        //! This represents the closest booking within a time frame as there can be multiple bookings.
        const QDateTime &getBookedUntilUtc() const { return m_bookedUntilUtc; }

        //! Has valid booking times?
        bool hasValidBookingTimes() const;

        //! Set booked from
        void setBookedFromUtc(const QDateTime &from) { m_bookedFromUtc = from; }

        //! Transfer booking times
        void setBookedFromUntil(const CAtcStation &otherStation);

        //! Booked now?
        bool isBookedNow() const;

        //! Is Com unit tuned to this stations frequency
        bool isComUnitTunedToFrequency(const Aviation::CComSystem &comUnit) const;

        //! Is passed frequency the frequency of this station
        bool isAtcStationFrequency(const PhysicalQuantities::CFrequency &frequency) const;

        //! When booked, 0 means now,
        //! negative values mean booking in past,
        //! positive values mean booking in future,
        //! no booking dates will result in null time
        PhysicalQuantities::CTime bookedWhen() const;

        //! Get ATIS
        const CInformationMessage &getAtis() const { return m_atis; }

        //! Set ATIS
        void setAtis(const CInformationMessage &atis) { m_atis = atis; }

        //! Set ATIS Message
        void setAtisMessage(const QString &atis) { m_atis.setMessage(atis); }

        //! Get METAR
        const CInformationMessage &getMetar() const { return m_metar; }

        //! Message per type
        const CInformationMessage &getInformationMessage(CInformationMessage::InformationType type) const;

        //! Set METAR
        void setMetar(const CInformationMessage &metar) { m_metar = metar; }

        //! Set METAR Message
        void setMetarMessage(const QString &metar) { m_metar.setMessage(metar); }

        //! Set given message
        bool setMessage(const CInformationMessage &message);

        //! Set booked until
        void setBookedUntilUtc(const QDateTime &until) { m_bookedUntilUtc = until; }

        //! \copydoc Geo::ICoordinateGeodetic::latitude
        virtual Geo::CLatitude latitude() const override;

        //! \copydoc Geo::ICoordinateGeodetic::longitude
        virtual Geo::CLongitude longitude() const override;

        //! \copydoc Geo::ICoordinateGeodetic::geodeticHeight
        const Aviation::CAltitude &geodeticHeight() const override;

        //! \copydoc Geo::ICoordinateGeodetic::normalVector
        virtual QVector3D normalVector() const override;

        //! \copydoc Geo::ICoordinateGeodetic::normalVectorDouble
        virtual std::array<double, 3> normalVectorDouble() const override;

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        QVariant propertyByIndex(CPropertyIndexRef index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc BlackMisc::Mixin::Index::comparePropertyByIndex
        int comparePropertyByIndex(CPropertyIndexRef index, const CAtcStation &compareValue) const;

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! \copydoc BlackMisc::Mixin::Icon::toIcon()
        BlackMisc::CIcons::IconIndex toIcon() const { return m_callsign.toIcon(); }

    private:
        CCallsign m_callsign;
        Network::CUser m_controller;
        PhysicalQuantities::CFrequency m_frequency;
        Geo::CCoordinateGeodetic m_position;
        PhysicalQuantities::CLength m_range;
        bool m_isOnline = false;
        bool m_isAfvCrossCoupled = false;
        QDateTime m_bookedFromUtc;
        QDateTime m_bookedUntilUtc;
        CInformationMessage m_atis { CInformationMessage::ATIS };
        CInformationMessage m_metar { CInformationMessage::METAR };

        BLACK_METACLASS(
            CAtcStation,
            BLACK_METAMEMBER(callsign),
            BLACK_METAMEMBER(controller),
            BLACK_METAMEMBER(frequency),
            BLACK_METAMEMBER(position),
            BLACK_METAMEMBER(range),
            BLACK_METAMEMBER(isOnline),
            BLACK_METAMEMBER(isAfvCrossCoupled),
            BLACK_METAMEMBER(bookedFromUtc),
            BLACK_METAMEMBER(bookedUntilUtc),
            BLACK_METAMEMBER(atis),
            BLACK_METAMEMBER(metar),
            BLACK_METAMEMBER(relativeDistance),
            BLACK_METAMEMBER(relativeBearing)
        );
    };
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAtcStation)

#endif // guard
