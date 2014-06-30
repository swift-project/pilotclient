/* Copyright (C) 2013 VATSIM Community / contributors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#ifndef NOMINMAX
#define NOMINMAX
#endif

#include "fsuipc.h"
#include <Windows.h>
// bug in FSUIPC_User.h, windows.h not included, so we have to import it first
#include "FSUIPC/FSUIPC_User.h"

#include "blacksim/fscommon/bcdconversions.h"
#include <QDebug>
#include <QLatin1Char>
#include <QDateTime>

using namespace BlackSim::FsCommon;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackMisc::Geo;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackSimPlugin
{
    namespace Fsx
    {

        CFsuipc::CFsuipc() : m_connected(false), m_validReadValues(false)
        { }

        CFsuipc::~CFsuipc()
        {
            this->disconnect();
        }

        bool CFsuipc::connect()
        {
            DWORD result;
            this->m_lastErrorMessage = "";
            if (this->m_connected) return this->m_connected; // already connected
            this->m_validReadValues = false;
            if (FSUIPC_Open(SIM_ANY, &result))
            {
                this->m_connected = true;
                int simIndex = static_cast<int>(FSUIPC_FS_Version);
                QString sim(
                    (simIndex >= 0 && simIndex < CFsuipc::simulators().size()) ?
                    CFsuipc::simulators().at(simIndex) :
                    "Unknown FS");
                QString ver("%1.%2.%3.%4%5");
                ver = ver.arg(QLatin1Char(48 + (0x0f & (FSUIPC_Version >> 28))))
                      .arg(QLatin1Char(48 + (0x0f & (FSUIPC_Version >> 24))))
                      .arg(QLatin1Char(48 + (0x0f & (FSUIPC_Version >> 20))))
                      .arg(QLatin1Char(48 + (0x0f & (FSUIPC_Version >> 16))))
                      .arg((FSUIPC_Version & 0xffff) ? "a" + (FSUIPC_Version & 0xff) - 1 : "");
                this->m_fsuipcVersion = QString("FSUIPC %1 (%2)").arg(ver).arg(sim);
                // KB_REMOVE: Remove this later
                qDebug() << "FSUIPC connected" << this->m_fsuipcVersion;
            }
            else
            {
                this->m_connected = false;
                int index = static_cast<int>(result);
                this->m_lastErrorMessage = CFsuipc::errorMessages().at(index);
                // KB_REMOVE: Remove this later
                qDebug() << "FSUIPC" << this->m_lastErrorMessage;
            }
            return this->m_connected;
        }

        void CFsuipc::disconnect()
        {
            FSUIPC_Close(); // Closing when it wasn't open is okay, so this is safe here
            this->m_connected = false;
            this->m_validReadValues = false;
        }

        void CFsuipc::process()
        {
            if (!this->m_connected) return;
            this->read();
        }

        void CFsuipc::read()
        {
            DWORD dwResult;
            char localFsTimeRaw[3];
            char modelNameRaw[256];
            qint16 com1ActiveRaw;
            qint16 com2ActiveRaw;
            qint16 com1StandbyRaw;
            qint16 com2StandbyRaw;
            qint16 transponderCodeRaw;
            qint8 xpdrModeSb3Raw;
            qint8 xpdrIdentSb3Raw;
            qint32 groundspeedRaw;
            qint32 pitchRaw;
            qint32 bankRaw;
            qint32 headingRaw;
            qint64 altitudeRaw;
            qint32 groundAltitudeRaw;
            qint64 latitudeRaw;
            qint64 longitudeRaw;

            // http://www.projectmagenta.com/all-fsuipc-offsets/
            // https://www.ivao.aero/softdev/ivap/fsuipc_sdk.asp
            // http://squawkbox.ca/doc/sdk/fsuipc.php

            if (FSUIPC_Read(0x0238, 3, localFsTimeRaw, &dwResult) &&

                    // COM settings
                    FSUIPC_Read(0x034e, 2, &com1ActiveRaw, &dwResult) &&
                    FSUIPC_Read(0x3118, 2, &com2ActiveRaw, &dwResult) &&
                    FSUIPC_Read(0x311a, 2, &com1StandbyRaw, &dwResult) &&
                    FSUIPC_Read(0x311c, 2, &com2StandbyRaw, &dwResult) &&
                    FSUIPC_Read(0x0354, 2, &transponderCodeRaw, &dwResult) &&

                    // COM Settings, transponder, SB3
                    FSUIPC_Read(0x7b91, 1, &xpdrModeSb3Raw, &dwResult) &&
                    FSUIPC_Read(0x7b92, 1, &xpdrIdentSb3Raw, &dwResult) &&

                    // Speeds, situation
                    FSUIPC_Read(0x02b4, 4, &groundspeedRaw, &dwResult) &&
                    FSUIPC_Read(0x0578, 4, &pitchRaw, &dwResult) &&
                    FSUIPC_Read(0x057c, 4, &bankRaw, &dwResult) &&
                    FSUIPC_Read(0x0580, 4, &headingRaw, &dwResult) &&
                    FSUIPC_Read(0x0570, 8, &altitudeRaw, &dwResult) &&

                    // Position
                    FSUIPC_Read(0x0560, 8, &latitudeRaw, &dwResult) &&
                    FSUIPC_Read(0x0568, 8, &longitudeRaw, &dwResult) &&
                    FSUIPC_Read(0x0020, 4, &groundAltitudeRaw, &dwResult) &&

                    // model name
                    FSUIPC_Read(0x3d00, 256, &modelNameRaw, &dwResult) &&

                    // If we wanted other reads/writes at the same time, we could put them here
                    FSUIPC_Process(&dwResult))
            {
                this->m_validReadValues = true;

                // time, basically as a heartbeat
                QString fsTime;
                fsTime.sprintf("%02d:%02d:%02d", localFsTimeRaw[0], localFsTimeRaw[1], localFsTimeRaw[2]);

                // model
                const QString modelName = QString(modelNameRaw); // to be used to distinguish offsets for different models
                m_model.setModelString(modelName);

                // COMs
                CComSystem com1 = this->m_aircraft.getCom1System();
                CComSystem com2 = this->m_aircraft.getCom2System();
                CTransponder xpdr = this->m_aircraft.getTransponder();

                // 2710 => 12710 => / 100.0 => 127.1
                com1ActiveRaw = (10000 + CBcdConversions::bcd2Dec(com1ActiveRaw));
                com2ActiveRaw = (10000 + CBcdConversions::bcd2Dec(com2ActiveRaw));
                com1StandbyRaw = (10000 + CBcdConversions::bcd2Dec(com1StandbyRaw));
                com2StandbyRaw = (10000 + CBcdConversions::bcd2Dec(com2StandbyRaw));
                com1.setFrequencyActiveMHz(com1ActiveRaw / 100.0);
                com2.setFrequencyActiveMHz(com2ActiveRaw / 100.0);
                com1.setFrequencyStandbyMHz(com1StandbyRaw / 100.0);
                com2.setFrequencyStandbyMHz(com2StandbyRaw / 100.0);

                transponderCodeRaw = CBcdConversions::bcd2Dec(transponderCodeRaw);
                xpdr.setTransponderCode(transponderCodeRaw);
                // Mode by SB3
                if (xpdrIdentSb3Raw > 0)
                {
                    // TODO: Reset value
                    xpdr.setTransponderMode(CTransponder::StateIdent);
                }
                else
                {
                    xpdr.setTransponderMode(
                        xpdrModeSb3Raw == 0 ? CTransponder::ModeC : CTransponder::StateStandby
                    );
                }
                this->m_aircraft.setCockpit(com1, com2, xpdr);

                // position
                const double latCorrectionFactor = 90.0 / (10001750.0 * 65536.0 * 65536.0);
                const double lonCorrectionFactor = 360.0 / (65536.0 * 65536.0 * 65536.0 * 65536.0);
                CAircraftSituation situation = this->m_aircraft.getSituation();
                CCoordinateGeodetic position = situation.getPosition();
                CLatitude lat(latitudeRaw * latCorrectionFactor, CAngleUnit::deg());
                CLongitude lon(longitudeRaw * lonCorrectionFactor, CAngleUnit::deg());
                CLength groundAltitude(groundAltitudeRaw / 256.0, CLengthUnit::m());
                position.setLatitude(lat);
                position.setLongitude(lon);
                position.setHeight(groundAltitude);
                situation.setPosition(position);

                // speeds, situation
                const double angleCorrectionFactor = 360.0 / 65536.0 / 65536.0; // see FSUIPC docu
                CAngle pitch = CAngle(pitchRaw * angleCorrectionFactor, CAngleUnit::deg());
                CAngle bank = CAngle(bankRaw * angleCorrectionFactor, CAngleUnit::deg());
                CHeading heading = CHeading(headingRaw * angleCorrectionFactor, CHeading::True, CAngleUnit::deg());
                CSpeed groundspeed(groundspeedRaw / 65536.0, CSpeedUnit::m_s());
                CAltitude altitude(altitudeRaw / (65536.0 * 65536.0), CAltitude::MeanSeaLevel, CLengthUnit::m());
                situation.setBank(bank);
                situation.setHeading(heading);
                situation.setPitch(pitch);
                situation.setGroundspeed(groundspeed);
                situation.setAltitude(altitude);
                this->m_aircraft.setSituation(situation);

                // qDebug() << m_aircraft;
            }
            else
            {
                this->m_validReadValues = false;
            }
        }

        double CFsuipc::intToFractional(double fractional)
        {
            double f = fractional / 10.0;
            if (f < 1.0) return f;
            return intToFractional(f);
        }
    }
}
