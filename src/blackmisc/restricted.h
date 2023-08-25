// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_RESTRICTED_H
#define BLACKMISC_RESTRICTED_H

namespace BlackMisc
{
    /*!
     * Restricted<T> is just an empty class, that can only be constructed by the class T.
     */
    template <typename T>
    class Restricted
    {
    private:
        friend T;

        //! Constructor is only available to the template parameter T.
        Restricted() {}
    };
}

#endif //  guard
