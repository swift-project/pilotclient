/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKCORE_KEYBOARD_H
#define BLACKCORE_KEYBOARD_H

#include "../blackmisc/hwkeyboardkey.h"
#include <QMultiMap>
#include <QObject>
#include <QPointer>
#include <functional>

namespace BlackCore
{
    /*!
     * \brief Abstract interface for native keyboard handling.
     * \todo Add implementation for Linux and OSX.
     */
    class IKeyboard : public QObject
    {
        Q_OBJECT
    public:

        //! \brief Handle to a registered hotkey function
        struct RegistrationHandle
        {
            //! \brief Constructor
            RegistrationHandle() {}

            BlackMisc::Hardware::CKeyboardKey m_key;  //!< Registered key set
            QPointer<QObject> m_receiver;           //!< Registered receiver
            std::function<void(bool)> function;     //!< Registered function
        };

        //! Operation mode
        enum Mode
        {
            Mode_Nominal,
            Mode_Capture
        };

        //! Constructor
        IKeyboard(QObject *parent = nullptr);

        //! Destructor
        virtual ~IKeyboard() {}

        //! Creates a native keyboard handler object
        static IKeyboard *getInstance();

        /*!
         * \brief Register a invokable slot as hotkey target
         * \param key
         * \param receiver
         * \param slotName
         * \return RegistrationHandle
         */
        IKeyboard::RegistrationHandle registerHotkey(BlackMisc::Hardware::CKeyboardKey key, QObject *receiver, const QByteArray &slotName)
        {
            auto function = [=](bool isPressed){ QMetaObject::invokeMethod(receiver, slotName, Q_ARG(bool, isPressed)); };
            return registerHotkeyImpl(key, receiver, function);
        }

        /*!
         * \brief Register a member function as hotkey target
         * \param key
         * \param receiver
         * \param slotPointer
         * \return RegistrationHandle
         */
        template <class T>
        IKeyboard::RegistrationHandle registerHotkey(BlackMisc::Hardware::CKeyboardKey key, T *receiver, void (T:: *slotPointer)(bool))
        {
            using namespace std::placeholders;
            auto function = std::bind(slotPointer, receiver, _1);
            return registerHotkeyImpl(key, receiver, function);
        }

        /*!
         * \brief Register a function object as hotkey target
         * \param key
         * \param receiver
         * \param functionObject
         * \return RegistrationHandle
         */
        template <class F>
        IKeyboard::RegistrationHandle registerHotkey(BlackMisc::Hardware::CKeyboardKey key, QObject *receiver, F functionObject)
        {
            return registerHotkeyImpl(key, receiver, functionObject);
        }

        /*!
         * \brief Unregister hotkey target
         * \param handle
         */
        void unregisterHotkey(const IKeyboard::RegistrationHandle &handle)
        {
            unregisterHotkeyImpl(handle);
        }

        //! \brief Unregister all hotkeys
        void unregisterAllHotkeys()
        {
            unregisterAllHotkeysImpl();
        }

        /*!
         * \brief Select a key combination as hotkey. This method returns immediatly.
         *        Listen for signals keySelectionChanged and keySelectionFinished
         *        to retrieve the user input.
         * \param ignoreNextKey
         *        Set to true, if you want to ignore the first key,
         *        e.g. [ENTER] in case you are running from command line.
         */
        virtual void startCapture(bool ignoreNextKey) = 0;

        /*!
         * \brief Returns the amount of registered hotkey functions
         * \return Size
         */
        virtual int sizeOfRegisteredFunctions() const = 0;

        /*!
         * \brief Triggers a key event manually and calls the registered functions.
         * \param key
         * \param isPressed
         */
        virtual void triggerKey(const BlackMisc::Hardware::CKeyboardKey key, bool isPressed) = 0;

    signals:

        /*!
         * \brief Key selection has changed, but is not finished yet.
         * \param key
         */
        void keySelectionChanged(BlackMisc::Hardware::CKeyboardKey key);

        /*!
         * \brief Key selection has finished.
         * \param key
         */
        void keySelectionFinished(BlackMisc::Hardware::CKeyboardKey key);

    protected:

        /*!
         * \brief Initializes the platform keyboard device
         */
        virtual bool init() = 0;

        /*!
         * \brief Register implementation
         */
        virtual IKeyboard::RegistrationHandle registerHotkeyImpl(BlackMisc::Hardware::CKeyboardKey key, QObject *receiver, std::function<void(bool)> function) = 0;

        /*!
         * \brief Unregister implementation
         */
        virtual void unregisterHotkeyImpl(const IKeyboard::RegistrationHandle &handle) = 0;

        //! \brief Unregister implementation
        virtual void unregisterAllHotkeysImpl() = 0;

    private:

        static IKeyboard *m_instance;
    };

    /*!
     * \brief Equal operator ==
     * \param lhs
     * \param rhs
     * \return
     */
    bool operator==(IKeyboard::RegistrationHandle const &lhs, IKeyboard::RegistrationHandle const &rhs);
}

#endif // BLACKCORE_KEYBOARD_H
