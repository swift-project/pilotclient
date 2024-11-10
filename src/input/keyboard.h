// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_INPUT_KEYBOARD_H
#define SWIFT_INPUT_KEYBOARD_H

#include "input/swiftinputexport.h"
#include "misc/input/keyboardkeylist.h"
#include "misc/input/hotkeycombination.h"
#include <QObject>
#include <memory>

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
        virtual ~IKeyboard() {}

        //! Creates a native keyboard handler object
        static std::unique_ptr<IKeyboard> create(QObject *parent = nullptr);

    signals:
        //! Key combination changed
        void keyCombinationChanged(const swift::misc::input::CHotkeyCombination &);

    protected:
        //! Initializes the platform keyboard device
        virtual bool init() = 0;
    };
} // ns

#endif // guard
