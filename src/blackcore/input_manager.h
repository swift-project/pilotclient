/* Copyright (C) 2013 VATSIM Community / contributors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_INPUTMANAGER_H
#define BLACKCORE_INPUTMANAGER_H

#include "blackinput/keyboard.h"
#include "blackmisc/hwkeyboardkeylist.h"
#include "blackmisc/hotkeyfunction.h"
#include "blackmisc/setkeyboardhotkeylist.h"
#include <QObject>
#include <QHash>
#include <type_traits>
#include <functional>

//! \file

namespace BlackCore
{
    //! Input manager handling hotkey function calls
    class CInputManager : public QObject
    {
        Q_OBJECT

    public:

        //! \brief Handle to a registered hotkey function
        struct RegistrationHandle
        {
            //! \brief Constructor
            RegistrationHandle() {}

            BlackMisc::CHotkeyFunction hotkeyFunction;  //!< Registered hotkey function
            QPointer<QObject> m_receiver;           //!< Registered receiver
            std::function<void(bool)> function;     //!< Registered function
        };

        //! Register a new hotkey function
        RegistrationHandle registerHotkeyFunc(const BlackMisc::CHotkeyFunction &hotkeyFunction, QObject *receiver, const QByteArray &slotName)
        {
            auto function = [=](bool isKeyDown){ QMetaObject::invokeMethod(receiver, slotName, Q_ARG(bool, isKeyDown)); };
            return registerHotkeyFuncImpl(hotkeyFunction, receiver, function);
        }

        //! Register a new hotkey function
        template <class RecvObj>
        RegistrationHandle registerHotkeyFunc(const BlackMisc::CHotkeyFunction &hotkeyFunction, RecvObj *receiver, void (RecvObj:: *slotPointer)(bool))
        {
            using namespace std::placeholders;
            auto function = std::bind(slotPointer, receiver, _1);
            return registerHotkeyFuncImpl(hotkeyFunction, receiver, function);
        }

        //! Register a new hotkey function
        template <class Func>
        RegistrationHandle registerHotkeyFunc(const BlackMisc::CHotkeyFunction &hotkeyFunction, QObject *receiver, Func functionObject)
        {
            return registerHotkeyFuncImpl(hotkeyFunction, receiver, functionObject);
        }

        //! Deletes all registered hotkeys. Be careful with this method!
        void resetAllHotkeyFuncs() { m_hashRegisteredFunctions.clear(); }

        //! Creates a native keyboard handler object
        static CInputManager *getInstance();

    public slots:

        //! Change hotkey settings
        void changeHotkeySettings(BlackMisc::Settings::CSettingKeyboardHotkeyList hotkeys);

        //! Call functions by hotkeyfunction
        void callFunctionsBy(const BlackMisc::CHotkeyFunction &hotkeyFunction, bool isKeyDown);

    protected:
        //! Constructor
        CInputManager(QObject *parent = nullptr);

    private slots:

        void ps_processKeyboardKeyDown(const BlackMisc::Hardware::CKeyboardKey &);

        void ps_processKeyboardKeyUp(const BlackMisc::Hardware::CKeyboardKey &);

    private:

        RegistrationHandle registerHotkeyFuncImpl(const BlackMisc::CHotkeyFunction &hotkeyFunction, QObject *receiver, std::function<void(bool)> function);

        static CInputManager *m_instance;

        BlackInput::IKeyboard *m_keyboard = nullptr;

        QHash<BlackMisc::CHotkeyFunction, std::function<void(bool)> > m_hashRegisteredFunctions;
        QHash<BlackMisc::Hardware::CKeyboardKey, BlackMisc::CHotkeyFunction> m_hashKeyboardKeyFunctions;
    };
}

#endif //BLACKCORE_INPUTMANAGER_H
