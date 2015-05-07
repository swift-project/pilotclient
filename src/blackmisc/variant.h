/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_VARIANT_H
#define BLACKMISC_VARIANT_H

#include "variant_private.h"
#include "blackmiscexport.h"
#include "blackmiscfreefunctions.h"
#include "tuple.h"
#include "compare.h"
#include "dbus.h"
#include "json.h"
#include <QVariant>
#include <QDateTime>
#include <QJsonValueRef>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonArray>

namespace BlackMisc
{
    class CPropertyIndex;
    class CIcon;

    namespace Mixin
    {

        /*!
         * CRTP class template from which a derived class can inherit common methods dealing with the metatype of the class.
         *
         * \see BLACKMISC_DECLARE_USING_MIXIN_METATYPE
         */
        template <class Derived, class... AdditionalTypes>
        class MetaType
        {
        public:
            //! Register metadata
            static void registerMetadata()
            {
                Private::MetaTypeHelper<Derived>::maybeRegisterMetaType();
                //! \todo KB: What does this monster do? Please some comment
                [](...){}((qRegisterMetaType<AdditionalTypes>(), qDBusRegisterMetaType<AdditionalTypes>(), 0)...);
            }

            //! Returns the Qt meta type ID of this object.
            int getMetaTypeId() const
            {
                return Private::MetaTypeHelper<Derived>::maybeGetMetaTypeId();
            }

            //! Returns true if this object is an instance of the class with the given meta type ID, or one of its subclasses.
            bool isA(int metaTypeId) const
            {
                if (metaTypeId == QMetaType::UnknownType) { return false; }
                if (metaTypeId == getMetaTypeId()) { return true; }
                return baseIsA(static_cast<const MetaBaseOfT<Derived> *>(derived()), metaTypeId);
            }

            //! Method to return CVariant
            //! \deprecated Use CVariant::to() instead.
            CVariant toCVariant() const;

            //! Set from CVariant
            //! \deprecated Use CVariant::from() instead.
            void convertFromCVariant(const CVariant &variant);

            //! Return QVariant, used with DBus QVariant lists
            //! \deprecated Use QVariant::fromValue() instead.
            QVariant toQVariant() const
            {
                return Private::MetaTypeHelper<Derived>::maybeToQVariant(*derived());
            }

            //! Set from QVariant
            //! \deprecated Use QVariant::value() instead.
            void convertFromQVariant(const QVariant &variant)
            {
                return Private::MetaTypeHelper<Derived>::maybeConvertFromQVariant(*derived(), variant);
            }

        private:
            const Derived *derived() const { return static_cast<const Derived *>(this); }
            Derived *derived() { return static_cast<Derived *>(this); }

            template <typename Base2> static bool baseIsA(const Base2 *base, int metaTypeId) { return base->isA(metaTypeId); }
            static bool baseIsA(const void *, int) { return false; }
        };

        /*!
         * Variant of MetaType mixin which also registers QList<Derived> with the type system.
         *
         * \see BLACKMISC_DECLARE_USING_MIXIN_METATYPE_AND_QLIST
         */
        template <class Derived>
        class MetaTypeAndQList : public MetaType<Derived, QList<Derived>>
        {};

        /*!
         * When a derived class and a base class both inherit from Mixin::MetaType,
         * the derived class uses this macro to disambiguate the inherited members.
         */
#       define BLACKMISC_DECLARE_USING_MIXIN_METATYPE(DERIVED)                  \
            using ::BlackMisc::Mixin::MetaType<DERIVED>::registerMetadata;      \
            using ::BlackMisc::Mixin::MetaType<DERIVED>::getMetaTypeId;         \
            using ::BlackMisc::Mixin::MetaType<DERIVED>::isA;                   \
            using ::BlackMisc::Mixin::MetaType<DERIVED>::toCVariant;            \
            using ::BlackMisc::Mixin::MetaType<DERIVED>::toQVariant;            \
            using ::BlackMisc::Mixin::MetaType<DERIVED>::convertFromCVariant;   \
            using ::BlackMisc::Mixin::MetaType<DERIVED>::convertFromQVariant;

        /*!
         * When a derived class and a base class both inherit from Mixin::MetaType,
         * the derived class uses this macro to disambiguate the inherited members.
         */
#       define BLACKMISC_DECLARE_USING_MIXIN_METATYPE_AND_QLIST(DERIVED)                \
            using ::BlackMisc::Mixin::MetaTypeAndQList<DERIVED>::registerMetadata;      \
            using ::BlackMisc::Mixin::MetaTypeAndQList<DERIVED>::getMetaTypeId;         \
            using ::BlackMisc::Mixin::MetaTypeAndQList<DERIVED>::isA;                   \
            using ::BlackMisc::Mixin::MetaTypeAndQList<DERIVED>::toCVariant;            \
            using ::BlackMisc::Mixin::MetaTypeAndQList<DERIVED>::toQVariant;            \
            using ::BlackMisc::Mixin::MetaTypeAndQList<DERIVED>::convertFromCVariant;   \
            using ::BlackMisc::Mixin::MetaTypeAndQList<DERIVED>::convertFromQVariant;

    } // Mixin

    /*!
     * This registers the value type T with the BlackMisc meta type system,
     * making it available for use with the extended feature set of BlackMisc::CVariant.
     *
     * The implementation (ab)uses the QMetaType converter function registration mechanism
     * to store a type-erased representation of the set of operations supported by T.
     * Unlike the singleton pattern, this approach means that CVariant can be used in plugins.
     */
    template <typename T>
    void registerMetaValueType()
    {
        if (QMetaType::hasRegisteredConverterFunction<T, Private::IValueObjectMetaInfo *>()) { return; }
        auto converter = [](const T &) { static Private::CValueObjectMetaInfo<T> info; return &info; };
        bool ok = QMetaType::registerConverter<T, Private::IValueObjectMetaInfo *>(converter);
        Q_ASSERT(ok);
        Q_UNUSED(ok);
    }

    /*!
     * Wrapper around QVariant which provides transparent access to CValueObject methods
     * of the contained object if it is registered with BlackMisc::registerMetaValueType.
     */
    class BLACKMISC_EXPORT CVariant :
        public Mixin::MetaType<CVariant>,
        public Mixin::EqualsByCompare<CVariant>,
        public Mixin::LessThanByCompare<CVariant>,
        public Mixin::DBusOperators<CVariant>,
        public Mixin::JsonOperators<CVariant>,
        public Mixin::String<CVariant>
    {
    public:
        //! Default constructor.
        CVariant() {}

        //! Copy constructor.
        CVariant(const CVariant &) = default;

        //! Move constructor.
        CVariant(CVariant &&other) : m_v(std::move(other.m_v)) {}

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

        //! \copydoc CValueObject::toPixmap
        QPixmap toPixmap() const;

        //! \copydoc CValueObject::toIcon
        CIcon toIcon() const;

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

    namespace Mixin
    {
        template <class Derived, class... AdditionalTypes>
        CVariant MetaType<Derived, AdditionalTypes...>::toCVariant() const
        {
            return CVariant(derived()->toQVariant());
        }
        template <class Derived, class... AdditionalTypes>
        void MetaType<Derived, AdditionalTypes...>::convertFromCVariant(const CVariant &variant)
        {
            derived()->convertFromQVariant(variant.getQVariant());
        }
    }

} // namespace

Q_DECLARE_METATYPE(BlackMisc::CVariant)

#endif
