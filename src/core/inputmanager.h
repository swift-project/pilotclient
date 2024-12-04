// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_INPUTMANAGER_H
#define SWIFT_CORE_INPUTMANAGER_H

#include <functional>
#include <memory>

#include <QHash>
#include <QObject>
#include <QPointer>
#include <QString>
#include <QStringList>
#include <QVector>

#include "core/application/applicationsettings.h"
#include "core/swiftcoreexport.h"
#include "input/joystick.h"
#include "input/keyboard.h"
#include "misc/icons.h"
#include "misc/input/hotkeycombination.h"
#include "misc/input/joystickbuttonlist.h"
#include "misc/settingscache.h"

namespace swift::core
{
    //! Input manager handling hotkey function calls
    class SWIFT_CORE_EXPORT CInputManager : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor
        CInputManager(QObject *parent = nullptr);

        //! Register new action
        void registerAction(const QString &action,
                            swift::misc::CIcons::IconIndex icon = swift::misc::CIcons::StandardIconEmpty16);

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
        QMap<QString, swift::misc::CIcons::IconIndex> allAvailableActionsAndIcons() const { return m_availableActions; }

        //! Enable event forwarding to core
        void setForwarding(bool enabled) { m_actionRelayingEnabled = enabled; }

        //! Call functions by hotkeyfunction
        void callFunctionsBy(const QString &action, bool isKeyDown, bool shouldEmit = true);

        //! Triggers a key event manually and calls the registered functions.
        void triggerKey(const swift::misc::input::CHotkeyCombination &combination, bool isPressed);

        //! Creates low level input devices. Once completed, hotkeys start to be processed.
        void createDevices();

        //! Releases all devices
        void releaseDevices();

        //! \copydoc swift::input::IJoystick::getAllAvailableJoystickButtons()
        swift::misc::input::CJoystickButtonList getAllAvailableJoystickButtons() const;

    signals:
        //! Event hotkeyfunction occured
        void remoteActionFromLocal(const QString &action, bool argument);

        //! Selected combination has changed
        void combinationSelectionChanged(const swift::misc::input::CHotkeyCombination &combination);

        //! Combination selection has finished
        void combinationSelectionFinished(const swift::misc::input::CHotkeyCombination &combination);

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

        void processKeyCombinationChanged(const swift::misc::input::CHotkeyCombination &combination);
        void processButtonCombinationChanged(const swift::misc::input::CHotkeyCombination &combination);

        //! Bind action
        int bindImpl(const QString &action, QObject *receiver, std::function<void(bool)> function);

        //! Process the hotkey combination
        void processCombination(const swift::misc::input::CHotkeyCombination &combination);

        std::unique_ptr<swift::input::IKeyboard> m_keyboard; //!< keyboard
        std::unique_ptr<swift::input::IJoystick> m_joystick; //!< joystick

        QMap<QString, swift::misc::CIcons::IconIndex> m_availableActions;
        QHash<swift::misc::input::CHotkeyCombination, QString> m_configuredActions;
        QSet<QString> m_activeActions;
        QVector<BindInfo> m_boundActions;

        bool m_actionRelayingEnabled = false;
        bool m_captureActive = false;
        swift::misc::input::CHotkeyCombination m_lastCombination;
        swift::misc::input::CHotkeyCombination m_capturedCombination;
        swift::misc::input::CHotkeyCombination m_combinationBeforeCapture;

        swift::misc::CSetting<application::TActionHotkeys> m_actionHotkeys { this,
                                                                             &CInputManager::reloadHotkeySettings };
    };
} // namespace swift::core

#endif // SWIFT_CORE_INPUTMANAGER_H
