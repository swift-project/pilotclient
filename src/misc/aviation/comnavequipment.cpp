//  SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
//  SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "comnavequipment.h"

SWIFT_DEFINE_VALUEOBJECT_MIXINS(swift::misc::aviation, CComNavEquipment)

namespace swift::misc::aviation
{
    CComNavEquipment::CComNavEquipment(ComNavEquipment comNavEquipment, CpdlcSatcomEquipment cpdlcSatcomEquipment)
        : m_equipment(comNavEquipment), m_cpdlcSatcomEquipment(cpdlcSatcomEquipment)
    {
        if (m_equipment == ComNavEquipment()) { m_equipment = NoEquip; }
    }

    CComNavEquipment::CComNavEquipment(QString equipment)
    {
        if (equipment.isEmpty()) { return; }

        m_equipment = {}; // Clear default flag

        auto append_equipment_flag_if_exist = [&equipment, this](ComNavEquipmentOption flag) {
            QString str = flagToString(flag);
            if (equipment.contains(str))
            {
                equipment = equipment.remove(str);
                m_equipment |= flag;
            }
        };

        auto append_satcom_flag_if_exist = [&equipment, this](CpdlcSatcomEquipmentOption flag) {
            QString str = flagToString(flag);
            if (equipment.contains(str))
            {
                equipment = equipment.remove(str);
                m_cpdlcSatcomEquipment |= flag;
            }
        };

        append_equipment_flag_if_exist(Standard);
        append_equipment_flag_if_exist(Gbas);
        append_equipment_flag_if_exist(Lpv);
        append_equipment_flag_if_exist(LoranC);
        append_equipment_flag_if_exist(Dme);
        append_equipment_flag_if_exist(FmcAcars);
        append_equipment_flag_if_exist(DFisAcars);
        append_equipment_flag_if_exist(PdcAcars);
        append_equipment_flag_if_exist(Adf);
        append_equipment_flag_if_exist(Gnss);
        append_equipment_flag_if_exist(HfRtf);
        append_equipment_flag_if_exist(InertiaNavigation);
        append_equipment_flag_if_exist(Mls);
        append_equipment_flag_if_exist(Ils);
        append_equipment_flag_if_exist(NoEquip);
        append_equipment_flag_if_exist(Vor);
        append_equipment_flag_if_exist(Pbn);
        append_equipment_flag_if_exist(Tacan);
        append_equipment_flag_if_exist(UhfRtf);
        append_equipment_flag_if_exist(VhfRtf);
        append_equipment_flag_if_exist(Rvsm);
        append_equipment_flag_if_exist(Mnps);
        append_equipment_flag_if_exist(Vhf833);
        append_equipment_flag_if_exist(Other);
        append_satcom_flag_if_exist(CpdlcAtn);
        append_satcom_flag_if_exist(CpdlcFansHfdl);
        append_satcom_flag_if_exist(CpdlcFansVdlA);
        append_satcom_flag_if_exist(CpdlcFansVdl2);
        append_satcom_flag_if_exist(CpdlcFansSatcomInmarsat);
        append_satcom_flag_if_exist(CpdlcFansSatcomMtsat);
        append_satcom_flag_if_exist(CpdlcFansSatcomIridium);
        append_satcom_flag_if_exist(AtcSatvoiceInmarsat);
        append_satcom_flag_if_exist(AtcSatvoiceMtsat);
        append_satcom_flag_if_exist(AtcSatvoiceIridium);
        append_satcom_flag_if_exist(CpdlcRcp400);
        append_satcom_flag_if_exist(CpdlcRcp240);
        append_satcom_flag_if_exist(SatvoiceRcp400);

        if (!equipment.isEmpty() && m_equipment == ComNavEquipment())
        {
            // Default if nothing correct is provided
            m_equipment = NoEquip;
            m_cpdlcSatcomEquipment = {};
        }
    }

    QStringList CComNavEquipment::enabledOptions() const
    {
        QStringList list;

        auto append_equipment_flag_if_exist = [&list, this](ComNavEquipmentOption flag) {
            if (m_equipment.testFlag(flag)) list << flagToString(flag);
        };

        auto append_satcom_flag_if_exist = [&list, this](CpdlcSatcomEquipmentOption flag) {
            if (m_cpdlcSatcomEquipment.testFlag(flag)) list << flagToString(flag);
        };

        append_equipment_flag_if_exist(Standard);
        append_equipment_flag_if_exist(Gbas);
        append_equipment_flag_if_exist(Lpv);
        append_equipment_flag_if_exist(LoranC);
        append_equipment_flag_if_exist(Dme);
        append_equipment_flag_if_exist(FmcAcars);
        append_equipment_flag_if_exist(DFisAcars);
        append_equipment_flag_if_exist(PdcAcars);
        append_equipment_flag_if_exist(Adf);
        append_equipment_flag_if_exist(Gnss);
        append_equipment_flag_if_exist(HfRtf);
        append_equipment_flag_if_exist(InertiaNavigation);
        append_satcom_flag_if_exist(CpdlcAtn);
        append_satcom_flag_if_exist(CpdlcFansHfdl);
        append_satcom_flag_if_exist(CpdlcFansVdlA);
        append_satcom_flag_if_exist(CpdlcFansVdl2);
        append_satcom_flag_if_exist(CpdlcFansSatcomInmarsat);
        append_satcom_flag_if_exist(CpdlcFansSatcomMtsat);
        append_satcom_flag_if_exist(CpdlcFansSatcomIridium);
        append_equipment_flag_if_exist(Mls);
        append_equipment_flag_if_exist(Ils);
        append_satcom_flag_if_exist(AtcSatvoiceInmarsat);
        append_satcom_flag_if_exist(AtcSatvoiceMtsat);
        append_satcom_flag_if_exist(AtcSatvoiceIridium);
        append_equipment_flag_if_exist(NoEquip);
        append_equipment_flag_if_exist(Vor);
        append_satcom_flag_if_exist(CpdlcRcp400);
        append_satcom_flag_if_exist(CpdlcRcp240);
        append_satcom_flag_if_exist(SatvoiceRcp400);
        append_equipment_flag_if_exist(Pbn);
        append_equipment_flag_if_exist(Tacan);
        append_equipment_flag_if_exist(UhfRtf);
        append_equipment_flag_if_exist(VhfRtf);
        append_equipment_flag_if_exist(Rvsm);
        append_equipment_flag_if_exist(Mnps);
        append_equipment_flag_if_exist(Vhf833);
        append_equipment_flag_if_exist(Other);

        return list;
    }

    QString CComNavEquipment::convertToQString(bool) const
    {
        const QString equipmentString = enabledOptions().join("");
        Q_ASSERT_X(!equipmentString.isEmpty(), Q_FUNC_INFO, "Equipment string should not be empty");
        return equipmentString;
    }

    QString CComNavEquipment::flagToString(CpdlcSatcomEquipmentOption flag)
    {
        switch (flag)
        {
        case CpdlcAtn: return QStringLiteral("J1");
        case CpdlcFansHfdl: return QStringLiteral("J2");
        case CpdlcFansVdlA: return QStringLiteral("J3");
        case CpdlcFansVdl2: return QStringLiteral("J4");
        case CpdlcFansSatcomInmarsat: return QStringLiteral("J5");
        case CpdlcFansSatcomMtsat: return QStringLiteral("J6");
        case CpdlcFansSatcomIridium: return QStringLiteral("J7");
        case AtcSatvoiceInmarsat: return QStringLiteral("M1");
        case AtcSatvoiceMtsat: return QStringLiteral("M2");
        case AtcSatvoiceIridium: return QStringLiteral("M3");
        case CpdlcRcp400: return QStringLiteral("P1");
        case CpdlcRcp240: return QStringLiteral("P2");
        case SatvoiceRcp400: return QStringLiteral("P3");
        default: return {};
        }
    }

    QString CComNavEquipment::flagToString(ComNavEquipmentOption flag)
    {
        switch (flag)
        {
        case Standard: return QStringLiteral("S");
        case Gbas: return QStringLiteral("A");
        case Lpv: return QStringLiteral("B");
        case LoranC: return QStringLiteral("C");
        case Dme: return QStringLiteral("D");
        case FmcAcars: return QStringLiteral("E1");
        case DFisAcars: return QStringLiteral("E2");
        case PdcAcars: return QStringLiteral("E3");
        case Adf: return QStringLiteral("F");
        case Gnss: return QStringLiteral("G");
        case HfRtf: return QStringLiteral("H");
        case InertiaNavigation: return QStringLiteral("I");
        case Mls: return QStringLiteral("K");
        case Ils: return QStringLiteral("L");
        case NoEquip: return QStringLiteral("N");
        case Vor: return QStringLiteral("O");
        case Pbn: return QStringLiteral("R");
        case Tacan: return QStringLiteral("T");
        case UhfRtf: return QStringLiteral("U");
        case VhfRtf: return QStringLiteral("V");
        case Rvsm: return QStringLiteral("W");
        case Mnps: return QStringLiteral("X");
        case Vhf833: return QStringLiteral("Y");
        case Other: return QStringLiteral("Z");
        default: return {};
        }
    }

    QStringList CComNavEquipment::allEquipmentLetters()
    {
        // In order as they appear in the final string
        static const QStringList r({ "S",  "A",  "B",  "C",  "D",  "E1", "E2", "E3", "F",  "G",  "H",  "I", "J1",
                                     "J2", "J3", "J4", "J5", "J6", "J7", "K",  "L",  "M1", "M2", "M3", "N", "O",
                                     "P1", "P2", "P3", "R",  "T",  "U",  "V",  "W",  "X",  "Y",  "Z" });
        return r;
    }

} // namespace swift::misc::aviation
