/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_HARDWARE_KEYBOARDKEYLIST_H
#define BLACKMISC_HARDWARE_KEYBOARDKEYLIST_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/hardware/keyboardkey.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"
#include <QObject>

namespace BlackMisc
{
    namespace Hardware
    {
        /*!
         * Value object encapsulating a list of keyboard keys.
         */
        class BLACKMISC_EXPORT CKeyboardKeyList : public CSequence<CKeyboardKey>
        {
        public:
            //! Default constructor
            CKeyboardKeyList();

            //! Construct from a base class object.
            CKeyboardKeyList(const CSequence<CKeyboardKey> &baseClass);

            //! \copydoc CValueObject::toQVariant
            QVariant toQVariant() const { return QVariant::fromValue(*this); }

            //! Register metadata
            static void registerMetadata();
        };

    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Hardware::CKeyboardKeyList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Hardware::CKeyboardKey>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Hardware::CKeyboardKey>)

#endif //guard
