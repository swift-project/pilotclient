//  SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
//  SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "flightplanaircraftinfo.h"

SWIFT_DEFINE_VALUEOBJECT_MIXINS(swift::misc::aviation, CFlightPlanAircraftInfo)

namespace swift::misc::aviation
{
    CFlightPlanAircraftInfo::CFlightPlanAircraftInfo(const CAircraftIcaoCode &aircraftIcao, const CComNavEquipment &comNavEquipment,
                                                     const CSsrEquipment &ssrEquipment, const CWakeTurbulenceCategory &wtc) : m_aircraftIcao(aircraftIcao),
                                                                                                                              m_comNavEquipment(comNavEquipment),
                                                                                                                              m_ssrEquipment(ssrEquipment),
                                                                                                                              m_wtc(wtc) {}

    CFlightPlanAircraftInfo::CFlightPlanAircraftInfo(QString equipmentCodeAndAircraft)
    {
        equipmentCodeAndAircraft = equipmentCodeAndAircraft.trimmed().toUpper().replace(" ", "");
        const int numberSlash = equipmentCodeAndAircraft.count("/");
        const int numberHypen = equipmentCodeAndAircraft.count("-");
        if (numberHypen == 1 && numberSlash == 2)
        {
            parseIcaoEquipmentCode(equipmentCodeAndAircraft);
        }
        else if (numberSlash >= 1 && numberSlash <= 2 && numberHypen == 0)
        {
            parseFaaEquipmentCode(equipmentCodeAndAircraft);
        }
        else
        {
            parseUnknownEquipmentCode(equipmentCodeAndAircraft);
        }
    }

    QString CFlightPlanAircraftInfo::asIcaoString() const
    {
        // Avoid returning empty wake turbulence categories and send it to the server.
        // This can in particular happen when translating from FAA codes to ICAO codes.
        // This sets a default  wake turbulence category of MEDIUM if the category is unknown otherwise.
        QChar wtc;
        if (!m_wtc.isUnknown())
        {
            wtc = m_wtc.toQString().at(0);
        }
        else
        {
            wtc = CWakeTurbulenceCategory(CWakeTurbulenceCategory::MEDIUM).toQString().at(0);
        }

        return m_aircraftIcao.getDesignator() % "/" % wtc % "-" % m_comNavEquipment.toQString() % "/" % m_ssrEquipment.toQString();
    }

    QString CFlightPlanAircraftInfo::asFaaString() const
    {
        // TCAS prefix (T/) is not used
        QString s;
        if (m_wtc.isCategory(CWakeTurbulenceCategory::HEAVY))
        {
            s = "H/";
        }
        else if (m_wtc.isCategory(CWakeTurbulenceCategory::SUPER))
        {
            s = "J/";
        }
        s += m_aircraftIcao.getDesignator() % "/" % equipmentToFaaCode(m_comNavEquipment, m_ssrEquipment);
        return s;
    }

    CAircraftIcaoCode CFlightPlanAircraftInfo::getAircraftIcao() const
    {
        return m_aircraftIcao;
    }

    CComNavEquipment CFlightPlanAircraftInfo::getComNavEquipment() const
    {
        return m_comNavEquipment;
    }

    CSsrEquipment CFlightPlanAircraftInfo::getSsrEquipment() const
    {
        return m_ssrEquipment;
    }

    CWakeTurbulenceCategory CFlightPlanAircraftInfo::getWtc() const
    {
        return m_wtc;
    }

    QString CFlightPlanAircraftInfo::convertToQString(bool) const
    {
        return asIcaoString();
    }

    void CFlightPlanAircraftInfo::parseIcaoEquipmentCode(const QString &equipment)
    {
        // Example: B789/H-SDE1E2E3FGHIJ2J3J4J5M1RWXY/LB1D1
        QStringList firstSplit = equipment.split("/");
        Q_ASSERT_X(firstSplit.size() == 3, Q_FUNC_INFO, "Cannot split string as required for the ICAO format");
        if (!CAircraftIcaoCode::isValidDesignator(firstSplit[0]) || firstSplit[1].isEmpty() || firstSplit[2].isEmpty())
        {
            return; // Invalid equipment code, leave everything default initialized
        }

        m_aircraftIcao = CAircraftIcaoCode(firstSplit[0]);

        try
        {
            m_ssrEquipment = CSsrEquipment(firstSplit[2]);
        }
        catch (const std::invalid_argument &)
        {
            m_ssrEquipment = CSsrEquipment();
        }

        QStringList secondSplit = firstSplit[1].split("-");
        if (secondSplit.size() != 2)
        {
            return; // Invalid code, leave everything else default initialized
        }

        if (!secondSplit[0].isEmpty())
        {
            try
            {
                // if the wake turbulence category incorrectly contains more than one letter
                // just take the first letter
                m_wtc = CWakeTurbulenceCategory(secondSplit[0].at(0));
            }
            catch (std::invalid_argument &)
            {
                m_wtc = CWakeTurbulenceCategory();
            }
        }
        else
        {
            m_wtc = CWakeTurbulenceCategory();
        }

        try
        {
            m_comNavEquipment = CComNavEquipment(secondSplit[1]);
        }
        catch (std::runtime_error &)
        {
            m_comNavEquipment = CComNavEquipment();
        }
    }

    void CFlightPlanAircraftInfo::parseFaaEquipmentCode(const QString &equipment)
    {
        // Example: H/A346/L
        QStringList split = equipment.split('/');
        Q_ASSERT_X(split.size() == 2 || split.size() == 3, Q_FUNC_INFO, "Cannot split string as required for the FAA format");
        bool missingEquipmentCode = false;

        if (CAircraftIcaoCode::isValidDesignator(split.at(split.size() - 2)))
        {
            m_aircraftIcao = CAircraftIcaoCode(split.at(split.size() - 2));
        }
        else if (CAircraftIcaoCode::isValidDesignator(split.at(split.size() - 1)))
        {
            // the equipment code is missing (like J/A388)
            m_aircraftIcao = CAircraftIcaoCode(split.at(split.size() - 1));
            missingEquipmentCode = true;
        }
        else
        {
            m_aircraftIcao = CAircraftIcaoCode();
        }

        // Check prefix (wake turbulence category)
        if (split.length() == 3)
        {
            const QString &prefix = split.at(0);
            if (prefix == "H" || prefix == "J")
            {
                m_wtc = CWakeTurbulenceCategory(prefix.at(0));
            }
        }
        else if (split.length() == 2 && split.at(0).size() == 1)
        {
            // the equipment code is missing (like J/A388)
            m_wtc = CWakeTurbulenceCategory(split.at(0).at(0));
            missingEquipmentCode = true;
        }
        else
        {
            m_wtc = CWakeTurbulenceCategory();
        }

        // Equipment Code
        if (missingEquipmentCode || split.at(split.size() - 1).isEmpty())
        {
            return; // No (empty) equipment code
        }

        // Always taking the first character. If the code contains more than one character, this is likely an error, but we will try it anyway
        QChar equipmentCode = split.at(split.size() - 1).at(0);
        auto [comNavEquipment, ssrEquipment] = faaCodeToEquipment(equipmentCode);
        m_comNavEquipment = comNavEquipment;
        m_ssrEquipment = ssrEquipment;
    }

    void CFlightPlanAircraftInfo::parseUnknownEquipmentCode(const QString &equipment)
    {
        // likely one part only
        QStringList split = equipment.split("/");
        if (split[0].length() > 1 && CAircraftIcaoCode::isValidDesignator(split[0]))
        {
            // only ICAO
            m_aircraftIcao = split[0];
        }
        else
        {
            // something invalid. Keep default initialized
        }
    }

    std::tuple<CComNavEquipment, CSsrEquipment> CFlightPlanAircraftInfo::faaCodeToEquipment(QChar equipmentCode)
    {
        CComNavEquipment equip;
        CSsrEquipment ssr;

        // COM/NAV equipment
        if (equipmentCode == 'H' || equipmentCode == 'O' || equipmentCode == 'W')
        {
            equip = CComNavEquipment({ CComNavEquipment::Rvsm }, {});
        }
        else if (equipmentCode == 'Z')
        {
            // PBN might not be the correct translation for "RNAV" but we use it to differentiate the codes
            equip = CComNavEquipment({ CComNavEquipment::Rvsm | CComNavEquipment::Pbn }, {});
        }
        else if (equipmentCode == 'L')
        {
            equip = CComNavEquipment({ CComNavEquipment::Rvsm | CComNavEquipment::Gnss }, {});
        }
        else if (equipmentCode == 'X' || equipmentCode == 'T' || equipmentCode == 'U')
        {
            equip = CComNavEquipment({}, {});
        }
        else if (equipmentCode == 'D' || equipmentCode == 'B' || equipmentCode == 'A')
        {
            equip = CComNavEquipment({ CComNavEquipment::Dme }, {});
        }
        else if (equipmentCode == 'M' || equipmentCode == 'N' || equipmentCode == 'P')
        {
            equip = CComNavEquipment({ CComNavEquipment::Tacan }, {});
        }
        else if (equipmentCode == 'Y' || equipmentCode == 'C' || equipmentCode == 'I')
        {
            // PBN might not be the correct translation for "RNAV" but we use it to differentiate the codes
            equip = CComNavEquipment({ CComNavEquipment::Pbn }, {});
        }
        else if (equipmentCode == 'V' || equipmentCode == 'S' || equipmentCode == 'G')
        {
            equip = CComNavEquipment({ CComNavEquipment::Gnss }, {});
        }

        // SSR equipment
        if (equipmentCode == 'W' || equipmentCode == 'Z' || equipmentCode == 'L' || equipmentCode == 'U' ||
            equipmentCode == 'A' || equipmentCode == 'P' || equipmentCode == 'I' || equipmentCode == 'G')
        {
            ssr = CSsrEquipment::SSrEquipment { CSsrEquipment::ModeAC };
        }
        else if (equipmentCode == 'H' || equipmentCode == 'O' || equipmentCode == 'X' || equipmentCode == 'D' ||
                 equipmentCode == 'M' || equipmentCode == 'Y' || equipmentCode == 'V')
        {
            // "O" corresponds to a failed Mode C transponder.
            // The ICAO format does not contain a separate code for a failed Mode C transponder
            ssr = CSsrEquipment::SSrEquipment { CSsrEquipment::None };
        }
        else if (equipmentCode == 'T' || equipmentCode == 'B' || equipmentCode == 'N' || equipmentCode == 'C' ||
                 equipmentCode == 'S')
        {
            // The ICAO format does not contain a separate code for a general NONE Mode C transponder. We use Mode A instead.
            ssr = CSsrEquipment::SSrEquipment { CSsrEquipment::ModeA };
        }

        return { equip, ssr };
    }

    QChar CFlightPlanAircraftInfo::equipmentToFaaCode(const CComNavEquipment &equip, const CSsrEquipment &ssr)
    {
        if (equip.hasEquipment(CComNavEquipment::Rvsm))
        {
            if (ssr.hasEquipment(CSsrEquipment::None))
            {
                // This could also be 'O' as we cannot differentiate between a failed transponder and a failed Mode C transponder
                return 'H';
            }

            // In the following, do not check the transponder capability, as the FAA codes only work with Mode C and not Mode S transponders
            if (equip.hasEquipment(CComNavEquipment::Gnss))
            {
                return 'L';
            }
            if (equip.hasEquipment(CComNavEquipment::Pbn))
            {
                // PBN is used for RNAV when converting from FAA string to CFlightPlanAircraftInfo
                return 'Z';
            }
            else
            {
                return 'W';
            }
        }
        else
        {
            if (equip.hasEquipment(CComNavEquipment::Gnss))
            {
                if (ssr.hasEquipment(CSsrEquipment::None))
                {
                    return 'V';
                }
                if (ssr.hasEquipment(CSsrEquipment::ModeAC))
                {
                    return 'G';
                }
                else
                {
                    return 'S';
                }
            }
            if (equip.hasEquipment(CComNavEquipment::Tacan))
            {
                if (ssr.hasEquipment(CSsrEquipment::None))
                {
                    return 'M';
                }
                if (ssr.hasEquipment(CSsrEquipment::ModeAC))
                {
                    return 'P';
                }
                else
                {
                    return 'N';
                }
            }
            if (equip.hasEquipment(CComNavEquipment::Pbn))
            {
                // PBN is used for RNAV when converting from FAA string to CFlightPlanAircraftInfo
                if (ssr.hasEquipment(CSsrEquipment::None))
                {
                    return 'Y';
                }
                if (ssr.hasEquipment(CSsrEquipment::ModeAC))
                {
                    return 'I';
                }
                else
                {
                    return 'C';
                }
            }
            if (equip.hasEquipment(CComNavEquipment::Dme))
            {
                if (ssr.hasEquipment(CSsrEquipment::None))
                {
                    return 'D';
                }
                if (ssr.hasEquipment(CSsrEquipment::ModeAC))
                {
                    return 'A';
                }
                else
                {
                    return 'B';
                }
            }

            // No DME
            if (ssr.hasEquipment(CSsrEquipment::None))
            {
                return 'X';
            }
            if (ssr.hasEquipment(CSsrEquipment::ModeAC))
            {
                return 'U';
            }
            else
            {
                return 'T';
            }
        }
    }

} // namespace swift::misc::aviation
