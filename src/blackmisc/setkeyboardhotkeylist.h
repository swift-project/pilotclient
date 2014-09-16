/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

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

            //! Contains given hotkey function?
            bool containsFunction(const CHotkeyFunction &function) const;

            //! Key for given function
            BlackMisc::Settings::CSettingKeyboardHotkey keyForFunction(const CHotkeyFunction &function) const;

            /*!
             * Fill the list with hotkeys
             * \param reset true, list will be be reset, otherwise values will not be overridde
             */
            void initAsHotkeyList(bool reset = true);

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            //! \copydoc CValueObject::convertFromQVariant
            virtual void convertFromQVariant(const QVariant &variant) override { BlackMisc::setFromQVariant(this, variant); }

            //! Register metadata
            static void registerMetadata();

        };

    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Settings::CSettingKeyboardHotkeyList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Settings::CSettingKeyboardHotkey>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Settings::CSettingKeyboardHotkey>)

#endif //guard
