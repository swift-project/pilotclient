// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_INPUT_JOYSTICKBUTTONLIST_H
#define BLACKMISC_INPUT_JOYSTICKBUTTONLIST_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/input/joystickbutton.h"
#include "blackmisc/sequence.h"

#include <QMetaType>
#include <initializer_list>
#include <tuple>

BLACK_DECLARE_SEQUENCE_MIXINS(BlackMisc::Input, CJoystickButton, CJoystickButtonList)

namespace BlackMisc::Input
{
    //! Value object encapsulating a list of joystick buttons.
    class BLACKMISC_EXPORT CJoystickButtonList :
        public CSequence<CJoystickButton>,
        public BlackMisc::Mixin::MetaType<CJoystickButtonList>
    {
    public:
        BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CJoystickButtonList)
        using CSequence::CSequence;

        //! Default constructor
        CJoystickButtonList() = default;

        //! Construct from a base class object.
        CJoystickButtonList(const CSequence<CJoystickButton> &baseClass);
    };

} // namespace

Q_DECLARE_METATYPE(BlackMisc::Input::CJoystickButtonList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Input::CJoystickButton>)

#endif // guard
