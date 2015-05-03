/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#include "valueobject.h" // outside include guard due to cyclic dependency hack (MS)

#ifndef BLACKMISC_VARIANT_H
#define BLACKMISC_VARIANT_H

#include "blackmiscexport.h"
#include <QVariant>
#include <QDateTime>
#include <QJsonValueRef>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonArray>

class QDBusArgument;

namespace BlackMisc
{
    class CVariant;

    //! \private
    template <> struct CValueObjectPolicy<CVariant> : public CValueObjectPolicy<>
    {
        using MetaType = Policy::MetaType::QMetaTypeAndDBusOnly;
        using Equals = Policy::Equals::OwnEquals;
        using LessThan = Policy::LessThan::OwnLessThan;
        using Compare = Policy::Compare::None;
        using Hash = Policy::Hash::Own;
        using DBus = Policy::DBus::Own;
        using Json = Policy::Json::Own;
    };

    /*!
     * Wrapper around QVariant which provides transparent access to CValueObject methods
     * of the contained object if it is registered with BlackMisc::registerMetaValueType.
     */
    class BLACKMISC_EXPORT CVariant : public CValueObject<CVariant>
    {
    public:
        //! Default constructor.
        CVariant() {}

        //! Copy constructor.
        CVariant(const CVariant &other) : CValueObject(other), m_v(other.m_v) {}

        //! Move constructor.
        CVariant(CVariant &&other) : CValueObject(std::move(other)), m_v(std::move(other.m_v)) {}

        //! Construct from a QVariant.
        CVariant(const QVariant &var) : m_v(var) {}

        //! Move-construct from a QVariant.
        CVariant(QVariant &&var) : m_v(std::move(var)) {}

        //! Construct a null variant of the given type.
        CVariant(QVariant::Type type) : m_v(type) {}

        //! Avoid unexpected implicit cast to QVariant::Type. (Use CVariant::from() instead.)
        CVariant(int) = delete;

        //! Implicit conversion from QString.
        CVariant(const QString &string) : m_v(string) {}

        //! Implicit conversion from C string.
        CVariant(const char *string) : m_v(string) {}

        //! Construct a variant from the given type and opaque pointer.
        CVariant(int typeId, const void *copy) : m_v(typeId, copy) {}

        //! \copydoc CValueObject::qHash
        friend uint qHash(const CVariant &var) { return var.getValueHash(); }

        //! Change the internal QVariant.
        void reset(const QVariant &var) { m_v = var; }

        //! Change the internal QVariant.
        void reset(QVariant &&var) { m_v = std::move(var); }

        //! Copy assignment operator.
        CVariant &operator =(const CVariant &other) { m_v = other.m_v; return *this; }

        //! Move assignment operatior.
        CVariant &operator =(CVariant && other) { m_v = std::move(other.m_v); return *this; }

        //! Change the internal QVariant
        CVariant &operator =(const QVariant &var) { m_v = var; return *this; }

        //! Change the internal QVariant
        CVariant &operator =(QVariant && var) { m_v = std::move(var); return *this; }

        //! Swap this variant with another.
        void swap(CVariant &other) { m_v.swap(other.m_v); }

        //! Swap the internal QVariant with another.
        void swap(QVariant &other) { m_v.swap(other); }

        //! Construct a variant from a value.
        template <typename T> static CVariant fromValue(T &&value) { return CVariant(QVariant::fromValue(std::forward<T>(value))); }

        //! Synonym for fromValue().
        template <typename T> static CVariant from(T &&value) { return CVariant(QVariant::fromValue(std::forward<T>(value))); }

        //! Change the value.
        template <typename T> void setValue(T &&value) { m_v.setValue(std::forward<T>(value)); }

        //! Synonym for setValue().
        template <typename T> void set(T &&value) { m_v.setValue(std::forward<T>(value)); }

        //! Return the value converted to the type T.
        template <typename T> T value() const { return m_v.value<T>(); }

        //! Synonym for value().
        template <typename T> T to() const { return m_v.value<T>(); }

        //! Returns the value converted to the type T, or a default if it can not be converted.
        //! \details Parameter is passed by value to avoid odr-using the argument in case it is
        //!          an inline-initialized static const integral data member without a definition (§9.4.2/3).
        template <typename T> T valueOrDefault(T def) const { return canConvert<T>() ? value<T>() : def; }

        //! Return the internal QVariant.
        const QVariant &getQVariant() const { return m_v; }

        //! \copydoc CValueObject::toQVariant
        QVariant toQVariant() const { return getQVariant(); }

        //! \copydoc CValueObject::convertFromQVariant
        void convertFromQVariant(const QVariant &v) { m_v = v; }

        //! True if this variant can be converted to the type with the given metatype ID.
        bool canConvert(int typeId) const { return m_v.canConvert(typeId); }

        //! True if this variant can be converted to the type T.
        template <typename T> bool canConvert() const { return m_v.canConvert<T>(); }

        //! Convert this variant to the type with the given metatype ID and return true if successful.
        bool convert(int typeId) { return m_v.convert(typeId); }

        //! \copydoc CValueObject::convertToQString
        QString convertToQString(bool i18n = false) const;

        //! Convert this variant to a bool.
        bool toBool() const { return m_v.toBool(); }

        //! Convert this variant to an integer.
        int toInt() const { return m_v.toInt(); }

        //! Convert this variant to double.
        double toDouble() const { return m_v.toDouble(); }

        //! Convert this variant to QDateTime.
        QDateTime toDateTime() const { return m_v.toDateTime(); }

        //! Set the variant to null.
        void clear() { m_v.clear(); }

        //! True if this variant is null.
        bool isNull() const { return m_v.isNull(); }

        //! True if this variant is valid.
        bool isValid() const { return m_v.isValid(); }

        //! Return the metatype ID of the value in this variant, or QMetaType::User if it is a user type.
        QMetaType::Type type() const { return static_cast<QMetaType::Type>(m_v.type()); }

        //! Return the typename of the value in this variant.
        const char *typeName() const { return m_v.typeName(); }

        //! Return the metatype ID of the value in this variant.
        int userType() const { return m_v.userType(); }

        //! \copydoc CValueObject::toJson
        QJsonObject toJson() const;

        //! \copydoc CValueObject::convertFromJson
        void convertFromJson(const QJsonObject &json);

        //! \copydoc CValueObject::marshallToDbus
        void marshallToDbus(QDBusArgument &argument) const;

        //! \copydoc CValueObject::unmarshallFromDbus
        void unmarshallFromDbus(const QDBusArgument &argument);

        //! Equal operator.
        friend bool operator ==(const CVariant &a, const CVariant &b) { return compare(a, b) == 0; }

        //! Not equal operator.
        //! \todo temporary, remove after refactoring
        friend bool operator !=(const CVariant &a, const CVariant &b) { return compare(a, b) != 0; }

        //! Less than operator.
        friend bool operator <(const CVariant &a, const CVariant &b) { return compare(a, b) < 0; }

        //! Greater than operator.
        //! \todo temporary, remove after refactoring
        friend bool operator >(const CVariant &a, const CVariant &b) { return compare(a, b) > 0; }

        //! Less than or equal operator.
        //! \todo temporary, remove after refactoring
        friend bool operator <=(const CVariant &a, const CVariant &b) { return compare(a, b) <= 0; }

        //! Greater than or equal operator.
        //! \todo temporary, remove after refactoring
        friend bool operator >=(const CVariant &a, const CVariant &b) { return compare(a, b) >= 0; }

        //! \copydoc CValueObject::compare
        friend int compare(const CVariant &a, const CVariant &b) { return compareImpl(a, b); }

        //! \copydoc CValueObject::setPropertyByIndex
        void setPropertyByIndex(const CVariant &variant, const CPropertyIndex &index);

        //! \copydoc CValueObject::propertyByIndex
        CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

        //! \copydoc CValueObject::propertyByIndexAsString
        QString propertyByIndexAsString(const CPropertyIndex &index, bool i18n = false) const;

        //! \copydoc CValueObject::equalsPropertyByIndex
        bool equalsPropertyByIndex(const CVariant &compareValue, const CPropertyIndex &index) const;

    private:
        QVariant m_v;

        Private::IValueObjectMetaInfo *getValueObjectMetaInfo() const { return Private::getValueObjectMetaInfo(m_v); }
        void *data() { return m_v.data(); }
        const void *data() const { return m_v.data(); }

        static int compareImpl(const CVariant &, const CVariant &);
        uint getValueHash() const;
    };

    //! Compare stored value of CVariant with any CValueObject derived class.
    template <class T, class = typename std::enable_if<QMetaTypeId<T>::Defined>::type>
    bool operator ==(const T &value, const CVariant &variant)
    {
        if (variant.canConvert<T>()) { return variant.value<T>() == value; }
        return false;
    }

    //! Compare stored value of CVariant with any CValueObject derived class.
    template <class T, class = typename std::enable_if<QMetaTypeId<T>::Defined>::type>
    bool operator !=(const T &value, const CVariant &variant)
    {
        return !(value == variant);
    }

    //! Compare stored value of CVariant with any CValueObject derived class.
    template <class T, class = typename std::enable_if<QMetaTypeId<T>::Defined>::type>
    bool operator ==(const CVariant &variant, const T &value)
    {
        return value == variant;
    }

    //! Compare stored value of CVariant with any CValueObject derived class.
    template <class T, class = typename std::enable_if<QMetaTypeId<T>::Defined>::type>
    bool operator !=(const CVariant &variant, const T &value)
    {
        return !(value == variant);
    }

    namespace Private
    {
        //! \private Needed so we can copy forward-declared CVariant.
        inline void assign(CVariant &a, const CVariant &b) { a = b; }
    }
} // namespace

Q_DECLARE_METATYPE(BlackMisc::CVariant)

#endif
