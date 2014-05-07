/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKMISC_KEYBOARDKEYLIST_H
#define BLACKMISC_KEYBOARDKEYLIST_H

#include "hwkeyboardkey.h"
#include "collection.h"
#include "sequence.h"
#include <QObject>

namespace BlackMisc
{
    namespace Hardware
    {
        /*!
         * Value object encapsulating a list of keyboard keys.
         */
        class CKeyboardKeyList : public CSequence<CKeyboardKey>
        {
        public:
            //! Default constructor
            CKeyboardKeyList();

            //! Construct from a base class object.
            CKeyboardKeyList(const CSequence<CKeyboardKey> &baseClass);

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override
            {
                return QVariant::fromValue(*this);
            }

            //! Contains given hotkey function?
            bool containsFunction(CKeyboardKey::HotkeyFunction function) const;

            //! Key for given function
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

Q_DECLARE_METATYPE(BlackMisc::Hardware::CKeyboardKeyList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Hardware::CKeyboardKey>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Hardware::CKeyboardKey>)

#endif //guard
