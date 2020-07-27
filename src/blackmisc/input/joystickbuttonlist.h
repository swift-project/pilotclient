/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_INPUT_JOYSTICKBUTTONLIST_H
#define BLACKMISC_INPUT_JOYSTICKBUTTONLIST_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/input/joystickbutton.h"
#include "blackmisc/sequence.h"
#include "blackmisc/variant.h"

#include <QMetaType>
#include <initializer_list>
#include <tuple>

namespace BlackMisc
{
    namespace Input
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

    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Input::CJoystickButtonList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Input::CJoystickButton>)

#endif //guard
