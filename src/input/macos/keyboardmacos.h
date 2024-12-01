// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_INPUT_KEYBOARDMACOS_H
#define SWIFT_INPUT_KEYBOARDMACOS_H

#include <ApplicationServices/ApplicationServices.h>
#include <IOKit/hid/IOHIDManager.h>

#include <QHash>

#include "input/keyboard.h"
#include "misc/input/hotkeycombination.h"
#include "misc/input/keycodes.h"

namespace swift::input
{
    //! MacOS implemenation of IKeyboard using hook procedure
    class CKeyboardMacOS : public IKeyboard
    {
        Q_OBJECT

    public:
        //! Copy Constructor
        CKeyboardMacOS(CKeyboardMacOS const &) = delete;

        //! Assignment operator
        CKeyboardMacOS &operator=(CKeyboardMacOS const &) = delete;

        //! Destructor
        virtual ~CKeyboardMacOS() override;

    protected:
        //! \copydoc IKeyboard::init()
        virtual bool init() override;

    private:
        friend class IKeyboard;

        //! Constructor
        CKeyboardMacOS(QObject *parent = nullptr);

        void processKeyEvent(IOHIDValueRef value);

        static swift::misc::input::KeyCode convertToKey(quint32 keyCode);
        static void valueCallback(void *context, IOReturn result, void *sender, IOHIDValueRef value);

        IOHIDManagerRef m_hidManager = nullptr;
        swift::misc::input::CHotkeyCombination m_keyCombination; //!< Current status of pressed keys;
    };
} // namespace swift::input

#endif // SWIFT_INPUT_KEYBOARDMACOS_H
