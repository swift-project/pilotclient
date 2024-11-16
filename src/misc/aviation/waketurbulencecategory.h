// SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef SWIFT_MISC_AVIATION_WAKETURBULENCECATEGORY_H
#define SWIFT_MISC_AVIATION_WAKETURBULENCECATEGORY_H

#include <QString>
#include <QtGlobal>

#include "misc/swiftmiscexport.h"
#include "misc/valueobject.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::aviation, CWakeTurbulenceCategory)

namespace swift::misc::aviation
{
    //! ICAO wake turbulence category
    class SWIFT_MISC_EXPORT CWakeTurbulenceCategory : public CValueObject<CWakeTurbulenceCategory>
    {
    public:
        //! ICAO wake turbulence categories
        enum WakeTurbulenceCategory
        {
            UNKNOWN, //!< required when converting from FAA equipment codes and for some database entries where the correct WTC is not available
            LIGHT, //!< light
            MEDIUM, //!< medium
            HEAVY, //!< heavy
            SUPER //!< super heavy
        };

        //! Create default object with unknown wake turbulence category
        CWakeTurbulenceCategory() = default;

        //! Create object with given wake turbulence category
        CWakeTurbulenceCategory(WakeTurbulenceCategory wtc);

        //! Create object from single wake turbulence category letter
        explicit CWakeTurbulenceCategory(QChar letter);

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! Is the wake turbulence category unknown?
        bool isUnknown() const { return m_wtc == UNKNOWN; }

        //! Is the wake turbulence category of this object the same as \p category?
        bool isCategory(WakeTurbulenceCategory category) const { return m_wtc == category; }

    private:
        WakeTurbulenceCategory m_wtc = UNKNOWN;

        SWIFT_METACLASS(
            CWakeTurbulenceCategory,
            SWIFT_METAMEMBER(wtc));
    };

} // namespace swift::misc::aviation

Q_DECLARE_METATYPE(swift::misc::aviation::CWakeTurbulenceCategory)

#endif // SWIFT_MISC_AVIATION_WAKETURBULENCECATEGORY_H
