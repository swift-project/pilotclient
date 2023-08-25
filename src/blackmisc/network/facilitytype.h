// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_NETWORK_FACILITYTYPE_H
#define BLACKMISC_NETWORK_FACILITYTYPE_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/valueobject.h"

BLACK_DECLARE_VALUEOBJECT_MIXINS(BlackMisc::Network, CFacilityType)

namespace BlackMisc::Network
{
    //! Value object encapsulating the ATC facility type, e.g. TWR, DEP, APP
    class BLACKMISC_EXPORT CFacilityType : public CValueObject<CFacilityType>
    {
    public:
        //! Login modes
        enum FacilityType
        {
            OBS, /*!< OBS */
            FSS, /*!< FSS */
            DEL, /*!< Delivery */
            GND, /*!< Ground */
            TWR, /*!< Tower */
            APP, /*!< Approach */
            CTR, /*!< Center */
            Unknown /*!< Unknown */
        };

        //! Default constructor.
        CFacilityType() = default;

        //! Constructor
        CFacilityType(FacilityType mode) : m_facilityType(mode) {}

        //! Get type
        FacilityType getFacilityType() const { return m_facilityType; }

        //! Set type
        void setFacilityType(FacilityType type) { m_facilityType = type; }

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

    private:
        FacilityType m_facilityType = Unknown;

        BLACK_METACLASS(
            CFacilityType,
            BLACK_METAMEMBER(facilityType)
        );
    };
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Network::CFacilityType)

#endif // guard
