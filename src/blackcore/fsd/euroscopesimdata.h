/* Copyright (C) 2021
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_FSD_EUROSCOPESIMDATA_H
#define BLACKCORE_FSD_EUROSCOPESIMDATA_H

#include "blackcore/fsd/messagebase.h"
#include "blackcore/fsd/enums.h"
#include "blackmisc/aviation/aircraftlights.h"

namespace BlackCore::Fsd
{
    //! Pilot data update broadcast by Euroscope Simulator every second.
    class BLACKCORE_EXPORT EuroscopeSimData : public MessageBase
    {
    public:
        //! Constructor
        EuroscopeSimData(const QString &sender, const QString &model, const QString &livery, quint64 timestamp,
                         double latitude, double longitude, double altitude, double heading, int bank, int pitch,
                         int groundSpeed, bool onGround, double gearPercent, double thrustPercent, const BlackMisc::Aviation::CAircraftLights &lights);

        //! Message converted to tokens
        QStringList toTokens() const;

        //! Construct from tokens
        static EuroscopeSimData fromTokens(const QStringList &tokens);

        //! PDU identifier
        static QString pdu() { return QStringLiteral("SIMDATA"); }

        //! @{
        //! Properties
        QString m_model;
        QString m_livery;
        quint64 m_timestamp = 0;
        double m_latitude = 0;
        double m_longitude = 0;
        double m_altitude = 0;
        double m_heading = 0;
        int m_bank = 0;
        int m_pitch = 0;
        int m_groundSpeed = 0;
        bool m_onGround = false;
        int m_gearPercent = 0;
        int m_thrustPercent = 0;
        BlackMisc::Aviation::CAircraftLights m_lights;
        //! @}

    private:
        EuroscopeSimData();
    };

    //! Equal to operator
    inline bool operator==(const EuroscopeSimData &lhs, const EuroscopeSimData &rhs)
    {
        return lhs.sender() == rhs.sender() &&
               lhs.receiver() == rhs.receiver() &&
               lhs.m_model == rhs.m_model &&
               lhs.m_livery == rhs.m_livery &&
               lhs.m_timestamp == rhs.m_timestamp &&
               qFuzzyCompare(lhs.m_latitude, rhs.m_latitude) &&
               qFuzzyCompare(lhs.m_longitude, rhs.m_longitude) &&
               qFuzzyCompare(lhs.m_altitude, rhs.m_altitude) &&
               qFuzzyCompare(lhs.m_heading, rhs.m_heading) &&
               lhs.m_bank == rhs.m_bank &&
               lhs.m_pitch == rhs.m_pitch &&
               lhs.m_groundSpeed == rhs.m_groundSpeed &&
               lhs.m_onGround == rhs.m_onGround &&
               lhs.m_gearPercent == rhs.m_gearPercent &&
               lhs.m_thrustPercent == rhs.m_thrustPercent &&
               lhs.m_lights == rhs.m_lights;
    }

    //! Not equal to operator
    inline bool operator!=(const EuroscopeSimData &lhs, const EuroscopeSimData &rhs)
    {
        return !(lhs == rhs);
    }
}

#endif // guard
