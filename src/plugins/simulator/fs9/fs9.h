// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_SIMPLUGIN_FS9_FS9SDK_H
#define SWIFT_SIMPLUGIN_FS9_FS9SDK_H

#include <array>

#include <QByteArray>
#include <QString>

#ifndef NOMINMAX
#    define NOMINMAX
#endif
#include <dplay8.h>

namespace swift::simplugin::fs9
{
    //! FS9 SDK
    class CFs9Sdk
    {
    public:
        //! Engine type
        enum EngineType
        {
            ENGINE_TYPE_PISTON = 0,
            ENGINE_TYPE_JET = 1,
            ENGINE_TYPE_NONE = 2,
            ENGINE_TYPE_HELO_TURBINE = 3,
            ENGINE_TYPE_TURBOPROP = 5
        };

        //! Multiplayer packet id
        enum MULTIPLAYER_PACKET_ID
        {
            MULTIPLAYER_PACKET_ID_BASE = 0x1000,
            MULTIPLAYER_PACKET_ID_PARAMS = MULTIPLAYER_PACKET_ID_BASE,
            MULTIPLAYER_PACKET_ID_CHANGE_PLAYER_PLANE = 0x1003,
            MULTIPLAYER_PACKET_ID_POSITION_SLEWMODE = 0x100C,
            MULTIPLAYER_PACKET_ID_POSITION_VELOCITY = 0x100D,
            MPCHAT_PACKET_ID_CHAT_TEXT_SEND = 0x1017,
        };

        //! Get FS9 application GUID
        static GUID guid()
        {
            return { 0x0808caa5, 0xe62c, 0x4691, { 0x89, 0x57, 0x5d, 0x45, 0x24, 0xb9, 0x22, 0xda } };
        }

        //! Returns the FS9 pitch multiplier
        static double pitchMultiplier() { return 256.0 / 90.0; }

        //! Return the FS9 bank multiplier
        static double bankMultiplier() { return 512.0 / 180.0; }

        //! Returns the FS9 heading multiplier
        static double headingMultiplier() { return 1024.0 / 360.0; }

        //! Max Path
        static double maxPath() { return 260; }
    };

    //! Multiplayer hacket header
    struct MULTIPLAYER_PACKET_HEADER
    {
        quint32 packet_id = 0; //!< Packet type
        quint32 data_size = 0; //!< Message size
    };

    /*!
     * Multiplayer packet aircraft parameter
     *
     * Most of the data is still unknown and needs to be further
     * reverse enginered.
     *
     * Data most likely to be part of it:
     * \li Wing animation
     * \li Flaps 0 == up, extending flaps starts from 255 and is decreasing. Cessna FULL(228), BOEING 747 Full (242)
     *
     * Max value for fully extended gear seems to be 0xA1
     */
    struct MULTIPLAYER_PACKET_PARAMS
    {
        quint32 application_time = 0; //!< Application time - ignored
        quint32 packet_index = 0; //!< Packet index
        quint8 unknown8 = 0; //!< Unknown data
        quint8 unknown9 = 0; //!< Unknown data
        quint8 flaps_left = 0; //!< Flaps left
        quint8 flaps_right = 0; //!< Flaps right
        quint8 unknown12 = 0; //!< Unknown data
        quint8 unknown13 = 0; //!< Unknown data
        quint8 unknown14 = 0; //!< Engine RPM?
        quint8 unknown15 = 0; //!< Unknown data
        quint8 unknown16 = 0; //!< Unknown data
        quint8 unknown17 = 0; //!< Unknown data
        quint8 unknown18 = 0; //!< Unknown data
        quint8 unknown19 = 0; //!< Unknown data
        quint8 gear_center = 0; //!< Gear center
        quint8 gear_left = 0; //!< Gear left
        quint8 gear_right = 0; //!< Gear right
        quint8 engine_1 = 0; //!< Engine 1 N1 in %
        quint8 engine_2 = 0; //!< Engine 2 N1 in %
        quint8 unknown25 = 0; //!< Unknown data
        quint8 unknown26 = 0; //!< Unknown data
        quint8 unknown27 = 0; //!< Unknown data
    };

    //! Multiplayer chat message
    struct MULTIPLAYER_PACKET_CHAT_TEXT
    {
        QString chat_data; //!< Chat message
    };

    //! Multiplayer player aircraft name packet
    struct MULTIPLAYER_PACKET_CHANGE_PLAYER_PLANE
    {
        CFs9Sdk::EngineType engine; //!< Engine type
        QString aircraft_name; //!< Aircraft model name
    };

    //! Reduced multiplayer position packet
    struct REDUCED_LLAPBH_DATA
    {
        quint32 pbh = 0; //!< Struct pitch/bank/heading
        qint32 lat_i = 0; //!< Latitude value - integer part
        qint32 lon_hi = 0; //!< Longitude value - integer part
        qint32 alt_i = 0; //!< Altitude value - integer part
        quint16 lat_f = 0; //!< Latitude value - decimal part
        quint16 lon_lo = 0; //!< Longitude value - decimal part
        quint16 alt_f = 0; //!< Altitude value - decimal part
    };

    //! Multiplayer position packet in slew mode
    struct MULTIPLAYER_PACKET_POSITION_SLEWMODE
    {
        quint32 application_time = 0; //!< Application time - ignored
        quint32 packet_index = 0; //!< Packet index
        std::array<quint8, 4> reserved = std::array<quint8, 4> { { 0, 0, 0, 0 } }; //!< Reserved
        quint32 pbh = 0; //!< Pitch/Bank/Heading
        qint32 lat_i = 0; //!< Latitude - integer
        qint32 lon_hi = 0; //!< Longitude - integer
        qint32 alt_i = 0; //!< Altitude - integer
        quint16 lat_f = 0; //!< Latitude - fraction
        quint16 lon_lo = 0; //!< Longitude - fraction
        quint16 alt_f = 0; //!< Altitude - fraction
        std::array<quint8, 2> unknown = std::array<quint8, 2> { { 0, 0 } }; //!< Unknown packet
    };

    //! Full multiplayer position and velocity packet
    struct MULTIPLAYER_PACKET_POSITION_VELOCITY
    {
        quint32 packet_index = 0; //!< Packet index
        quint32 application_time = 0; //!< Application time - ignored
        qint32 lat_velocity = 0; //!< Latitude velocity
        qint32 lon_velocity = 0; //!< Longitude velocity
        qint32 alt_velocity = 0; //!< Altitude velocity
        quint32 ground_velocity = 0; //!< Ground velocity
        std::array<quint8, 4> reserved = std::array<quint8, 4> { { 0, 0, 0, 0 } }; //!< Reserved
        quint32 pbh = 0; //!< Pitch/Bank/Heading
        qint32 lat_i = 0; //!< Latitude - integer
        qint32 lon_hi = 0; //!< Longitude - integer
        qint32 alt_i = 0; //!< Altitude - integer
        quint16 lat_f = 0; //!< Latitude - fraction
        quint16 lon_lo = 0; //!< Longitude - fraction
        quint16 alt_f = 0; //!< Altitude - fraction
    };

    //! Player info
    struct PLAYER_INFO_STRUCT
    {
        //! Player info flags
        enum PLAYER_INFO_FLAGS
        {
            OBSERVER = 0x0001,
            PARAMS_RECV = 0x0002,
            PARAMS_SEND = 0x0004
        };

        quint32 dwFlags = 0; //!< Player flags
        char szAircraft[MAX_PATH + 1]; //!< Aircraft model type
    };

    //! Connect attempt info
    struct CONNECT_ATTEMPT_INFO
    {
        DWORD dwPlayerInfoFlags = 0; //!< Player info flags
        DWORD dwNameOffset = 0; //!< Name offset
        DWORD dwNameSize = 0; //!< Name size
        DWORD dwAircraftOffset = 0; //!< Aircraft offset
        DWORD dwAircraftSize = 0; //!< Aircraft size
        DWORD dwEngineType = 0; //!< Engine type
    };

    //! Pitch/Bank/Heading
    union FS_PBH
    {
        unsigned int pbh = 0; //!< Pitch/Bank/Heading as integer value

        //! PBH
        struct
        {
            unsigned int unused : 1; //!< unused bit
            unsigned int onground : 1; //!< Onground flag
            unsigned int hdg : 10; //!< Heading
            int bank : 10; //!< Bank
            int pitch : 10; //!< Pitch
        };
    };
} // namespace swift::simplugin::fs9

#endif // SWIFT_SIMPLUGIN_FS9_FS9SDK_H
