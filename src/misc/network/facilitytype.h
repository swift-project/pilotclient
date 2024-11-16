// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_NETWORK_FACILITYTYPE_H
#define SWIFT_MISC_NETWORK_FACILITYTYPE_H

#include "misc/swiftmiscexport.h"
#include "misc/valueobject.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::network, CFacilityType)

namespace swift::misc::network
{
    //! Value object encapsulating the ATC facility type, e.g. TWR, DEP, APP
    class SWIFT_MISC_EXPORT CFacilityType : public CValueObject<CFacilityType>
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

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

    private:
        FacilityType m_facilityType = Unknown;

        SWIFT_METACLASS(
            CFacilityType,
            SWIFT_METAMEMBER(facilityType));
    };
} // namespace swift::misc::network

Q_DECLARE_METATYPE(swift::misc::network::CFacilityType)

#endif // guard
