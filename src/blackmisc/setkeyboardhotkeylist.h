/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file

#ifndef BLACKMISC_SETTINGS_KEYBOARDHOTKEYLIST_H
#define BLACKMISC_SETTINGS_KEYBOARDHOTKEYLIST_H

#include "hwkeyboardkey.h"
#include "setkeyboardhotkey.h"
#include "collection.h"
#include "sequence.h"
#include <QObject>

namespace BlackMisc
{
    namespace Settings
    {
        /*!
         * Value object encapsulating a list of keyboard keys.
         */
        class CSettingKeyboardHotkeyList : public CSequence<CSettingKeyboardHotkey>
        {
        public:
            //! Default constructor
            CSettingKeyboardHotkeyList();

            //! Construct from a base class object.
            CSettingKeyboardHotkeyList(const CSequence<CSettingKeyboardHotkey> &baseClass);

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            //! Contains given hotkey function?
            bool containsFunction(const CHotkeyFunction &function) const;

            //! Key for given function
            BlackMisc::Settings::CSettingKeyboardHotkey keyForFunction(const CHotkeyFunction &function) const;

            //! Register metadata
            static void registerMetadata();

            /*!
             * Fill the list with hotkeys
             * \param reset true, list will be be reset, otherwise values will not be overridde
             */
            void initAsHotkeyList(bool reset = true);
        };

    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Settings::CSettingKeyboardHotkeyList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Settings::CSettingKeyboardHotkey>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Settings::CSettingKeyboardHotkey>)

#endif //guard
