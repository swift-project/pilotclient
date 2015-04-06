/* Copyright (C) 2014
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKINPUT_JOYSTICK_H
#define BLACKINPUT_JOYSTICK_H

#include "blackmisc/hardware/joystickbutton.h"
#include <QMultiMap>
#include <QObject>
#include <QPointer>
#include <functional>

namespace BlackInput
{
    /*!
     * \brief Abstract interface for native joystick handling.
     * \todo Add implementation for Linux and OSX.
     */
    class IJoystick : public QObject
    {
        Q_OBJECT

    public:

        //! Operation mode
        enum Mode
        {
            ModeNominal,
            ModeCapture
        };

        //! Constructor
        IJoystick(QObject *parent = nullptr);

        //! Destructor
        virtual ~IJoystick() {}

        //! Start joystick button selection for settings configuration
        virtual void startCapture() = 0;

        //! Simulating press/release of a joystick button
        virtual void triggerButton(const BlackMisc::Hardware::CJoystickButton button, bool isPressed) = 0;

        //! Creates a native joystick handler object
        static IJoystick *getInstance();

    signals:

        //! User has selected a joystick button
        void buttonSelectionFinished(const BlackMisc::Hardware::CJoystickButton &button);

        //! Button down
        void buttonDown(const BlackMisc::Hardware::CJoystickButton &);

        //! Button up
        void buttonUp(const BlackMisc::Hardware::CJoystickButton &);

    private:

        static IJoystick *m_instance;
    };
}

#endif // BLACKINPUT_JOYSTICK_H
