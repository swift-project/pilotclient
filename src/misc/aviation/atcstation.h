// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_AVIATION_ATCSTATION_H
#define SWIFT_MISC_AVIATION_ATCSTATION_H

#include "misc/aviation/callsign.h"
#include "misc/aviation/comsystem.h"
#include "misc/aviation/informationmessage.h"
#include "misc/swiftmiscexport.h"
#include "misc/geo/coordinategeodetic.h"
#include "misc/geo/latitude.h"
#include "misc/geo/longitude.h"
#include "misc/metaclass.h"
#include "misc/network/user.h"
#include "misc/pq/frequency.h"
#include "misc/pq/length.h"
#include "misc/pq/time.h"
#include "misc/propertyindexref.h"
#include "misc/valueobject.h"

#include <QDateTime>
#include <QMetaType>
#include <QString>
#include <QVector3D>
#include <array>
#include <tuple>

BLACK_DECLARE_VALUEOBJECT_MIXINS(swift::misc::aviation, CAtcStation)

namespace swift::misc::aviation
{
    //! Value object encapsulating information about an ATC station.
    class SWIFT_MISC_EXPORT CAtcStation : public CValueObject<CAtcStation>, public geo::ICoordinateWithRelativePosition
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
            IndexLogoffTime,
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
        CAtcStation(const CCallsign &callsign, const network::CUser &controller,
                    const physical_quantities::CFrequency &frequency,
                    const geo::CCoordinateGeodetic &pos, const physical_quantities::CLength &range,
                    bool isOnline = false, const QDateTime &logoffTimeUtc = QDateTime(),
                    const CInformationMessage &atis = CInformationMessage(CInformationMessage::ATIS), const CInformationMessage &metar = CInformationMessage(CInformationMessage::METAR));

        //! Has expected logoff time?
        bool hasLogoffTimeUtc() const;

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
        const swift::misc::network::CUser &getController() const { return m_controller; }

        //! Get controller name.
        QString getControllerRealName() const { return m_controller.getRealName(); }

        //! Callsign and controller's name if available
        QString getCallsignAndControllerRealName() const;

        //! Get controller name.
        QString getControllerId() const { return m_controller.getId(); }

        //! Set controller
        void setController(const network::CUser &controller);

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
        const physical_quantities::CFrequency &getFrequency() const { return m_frequency; }

        //! Set frequency
        void setFrequency(const swift::misc::physical_quantities::CFrequency &frequency);

        //! Get the position of the center of the controller's area of visibility.
        const geo::CCoordinateGeodetic &getPosition() const { return m_position; }

        //! Set position
        void setPosition(const swift::misc::geo::CCoordinateGeodetic &position) { m_position = position; }

        //! Get the radius of the controller's area of visibility.
        const physical_quantities::CLength &getRange() const { return m_range; }

        //! Set range
        void setRange(const physical_quantities::CLength &range) { m_range = range; }

        //! In range? If range and distance to own aircraft are not available false
        bool isInRange() const;

        //! Is station online?
        bool isOnline() const { return m_isOnline; }

        //! Set online
        bool setOnline(bool online);

        //! Is AFV cross coupled?
        bool isAfvCrossCoupled() const { return m_isAfvCrossCoupled; }

        //! Set AFV cross coupled
        void setAfvCrossCoupled(bool coupled) { m_isAfvCrossCoupled = coupled; }

        //! Return the expected logoff time (UTC). This data comes
        //! from the controller through its ATIS line.
        const QDateTime &getLogoffTimeUtc() const { return m_logoffTimeUtc; }

        //! Is Com unit tuned to this stations frequency
        bool isComUnitTunedToFrequency(const aviation::CComSystem &comUnit) const;

        //! Is passed frequency the frequency of this station
        bool isAtcStationFrequency(const physical_quantities::CFrequency &frequency) const;

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

        //! Set expected logoff time (UTC)
        void setLogoffTimeUtc(const QDateTime &logoffTimeUtc) { m_logoffTimeUtc = logoffTimeUtc; }

        //! \copydoc geo::ICoordinateGeodetic::latitude
        virtual geo::CLatitude latitude() const override;

        //! \copydoc geo::ICoordinateGeodetic::longitude
        virtual geo::CLongitude longitude() const override;

        //! \copydoc geo::ICoordinateGeodetic::geodeticHeight
        const aviation::CAltitude &geodeticHeight() const override;

        //! \copydoc geo::ICoordinateGeodetic::normalVector
        virtual QVector3D normalVector() const override;

        //! \copydoc geo::ICoordinateGeodetic::normalVectorDouble
        virtual std::array<double, 3> normalVectorDouble() const override;

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc swift::misc::mixin::Index::comparePropertyByIndex
        int comparePropertyByIndex(CPropertyIndexRef index, const CAtcStation &compareValue) const;

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! \copydoc swift::misc::mixin::Icon::toIcon()
        swift::misc::CIcons::IconIndex toIcon() const { return m_callsign.toIcon(); }

    private:
        CCallsign m_callsign;
        network::CUser m_controller;
        physical_quantities::CFrequency m_frequency;
        geo::CCoordinateGeodetic m_position;
        physical_quantities::CLength m_range;
        bool m_isOnline = false;
        bool m_isAfvCrossCoupled = false;
        QDateTime m_logoffTimeUtc;
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
            BLACK_METAMEMBER(logoffTimeUtc),
            BLACK_METAMEMBER(atis),
            BLACK_METAMEMBER(metar),
            BLACK_METAMEMBER(relativeDistance),
            BLACK_METAMEMBER(relativeBearing)
        );
    };
} // namespace

Q_DECLARE_METATYPE(swift::misc::aviation::CAtcStation)

#endif // guard
