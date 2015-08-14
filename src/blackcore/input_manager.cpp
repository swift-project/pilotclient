/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "input_manager.h"
#include "blackmisc/input/keyboardkeylist.h"

using namespace BlackInput;
using namespace BlackMisc;
using namespace BlackMisc::Input;

namespace BlackCore
{

    CInputManager::CInputManager(QObject *parent) :
        QObject(parent),
        m_keyboard(std::move(IKeyboard::create(this))),
        m_joystick(std::move(IJoystick::create(this)))
    {
        connect(m_keyboard.get(), &IKeyboard::keyCombinationChanged, this, &CInputManager::ps_processKeyCombinationChanged);
        connect(m_joystick.get(), &IJoystick::buttonCombinationChanged, this, &CInputManager::ps_processButtonCombinationChanged);
    }

    CInputManager *CInputManager::instance()
    {
        static CInputManager instance;
        return &instance;
    }

    void CInputManager::registerAction(const QString &action)
    {
        if (!m_availableActions.contains(action))
        {
            m_availableActions.push_back(action);
            emit hotkeyActionRegistered( { action } );
        }
    }

    void CInputManager::registerRemoteActions(const QStringList &actions)
    {
        for (const auto &action : actions)
        {
            if (!m_availableActions.contains(action))
            {
                m_availableActions.push_back(action);
                emit hotkeyActionRegistered( { action } );
            }
        }
    }

    void CInputManager::unbind(int index)
    {
        auto info = std::find_if (m_boundActions.begin(), m_boundActions.end(), [index] (const BindInfo &info) { return info.m_index == index; });
        if (info != m_boundActions.end())
        {
            m_boundActions.erase(info);
        }
    }

    void CInputManager::ps_changeHotkeySettings()
    {
        m_configuredActions.clear();
        for (CActionHotkey actionHotkey : m_actionHotkeys.get())
        {
            CHotkeyCombination combination = actionHotkey.getCombination();
            if (combination.isEmpty()) continue;

            m_configuredActions.insert(combination, actionHotkey.getAction());
        }
    }

    void CInputManager::ps_processKeyCombinationChanged(const CHotkeyCombination &combination)
    {
        // Merge in the joystick part
        CHotkeyCombination copy(combination);
        copy.setJoystickButtons(m_lastCombination.getJoystickButtons());
        processCombination(copy);
    }

    void CInputManager::ps_processButtonCombinationChanged(const CHotkeyCombination &combination)
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

    void CInputManager::callFunctionsBy(const QString &action, bool isKeyDown)
    {
        if (action.isEmpty()) { return; }
        if(m_actionRelayingEnabled) emit remoteActionFromLocal(action, isKeyDown);

        for (const auto &boundAction : m_boundActions)
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

    void CInputManager::processCombination(const CHotkeyCombination &combination)
    {
        if (m_captureActive)
        {
            if (combination.size() < m_capturedCombination.size())
            {
                emit combinationSelectionFinished(m_capturedCombination);
                m_captureActive = false;
            }
            else
            {
                emit combinationSelectionChanged(combination);
                m_capturedCombination = combination;
            }
        }

        QString previousAction = m_configuredActions.value(m_lastCombination);
        QString action = m_configuredActions.value(combination);
        m_lastCombination = combination;
        callFunctionsBy(previousAction, false);
        callFunctionsBy(action, true);
    }
}
