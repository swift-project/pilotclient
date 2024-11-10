// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_INPUT_JOYSTICK_H
#define SWIFT_INPUT_JOYSTICK_H

#include "input/swiftinputexport.h"
#include "blackmisc/input/hotkeycombination.h"
#include <QObject>
#include <memory>

namespace swift::input
{
    /*!
     * Abstract interface for native joystick handling.
     */
    class SWIFT_INPUT_EXPORT IJoystick : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor
        IJoystick(QObject *parent = nullptr);

        //! Destructor
        virtual ~IJoystick() {}

        //! Creates a native joystick handler object
        static std::unique_ptr<IJoystick> create(QObject *parent = nullptr);

        //! Get all available joystick buttons
        virtual BlackMisc::Input::CJoystickButtonList getAllAvailableJoystickButtons() const { return {}; }

    signals:
        //! Joystick button combination has changed
        void buttonCombinationChanged(const BlackMisc::Input::CHotkeyCombination &);

    protected:
        //! Initializes the platform joystick devices
        virtual bool init() { return false; }
    };
}

#endif // guard
