/* Copyright (C) 2015
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_INPUT_ACTIONHOTKEY_H
#define BLACKMISC_INPUT_ACTIONHOTKEY_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/input/hotkeycombination.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/identifier.h"
#include <QStringList>

namespace BlackMisc
{
    namespace Input
    {
        //! Value object encapsulating a action hotkey
        class BLACKMISC_EXPORT CActionHotkey : public CValueObject<CActionHotkey>
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexIdentifier = BlackMisc::CPropertyIndex::GlobalIndexCSettingKeyboardHotkey,
                IndexIdentifierAsString,
                IndexCombination,
                IndexCombinationAsString,
                IndexAction,
                IndexActionAsString,
                IndexObject, // just for updates
            };

            //! Default constructor
            CActionHotkey() = default;

            //! Constructor
            CActionHotkey(const QString &action);

            //! Constructor
            CActionHotkey(const CIdentifier &identifier, const CHotkeyCombination &combination, const QString &action);

            //! Get hotkey combination
            const CHotkeyCombination &getCombination() const { return m_combination; }

            //! Set hotkey combination
            void setCombination(const CHotkeyCombination &combination);

            //! Action
            QString getAction() const { return m_action; }

            //! Set function
            void setAction(const QString &action) { m_action = action; }

            //! Set applicable machine
            void setApplicableMachine(const CIdentifier &identifier) { m_identifier = identifier; }

            //! Get applicable machine
            CIdentifier getApplicableMachine() const { return m_identifier; }

            //! Set object
            void setObject(const CActionHotkey &obj);

            //! Is hotkey valid?
            bool isValid() const { return !m_identifier.getMachineName().isEmpty() && !m_combination.isEmpty() && !m_action.isEmpty(); }

            //! \copydoc CValueObject::setPropertyByIndex
            void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index);

            //! \copydoc CValueObject::propertyByIndex
            CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc CValueObject::convertToQString
            QString convertToQString(bool i18n = false) const;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CActionHotkey)
            CIdentifier m_identifier; //!< Identifier to which machine this hotkey belongs to
            CHotkeyCombination m_combination; //!< hotkey combination
            QString m_action; //!< hotkey action
        };
    }
}

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Input::CActionHotkey, (
                                   attr(o.m_identifier),
                                   attr(o.m_combination),
                                   attr(o.m_action)
                                   ))
Q_DECLARE_METATYPE(BlackMisc::Input::CActionHotkey)

#endif // guard
