//  SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
//  SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "ssrequipment.h"

SWIFT_DEFINE_VALUEOBJECT_MIXINS(swift::misc::aviation, CSsrEquipment)

namespace swift::misc::aviation
{
    CSsrEquipment::CSsrEquipment(SSrEquipment equipment) : m_equipment(equipment)
    {
        if (m_equipment == SSrEquipment()) { m_equipment = None; }
    }

    CSsrEquipment::CSsrEquipment(QString equipment)
    {

        if (equipment.isEmpty()) { return; }

        m_equipment = {}; // Clear default flag

        auto append_equipment_flag_if_exist = [&equipment, this](SsrEquipmentOption flag) {
            QString str = flagToString(flag);
            if (equipment.contains(str))
            {
                equipment = equipment.remove(str);
                m_equipment |= flag;
            }
        };

        append_equipment_flag_if_exist(None);
        append_equipment_flag_if_exist(ModeA);
        append_equipment_flag_if_exist(ModeAC);
        append_equipment_flag_if_exist(ModeSTypeE);
        append_equipment_flag_if_exist(ModeSTypeH);
        append_equipment_flag_if_exist(ModeSTypeI);
        append_equipment_flag_if_exist(ModeSTypeL);
        append_equipment_flag_if_exist(ModeSTypeX);
        append_equipment_flag_if_exist(ModeSTypeP);
        append_equipment_flag_if_exist(ModeSTypeS);
        append_equipment_flag_if_exist(AdsBB1);
        append_equipment_flag_if_exist(AdsBB2);
        append_equipment_flag_if_exist(AdsBU1);
        append_equipment_flag_if_exist(AdsBU2);
        append_equipment_flag_if_exist(AdsBV1);
        append_equipment_flag_if_exist(AdsBV2);
        append_equipment_flag_if_exist(AdsCD1);
        append_equipment_flag_if_exist(AdsCG1);

        if (!equipment.isEmpty() && m_equipment == SSrEquipment())
        {
            // Default if nothing correct is provided
            m_equipment = None;
        }
    }

    QStringList CSsrEquipment::enabledOptions() const
    {
        QStringList list;

        // Append flag (as string) to list if flag exists in current equipment
        auto append_flag_if_exist = [&list, this](SsrEquipmentOption flag) {
            if (m_equipment.testFlag(flag)) list << flagToString(flag);
        };

        append_flag_if_exist(None);
        append_flag_if_exist(ModeA);
        append_flag_if_exist(ModeAC);
        append_flag_if_exist(ModeSTypeE);
        append_flag_if_exist(ModeSTypeH);
        append_flag_if_exist(ModeSTypeI);
        append_flag_if_exist(ModeSTypeL);
        append_flag_if_exist(ModeSTypeX);
        append_flag_if_exist(ModeSTypeP);
        append_flag_if_exist(ModeSTypeS);
        append_flag_if_exist(AdsBB1);
        append_flag_if_exist(AdsBB2);
        append_flag_if_exist(AdsBU1);
        append_flag_if_exist(AdsBU2);
        append_flag_if_exist(AdsBV1);
        append_flag_if_exist(AdsBV2);
        append_flag_if_exist(AdsCD1);
        append_flag_if_exist(AdsCG1);

        return list;
    }

    QString CSsrEquipment::convertToQString(bool) const
    {
        const QString equipmentString = enabledOptions().join("");
        Q_ASSERT_X(!equipmentString.isEmpty(), Q_FUNC_INFO, "Equipment string should not be empty");
        return equipmentString;
    }

    QString CSsrEquipment::flagToString(CSsrEquipment::SSrEquipment flag)
    {
        if (flag == None)
        {
            static const QString q("N");
            return q;
        }
        if (flag == ModeA)
        {
            static const QString q("A");
            return q;
        }
        if (flag == ModeAC)
        {
            static const QString q("C");
            return q;
        }
        if (flag == ModeSTypeE)
        {
            static const QString q("E");
            return q;
        }
        if (flag == ModeSTypeH)
        {
            static const QString q("H");
            return q;
        }
        if (flag == ModeSTypeI)
        {
            static const QString q("I");
            return q;
        }
        if (flag == ModeSTypeL)
        {
            static const QString q("L");
            return q;
        }
        if (flag == ModeSTypeX)
        {
            static const QString q("X");
            return q;
        }
        if (flag == ModeSTypeP)
        {
            static const QString q("P");
            return q;
        }
        if (flag == ModeSTypeS)
        {
            static const QString q("S");
            return q;
        }
        if (flag == AdsBB1)
        {
            static const QString q("B1");
            return q;
        }
        if (flag == AdsBB2)
        {
            static const QString q("B2");
            return q;
        }
        if (flag == AdsBU1)
        {
            static const QString q("U1");
            return q;
        }
        if (flag == AdsBU2)
        {
            static const QString q("U2");
            return q;
        }
        if (flag == AdsBV1)
        {
            static const QString q("V1");
            return q;
        }
        if (flag == AdsBV2)
        {
            static const QString q("V2");
            return q;
        }
        if (flag == AdsCD1)
        {
            static const QString q("D1");
            return q;
        }
        if (flag == AdsCG1)
        {
            static const QString q("G1");
            return q;
        }
        return {};
    }

    QStringList CSsrEquipment::allEquipmentLetters()
    {
        // In order as they appear in the final string
        static const QStringList r(
            { "N", "A", "C", "E", "H", "I", "L", "X", "P", "S", "B1", "B2", "U1", "U2", "V1", "V2", "D1", "G1" });
        return r;
    }

} // namespace swift::misc::aviation
