/* Copyright (C) 2015
* swift project Community / Contributors
*
* This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
* directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
* including this file, may be copied, modified, propagated, or distributed except according to the terms
* contained in the LICENSE file.
*/

#include "lockfree.h"

namespace BlackMisc
{
    namespace Private
    {
        // this is only needed for GCC <= 4.9, but we define it for all compilers to avoid warning about empty cpp file
        QMutex *atomicSharedPtrMutex()
        {
            static QMutex mutex(QMutex::Recursive);
            return &mutex;
        }
    }
};
