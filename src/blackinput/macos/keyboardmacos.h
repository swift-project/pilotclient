/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKINPUT_KEYBOARDMACOS_H
#define BLACKINPUT_KEYBOARDMACOS_H

#include "blackinput/keyboard.h"
#include "blackmisc/input/hotkeycombination.h"
#include "blackmisc/input/keycodes.h"

#include <IOKit/hid/IOHIDManager.h>

#include <ApplicationServices/ApplicationServices.h>
#include <QHash>

namespace BlackInput
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

        static BlackMisc::Input::KeyCode convertToKey(quint32 keyCode);
        static void valueCallback(void *context, IOReturn result, void *sender, IOHIDValueRef value);

        IOHIDManagerRef m_hidManager = nullptr;
        BlackMisc::Input::CHotkeyCombination m_keyCombination; //!< Current status of pressed keys;
    };
} // ns

#endif // guard
