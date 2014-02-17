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

        void CKeyboardKeyList::initAsHotkeyList()
        {
            this->clear();
            this->push_back(CKeyboardKey(QChar::Null, CKeyboardKey::ModifierNone, CKeyboardKey::ModifierNone, CKeyboardKey::HotkeyPtt));
            this->push_back(CKeyboardKey(QChar::Null, CKeyboardKey::ModifierNone, CKeyboardKey::ModifierNone, CKeyboardKey::HotkeyOpacity50));
            this->push_back(CKeyboardKey(QChar::Null, CKeyboardKey::ModifierNone, CKeyboardKey::ModifierNone, CKeyboardKey::HotkeyOpacity100));
            this->push_back(CKeyboardKey(QChar::Null, CKeyboardKey::ModifierNone, CKeyboardKey::ModifierNone, CKeyboardKey::HotkeyToggleCom1));
            this->push_back(CKeyboardKey(QChar::Null, CKeyboardKey::ModifierNone, CKeyboardKey::ModifierNone, CKeyboardKey::HotkeyToggleCom2));
        }

    } // namespace
} // namespace
