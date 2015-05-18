/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

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
