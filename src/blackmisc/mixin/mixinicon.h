// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_MIXIN_MIXINICON_H
#define BLACKMISC_MIXIN_MIXINICON_H

#include "blackmisc/icons.h"
#include <QIcon>
#include <QPixmap>
#include <QString>
#include <tuple>

namespace BlackMisc
{
    class CIcon;

    namespace Mixin
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
         * When a derived class and a base class both inherit from Mixin::Icon,
         * the derived class uses this macro to disambiguate the inherited members.
         */
#define BLACKMISC_DECLARE_USING_MIXIN_ICON(DERIVED) \
    using ::BlackMisc::Mixin::Icon<DERIVED>::toIcon;
    }
} // namespace

#endif // guard
