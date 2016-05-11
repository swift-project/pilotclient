/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "actionhotkeylist.h"

#include "blackmisc/input/actionhotkeylist.h"
#include "blackmisc/input/hotkeycombination.h"

namespace BlackMisc
{
    namespace Input
    {
        CActionHotkeyList::CActionHotkeyList(const CSequence<CActionHotkey> &baseClass) :
            CSequence<CActionHotkey>(baseClass)
        { }

        CActionHotkeyList CActionHotkeyList::findSubsetsOf(const CActionHotkey &other)
        {
            CActionHotkeyList subsets;
            for (const auto &actionHotkey : *this)
            {
                if (actionHotkey.getCombination().isSubsetOf(other.getCombination()))
                {
                    subsets.push_back(actionHotkey);
                }
            }
            return subsets;
        }

        CActionHotkeyList CActionHotkeyList::findSupersetsOf(const CActionHotkey &other)
        {
            CActionHotkeyList supersets;
            for (const auto &actionHotkey : *this)
            {
                if (other.getCombination().isSubsetOf(actionHotkey.getCombination()))
                {
                    supersets.push_back(actionHotkey);
                }
            }
            return supersets;
        }
    }
}
