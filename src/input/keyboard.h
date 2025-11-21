// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_INPUT_KEYBOARD_H
#define SWIFT_INPUT_KEYBOARD_H

#include <memory>

#include <QObject>

#include "input/swiftinputexport.h"
#include "misc/input/hotkeycombination.h"
#include "misc/input/keyboardkeylist.h"

namespace swift::input
{
    /*!
     * Abstract interface for native keyboard handling.
     */
    class SWIFT_INPUT_EXPORT IKeyboard : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor
        IKeyboard(QObject *parent = nullptr);

        //! Destructor
        ~IKeyboard() override = default;

        //! Creates a native keyboard handler object
        static std::unique_ptr<IKeyboard> create(QObject *parent = nullptr);

    signals:
        //! Key combination changed
        void keyCombinationChanged(const swift::misc::input::CHotkeyCombination &);

    protected:
        //! Initializes the platform keyboard device
        virtual bool init() = 0;
    };
} // namespace swift::input

#endif // SWIFT_INPUT_KEYBOARD_H
