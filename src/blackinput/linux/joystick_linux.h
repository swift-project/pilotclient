/* Copyright (C) 2014
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKINPUT_JOYSTICKLINUX_H
#define BLACKINPUT_JOYSTICKLINUX_H

//! \file

#include "blackinput/joystick.h"
#include "blackmisc/hwjoystickbutton.h"
#include "blackmisc/collection.h"
#include <QSet>

namespace BlackInput
{
    //! Linux implemenation of IJoystick with DirectInput
    class CJoystickLinux : public IJoystick
    {
        Q_OBJECT

    public:

        //! Copy Constructor
        CJoystickLinux(CJoystickLinux const &) = delete;

        //! Assignment operator
        CJoystickLinux &operator=(CJoystickLinux const &) = delete;

        //! \brief Destructor
        virtual ~CJoystickLinux();

        //! \copydoc IJoystick::startCapture()
        virtual void startCapture() override;

        //! \copydoc IJoystick::triggerButton()
        virtual void triggerButton(const BlackMisc::Hardware::CJoystickButton button, bool isPressed) override;

    private:

        friend class IJoystick;

        //! Destructor
        CJoystickLinux(QObject *parent = nullptr);

        IJoystick::Mode m_mode = ModeNominal; //!< Current working mode
    };

} // namespace BlackInput

#endif // BLACKINPUT_JOYSTICKLINUX_H
