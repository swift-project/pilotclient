/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "setkeyboardhotkeylist.h"
#include "predicates.h"

namespace BlackMisc
{
    using namespace Hardware;

    namespace Settings
    {
        /*
         * Constructor
         */
        CSettingKeyboardHotkeyList::CSettingKeyboardHotkeyList() { }

        /*
         * Construct from base class object
         */
        CSettingKeyboardHotkeyList::CSettingKeyboardHotkeyList(const CSequence<CSettingKeyboardHotkey> &baseClass) :
            CSequence<CSettingKeyboardHotkey>(baseClass)
        { }

        /*
         * Contains this hotkey function?
         */
        bool CSettingKeyboardHotkeyList::containsFunction(const CHotkeyFunction &function) const
        {
            return CSequence::contains(&CSettingKeyboardHotkey::getFunction, function);
        }

        /*
         * Key for function
         */
        CSettingKeyboardHotkey CSettingKeyboardHotkeyList::keyForFunction(const CHotkeyFunction &function) const
        {
            return this->findBy(&CSettingKeyboardHotkey::getFunction, function).frontOrDefault();
        }

        /*
         * Register metadata
         */
        void CSettingKeyboardHotkeyList::registerMetadata()
        {
            qRegisterMetaType<BlackMisc::CSequence<CSettingKeyboardHotkey>>();
            qDBusRegisterMetaType<BlackMisc::CSequence<CSettingKeyboardHotkey>>();
            qRegisterMetaType<BlackMisc::CCollection<CSettingKeyboardHotkey>>();
            qDBusRegisterMetaType<BlackMisc::CCollection<CSettingKeyboardHotkey>>();
            qRegisterMetaType<CSettingKeyboardHotkeyList>();
            qDBusRegisterMetaType<CSettingKeyboardHotkeyList>();
        }

        /*
         * Hotkey list
         */
        void CSettingKeyboardHotkeyList::initAsHotkeyList(bool reset)
        {
            if (reset) this->clear();
            if (!this->containsFunction(CHotkeyFunction::Ptt())) push_back(CSettingKeyboardHotkey(CHotkeyFunction::Ptt()));
            if (!this->containsFunction(CHotkeyFunction::ToggleCom1())) push_back(CSettingKeyboardHotkey(CHotkeyFunction::ToggleCom1()));
            if (!this->containsFunction(CHotkeyFunction::ToggleCom2())) push_back(CSettingKeyboardHotkey(CHotkeyFunction::ToggleCom2()));
            if (!this->containsFunction(CHotkeyFunction::Opacity50())) push_back(CSettingKeyboardHotkey(CHotkeyFunction::Opacity50()));
            if (!this->containsFunction(CHotkeyFunction::Opacity100())) push_back(CSettingKeyboardHotkey(CHotkeyFunction::Opacity100()));
            if (!this->containsFunction(CHotkeyFunction::ToogleWindowsStayOnTop())) push_back(CSettingKeyboardHotkey(CHotkeyFunction::ToogleWindowsStayOnTop()));
        }

    } // namespace
} // namespace
