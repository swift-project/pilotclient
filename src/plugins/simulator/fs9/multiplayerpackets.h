// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef SWIFT_SIMPLUGIN_FS9_MULTIPLAYER_PACKETS_H
#define SWIFT_SIMPLUGIN_FS9_MULTIPLAYER_PACKETS_H

#include <tuple>

#include <QString>
#include <QtGlobal>

#include "fs9.h"

//! \file

namespace swift::simplugin::fs9
{
    //! Multiplayer param packet - aircraft configuration
    struct MPParam : public MULTIPLAYER_PACKET_PARAMS
    {
        //! Return tuple of member variables
        auto getTuple()
        {
            return std::tie(application_time, packet_index,
                            unknown8, unknown9, flaps_left, flaps_right, unknown12, unknown13, unknown14, unknown15,
                            unknown16, unknown17, unknown18, unknown19, gear_center, gear_left, gear_right,
                            engine_1, engine_2, unknown25, unknown26, unknown27);
        }

        //! Return const tuple of member variables
        auto getTuple() const
        {
            return std::tie(application_time, packet_index,
                            unknown8, unknown9, flaps_left, flaps_right, unknown12, unknown13, unknown14, unknown15,
                            unknown16, unknown17, unknown18, unknown19, gear_center, gear_left, gear_right,
                            engine_1, engine_2, unknown25, unknown26, unknown27);
        }

        //! Struct size
        qint32 size() const { return 28; }
    };

    //! Multiplayer packet - change player plane
    struct MPChangePlayerPlane : public MULTIPLAYER_PACKET_CHANGE_PLAYER_PLANE
    {
        //! Return tuple of member variables
        std::tuple<CFs9Sdk::EngineType &, QString &> getTuple()
        {
            return std::tie(engine, aircraft_name);
        }

        //! Return const tuple of member variables
        std::tuple<const CFs9Sdk::EngineType &, const QString &> getTuple() const
        {
            return std::tie(engine, aircraft_name);
        }

        //! Struct size
        qint32 size() const { return sizeof(CFs9Sdk::EngineType) + aircraft_name.size() + 1; }
    };

    //! Multiplayer packet - chat text
    struct MPChatText : public MULTIPLAYER_PACKET_CHAT_TEXT
    {
        //! Return tuple of member variables
        std::tuple<QString &> getTuple()
        {
            return std::tie(chat_data);
        }

        //! Return const tuple of member variables
        std::tuple<const QString &> getTuple() const
        {
            return std::tie(chat_data);
        }

        //! Struct size
        qint32 size() const { return chat_data.size() + 1; }
    };

    //! Multiplayer packet in slew mode
    struct MPPositionSlewMode : public MULTIPLAYER_PACKET_POSITION_SLEWMODE
    {
        //! Return tuple of member variables
        auto getTuple()
        {
            return std::tie(application_time, packet_index,
                            reserved[0], reserved[1], reserved[2], reserved[3],
                            pbh, lat_i, lon_hi, alt_i,
                            lat_f, lon_lo, alt_f);
        }

        //! Return const tuple of member variables
        auto getTuple() const
        {
            return std::tie(application_time, packet_index,
                            reserved[0], reserved[1], reserved[2], reserved[3],
                            pbh, lat_i, lon_hi, alt_i,
                            lat_f, lon_lo, alt_f);
        }

        //! Struct size
        qint32 size() const { return 36; }
    };

    //! Multiplayer packet - position and velocity
    struct MPPositionVelocity : public MULTIPLAYER_PACKET_POSITION_VELOCITY
    {
        //! Return tuple of member variables
        auto getTuple()
        {
            return std::tie(packet_index, application_time, lat_velocity,
                            lon_velocity, alt_velocity, ground_velocity,
                            reserved[0], reserved[1], reserved[2], reserved[3],
                            pbh, lat_i, lon_hi, alt_i, lat_f, lon_lo, alt_f);
        }

        //! Return const tuple of member variables
        auto getTuple() const
        {
            return std::tie(packet_index, application_time, lat_velocity,
                            lon_velocity, alt_velocity, ground_velocity,
                            reserved[0], reserved[1], reserved[2], reserved[3],
                            pbh, lat_i, lon_hi, alt_i, lat_f, lon_lo, alt_f);
        }

        //! Struct size
        qint32 size() const { return 52; }
    };
} // namespace swift::simplugin::fs9

#endif // SWIFT_SIMPLUGIN_FS9_MULTIPLAYER_PACKETS_H
