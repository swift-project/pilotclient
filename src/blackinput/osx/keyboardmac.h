/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKINPUT_KEYBOARD_MAC_H
#define BLACKINPUT_KEYBOARD_MAC_H

#include "blackinput/keyboard.h"
#include "blackmisc/input/hotkeycombination.h"
#include <QHash>

class __CGEvent;

typedef __CGEvent* CGEventRef; //!< Mac event definition
typedef unsigned int CGEventType; //!< Max event type definition

class __CGEventTapProxy;
typedef __CGEventTapProxy* CGEventTapProxy; //!< Max event proxy definition

namespace BlackInput
{
    //! Mac OSX implemenation of IKeyboard using hook procedure
    class CKeyboardMac : public IKeyboard
    {
        Q_OBJECT

    public:
        //! Copy Constructor
        CKeyboardMac(CKeyboardMac const&) = delete;

        //! Assignment operator
        CKeyboardMac &operator=(CKeyboardMac const&) = delete;

        //! Destructor
        virtual ~CKeyboardMac();

        //! Process key event
        virtual void processKeyEvent(CGEventType type, CGEventRef event);

    protected:

        //! \copydoc IKeyboard::init()
        virtual bool init() override;

    private:

        friend class IKeyboard;

        //! Constructor
        CKeyboardMac(QObject *parent = nullptr);
        BlackMisc::Input::KeyCode convertToKey(int keyCode);

        static CGEventRef myCGEventCallback(CGEventTapProxy proxy,
                                     CGEventType type,
                                     CGEventRef event,
                                     void *refcon);

        BlackMisc::Input::CHotkeyCombination m_keyCombination;    //!< Current status of pressed keys;
    };
}

#endif // BLACKINPUT_KEYBOARD_MAC_H
