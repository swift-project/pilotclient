/* Copyright (C) 2019
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "serializer.h"
#include "blackmisc/verify.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/logcategories.h"
#include "blackconfig/buildconfig.h"

using namespace BlackConfig;
using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;

namespace BlackCore
{
    namespace Fsd
    {
        //! \cond

        namespace Private
        {
            // log each issue only once
            static QStringList s_invalidAtcRatings;
            static QStringList s_invalidPilotRatings;
            static QStringList s_invalidSimType;
            static QStringList s_invalidFacilityType;
            static QStringList s_invalidQueryType;

            //! Log unknown message types or ratings
            void logUnknownType(const QString &message)
            {
                if (CBuildConfig::isLocalDeveloperDebugBuild())
                {
                    // developers should record these types and EXPLICITLY exclude them
                    const QByteArray msg = message.toLatin1();
                    BLACK_VERIFY_X(false, Q_FUNC_INFO, msg);
                }

                CLogMessage(CLogCategories::fsd()).info(u"%1. Please report this to the DEVELOPERS!") << message;
            }
        }

        template<>
        QString toQString(const AtcRating &value)
        {
            switch (value)
            {
            case AtcRating::Unknown:        return "0";
            case AtcRating::Observer:       return "1";
            case AtcRating::Student:        return "2";
            case AtcRating::Student2:       return "3";
            case AtcRating::Student3:       return "4";
            case AtcRating::Controller1:    return "5";
            case AtcRating::Controller2:    return "6";
            case AtcRating::Controller3:    return "7";
            case AtcRating::Instructor1:    return "8";
            case AtcRating::Instructor2:    return "9";
            case AtcRating::Instructor3:    return "10";
            case AtcRating::Supervisor:     return "11";
            case AtcRating::Administrator:  return "12";
            }

            Q_UNREACHABLE();
            return "0";
        }

        template<>
        AtcRating fromQString(const QString &str)
        {
            // empty string intentionally ignored, also for UNIT test and 3rd parth networks
            if (str.isEmpty()) return AtcRating::Unknown;

            // valid types
            if (str == "1") return AtcRating::Observer;
            else if (str == "2")  return AtcRating::Student;
            else if (str == "3")  return AtcRating::Student2;
            else if (str == "4")  return AtcRating::Student3;
            else if (str == "5")  return AtcRating::Controller1;
            else if (str == "6")  return AtcRating::Controller2;
            else if (str == "7")  return AtcRating::Controller3;
            else if (str == "8")  return AtcRating::Instructor1;
            else if (str == "9")  return AtcRating::Instructor2;
            else if (str == "10") return AtcRating::Instructor3;
            else if (str == "11") return AtcRating::Supervisor;
            else if (str == "12") return AtcRating::Administrator;

            // we should NOT get here
            if (!Private::s_invalidAtcRatings.contains(str))
            {
                Private::s_invalidAtcRatings.push_back(str);
                const QString msg = QStringLiteral("FSD unknown ATC rating '%1'").arg(str);
                Private::logUnknownType(msg);
            }
            return AtcRating::Unknown;
        }

        template<>
        QString toQString(const PilotRating &value)
        {
            switch (value)
            {
            case PilotRating::Unknown:      return "0";
            case PilotRating::Student:      return "1";
            case PilotRating::VFR:          return "2";
            case PilotRating::IFR:          return "3";
            case PilotRating::Instructor:   return "4";
            case PilotRating::Supervisor:   return "5";
            }

            Q_UNREACHABLE();
            return "0";
        }

        template<>
        PilotRating fromQString(const QString &str)
        {
            // empty string intentionally ignored, also for UNIT test and 3rd parth networks
            if (str.isEmpty()) return PilotRating::Unknown;

            // valid types
            if (str == "0") return PilotRating::Unknown;
            else if (str == "1") return PilotRating::Student;
            else if (str == "2") return PilotRating::VFR;
            else if (str == "3") return PilotRating::IFR;
            else if (str == "4") return PilotRating::Instructor;
            else if (str == "5") return PilotRating::Supervisor;

            // we should NOT get here
            if (!Private::s_invalidPilotRatings.contains(str))
            {
                Private::s_invalidPilotRatings.push_back(str);
                const QString msg = QStringLiteral("FSD Unknown Pilot rating '%1'").arg(str);
                Private::logUnknownType(msg);
            }
            return PilotRating::Unknown;
        }

        template<>
        QString toQString(const SimType &value)
        {
            switch (value)
            {
            case SimType::Unknown:      return "0";
            case SimType::MSFS95:       return "1";
            case SimType::MSFS98:       return "2";
            case SimType::MSCFS:        return "3";
            case SimType::MSFS2000:     return "4";
            case SimType::MSCFS2:       return "5";
            case SimType::MSFS2002:     return "6";
            case SimType::MSCFS3:       return "7";
            case SimType::MSFS2004:     return "8";
            case SimType::MSFSX:        return "9";
            case SimType::XPLANE8:      return "12";
            case SimType::XPLANE9:      return "13";
            case SimType::XPLANE10:     return "14";
            case SimType::XPLANE11:     return "16";
            case SimType::FlightGear:   return "25";
            case SimType::P3Dv1:        return "30";
            case SimType::P3Dv2:        return "30";
            case SimType::P3Dv3:        return "30";
            case SimType::P3Dv4:        return "30";

            // future versions
            case SimType::XPLANE12:
            case SimType::P3Dv5:
            case SimType::MSFS2020:
                return "0";
            }

            Q_UNREACHABLE();
            return "0";
        }

        template<>
        SimType fromQString(const QString &str)
        {
            // empty string intentionally ignored, also for UNIT test and 3rd parth networks
            if (str.isEmpty()) return SimType::Unknown;

            // valid types
            if (str == "0") return SimType::Unknown;
            else if (str == "1")  return SimType::MSFS95;
            else if (str == "2")  return SimType::MSFS98;
            else if (str == "3")  return SimType::MSCFS;
            else if (str == "4")  return SimType::MSFS2000;
            else if (str == "5")  return SimType::MSCFS2;
            else if (str == "6")  return SimType::MSFS2002;
            else if (str == "7")  return SimType::MSCFS3;
            else if (str == "8")  return SimType::MSFS2004;
            else if (str == "9")  return SimType::MSFSX;
            else if (str == "12") return SimType::XPLANE8;
            else if (str == "13") return SimType::XPLANE9;
            else if (str == "14") return SimType::XPLANE10;
            else if (str == "16") return SimType::XPLANE11;
            else if (str == "25") return SimType::FlightGear;
            // Still ambigious which P3D version. No standard defined yet.
            else if (str == "30") return SimType::P3Dv4;

            // we should NOT get here
            if (!Private::s_invalidSimType.contains(str))
            {
                Private::s_invalidSimType.push_back(str);
                const QString msg = QStringLiteral("FSD unknown SimType '%1'").arg(str);
                Private::logUnknownType(msg);
            }
            return SimType::Unknown;
        }

        template<>
        QString toQString(const CFacilityType &value)
        {
            switch (value.getFacilityType())
            {
            case CFacilityType::OBS: return "0";
            case CFacilityType::FSS: return "1";
            case CFacilityType::DEL: return "2";
            case CFacilityType::GND: return "3";
            case CFacilityType::TWR: return "4";
            case CFacilityType::APP: return "5";
            case CFacilityType::CTR: return "6";
            case CFacilityType::Unknown: return {};
            }

            Q_UNREACHABLE();
            return {};
        }

        template<>
        CFacilityType fromQString(const QString &str)
        {
            // empty string intentionally ignored, also for UNIT test and 3rd parth networks
            if (str.isEmpty()) return CFacilityType::Unknown;

            if (str == "0") return CFacilityType::OBS;
            if (str == "1") return CFacilityType::FSS;
            else if (str == "2") return CFacilityType::DEL;
            else if (str == "3") return CFacilityType::GND;
            else if (str == "4") return CFacilityType::TWR;
            else if (str == "5") return CFacilityType::APP;
            else if (str == "6") return CFacilityType::CTR;

            // we should NOT get here
            if (!Private::s_invalidFacilityType.contains(str))
            {
                Private::s_invalidFacilityType.push_back(str);
                const QString msg = QStringLiteral("FSD unknown CFacilityType '%1'");
                Private::logUnknownType(msg);
            }

            return CFacilityType::Unknown;
        }

        template<>
        QString toQString(const ClientQueryType &value)
        {
            switch (value)
            {
            case ClientQueryType::IsValidATC:     return "ATC";
            case ClientQueryType::Capabilities:   return "CAPS";
            case ClientQueryType::Com1Freq:       return "C?";
            case ClientQueryType::RealName:       return "RN";
            case ClientQueryType::Server:         return "SV";
            case ClientQueryType::ATIS:           return "ATIS";
            case ClientQueryType::PublicIP:       return "IP";
            case ClientQueryType::INF:            return "INF";
            case ClientQueryType::FP:             return "FP";
            case ClientQueryType::AircraftConfig: return "ACC";
            case ClientQueryType::Unknown:        return "Unknown query type";
            }

            Q_UNREACHABLE();
            return "Unknown query type";
        }

        template<>
        ClientQueryType fromQString(const QString &str)
        {
            // empty string intentionally ignored, also for UNIT test, 3rd party networks
            if (str.isEmpty()) return ClientQueryType::Unknown;

            // valid queries
            // second part of a $CQ:, e.g. $CQ:DI, $CQ:ATC
            if (str == "ATC")  return ClientQueryType::IsValidATC;
            if (str == "CAPS") return ClientQueryType::Capabilities;
            if (str == "C?")   return ClientQueryType::Com1Freq;
            if (str == "RN")   return ClientQueryType::RealName;
            if (str == "SV")   return ClientQueryType::Server;
            if (str == "ATIS") return ClientQueryType::ATIS;
            if (str == "IP")   return ClientQueryType::PublicIP;
            if (str == "INF")  return ClientQueryType::INF;
            if (str == "FP")   return ClientQueryType::FP;
            if (str == "ACC")  return ClientQueryType::AircraftConfig;

            // intentionally ignored (ATC ONLY)
            // discussion: https://discordapp.com/channels/539048679160676382/539925070550794240/669186795790925848
            if (str == "BC")   return ClientQueryType::Unknown;
            if (str == "BY")   return ClientQueryType::Unknown; // CCP_Break
            if (str == "DI")   return ClientQueryType::Unknown; // CCP_ASRC_DI
            if (str == "DP")   return ClientQueryType::Unknown; // CCP_PushToDepartureList
            if (str == "DR")   return ClientQueryType::Unknown; // CCP_DropTrack
            if (str == "FA")   return ClientQueryType::Unknown;
            if (str == "HC")   return ClientQueryType::Unknown; // CCP_HandoffCancelled
            if (str == "HI")   return ClientQueryType::Unknown; // CCP_NoBreak
            if (str == "HT")   return ClientQueryType::Unknown; // CCP_HandoffTo
            if (str == "ID")   return ClientQueryType::Unknown; // CCP_ASRC_ID
            if (str == "IH")   return ClientQueryType::Unknown; // CCP_IHave
            if (str == "IT")   return ClientQueryType::Unknown; // CCP_StartTrack
            if (str == "PT")   return ClientQueryType::Unknown; // CCP_Pointout
            if (str == "SC")   return ClientQueryType::Unknown;
            if (str == "ST")   return ClientQueryType::Unknown; // CCP_PushStrip
            if (str == "TA")   return ClientQueryType::Unknown;
            if (str == "VT")   return ClientQueryType::Unknown;
            if (str == "VER")  return ClientQueryType::Unknown; // CCP_Version
            if (str == "WH")   return ClientQueryType::Unknown; // CCP_WhoHas
            // -- help
            if (str == "HLP")   return ClientQueryType::Unknown; // CCP_HelpOn
            if (str == "NOHLP") return ClientQueryType::Unknown; // CCP_HelpOff

            // Query types from customized FSD servers
            if (str == "NEWATIS")   return ClientQueryType::Unknown;
            if (str == "NEWINFO")   return ClientQueryType::Unknown; // probably causing the Linux crash

            // we should NOT get here
            if (!Private::s_invalidQueryType.contains(str))
            {
                Private::s_invalidQueryType.push_back(str);
                const QString msg = QStringLiteral("FSD unknown ClientQueryType '%1'").arg(str);
                Private::logUnknownType(msg);
            }

            return ClientQueryType::Unknown;
        }

        template<>
        QString toQString(const FlightType &value)
        {
            switch (value)
            {
            case FlightType::IFR:  return "I";
            case FlightType::VFR:  return "V";
            case FlightType::SVFR: return "S";
            case FlightType::DVFR: return "D";
            }

            Q_UNREACHABLE();
            return {};
        }

        template<>
        FlightType fromQString(const QString &str)
        {
            if (str == QLatin1String("I"))       return FlightType::IFR;
            else if (str == QLatin1String("V"))  return FlightType::VFR;
            else if (str == QLatin1String("S"))  return FlightType::SVFR;
            else if (str == QLatin1String("D"))  return FlightType::DVFR;

            return FlightType::IFR;
        }

        template<>
        QString toQString(const CTransponder::TransponderMode &value)
        {
            switch (value)
            {
            case CTransponder::StateStandby:
                return QStringLiteral("S");
            case CTransponder::ModeMil1:
            case CTransponder::ModeMil2:
            case CTransponder::ModeMil3:
            case CTransponder::ModeMil4:
            case CTransponder::ModeMil5:
            case CTransponder::ModeA:
            case CTransponder::ModeC:
            case CTransponder::ModeS:
                return QStringLiteral("N");
            case CTransponder::StateIdent:
                return QStringLiteral("Y");
            }

            Q_UNREACHABLE();
            return QStringLiteral("S");
        }

        template<>
        CTransponder::TransponderMode fromQString(const QString &str)
        {
            if (str == "S")       return CTransponder::StateStandby;
            else if (str == "N")  return CTransponder::ModeC;
            else if (str == "Y")  return CTransponder::StateIdent;

            return CTransponder::StateStandby;
        }

        template<>
        QString toQString(const Capabilities &value)
        {
            switch (value)
            {
            case Capabilities::None:           return {};
            case Capabilities::AtcInfo:        return "ATCINFO";
            case Capabilities::SecondaryPos:   return "SECPOS";
            case Capabilities::AircraftInfo:   return "MODELDESC";
            case Capabilities::OngoingCoord:   return "ONGOINGCOORD";
            case Capabilities::InterminPos:    return "INTERIMPOS";
            case Capabilities::FastPos:        return "FASTPOS";
            case Capabilities::VisPos:         return "VISUPDATE";
            case Capabilities::Stealth:        return "STEALTH";
            case Capabilities::AircraftConfig: return "ACCONFIG";
            }

            return {};
        }

        template<>
        Capabilities fromQString(const QString &str)
        {
            if (str == "ATCINFO")           return Capabilities::AtcInfo;
            else if (str == "SECPOS")       return Capabilities::SecondaryPos;
            else if (str == "MODELDESC")    return Capabilities::AircraftInfo;
            else if (str == "ONGOINGCOORD") return Capabilities::OngoingCoord;
            else if (str == "INTERIMPOS")   return Capabilities::InterminPos;
            else if (str == "FASTPOS")      return Capabilities::FastPos;
            else if (str == "VISUPDATE")    return Capabilities::VisPos;
            else if (str == "STEALTH")      return Capabilities::Stealth;
            else if (str == "ACCONFIG")     return Capabilities::AircraftConfig;

            return Capabilities::None;
        }

        template<>
        AtisLineType fromQString(const QString &str)
        {
            if (str == "V") return AtisLineType::VoiceRoom;
            else if (str == "Z") return AtisLineType::ZuluLogoff;
            else if (str == "T") return AtisLineType::TextMessage;
            else if (str == "E") return AtisLineType::LineCount;

            return AtisLineType::Unknown;
        }

        //! \endcond

    } // ns
} // ns
