// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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

BLACK_DECLARE_SEQUENCE_MIXINS(BlackMisc::Input, CKeyboardKey, CKeyboardKeyList)

namespace BlackMisc::Input
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
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Input::CKeyboardKeyList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Input::CKeyboardKey>)

#endif // guard
