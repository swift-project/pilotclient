//  SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
//  SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef SWIFT_MISC_AVIATION_SSREQUIPMENT_H
#define SWIFT_MISC_AVIATION_SSREQUIPMENT_H

#include "misc/swiftmiscexport.h"
#include "misc/valueobject.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::aviation, CSsrEquipment)

namespace swift::misc::aviation
{
    //! ICAO flightplan field 10b
    class SWIFT_MISC_EXPORT CSsrEquipment : public swift::misc::CValueObject<CSsrEquipment>
    {
    public:
        //! Surveillance equipment options
        enum SsrEquipmentOption : int
        {
            None = (1 << 0),
            ModeA = (1 << 1),
            ModeAC = (1 << 2),
            ModeSTypeE = (1 << 3),
            ModeSTypeH = (1 << 4),
            ModeSTypeI = (1 << 5),
            ModeSTypeL = (1 << 6),
            ModeSTypeX = (1 << 7),
            ModeSTypeP = (1 << 8),
            ModeSTypeS = (1 << 9),
            AdsBB1 = (1 << 10),
            AdsBB2 = (1 << 11),
            AdsBU1 = (1 << 12),
            AdsBU2 = (1 << 13),
            AdsBV1 = (1 << 14),
            AdsBV2 = (1 << 15),
            AdsCD1 = (1 << 16),
            AdsCG1 = (1 << 17)
        };
        Q_DECLARE_FLAGS(SSrEquipment, SsrEquipmentOption)

        //! Create default SSR equipment with "None" equipment enabled
        CSsrEquipment() = default;

        //! Create object with given equipment
        CSsrEquipment(SSrEquipment equipment);

        //! Create object from an ICAO SSR equipment string (for example "LB1")
        explicit CSsrEquipment(QString equipment);

        //! Get all possible SSR equipment code letters
        static QStringList allEquipmentLetters();

        //! Get all enabled SSR equipment codes of this object as a list
        QStringList enabledOptions() const;

        //! Get the SSR equipment string of this object (for example "LB1")
        QString convertToQString(bool i18n = false) const;

        //! Does this object contains \p equip?
        bool hasEquipment(SsrEquipmentOption equip) const { return m_equipment.testFlag(equip); }

    private:
        //! Get the string for the specific SSR equipment
        static QString flagToString(SSrEquipment flag);

        SSrEquipment m_equipment = None;

        SWIFT_METACLASS(
            CSsrEquipment,
            SWIFT_METAMEMBER(equipment));
    };

} // namespace swift::misc::aviation

Q_DECLARE_METATYPE(swift::misc::aviation::CSsrEquipment)

#endif // SWIFT_MISC_AVIATION_SSREQUIPMENT_H
