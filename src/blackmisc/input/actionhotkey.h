/* Copyright (C) 2015
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_INPUT_ACTIONHOTKEY_H
#define BLACKMISC_INPUT_ACTIONHOTKEY_H

#include "blackmisc/input/hotkeycombination.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/propertyindexref.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/identifier.h"
#include "blackmisc/blackmiscexport.h"

#include <QMetaType>
#include <QString>

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
                IndexIdentifier = CPropertyIndexRef::GlobalIndexCSettingKeyboardHotkey,
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
            const QString &getAction() const { return m_action; }

            //! Set function
            void setAction(const QString &action) { m_action = action; }

            //! The identifier
            const CIdentifier &getIdentifier() const { return m_identifier; }

            //! Set applicable machine
            void setApplicableMachine(const CIdentifier &identifier) { m_identifier = identifier; }

            //! Get applicable machine
            const CIdentifier &getApplicableMachine() const { return m_identifier; }

            //! Key for the same machine id?
            bool isForSameMachineId(const CActionHotkey &key) const;

            //! Key for the same machine name
            bool isForSameMachineName(const CActionHotkey &key) const;

            //! Key for the same machine (same name or id)?
            bool isForSameMachine(const CActionHotkey &key) const;

            //! Local machine
            void updateToCurrentMachine();

            //! Set object
            void setObject(const CActionHotkey &obj);

            //! Is hotkey valid?
            bool isValid() const { return !m_identifier.getMachineName().isEmpty() && !m_combination.isEmpty() && !m_action.isEmpty(); }

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            QVariant propertyByIndex(CPropertyIndexRef index) const;

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

        private:
            CIdentifier        m_identifier;  //!< Identifier to which machine this hotkey belongs to
            CHotkeyCombination m_combination; //!< hotkey combination
            QString            m_action;      //!< hotkey action

            BLACK_METACLASS(
                CActionHotkey,
                BLACK_METAMEMBER(identifier),
                BLACK_METAMEMBER(combination),
                BLACK_METAMEMBER(action)
            );
        };
    } // ns
} // ns

Q_DECLARE_METATYPE(BlackMisc::Input::CActionHotkey)

#endif // guard
