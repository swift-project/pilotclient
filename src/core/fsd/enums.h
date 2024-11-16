// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_FSD_ENUMS_H
#define SWIFT_CORE_FSD_ENUMS_H

#include <QMetaType>

namespace swift::core::fsd
{
    /*! ATC ratings */
    enum class AtcRating
    {
        Unknown, /*!< Unknown */
        Observer, /*!< OBS */
        Student, /*!< S1 */
        Student2, /*!< S2 */
        Student3, /*!< S3 */
        Controller1, /*!< C1 */
        Controller2, /*!< C2 */
        Controller3, /*!< C3 */
        Instructor1, /*!< I1 */
        Instructor2, /*!< I2 */
        Instructor3, /*!< I3 */
        Supervisor, /*!< SUP */
        Administrator /*!< ADM */
    };

    /*! Pilot ratings */
    enum class PilotRating
    {
        Unknown, /*!< Unknown rating */
        Student, /*!< P1 */
        VFR, /*!< P2 */
        IFR, /*!< P3 */
        Instructor, /*!< Instructor */
        Supervisor /*!< SUP */
    };

    /*! Flight simulator type */
    enum class SimType
    {
        Unknown, /*!< Unknown simulator type */
        MSFS95, /*!< MS Flight Simulator 95 */
        MSFS98, /*!< MS Flight Simulator 98 */
        MSCFS, /*!< MS Combat Flight Simulator */
        MSFS2000, /*!< MS Flight Simulator 2000 */
        MSCFS2, /*!< MS Combat Flight Simulator 2 */
        MSFS2002, /*!< MS Flight Simulator 2002 */
        MSCFS3, /*!< MS Combat Flight Simulator 3 */
        MSFS2004, /*!< MS Flight Simulator 2004 */
        MSFSX, /*!< MS Flight Simulator X */
        MSFS, /*!< MS Flight Simulator 2020 */
        XPLANE8, /*!< X-Plane 8 */
        XPLANE9, /*!< X-Plane 9 */
        XPLANE10, /*!< X-Plane 10 */
        XPLANE11, /*!< X-Plane 11 */
        XPLANE12, /*!< X-Plane 12 */
        P3Dv1, /*!< Prepar3D V1 */
        P3Dv2, /*!< Prepar3D V2 */
        P3Dv3, /*!< Prepar3D V3 */
        P3Dv4, /*!< Prepar3D V4 */
        P3Dv5, /*!< Prepar3D V5 */
        FlightGear /*!< Flight Gear */
    };

    //! Client query types
    enum class ClientQueryType
    {
        Unknown, /*!< Unknown client query type */
        IsValidATC, /*!< Is this client working ATC or just an observer. */
        Capabilities, /*!< What capabilities does this client have? */
        Com1Freq, /*!< What is your COM1 Frequency? Response by pilot clients only. */
        RealName, /*!< What is your real-name (and other ATC data) */
        Server, /*!< What server are you on? */
        ATIS, /*!< What is your ATIS? Reponse by ATC clients only. */
        PublicIP, /*!< What is my public IP address? */
        INF, /*!< Supervisor Privileged Information Request. */
        FP, /*!< Send Cached Flight Plan. Response by SERVER. */
        AircraftConfig, /*!< Aircraft Configuration */
        EuroscopeSimData /*!< Broadcast to announce we request SIMDATA packets. */
        // There are many more which are only relevant to ATC clients.
    };

    //! Flight types
    enum class FlightType
    {
        IFR, /*!< IFR flight rules. */
        VFR, /*!< Visual flight rules. */
        SVFR, /*!< Special visual flight rules. */
        DVFR /*!< Defense visual Flight Rules. */
    };

    //! Server error codes
    enum class ServerErrorCode
    {
        NoError, /*!< No error */
        CallsignInUse, /*!< Callsign in use */
        InvalidCallsign, /*!< Invalid callsign */
        AlreadyRegistered, /*!< Already registered */
        SyntaxError, /*!< Syntax error */
        InvalidSrcCallsign, /*!< Invalid source callsign */
        InvalidCidPassword, /*!< Invalid CID/password */
        NoSuchCallsign, /*!< No such callsign */
        NoFlightPlan, //!< No flightplan
        NoWeatherProfile, /*!< No such weather profile */
        InvalidRevision, /*!< Invalid protocol revision */
        RequestedLevelTooHigh, /*!< Requested level too high */
        ServerFull, /*!< Too many clients connected  */
        CidSuspended, /*!< CID/PID was suspended */
        InvalidCtrl, /*!< Not valid control */
        RatingTooLow, /*!< Rating too low for this position */
        InvalidClient, /*!< Unauthorized client software */
        AuthTimeout, /*!< Wrong server type */
        Unknown /*!< Unknown error */
    };

    //! FSD Server type
    enum class ServerType
    {
        LegacyFsd, //!< Legacy FSD
        Vatsim //!< VATSIM server
    };

    //! Client capability flags */
    enum class Capabilities : int
    {
        /*! None. */
        None = (1 << 0),

        /*! Can accept ATIS responses. */
        AtcInfo = (1 << 1),

        /*! Can send/receive secondary visibility center points (ATC/Server only). */
        SecondaryPos = (1 << 2),

        /*!
         * Can send/receive modern model packets.
         *
         * This should be the standard for any new pilot client. Also all older VATSIM clients
         * starting from SB3 do support this capability.
         * Aircraft info contains
         * \li Aircraft ICAO identifier
         * \li Airline ICAO identifier (optional)
         * \li Airline livery (optional)
         */
        AircraftInfo = (1 << 3),

        /*! Can send/receive inter-facility coordination packets (ATC only). */
        OngoingCoord = (1 << 4),

        /*!
         * Can send/receive Interim position updates (pilot only)
         * \deprecated Used only by Squawkbox with high precision errors. Use
         * FastPos instead.
         */
        InterminPos = (1 << 5),

        /*! Can send/receive fast position updates (pilot only). */
        FastPos = (1 << 6),

        /*! Can send/receive visual position updates (pilot only). */
        VisPos = (1 << 7),

        /*! Stealth mode */
        Stealth = (1 << 8),

        /*! Aircraft Config */
        AircraftConfig = (1 << 9),

        /*! Process aircraft ICAO in flightplan as ICAO equipment code (e.g. B737/M-SDE2E3FGHIRWXY/LB1) */
        IcaoEquipment = (1 << 10)
    };

    //! @{
    //! Operators
    inline Capabilities operator|(Capabilities lhs, Capabilities rhs)
    {
        return static_cast<Capabilities>(static_cast<int>(lhs) | static_cast<int>(rhs));
    }

    inline Capabilities &operator|=(Capabilities &lhs, Capabilities rhs)
    {
        lhs = lhs | rhs;
        return lhs;
    }

    inline bool operator&(Capabilities lhs, Capabilities rhs)
    {
        return static_cast<int>(lhs) & static_cast<int>(rhs);
    }
    //! @}

    //! ATIS line type
    enum class AtisLineType
    {
        Unknown,
        VoiceRoom,
        TextMessage,
        ZuluLogoff,
        LineCount
    };
} // namespace swift::core::fsd

Q_DECLARE_METATYPE(swift::core::fsd::AtcRating)
Q_DECLARE_METATYPE(swift::core::fsd::PilotRating)
Q_DECLARE_METATYPE(swift::core::fsd::SimType)
Q_DECLARE_METATYPE(swift::core::fsd::ClientQueryType)
Q_DECLARE_METATYPE(swift::core::fsd::FlightType)
Q_DECLARE_METATYPE(swift::core::fsd::ServerErrorCode)
Q_DECLARE_METATYPE(swift::core::fsd::ServerType)
Q_DECLARE_METATYPE(swift::core::fsd::Capabilities)

#endif // guard
