//  SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
//  SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef BLACKMISC_AVIATION_COMNAVEQUIPMENT_H
#define BLACKMISC_AVIATION_COMNAVEQUIPMENT_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/valueobject.h"

BLACK_DECLARE_VALUEOBJECT_MIXINS(BlackMisc::Aviation, CComNavEquipment)

namespace BlackMisc::Aviation
{
    //! ICAO flightplan field 10a
    class BLACKMISC_EXPORT CComNavEquipment : public BlackMisc::CValueObject<CComNavEquipment>
    {
    public:
        // QFlag uses int as an underlying type. Hence, the equipment is split up into two enums.
        // Once Qt adds a QFlag64, these can be united.
        // See https://bugreports.qt.io/browse/QTBUG-53178

        //! CPLDC and SATCOM equipment options
        enum CpdlcSatcomEquipmentOption : int
        {
            CpdlcAtn = (1 << 0),
            CpdlcFansHfdl = (1 << 1),
            CpdlcFansVdlA = (1 << 2),
            CpdlcFansVdl2 = (1 << 3),
            CpdlcFansSatcomInmarsat = (1 << 4),
            CpdlcFansSatcomMtsat = (1 << 5),
            CpdlcFansSatcomIridium = (1 << 6),
            AtcSatvoiceInmarsat = (1 << 7),
            AtcSatvoiceMtsat = (1 << 8),
            AtcSatvoiceIridium = (1 << 9),
            CpdlcRcp400 = (1 << 10),
            CpdlcRcp240 = (1 << 11),
            SatvoiceRcp400 = (1 << 12),
        };

        //! COM/NAV equipment options
        enum ComNavEquipmentOption : int
        {
            Standard = (1 << 0),
            Gbas = (1 << 1),
            Lpv = (1 << 2),
            LoranC = (1 << 3),
            Dme = (1 << 4),
            FmcAcars = (1 << 5),
            DFisAcars = (1 << 6),
            PdcAcars = (1 << 7),
            Adf = (1 << 8),
            Gnss = (1 << 9),
            HfRtf = (1 << 10),
            InertiaNavigation = (1 << 11),
            Mls = (1 << 12),
            Ils = (1 << 13),
            NoEquip = (1 << 14),
            Vor = (1 << 15),
            Pbn = (1 << 16),
            Tacan = (1 << 17),
            UhfRtf = (1 << 18),
            VhfRtf = (1 << 19),
            Rvsm = (1 << 20),
            Mnps = (1 << 21),
            Vhf833 = (1 << 22),
            Other = (1 << 23)
        };

        Q_DECLARE_FLAGS(ComNavEquipment, ComNavEquipmentOption)
        Q_DECLARE_FLAGS(CpdlcSatcomEquipment, CpdlcSatcomEquipmentOption)

        //! Create default equipment with Standard COM/NAV
        CComNavEquipment() = default;

        //! Create object with given COM/NAV, CPDLC and SATCOM equipment
        CComNavEquipment(ComNavEquipment comNavEquipment, CpdlcSatcomEquipment cpdlcSatcomEquipment);

        //! Create object from an ICAO equipment string (for example "SDE2E3FGHIJ1RWXY")
        explicit CComNavEquipment(QString equipment);

        //! Get all possible equipment code letters
        static QStringList allEquipmentLetters();

        //! @{
        //! Does this object contains \p equip?
        bool hasEquipment(ComNavEquipmentOption equip) const { return m_equipment.testFlag(equip); }
        bool hasEquipment(CpdlcSatcomEquipmentOption equip) const { return m_cpdlcSatcomEquipment.testFlag(equip); }
        //! @}

        //! Get all enabled equipment codes of this object as a list
        QStringList enabledOptions() const;

        //! Get the equipment string of this object (for example "SDE2E3FGHIJ1RWXY")
        QString convertToQString(bool i18n = false) const;

    private:
        //! @{
        //! Get the string for the specific equipment
        static QString flagToString(CpdlcSatcomEquipmentOption flag);
        static QString flagToString(ComNavEquipmentOption flag);
        //! @}

        ComNavEquipment m_equipment = Standard;
        CpdlcSatcomEquipment m_cpdlcSatcomEquipment;

        BLACK_METACLASS(
            CComNavEquipment,
            BLACK_METAMEMBER(equipment),
            BLACK_METAMEMBER(cpdlcSatcomEquipment)
        );
    };
}

Q_DECLARE_METATYPE(BlackMisc::Aviation::CComNavEquipment)

#endif // BLACKMISC_AVIATION_COMNAVEQUIPMENT_H
