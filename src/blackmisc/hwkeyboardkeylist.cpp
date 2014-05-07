/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "hwkeyboardkeylist.h"
#include "predicates.h"

namespace BlackMisc
{
    namespace Hardware
    {
        /*
         * Constructor
         */
        CKeyboardKeyList::CKeyboardKeyList() { }

        /*
         * Construct from base class object
         */
        CKeyboardKeyList::CKeyboardKeyList(const CSequence<CKeyboardKey> &baseClass) :
            CSequence<CKeyboardKey>(baseClass)
        { }

        /*
         * Contains this hotkey function?
         */
        bool CKeyboardKeyList::containsFunction(CKeyboardKey::HotkeyFunction function) const
        {
            return CSequence::contains(&CKeyboardKey::getFunction, function);
        }

        }

        /*
         * Register metadata
         */
        void CKeyboardKeyList::registerMetadata()
        {
            qRegisterMetaType<BlackMisc::CSequence<CKeyboardKey>>();
            qDBusRegisterMetaType<BlackMisc::CSequence<CKeyboardKey>>();
            qRegisterMetaType<BlackMisc::CCollection<CKeyboardKey>>();
            qDBusRegisterMetaType<BlackMisc::CCollection<CKeyboardKey>>();
            qRegisterMetaType<CKeyboardKeyList>();
            qDBusRegisterMetaType<CKeyboardKeyList>();
        }

        /*
         * Hotkey list
         */
        void CKeyboardKeyList::initAsHotkeyList(bool reset)
        {
            if (reset) this->clear();
            if (!this->containsFunction(CKeyboardKey::HotkeyPtt)) this->push_back(CKeyboardKey(CKeyboardKey::HotkeyPtt));
            if (!this->containsFunction(CKeyboardKey::HotkeyOpacity50)) this->push_back(CKeyboardKey(CKeyboardKey::HotkeyOpacity50));
            if (!this->containsFunction(CKeyboardKey::HotkeyOpacity100)) this->push_back(CKeyboardKey(CKeyboardKey::HotkeyOpacity100));
            if (!this->containsFunction(CKeyboardKey::HotkeyToggleCom1)) this->push_back(CKeyboardKey(CKeyboardKey::HotkeyToggleCom1));
            if (!this->containsFunction(CKeyboardKey::HotkeyToggleCom2)) this->push_back(CKeyboardKey(CKeyboardKey::HotkeyToggleCom2));
        }

    } // namespace
} // namespace
