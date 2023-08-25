// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_INPUT_ACTIONHOTKEYLIST_H
#define BLACKMISC_INPUT_ACTIONHOTKEYLIST_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/input/actionhotkey.h"
#include "blackmisc/sequence.h"

#include <QMetaType>
#include <QString>
#include <initializer_list>
#include <tuple>

BLACK_DECLARE_SEQUENCE_MIXINS(BlackMisc::Input, CActionHotkey, CActionHotkeyList)

namespace BlackMisc::Input
{
    //! Value object encapsulating a list of hotkeys
    class BLACKMISC_EXPORT CActionHotkeyList :
        public CSequence<CActionHotkey>,
        public Mixin::MetaType<CActionHotkeyList>
    {
    public:
        BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CActionHotkeyList)
        using CSequence::CSequence;

        //! Default constructor
        CActionHotkeyList() = default;

        //! Construct from a base class object.
        CActionHotkeyList(const CSequence<CActionHotkey> &baseClass);

        //! Returns true if this list has a action hotkey with a combination which is a subset of other
        //! Example: List contains CTRL and other has combination CTRL-F
        CActionHotkeyList findSubsetsOf(const CActionHotkey &other) const;

        //! Returns true if this list has a hotkey with a combination for which other is a superset of other
        //! Example: List contains CTRL-F and other has combination CTRL
        CActionHotkeyList findSupersetsOf(const CActionHotkey &other) const;

        //! Find hotkeys for the same machine
        CActionHotkeyList findBySameMachine(const CActionHotkey &key) const;

        //! Contains action?
        bool containsAction(const QString &action) const;

        //! Update for my machine
        void updateToCurrentMachine();
    };
} // ns

Q_DECLARE_METATYPE(BlackMisc::Input::CActionHotkeyList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Input::CActionHotkey>)

#endif // guard
