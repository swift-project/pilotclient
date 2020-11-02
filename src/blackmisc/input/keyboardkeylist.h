/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_INPUT_KEYBOARDKEYLIST_H
#define BLACKMISC_INPUT_KEYBOARDKEYLIST_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/input/keyboardkey.h"
#include "blackmisc/sequence.h"

#include <QStringList>
#include <QMetaType>
#include <initializer_list>
#include <tuple>

namespace BlackMisc
{
    namespace Input
    {
        //! Value object encapsulating a list of keyboard keys.
        class BLACKMISC_EXPORT CKeyboardKeyList :
            public CSequence<CKeyboardKey>,
            public Mixin::MetaType<CKeyboardKeyList>
        {
        public:
            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CKeyboardKeyList)
            using CSequence::CSequence;

            //! Default constructor
            CKeyboardKeyList();

            //! Construct from a base class object.
            CKeyboardKeyList(const CSequence<CKeyboardKey> &baseClass);

            //! All key strings
            QStringList getKeyStrings() const;

            //! Get all supported keys
            static const CKeyboardKeyList &allSupportedKeys();
        };
    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Input::CKeyboardKeyList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Input::CKeyboardKey>)

#endif //guard
