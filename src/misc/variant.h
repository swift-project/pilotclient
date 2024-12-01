// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_VARIANT_H
#define SWIFT_MISC_VARIANT_H

#include <algorithm>
#include <type_traits>

#include <QDBusArgument>
#include <QDateTime>
#include <QJsonObject>
#include <QMetaType>
#include <QPixmap>
#include <QSequentialIterable>
#include <QString>
#include <QVariant>
#include <QtGlobal>

#include "misc/icons.h"
#include "misc/inheritancetraits.h"
#include "misc/mixin/mixincompare.h"
#include "misc/mixin/mixindatastream.h"
#include "misc/mixin/mixindbus.h"
#include "misc/mixin/mixinjson.h"
#include "misc/mixin/mixinstring.h"
#include "misc/range.h"
#include "misc/swiftmiscexport.h"
#include "misc/variantprivate.h"

namespace swift::misc
{
    class CIcon;
    class CPropertyIndex;

    /*!
     * This registers the value type T with the swift::misc:: meta type system,
     * making it available for use with the extended feature set of swift::misc::CVariant.
     *
     * The implementation (ab)uses the QMetaType converter function registration mechanism
     * to store a type-erased representation of the set of operations supported by T.
     */
    template <typename T>
    void registerMetaValueType()
    {
        if (QMetaType::hasRegisteredConverterFunction<T, private_ns::IValueObjectMetaInfo *>()) { return; }
        bool ok = QMetaType::registerConverter<T, private_ns::IValueObjectMetaInfo *>(
            private_ns::CValueObjectMetaInfo<T>::instance);
        Q_ASSERT(ok);
        Q_UNUSED(ok);
    }

    /*!
     * Wrapper around QVariant which provides transparent access to CValueObject methods
     * of the contained object if it is registered with swift::misc::registerMetaValueType.
     */
    class SWIFT_MISC_EXPORT CVariant :
        public mixin::EqualsByCompare<CVariant>,
        public mixin::LessThanByCompare<CVariant>,
        public mixin::DBusOperators<CVariant>,
        public mixin::DataStreamOperators<CVariant>,
        public mixin::JsonOperators<CVariant>,
        public mixin::String<CVariant>
    {
        template <typename>
        struct tag
        {};

    public:
        //! Default constructor.
        CVariant() {}

        //! Copy constructor.
        CVariant(const CVariant &) = default;

        //! Move constructor.
        CVariant(CVariant &&other) noexcept = default;

        //! Construct from a QVariant.
        CVariant(const QVariant &var) : m_v(var) {}

        //! Move-construct from a QVariant.
        CVariant(QVariant &&var) noexcept : m_v(std::move(var)) {}

        //! Construct a null variant of the given type.
        CVariant(QVariant::Type type) : m_v(type) {}

        //! Avoid unexpected implicit cast to QVariant::Type. (Use CVariant::from() instead.)
        CVariant(int) = delete;

        //! Implicit conversion from QString.
        CVariant(const QString &string) : m_v(string) {}

        //! Implicit conversion from C string.
        CVariant(const char *string) : m_v(string) {}

        //! Construct a variant from the given type and opaque pointer.
        CVariant(int typeId, const void *copy) : m_v(QMetaType(typeId), copy) {}

        //! \copydoc CValueObject::qHash
        friend size_t qHash(const CVariant &var) { return var.getValueHash(); }

        //! Change the internal QVariant.
        void reset(const QVariant &var) { m_v = var; }

        //! Change the internal QVariant.
        void reset(QVariant &&var) { m_v = std::move(var); }

        //! Copy assignment operator.
        CVariant &operator=(const CVariant &other) = default;

        //! Move assignment operatior.
        CVariant &operator=(CVariant &&other) noexcept = default;

        //! Change the internal QVariant
        CVariant &operator=(const QVariant &var)
        {
            m_v = var;
            return *this;
        }

        //! Change the internal QVariant
        CVariant &operator=(QVariant &&var) noexcept
        {
            m_v = std::move(var);
            return *this;
        }

        //! Swap this variant with another.
        void swap(CVariant &other) noexcept { m_v.swap(other.m_v); }

        //! Swap the internal QVariant with another.
        void swap(QVariant &other) noexcept { m_v.swap(other); }

        //! Construct a variant from a value.
        template <typename T>
        static CVariant fromValue(T &&value)
        {
            static_assert(!std::is_same_v<CVariant, std::decay_t<T>>, "CVariant is an illegal type!");
            return CVariant(QVariant::fromValue(std::forward<T>(value)));
        }

        //! Synonym for fromValue().
        template <typename T>
        static CVariant from(T &&value)
        {
            static_assert(!std::is_same_v<CVariant, std::decay_t<T>>, "CVariant is an illegal type!");
            return CVariant(QVariant::fromValue(std::forward<T>(value)));
        }

        //! Change the value.
        template <typename T>
        void setValue(T &&value)
        {
            m_v.setValue(std::forward<T>(value));
        }

        //! Synonym for setValue().
        template <typename T>
        void set(T &&value)
        {
            m_v.setValue(std::forward<T>(value));
        }

        //! Return the value converted to the type T.
        template <typename T>
        T value() const
        {
            return to(tag<T>());
        }

        //! Synonym for value().
        template <typename T>
        T to() const
        {
            return to(tag<T>());
        }

        //! Returns the value converted to the type T, or a default if it can not be converted.
        //! \details Parameter is passed by value to avoid odr-using the argument in case it is
        //!          an inline-initialized static const integral data member without a definition (§9.4.2/3).
        template <typename T>
        T valueOrDefault(T def) const
        {
            return canConvert<T>() ? value<T>() : def;
        }

        //! Return the internal QVariant.
        const QVariant &getQVariant() const { return m_v; }

        //! Return the internal QVariant.
        operator const QVariant &() const { return m_v; }

        //! Return the internal QVariant.
        operator QVariant() && { return std::move(m_v); }

        //! True if this variant can be converted to the type with the given metatype ID.
        bool canConvert(int typeId) const;

        //! True if this variant can be converted to the type T.
        template <typename T>
        bool canConvert() const
        {
            return canConvert(qMetaTypeId<T>());
        }

        //! Convert this variant to the type with the given metatype ID and return true if successful.
        bool convert(int typeId);

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! True if this variant's type is an integral type.
        bool isIntegral() const;

        //! True if this variant's type is an integral or floating-point type.
        bool isArithmetic() const;

        //! Convert this variant to a bool.
        bool toBool() const { return m_v.toBool(); }

        //! Convert this variant to an integer.
        int toInt(bool *ok = nullptr) const { return m_v.toInt(ok); }

        //! Convert this variant to a longlong integer.
        qlonglong toLongLong(bool *ok = nullptr) const { return m_v.toLongLong(ok); }

        //! Convert this variant to a unsigned longlong integer.
        qulonglong toULongLong(bool *ok = nullptr) const { return m_v.toULongLong(ok); }

        //! COnvert to qint64, which is used for all timestamps
        qint64 toQInt64(bool *ok = nullptr) const;

        //! Convert this variant to double.
        double toDouble(bool *ok = nullptr) const { return m_v.toDouble(ok); }

        //! Convert this variant to QDateTime.
        QDateTime toDateTime() const { return m_v.toDateTime(); }

        //! Convert this variant to QUrl.
        QUrl toUrl() const { return m_v.toUrl(); }

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

        //! Return the QMetaType of the type in this variant.
        QMetaType metaType() const { return QMetaType(userType()); }

        //! \copydoc swift::misc::mixin::JsonByMetaClass::toJson
        QJsonObject toJson() const;

        //! Convenience function JSON as string
        QString toJsonString(QJsonDocument::JsonFormat format = QJsonDocument::Indented) const;

        //! \copydoc swift::misc::mixin::JsonByMetaClass::convertFromJson
        void convertFromJson(const QJsonObject &json);

        //! Call convertFromJson, catch any CJsonException that is thrown and return it as CStatusMessage.
        CStatusMessage convertFromJsonNoThrow(const QJsonObject &json, const CLogCategoryList &categories,
                                              const QString &prefix);

        //! To compact JSON format.
        QJsonObject toMemoizedJson() const;

        //! From compact JSON format.
        void convertFromMemoizedJson(const QJsonObject &json, bool allowFallbackToJson);

        //! Call convertFromMemoizedJson, catch any CJsonException that is thrown and return it as CStatusMessage.
        CStatusMessage convertFromMemoizedJsonNoThrow(const QJsonObject &json, const CLogCategoryList &categories,
                                                      const QString &prefix);

        //! \copydoc swift::misc::mixin::DBusByMetaClass::marshallToDbus
        void marshallToDbus(QDBusArgument &argument) const;

        //! \copydoc swift::misc::mixin::DBusByMetaClass::unmarshallFromDbus
        void unmarshallFromDbus(const QDBusArgument &argument);

        //! \copydoc swift::misc::mixin::DataStreamByMetaClass::marshalToDataStream
        void marshalToDataStream(QDataStream &stream) const;

        //! \copydoc swift::misc::mixin::DataStreamByMetaClass::unmarshalFromDataStream
        void unmarshalFromDataStream(QDataStream &stream);

        //! \copydoc CValueObject::compare
        friend int compare(const CVariant &a, const CVariant &b) { return compareImpl(a, b); }

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(swift::misc::CPropertyIndexRef index) const;

        //! \copydoc CValueObject::equalsPropertyByIndex
        bool equalsPropertyByIndex(const CVariant &compareValue, CPropertyIndexRef index) const;

        //! \copydoc CIcon::toPixmap
        QPixmap toPixmap() const;

        //! \copydoc swift::misc::mixin::Icon::toIcon
        CIcons::IconIndex toIcon() const;

        //! \copydoc swift::misc::mixin::MetaType::registerMetadata
        static void registerMetadata();

        //! \copydoc swift::misc::mixin::MetaType::getMetaTypeId
        int getMetaTypeId() const;

        //! \copydoc swift::misc::mixin::MetaType::getClassName
        QString getClassName() const;

        //! \copydoc swift::misc::mixin::MetaType::isA
        bool isA(int metaTypeId) const;

        //! If this is an event subscription, return true if it matches the given event.
        bool matches(const CVariant &event) const;

    private:
        QVariant m_v;

        private_ns::IValueObjectMetaInfo *getValueObjectMetaInfo() const
        {
            return private_ns::getValueObjectMetaInfo(m_v);
        }
        void *data() { return m_v.data(); }
        const void *data() const { return m_v.data(); }

        static int compareImpl(const CVariant &, const CVariant &);
        size_t getValueHash() const;

        template <typename T>
        T to(tag<T>) const
        {
            auto copy = *this;
            copy.convert(qMetaTypeId<T>());
            return *static_cast<const T *>(copy.data());
        }
        template <typename T>
        QList<T> to(tag<QList<T>>) const
        {
            return toImpl<QList<T>>();
        }
        template <typename T>
        CSequence<T> to(tag<CSequence<T>>) const
        {
            return toImpl<CSequence<T>>();
        }
        template <typename T>
        T toImpl() const
        {
            using VT = typename T::value_type;
            T result;
            if (isVariantList())
            {
                for (const auto &v : m_v.value<QVector<CVariant>>()) { result.push_back(v.value<VT>()); }
            }
            else
            {
                for (const auto &v : m_v.value<QSequentialIterable>()) { result.push_back(v.value<VT>()); }
            }
            return result;
        }
        bool isVariantList() const;
    };
} // namespace swift::misc

Q_DECLARE_METATYPE(swift::misc::CVariant)

namespace swift::misc::private_ns
{
    //! \cond PRIVATE
    template <typename T>
    void MetaTypeHelper<T>::maybeRegisterMetaList()
    {
        if constexpr (canConvertVariantList<T>(0))
        {
            if (QMetaType::hasRegisteredConverterFunction(QMetaType(qMetaTypeId<T>()),
                                                          QMetaType(qMetaTypeId<QVector<CVariant>>())))
            {
                return;
            }

            QMetaType::registerConverter<T, QVector<CVariant>>([](const T &list) -> QVector<CVariant> {
                return list.transform([](const typename T::value_type &v) { return CVariant::from(v); });
            });
            QMetaType::registerConverter<QVector<CVariant>, T>([](const QVector<CVariant> &list) -> T {
                return makeRange(list).transform([](const CVariant &v) { return v.to<typename T::value_type>(); });
            });
        }
    }
    //! \endcond
} // namespace swift::misc::private_ns

#endif // SWIFT_MISC_VARIANT_H
