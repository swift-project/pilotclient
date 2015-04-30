/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_HARDWARE_KEYBOARDKEY_H
#define BLACKMISC_HARDWARE_KEYBOARDKEY_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include <QStringList>
#include <QKeySequence>

namespace BlackMisc
{
    namespace Hardware
    {
        //! Value object representing a keyboard key.
        class BLACKMISC_EXPORT CKeyboardKey : public CValueObject<CKeyboardKey>
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexKey = 0,
                IndexKeyAsString,
                IndexKeyAsStringRepresentation,
                IndexModifier1,
                IndexModifier2,
                IndexModifier1AsString,
                IndexModifier2AsString,
                IndexKeyObject, // just for updates
            };

            //! Modifier
            enum Modifier
            {
                ModifierNone,
                ModifierCtrlLeft,
                ModifierCtrlRight,
                ModifierCtrlAny,
                ModifierAltLeft,
                ModifierAltRight,
                ModifierAltAny,
                ModifierShiftLeft,
                ModifierShiftRight,
                ModifierShiftAny,
                ModifierMeta,
                ModifierNum
            };

            //! Default constructor
            CKeyboardKey();

            //! Constructor
            CKeyboardKey(Qt::Key keyCode, Modifier modifier1 = ModifierNone, Modifier modifier2 = ModifierNone);

            //! Get key code
            Qt::Key getKey() const { return this->m_qtKey; }

            //! Get key code
            QString getKeyAsString() const { return QString(this->getKey()); }

            //! Get key code
            QString getKeyAsStringRepresentation() const { return CKeyboardKey::toStringRepresentation(this->m_qtKey); }

            //! As Qt::Key
            Qt::Key getKeyAsQtKey() const { return static_cast<Qt::Key>(this->m_qtKey);}

            //! Set key code
            void setKey(const QString &key);

            //! Set key code
            void setKey(const QChar &key);

            //! Set key code
            void setKey(const Qt::Key key) { this->m_qtKey = key; }

            //! Set key code
            void setKey(int key) { this->m_qtKey = static_cast<Qt::Key>(key); }

            /*!
             * Add modifier
             * \param modifier
             * \return True if modifier was added
             */
            bool addModifier(const Modifier &modifier);

            /*!
             * add modifier
             * \param modifier
             * \return True if modifier was added
             */
            bool addModifier(const QString &modifier);

            //! number of modifiers
            int numberOfModifiers() const;

            //! Modifier 1
            Modifier getModifier1() const { return this->m_modifier1; }

            //! Modifier 2
            Modifier getModifier2() const { return this->m_modifier2; }

            //! Modifier 1
            QString getModifier1AsString() const { return modifierToString(this->m_modifier1); }

            //! Modifier 2
            QString getModifier2AsString() const { return modifierToString(this->m_modifier2); }

            /*!
             * Remove modifier from key
             * \param modifier
             * \return True if modifier was removed
             */
            bool removeModifier(const Modifier &modifier);

            /*!
             * Remove modifier from key
             * \param modifier
             * \return True if modifier was removed
             */
            bool removeModifier(const QString &modifier);

            //! Set modifier 1
            void setModifier1(const QString &modifier) { this->m_modifier1 = modifierFromString(modifier); }

            //! Set modifier 1
            void setModifier1(const Modifier &modifier) { this->m_modifier1 = modifier; }

            //! Set modifier 2
            void setModifier2(const QString &modifier) { this->m_modifier2 = modifierFromString(modifier); }

            //! Set modifier 2
            void setModifier2(const Modifier &modifier) { this->m_modifier2 = modifier; }

            //! Is empty, (no key, no modifier)?
            bool isEmpty() const { return !this->hasModifier() && !this->hasKey(); }

            //! Modifier?
            bool hasModifier() const
            {
                return this->m_modifier1 != ModifierNone || this->m_modifier2 != ModifierNone;
            }

            //! Do we have this modifier?
            bool hasModifier(Modifier modifier) const
            {
                return m_modifier1 == modifier || m_modifier2 == modifier;
            }

            //! ALT = ALT/R or ALT/L, CTRL = CTRL/R or left, ...
            static bool equalsModifierReleaxed(Modifier m1, Modifier m2);

            //! with key?
            bool hasKey() const
            {
                return !(this->m_qtKey == Qt::Key_unknown);
            }

            //! Key + modifier?
            bool hasKeyAndModifier() const
            {
                return this->hasKey() && this->hasModifier();
            }

            //! Order Modifiers
            void cleanup();

            //! Set key object
            void setKeyObject(const BlackMisc::Hardware::CKeyboardKey &key);

            //! CTRL will be consider equal CTRL-left/reigt, ALT = ALT-left/right ..
            bool equalsWithRelaxedModifiers(const CKeyboardKey &key) const;

            //! \copydoc CValueObject::setPropertyByIndex
            virtual void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index) override;

            //! \copydoc CValueObject::propertyByIndex
            virtual CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const override;

            //! Modifier to string
            static QString modifierToString(Modifier modifier);

            //! Modifier from string
            static Modifier modifierFromString(const QString &modifier);

            //! Modifier from Qt::Modifier
            static Modifier modifierFromQtModifier(Qt::Modifier qtModifier);

            //! all modifiers
            static const QStringList &modifiers();

            /*!
             * CKeyboardKey::toStringRepresentation
             */
            static QString toStringRepresentation(int key);

            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const override;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CKeyboardKey)
            Qt::Key m_qtKey; //!< code similar to Qt::Key
            Modifier m_modifier1;
            Modifier m_modifier2;
        };
    }
}

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Hardware::CKeyboardKey, (o.m_qtKey, o.m_modifier1, o.m_modifier2))
Q_DECLARE_METATYPE(BlackMisc::Hardware::CKeyboardKey)

#endif // guard
