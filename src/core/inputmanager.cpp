// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "core/inputmanager.h"

#include <limits.h>

#include <QtGlobal>

#include "misc/input/actionhotkey.h"
#include "misc/range.h"
#include "misc/sequence.h"

// clazy:excludeall=detaching-member

using namespace swift::input;
using namespace swift::misc;
using namespace swift::misc::input;

namespace swift::core
{
    CInputManager::CInputManager(QObject *parent) : QObject(parent) { reloadHotkeySettings(); }

    void CInputManager::registerAction(const QString &action, CIcons::IconIndex icon)
    {
        if (!m_availableActions.contains(action))
        {
            m_availableActions.insert(action, icon);
            emit this->hotkeyActionRegistered({ action });
        }
    }

    void CInputManager::registerRemoteActions(const QStringList &actions)
    {
        for (const auto &action : actions)
        {
            if (!m_availableActions.contains(action))
            {
                m_availableActions.insert(action, {});
                emit this->hotkeyActionRegistered({ action });
            }
        }
    }

    void CInputManager::unbind(int index)
    {
        auto info = std::find_if(m_boundActions.begin(), m_boundActions.end(),
                                 [index](const BindInfo &info) { return info.m_index == index; });
        if (info != m_boundActions.end()) { m_boundActions.erase(info); }
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
        // mixed on purpose, as any joystick keyboard combination is possible
        CHotkeyCombination copy(combination);
        copy.setJoystickButtons(m_lastCombination.getJoystickButtons());
        processCombination(copy);
    }

    void CInputManager::processButtonCombinationChanged(const CHotkeyCombination &combination)
    {
        // Merge in the keyboard keys
        // mixed on purpose, as any joystick keyboard combination is possible
        CHotkeyCombination copy(combination);
        copy.setKeyboardKeys(m_lastCombination.getKeyboardKeys());
        processCombination(copy);
    }

    void CInputManager::startCapture()
    {
        m_captureActive = true;
        m_capturedCombination = {};
        m_combinationBeforeCapture = m_lastCombination;
    }

    void CInputManager::callFunctionsBy(const QString &action, bool isKeyDown, bool shouldEmit)
    {
        if (action.isEmpty()) { return; }
        if (m_actionRelayingEnabled && shouldEmit) { emit remoteActionFromLocal(action, isKeyDown); }

        for (const auto &boundAction : std::as_const(m_boundActions))
        {
            if (boundAction.m_action == action) { boundAction.m_function(isKeyDown); }
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
        connect(m_keyboard.get(), &IKeyboard::keyCombinationChanged, this, &CInputManager::processKeyCombinationChanged,
                Qt::QueuedConnection);
        connect(m_joystick.get(), &IJoystick::buttonCombinationChanged, this,
                &CInputManager::processButtonCombinationChanged, Qt::QueuedConnection);
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

    int CInputManager::bindImpl(const QString &action, QObject *receiver, std::function<void(bool)> function)
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
            CHotkeyCombination deltaCombination = currentCombination.getDeltaComparedTo(m_combinationBeforeCapture);

            // Don't continue if there is no relevant combination yet
            if (m_capturedCombination.isEmpty() && deltaCombination.isEmpty()) { return; }

            if (deltaCombination.size() < m_capturedCombination.size())
            {
                emit combinationSelectionFinished(m_capturedCombination);
                m_captureActive = false;
            }
            else
            {
                emit combinationSelectionChanged(deltaCombination);
                m_capturedCombination = deltaCombination;
            }
            return;
        }

        QSet<QString> newActiveActions;
        for (const auto [combination, action] : makePairsRange(std::as_const(m_configuredActions)))
        {
            if (combination.isSubsetOf(currentCombination)) { newActiveActions.insert(action); }
        }

        const QSet<QString> pressedActions = newActiveActions - m_activeActions;
        const QSet<QString> releasedActions = m_activeActions - newActiveActions;
        m_activeActions = newActiveActions;
        for (const QString &action : pressedActions) { callFunctionsBy(action, true); }
        for (const QString &action : releasedActions) { callFunctionsBy(action, false); }

        // combination
        m_lastCombination = currentCombination;
    }
} // namespace swift::core
