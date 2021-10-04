/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
            OBS,     /*!< OBS */
            FSS,     /*!< FSS */
            DEL,     /*!< Delivery */
            GND,     /*!< Ground */
            TWR,     /*!< Tower */
            APP,     /*!< Approach */
            CTR,     /*!< Center */
            Unknown  /*!< Unknown */
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
