// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_INPUT_JOYSTICKBUTTONLIST_H
#define SWIFT_MISC_INPUT_JOYSTICKBUTTONLIST_H

#include "misc/swiftmiscexport.h"
#include "misc/collection.h"
#include "misc/input/joystickbutton.h"
#include "misc/sequence.h"

#include <QMetaType>
#include <initializer_list>
#include <tuple>

SWIFT_DECLARE_SEQUENCE_MIXINS(swift::misc::input, CJoystickButton, CJoystickButtonList)

namespace swift::misc::input
{
    //! Value object encapsulating a list of joystick buttons.
    class SWIFT_MISC_EXPORT CJoystickButtonList :
        public CSequence<CJoystickButton>,
        public swift::misc::mixin::MetaType<CJoystickButtonList>
    {
    public:
        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CJoystickButtonList)
        using CSequence::CSequence;

        //! Default constructor
        CJoystickButtonList() = default;

        //! Construct from a base class object.
        CJoystickButtonList(const CSequence<CJoystickButton> &baseClass);
    };

} // namespace

Q_DECLARE_METATYPE(swift::misc::input::CJoystickButtonList)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::misc::input::CJoystickButton>)

#endif // guard
