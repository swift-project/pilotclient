/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_MIXIN_MIXINICON_H
#define BLACKMISC_MIXIN_MIXINICON_H

#include "blackmisc/icon.h"
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
            CIcons::IconIndex toIcon() const { return IconIndex; }

        private:
            const Derived *derived() const { return static_cast<const Derived *>(this); }
            Derived *derived() { return static_cast<Derived *>(this); }
        };

        /*!
         * When a derived class and a base class both inherit from Mixin::Icon,
         * the derived class uses this macro to disambiguate the inherited members.
         */
#       define BLACKMISC_DECLARE_USING_MIXIN_ICON(DERIVED)      \
            using ::BlackMisc::Mixin::Icon<DERIVED>::toIcon;
    }
} // namespace

#endif // guard
