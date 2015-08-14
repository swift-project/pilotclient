/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKINPUT_KEYBOARD_H
#define BLACKINPUT_KEYBOARD_H

#include "blackinputexport.h"
#include "blackmisc/input/keyboardkeylist.h"
#include "blackmisc/input/hotkeycombination.h"
#include <QObject>
#include <memory>

namespace BlackInput
{
    /*!
     * Abstract interface for native keyboard handling.
     * \todo Add implementation for Linux and OSX.
     */
    class BLACKINPUT_EXPORT IKeyboard : public QObject
    {
        Q_OBJECT
    public:
        //! Constructor
        IKeyboard(QObject *parent = nullptr);

        //! Destructor
        virtual ~IKeyboard() {}

        //! Creates a native keyboard handler object
        static std::unique_ptr<IKeyboard> create(QObject *parent = nullptr);

    signals:
        //! Key combination changed
        void keyCombinationChanged(const BlackMisc::Input::CHotkeyCombination &);

    protected:

        /*!
         * Initializes the platform keyboard device
         */
        virtual bool init() = 0;

    private:
        static IKeyboard *m_instance;
    };
}

#endif // guard
