/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKMISC_KEYBOARDKEY_H
#define BLACKMISC_KEYBOARDKEY_H

#include "valueobject.h"
#include <QStringList>
#include <QKeySequence>

namespace BlackMisc
{
    namespace Hardware
    {
        //! Value object representing a keyboard key.
        class CKeyboardKey : public CValueObject
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
                IndexFunction,
                IndexFunctionAsString,
                IndexKeyObject, // just for updates
            };

            //! Function
            enum HotkeyFunction
            {
                HotkeyNone,
                HotkeyPtt,
                HotkeyToggleCom1,
                HotkeyToggleCom2,
                HotkeyOpacity50,
                HotkeyOpacity100,
                HotkeyToogleWindowsStayOnTop
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

            //! Constructor by function
            CKeyboardKey(HotkeyFunction function);

            //! Constructor
            CKeyboardKey(Qt::Key keyCode, quint32 nativeVirtualKey, Modifier modifier1 = ModifierNone, Modifier modifier2 = ModifierNone, const HotkeyFunction &function = HotkeyNone);

            //! Destructor
            ~CKeyboardKey() {}

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override
            {
                return QVariant::fromValue(*this);
            }

            //! \copydoc CValueObject::getValueHash
            virtual uint getValueHash() const override;

            //! \copydoc CValueObject::toJson
            virtual QJsonObject toJson() const override;

            //! \copydoc CValueObject::fromJson
            void fromJson(const QJsonObject &json) override;

            //! Register metadata
            static void registerMetadata();

            //! \copydoc TupleConverter<>::jsonMembers()
            static const QStringList &jsonMembers();

            //! Equal?
            bool operator ==(const CKeyboardKey &other) const;

            //! Unequal operator !=
            bool operator !=(const CKeyboardKey &other) const;

            //! <
            bool operator<(CKeyboardKey const &other) const;

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

            //! Native virtual key
            void setNativeVirtualKey(quint32 virtualKey) { this->m_nativeVirtualKey = virtualKey; }

            //! Native virtual key
            quint32 getNativeVirtualKey() const { return this->m_nativeVirtualKey; }

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

            //! Function (optional)
            HotkeyFunction getFunction() const { return this->m_function; }

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

            //! Function (optional)
            QString getFunctionAsString() const;

            //! Set key object
            void setKeyObject(const BlackMisc::Hardware::CKeyboardKey &key);

            //! Set function
            void setFunction(const HotkeyFunction &function) { this->m_function = function; }

            //! CTRL will be consider equal CTRL-left/reigt, ALT = ALT-left/right ..
            bool equalsWithRelaxedModifiers(const CKeyboardKey &key, bool ignoreFunction = false) const;

            //! Equal, but function value ignored
            bool equalsWithoutFunction(const CKeyboardKey &key) const;

            //! \copydoc CValueObject::setPropertyByIndex
            virtual void setPropertyByIndex(const QVariant &variant, int index);

            //! \copydoc CValueObject::propertyByIndex
            virtual QVariant propertyByIndex(int index) const;

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
             * \param key
             * \return
             */
            static QString toStringRepresentation(int key);

        protected:
            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const override;

            //! \copydoc CValueObject::getMetaTypeId
            virtual int getMetaTypeId() const override;

            //! \copydoc CValueObject::isA
            virtual bool isA(int metaTypeId) const override;

            //! \copydoc CValueObject::compareImpl(otherBase)
            virtual int compareImpl(const CValueObject &otherBase) const override;

            //! \copydoc CValueObject::marshallToDbus
            virtual void marshallToDbus(QDBusArgument &argument) const override;

            //! \copydoc CValueObject::unmarshallFromDbus
            virtual void unmarshallFromDbus(const QDBusArgument &argument) override;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CKeyboardKey)
            Qt::Key m_qtKey; //!< code similar to Qt::Key
            quint32 m_nativeVirtualKey; //!< virtual key code TODO: Risk in platform independent comparisons
            Modifier m_modifier1;
            Modifier m_modifier2;
            HotkeyFunction m_function;

        };
    } // class
} // BlackMisc

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Hardware::CKeyboardKey, (o.m_qtKey, o.m_nativeVirtualKey, o.m_modifier1, o.m_modifier2, o.m_function))
Q_DECLARE_METATYPE(BlackMisc::Hardware::CKeyboardKey)

#endif // guard
