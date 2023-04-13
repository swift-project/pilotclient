/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
