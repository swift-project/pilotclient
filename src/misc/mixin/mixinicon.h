// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_MIXIN_MIXINICON_H
#define SWIFT_MISC_MIXIN_MIXINICON_H

#include "misc/icons.h"
#include <QIcon>
#include <QPixmap>
#include <QString>
#include <tuple>

namespace swift::misc
{
    class CIcon;

    namespace mixin
    {
        /*!
         * CRTP class template from which a derived class can inherit icon-related functions.
         */
        template <class Derived, CIcons::IconIndex IconIndex = CIcons::StandardIconUnknown16>
        class Icon
        {
        public:
            //! As icon, not implemented by all classes
            CIcons::IconIndex toIcon() const;

        private:
            const Derived *derived() const;
            Derived *derived();
        };

        template <class Derived, CIcons::IconIndex IconIndex>
        CIcons::IconIndex Icon<Derived, IconIndex>::toIcon() const
        {
            return IconIndex;
        }

        template <class Derived, CIcons::IconIndex IconIndex>
        const Derived *Icon<Derived, IconIndex>::derived() const
        {
            return static_cast<const Derived *>(this);
        }

        template <class Derived, CIcons::IconIndex IconIndex>
        Derived *Icon<Derived, IconIndex>::derived()
        {
            return static_cast<Derived *>(this);
        }

        /*!
         * When a derived class and a base class both inherit from mixin::Icon,
         * the derived class uses this macro to disambiguate the inherited members.
         */
#define SWIFT_MISC_DECLARE_USING_MIXIN_ICON(DERIVED) \
    using ::swift::misc::mixin::Icon<DERIVED>::toIcon;
    }
} // namespace

#endif // guard
