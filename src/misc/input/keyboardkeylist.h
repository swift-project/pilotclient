// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_INPUT_KEYBOARDKEYLIST_H
#define SWIFT_MISC_INPUT_KEYBOARDKEYLIST_H

#include "misc/swiftmiscexport.h"
#include "misc/collection.h"
#include "misc/input/keyboardkey.h"
#include "misc/sequence.h"

#include <QStringList>
#include <QMetaType>
#include <initializer_list>
#include <tuple>

BLACK_DECLARE_SEQUENCE_MIXINS(swift::misc::input, CKeyboardKey, CKeyboardKeyList)

namespace swift::misc::input
{
    //! Value object encapsulating a list of keyboard keys.
    class SWIFT_MISC_EXPORT CKeyboardKeyList :
        public CSequence<CKeyboardKey>,
        public mixin::MetaType<CKeyboardKeyList>
    {
    public:
        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CKeyboardKeyList)
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
} // namespace

Q_DECLARE_METATYPE(swift::misc::input::CKeyboardKeyList)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::misc::input::CKeyboardKey>)

#endif // guard
