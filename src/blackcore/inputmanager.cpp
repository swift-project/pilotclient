/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/inputmanager.h"
#include "blackmisc/compare.h"
#include "blackmisc/dictionary.h"
#include "blackmisc/input/actionhotkey.h"
#include "blackmisc/range.h"
#include "blackmisc/sequence.h"

#include <limits.h>
#include <QtGlobal>

// clazy:excludeall=detaching-member

using namespace BlackInput;
using namespace BlackMisc;
using namespace BlackMisc::Input;

namespace BlackCore
{
    CInputManager::CInputManager(QObject *parent) :
        QObject(parent)
    {
        reloadHotkeySettings();
    }

    void CInputManager::registerAction(const QString &action, const QPixmap &icon)
    {
        if (!m_availableActions.contains(action))
        {
            m_availableActions.insert(action, icon);
            emit hotkeyActionRegistered({ action });
        }
    }

    void CInputManager::registerRemoteActions(const QStringList &actions)
    {
        for (const auto &action : actions)
        {
            if (!m_availableActions.contains(action))
            {
                m_availableActions.insert(action, {});
                emit hotkeyActionRegistered({ action });
            }
        }
    }

    void CInputManager::unbind(int index)
    {
        auto info = std::find_if(m_boundActions.begin(), m_boundActions.end(), [index](const BindInfo & info) { return info.m_index == index; });
        if (info != m_boundActions.end())
        {
            m_boundActions.erase(info);
        }
    }

    void CInputManager::reloadHotkeySettings()
    {
        m_configuredActions.clear();
        for (const CActionHotkey &actionHotkey : m_actionHotkeys.getThreadLocal())
        {
            if (!actionHotkey.getApplicableMachine().isFromLocalMachine()) { continue; }
            CHotkeyCombination combination = actionHotkey.getCombination();
            if (combination.isEmpty()) continue;

            m_configuredActions.insert(combination, actionHotkey.getAction());
        }
    }

    void CInputManager::processKeyCombinationChanged(const CHotkeyCombination &combination)
    {
        // Merge in the joystick part
        CHotkeyCombination copy(combination);
        copy.setJoystickButtons(m_lastCombination.getJoystickButtons());
        processCombination(copy);
    }

    void CInputManager::processButtonCombinationChanged(const CHotkeyCombination &combination)
    {
        // Merge in the keyboard keys
        CHotkeyCombination copy(combination);
        copy.setKeyboardKeys(m_lastCombination.getKeyboardKeys());
        processCombination(copy);
    }

    void CInputManager::startCapture()
    {
        m_captureActive = true;
        m_capturedCombination = {};
    }

    void CInputManager::callFunctionsBy(const QString &action, bool isKeyDown, bool shouldEmit)
    {
        if (action.isEmpty()) { return; }
        if (m_actionRelayingEnabled && shouldEmit) { emit remoteActionFromLocal(action, isKeyDown); }

        for (const auto &boundAction : as_const(m_boundActions))
        {
            if (boundAction.m_action == action)
            {
                boundAction.m_function(isKeyDown);
            }
        }
    }

    void CInputManager::triggerKey(const CHotkeyCombination &combination, bool isPressed)
    {
        Q_UNUSED(isPressed)
        QString previousAction = m_configuredActions.value(m_lastCombination);
        QString action = m_configuredActions.value(combination);
        callFunctionsBy(previousAction, false);
        callFunctionsBy(action, true);
        m_lastCombination = combination;
    }

    void CInputManager::createDevices()
    {
        m_keyboard = IKeyboard::create(this);
        m_joystick = IJoystick::create(this);
        connect(m_keyboard.get(), &IKeyboard::keyCombinationChanged, this, &CInputManager::processKeyCombinationChanged, Qt::QueuedConnection);
        connect(m_joystick.get(), &IJoystick::buttonCombinationChanged, this, &CInputManager::processButtonCombinationChanged, Qt::QueuedConnection);
    }

    void CInputManager::releaseDevices()
    {
        m_keyboard.reset();
        m_joystick.reset();
    }

    CJoystickButtonList CInputManager::getAllAvailableJoystickButtons() const
    {
        return m_joystick->getAllAvailableJoystickButtons();
    }

    int CInputManager::bindImpl(const QString &action, QObject *receiver, std::function<void (bool)> function)
    {
        static int index = 0;
        Q_ASSERT(index < INT_MAX);
        BindInfo info;
        info.m_index = index;
        ++index;
        info.m_function = function;
        info.m_action = action;
        info.m_receiver = receiver;
        m_boundActions.push_back(info);
        return info.m_index;
    }

    void CInputManager::processCombination(const CHotkeyCombination &currentCombination)
    {
        if (m_captureActive)
        {
            if (currentCombination.size() < m_capturedCombination.size())
            {
                emit combinationSelectionFinished(m_capturedCombination);
                m_captureActive = false;
            }
            else
            {
                emit combinationSelectionChanged(currentCombination);
                m_capturedCombination = currentCombination;
            }
            return;
        }

        for (const CHotkeyCombination &combination : makeKeysRange(as_const(m_configuredActions)))
        {
            QString action = m_configuredActions.value(combination);
            if (combination.isSubsetOf(currentCombination))
            {
                if (!m_activeActions.contains(action))
                {
                    m_activeActions.insert(action);
                    callFunctionsBy(action, true);
                }
            }
            else
            {
                if (m_activeActions.contains(action))
                {
                    callFunctionsBy(action, false);
                    m_activeActions.remove(action);
                }
            }
        }

        m_lastCombination = currentCombination;
    }
}
