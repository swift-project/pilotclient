// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_INPUT_ACTIONHOTKEY_H
#define SWIFT_MISC_INPUT_ACTIONHOTKEY_H

#include <QMetaType>
#include <QString>

#include "misc/identifier.h"
#include "misc/input/hotkeycombination.h"
#include "misc/metaclass.h"
#include "misc/propertyindexref.h"
#include "misc/swiftmiscexport.h"
#include "misc/valueobject.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::input, CActionHotkey)

namespace swift::misc::input
{
    //! Value object encapsulating a action hotkey
    class SWIFT_MISC_EXPORT CActionHotkey : public CValueObject<CActionHotkey>
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
        bool isValid() const
        {
            return !m_identifier.getMachineName().isEmpty() && !m_combination.isEmpty() && !m_action.isEmpty();
        }

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

    private:
        CIdentifier m_identifier; //!< Identifier to which machine this hotkey belongs to
        CHotkeyCombination m_combination; //!< hotkey combination
        QString m_action; //!< hotkey action

        SWIFT_METACLASS(
            CActionHotkey,
            SWIFT_METAMEMBER(identifier),
            SWIFT_METAMEMBER(combination),
            SWIFT_METAMEMBER(action));
    };
} // namespace swift::misc::input

Q_DECLARE_METATYPE(swift::misc::input::CActionHotkey)

#endif // guard
