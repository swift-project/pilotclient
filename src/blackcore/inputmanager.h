/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_INPUTMANAGER_H
#define BLACKCORE_INPUTMANAGER_H

#include "blackcore/blackcoreexport.h"
#include "blackcore/application/applicationsettings.h"
#include "blackinput/joystick.h"
#include "blackinput/keyboard.h"
#include "blackmisc/input/hotkeycombination.h"
#include "blackmisc/input/joystickbuttonlist.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/icons.h"

#include <QHash>
#include <QObject>
#include <QPointer>
#include <QString>
#include <QStringList>
#include <QVector>
#include <algorithm>
#include <functional>
#include <memory>

namespace BlackCore
{
    //! Input manager handling hotkey function calls
    class BLACKCORE_EXPORT CInputManager : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor
        CInputManager(QObject *parent = nullptr);

        //! Register new action
        void registerAction(const QString &action, BlackMisc::CIcons::IconIndex icon = BlackMisc::CIcons::StandardIconEmpty16);

        //! Register remote actions
        void registerRemoteActions(const QStringList &actions);

        //! Register a new hotkey function
        //! \remark RecvObj has to be a QObject
        template <typename RecvObj>
        int bind(const QString &action, RecvObj *receiver, void (RecvObj::*slotPointer)(bool))
        {
            using namespace std::placeholders;
            auto function = std::bind(slotPointer, receiver, _1);
            return bindImpl(action, receiver, function);
        }

        //! Register a new hotkey function
        template <typename Func>
        int bind(const QString &action, QObject *receiver, Func functionObject)
        {
            return bindImpl(action, receiver, functionObject);
        }

        //! Unbind a slot
        void unbind(int index);

        //! Select a key combination as hotkey. This method returns immediatly.
        //! Listen for signals combinationSelectionChanged and combinationSelectionFinished
        //! to retrieve the user input.
        void startCapture();

        //! Deletes all registered hotkeys. Be careful with this method!
        void resetAllActions() { m_configuredActions.clear(); }

        //! Get all available and known actions
        QStringList allAvailableActions() const { return m_availableActions.keys(); }

        //! All actions and their icons (if any)
        QMap<QString, BlackMisc::CIcons::IconIndex> allAvailableActionsAndIcons() const { return m_availableActions; }

        //! Enable event forwarding to core
        void setForwarding(bool enabled) { m_actionRelayingEnabled = enabled; }

        //! Call functions by hotkeyfunction
        void callFunctionsBy(const QString &action, bool isKeyDown, bool shouldEmit = true);

        //! Triggers a key event manually and calls the registered functions.
        void triggerKey(const BlackMisc::Input::CHotkeyCombination &combination, bool isPressed);

        //! Creates low level input devices. Once completed, hotkeys start to be processed.
        void createDevices();

        //! Releases all devices
        void releaseDevices();

        //! \copydoc BlackInput::IJoystick::getAllAvailableJoystickButtons()
        BlackMisc::Input::CJoystickButtonList getAllAvailableJoystickButtons() const;

    signals:
        //! Event hotkeyfunction occured
        void remoteActionFromLocal(const QString &action, bool argument);

        //! Selected combination has changed
        void combinationSelectionChanged(const BlackMisc::Input::CHotkeyCombination &combination);

        //! Combination selection has finished
        void combinationSelectionFinished(const BlackMisc::Input::CHotkeyCombination &combination);

        //! New hotkey action is registered
        void hotkeyActionRegistered(const QStringList &actions);

    private:
        //! Handle to a bound action
        struct BindInfo
        {
            // Using unique int index for identification because std::function does not have a operator==
            int m_index = 0;
            QString m_action;
            QPointer<QObject> m_receiver;
            std::function<void(bool)> m_function;
        };

        //! Reload hotkey settings
        void reloadHotkeySettings();

        void processKeyCombinationChanged(const BlackMisc::Input::CHotkeyCombination &combination);
        void processButtonCombinationChanged(const BlackMisc::Input::CHotkeyCombination &combination);

        //! Bind action
        int bindImpl(const QString &action, QObject *receiver, std::function<void(bool)> function);

        //! Process the hotkey combination
        void processCombination(const BlackMisc::Input::CHotkeyCombination &combination);

        std::unique_ptr<BlackInput::IKeyboard> m_keyboard; //!< keyboard
        std::unique_ptr<BlackInput::IJoystick> m_joystick; //!< joystick

        QMap<QString, BlackMisc::CIcons::IconIndex> m_availableActions;
        QHash<BlackMisc::Input::CHotkeyCombination, QString> m_configuredActions;
        QSet<QString> m_activeActions;
        QVector<BindInfo> m_boundActions;

        bool m_actionRelayingEnabled = false;
        bool m_captureActive = false;
        BlackMisc::Input::CHotkeyCombination m_lastCombination;
        BlackMisc::Input::CHotkeyCombination m_capturedCombination;
        BlackMisc::Input::CHotkeyCombination m_combinationBeforeCapture;

        BlackMisc::CSetting<Application::TActionHotkeys> m_actionHotkeys { this, &CInputManager::reloadHotkeySettings };
    };
} // ns

#endif // guard
