/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/hardware/keyboardkeylist.h"
#include "blackmisc/predicates.h"

namespace BlackMisc
{
    namespace Hardware
    {

        CKeyboardKeyList::CKeyboardKeyList() { }

        CKeyboardKeyList::CKeyboardKeyList(const CSequence<CKeyboardKey> &baseClass) :
            CSequence<CKeyboardKey>(baseClass)
        { }

        void CKeyboardKeyList::registerMetadata()
        {
            qRegisterMetaType<BlackMisc::CSequence<CKeyboardKey>>();
            qDBusRegisterMetaType<BlackMisc::CSequence<CKeyboardKey>>();
            qRegisterMetaType<BlackMisc::CCollection<CKeyboardKey>>();
            qDBusRegisterMetaType<BlackMisc::CCollection<CKeyboardKey>>();
            qRegisterMetaType<CKeyboardKeyList>();
            qDBusRegisterMetaType<CKeyboardKeyList>();
            registerMetaValueType<CKeyboardKeyList>();
        }

    } // namespace
} // namespace
