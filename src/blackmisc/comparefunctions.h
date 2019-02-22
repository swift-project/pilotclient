/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_COMPAREFUNCTIONS_H
#define BLACKMISC_COMPAREFUNCTIONS_H

#include "blackmisc/blackmiscexport.h"
#include <QDateTime>
#include <QtGlobal>

namespace BlackMisc
{
    namespace Compare
    {
        //! Compare bool
        BLACKMISC_EXPORT int compare(bool a, bool b);

        //! Compare int
        BLACKMISC_EXPORT int compare(int a, int b);

        //! Compare uint
        BLACKMISC_EXPORT int compare(uint a, uint b);

        //! Compare qint64
        BLACKMISC_EXPORT int compare(qint64 a, qint64 b);

        //! Compare double
        BLACKMISC_EXPORT int compare(double a, double b);

        //! Compare double
        BLACKMISC_EXPORT int compare(const QDateTime &a, const QDateTime &b);
    } // ns
} // ns

#endif
