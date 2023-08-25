// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKINPUT_KEYBOARD_H
#define BLACKINPUT_KEYBOARD_H

#include "blackinput/blackinputexport.h"
#include "blackmisc/input/keyboardkeylist.h"
#include "blackmisc/input/hotkeycombination.h"
#include <QObject>
#include <memory>

namespace BlackInput
{
    /*!
     * Abstract interface for native keyboard handling.
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
        //! Initializes the platform keyboard device
        virtual bool init() = 0;
    };
} // ns

#endif // guard
