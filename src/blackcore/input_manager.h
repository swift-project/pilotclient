/* Copyright (C) 2013 VATSIM Community / contributors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_INPUTMANAGER_H
#define BLACKCORE_INPUTMANAGER_H

#include "blackcoreexport.h"
#include "blackcore/settings/application.h"
#include "blackinput/keyboard.h"
#include "blackinput/joystick.h"
#include "blackmisc/input/hotkeycombination.h"
#include <QObject>
#include <QHash>
#include <QVector>
#include <type_traits>
#include <functional>

//! \file

namespace BlackCore
{
    //! Input manager handling hotkey function calls
    class BLACKCORE_EXPORT CInputManager : public QObject
    {
        Q_OBJECT

    public:
        //! Register new action
        void registerAction(const QString &action);

        //! Register remote actions
        void registerRemoteActions(const QStringList &actions);

        //! Register a new hotkey function
        template <typename RecvObj>
        int bind(const QString &action, RecvObj *receiver, void (RecvObj:: *slotPointer)(bool))
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

        //!
        //! Select a key combination as hotkey. This method returns immediatly.
        //! Listen for signals combinationSelectionChanged and combinationSelectionFinished
        //! to retrieve the user input.
        void startCapture();

        //! Deletes all registered hotkeys. Be careful with this method!
        void resetAllActions() { m_configuredActions.clear(); }

        //! Get all available and known actions
        QStringList allAvailableActions() const { return m_availableActions; }

        //! Enable event forwarding to core
        void setForwarding(bool enabled) { m_actionRelayingEnabled = enabled; }

        //! Call functions by hotkeyfunction
        void callFunctionsBy(const QString &action, bool isKeyDown);

        //! Triggers a key event manually and calls the registered functions.
        void triggerKey(const BlackMisc::Input::CHotkeyCombination &combination, bool isPressed);

        //! Creates a native keyboard handler object
        static CInputManager *instance();

    signals:
        //! Event hotkeyfunction occured
        void remoteActionFromLocal(const QString &action, bool argument);

        //! Selected combination has changed
        void combinationSelectionChanged(const BlackMisc::Input::CHotkeyCombination &combination);

        //! Combination selection has finished
        void combinationSelectionFinished(const BlackMisc::Input::CHotkeyCombination &combination);

        //! New hotkey action is registered
        void hotkeyActionRegistered(const QStringList &actions);

    protected:
        //! Constructor
        CInputManager(QObject *parent = nullptr);

    private slots:

        void ps_processKeyCombinationChanged(const BlackMisc::Input::CHotkeyCombination &combination);

        void ps_processButtonCombinationChanged(const BlackMisc::Input::CHotkeyCombination &combination);

        //! Change hotkey settings
        void ps_changeHotkeySettings();

    private:
        //! Handle to a bound action
        struct BindInfo
        {
            // Using unique int intex for identification because std::function does not have a operator==
            int m_index = 0;
            QString m_action;
            QPointer<QObject> m_receiver;
            std::function<void(bool)> m_function;
        };

        int bindImpl(const QString &action, QObject *receiver, std::function<void(bool)> function);

        void processCombination(const BlackMisc::Input::CHotkeyCombination &combination);

        static CInputManager *m_instance;

        std::unique_ptr<BlackInput::IKeyboard> m_keyboard;
        std::unique_ptr<BlackInput::IJoystick> m_joystick;

        QStringList m_availableActions;
        QHash<BlackMisc::Input::CHotkeyCombination, QString> m_configuredActions;
        QVector<BindInfo> m_boundActions;

        bool m_actionRelayingEnabled = false;
        bool m_captureActive = false;
        BlackMisc::Input::CHotkeyCombination m_lastCombination;
        BlackMisc::Input::CHotkeyCombination m_capturedCombination;

        BlackCore::CSetting<BlackCore::Settings::Application::ActionHotkeys> m_actionHotkeys { this, &CInputManager::ps_changeHotkeySettings };
    };
}

#endif //BLACKCORE_INPUTMANAGER_H
