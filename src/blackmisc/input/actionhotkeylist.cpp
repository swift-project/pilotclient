/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "actionhotkeylist.h"

#include "blackmisc/input/actionhotkeylist.h"
#include "blackmisc/input/hotkeycombination.h"

namespace BlackMisc::Input
{
    CActionHotkeyList::CActionHotkeyList(const CSequence<CActionHotkey> &baseClass) :
        CSequence<CActionHotkey>(baseClass)
    { }

    CActionHotkeyList CActionHotkeyList::findSubsetsOf(const CActionHotkey &other) const
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

    CActionHotkeyList CActionHotkeyList::findSupersetsOf(const CActionHotkey &other) const
    {
        CActionHotkeyList supersets;
        for (const CActionHotkey &actionHotkey : *this)
        {
            if (other.getCombination().isSubsetOf(actionHotkey.getCombination()))
            {
                supersets.push_back(actionHotkey);
            }
        }
        return supersets;
    }

    CActionHotkeyList CActionHotkeyList::findBySameMachine(const CActionHotkey &key) const
    {
        CActionHotkeyList sameMachineKeys;
        for (const CActionHotkey &actionHotkey : *this)
        {
            if (!actionHotkey.isForSameMachineId(key)) { continue; }
            sameMachineKeys.push_back(actionHotkey);
        }
        return sameMachineKeys;
    }

    bool CActionHotkeyList::containsAction(const QString &action) const
    {
        return this->contains(&CActionHotkey::getAction, action);
    }

    void CActionHotkeyList::updateToCurrentMachine()
    {
        const CIdentifier comparison("comparison for local machine");
        for (CActionHotkey &actionHotkey : *this)
        {
            // to avoid issue we always update
            actionHotkey.updateToCurrentMachine();

            // const bool sameMachine = actionHotkey.getIdentifier().hasSameMachineNameOrId(comparison);
            // if (sameMachine) { actionHotkey.updateToCurrentMachine(); }
        }
    }
} // ns
