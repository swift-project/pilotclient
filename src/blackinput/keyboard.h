/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
