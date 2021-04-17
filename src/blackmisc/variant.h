/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_VARIANT_H
#define BLACKMISC_VARIANT_H


#include "blackmisc/blackmiscexport.h"
#include "blackmisc/mixin/mixincompare.h"
#include "blackmisc/mixin/mixindbus.h"
#include "blackmisc/mixin/mixindatastream.h"
#include "blackmisc/inheritancetraits.h"
#include "blackmisc/mixin/mixinjson.h"
#include "blackmisc/range.h"
#include "blackmisc/mixin/mixinstring.h"
#include "blackmisc/mixin/mixinmetatype.h"
#include "blackmisc/variantprivate.h"
#include "blackmisc/icons.h"

#include <QDBusArgument>
#include <QDateTime>
#include <QJsonObject>
#include <QMetaType>
#include <QPixmap>
#include <QString>
#include <QVariant>
#include <QtGlobal>
#include <algorithm>
#include <type_traits>

namespace BlackMisc
{
    class CIcon;
    class CPropertyIndex;

    /*!
     * Wrapper around QVariant which provides transparent access to CValueObject methods
     * of the contained object if it is registered with BlackMisc::registerMetaValueType.
     */
    class BLACKMISC_EXPORT CVariant :
        public Mixin::MetaType<CVariant>,
        public Mixin::EqualsByCompare<CVariant>,
        public Mixin::LessThanByCompare<CVariant>,
        public Mixin::DBusOperators<CVariant>,
        public Mixin::DataStreamOperators<CVariant>,
        public Mixin::JsonOperators<CVariant>,
        public Mixin::String<CVariant>
    {
        template <typename> struct tag {};

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
        CVariant(int typeId, const void *copy) : m_v(typeId, copy) {}

        //! \copydoc CValueObject::qHash
        friend uint qHash(const CVariant &var) { return var.getValueHash(); }

        //! Change the internal QVariant.
        void reset(const QVariant &var) { m_v = var; }

        //! Change the internal QVariant.
        void reset(QVariant &&var) { m_v = std::move(var); }

        //! Copy assignment operator.
        CVariant &operator =(const CVariant &other) = default;

        //! Move assignment operatior.
        CVariant &operator =(CVariant &&other) noexcept = default;

        //! Change the internal QVariant
        CVariant &operator =(const QVariant &var) { m_v = var; return *this; }

        //! Change the internal QVariant
        CVariant &operator =(QVariant &&var) noexcept { m_v = std::move(var); return *this; }

        //! Swap this variant with another.
        void swap(CVariant &other) noexcept { m_v.swap(other.m_v); }

        //! Swap the internal QVariant with another.
        void swap(QVariant &other) noexcept { m_v.swap(other); }

        //! Construct a variant from a value.
        template <typename T> static CVariant fromValue(T &&value)
        {
            static_assert(!std::is_same_v<CVariant, std::decay_t<T>>, "CVariant is an illegal type!");
            return CVariant(QVariant::fromValue(std::forward<T>(value)));
        }

        //! Synonym for fromValue().
        template <typename T> static CVariant from(T &&value)
        {
            static_assert(!std::is_same_v<CVariant, std::decay_t<T>>, "CVariant is an illegal type!");
            return CVariant(QVariant::fromValue(std::forward<T>(value)));
        }

        //! Change the value.
        template <typename T> void setValue(T &&value) { m_v.setValue(std::forward<T>(value)); }

        //! Synonym for setValue().
        template <typename T> void set(T &&value) { m_v.setValue(std::forward<T>(value)); }

        //! Return the value converted to the type T.
        template <typename T> T value() const { return to(tag<T>()); }

        //! Synonym for value().
        template <typename T> T to() const { return to(tag<T>()); }

        //! Returns the value converted to the type T, or a default if it can not be converted.
        //! \details Parameter is passed by value to avoid odr-using the argument in case it is
        //!          an inline-initialized static const integral data member without a definition (§9.4.2/3).
        template <typename T> T valueOrDefault(T def) const { return canConvert<T>() ? value<T>() : def; }

        //! Return the internal QVariant.
        const QVariant &getQVariant() const { return m_v; }

        //! Return the internal QVariant.
        operator const QVariant &() const { return m_v; }

        //! Return the internal QVariant.
        operator QVariant() && { return std::move(m_v); }

        //! True if this variant can be converted to the type with the given metatype ID.
        bool canConvert(int typeId) const;

        //! True if this variant can be converted to the type T.
        template <typename T> bool canConvert() const { return canConvert(qMetaTypeId<T>()); }

        //! Convert this variant to the type with the given metatype ID and return true if successful.
        bool convert(int typeId);

        //! \copydoc BlackMisc::Mixin::String::toQString
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

        //! \copydoc BlackMisc::Mixin::JsonByMetaClass::toJson
        QJsonObject toJson() const;

        //! Convenience function JSON as string
        QString toJsonString(QJsonDocument::JsonFormat format = QJsonDocument::Indented) const;

        //! \copydoc BlackMisc::Mixin::JsonByMetaClass::convertFromJson
        void convertFromJson(const QJsonObject &json);

        //! Call convertFromJson, catch any CJsonException that is thrown and return it as CStatusMessage.
        CStatusMessage convertFromJsonNoThrow(const QJsonObject &json, const CLogCategoryList &categories, const QString &prefix);

        //! To compact JSON format.
        QJsonObject toMemoizedJson() const;

        //! From compact JSON format.
        void convertFromMemoizedJson(const QJsonObject &json, bool allowFallbackToJson);

        //! Call convertFromMemoizedJson, catch any CJsonException that is thrown and return it as CStatusMessage.
        CStatusMessage convertFromMemoizedJsonNoThrow(const QJsonObject &json, const CLogCategoryList &categories, const QString &prefix);

        //! \copydoc BlackMisc::Mixin::DBusByMetaClass::marshallToDbus
        void marshallToDbus(QDBusArgument &argument) const;

        //! \copydoc BlackMisc::Mixin::DBusByMetaClass::unmarshallFromDbus
        void unmarshallFromDbus(const QDBusArgument &argument);

        //! \copydoc BlackMisc::Mixin::DataStreamByMetaClass::marshalToDataStream
        void marshalToDataStream(QDataStream &stream) const;

        //! \copydoc BlackMisc::Mixin::DataStreamByMetaClass::unmarshalFromDataStream
        void unmarshalFromDataStream(QDataStream &stream);

        //! \copydoc CValueObject::compare
        friend int compare(const CVariant &a, const CVariant &b) { return compareImpl(a, b); }

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        QVariant propertyByIndex(BlackMisc::CPropertyIndexRef index) const;

        //! \copydoc CValueObject::equalsPropertyByIndex
        bool equalsPropertyByIndex(const CVariant &compareValue, CPropertyIndexRef index) const;

        //! \copydoc CIcon::toPixmap
        QPixmap toPixmap() const;

        //! \copydoc BlackMisc::Mixin::Icon::toIcon
        CIcons::IconIndex toIcon() const;

        //! If this is an event subscription, return true if it matches the given event.
        bool matches(const CVariant &event) const;

    private:
        QVariant m_v;

        Private::IValueObjectMetaInfo *getValueObjectMetaInfo() const { return Private::getValueObjectMetaInfo(m_v); }
        void *data() { return m_v.data(); }
        const void *data() const { return m_v.data(); }

        static int compareImpl(const CVariant &, const CVariant &);
        uint getValueHash() const;

        template <typename T> T to(tag<T>) const
        {
            auto copy = *this; copy.convert(qMetaTypeId<T>()); return *static_cast<const T*>(copy.data());
        }
        template <typename T> QList<T> to(tag<QList<T>>) const { return toImpl<QList<T>>(); }
        template <typename T> QVector<T> to(tag<QVector<T>>) const { return toImpl<QVector<T>>(); }
        template <typename T> CSequence<T> to(tag<CSequence<T>>) const { return toImpl<CSequence<T>>(); }
        template <typename T> T toImpl() const
        {
            using VT = typename T::value_type;
            T result;
            if (isVariantList()) { for (const auto &v : m_v.value<QVector<CVariant>>())   { result.push_back(v.value<VT>()); } }
            else                 { for (const auto &v : m_v.value<QSequentialIterable>()) { result.push_back(v.value<VT>()); } }
            return result;
        }
        bool isVariantList() const;
    };
} // namespace

Q_DECLARE_METATYPE(BlackMisc::CVariant)

namespace BlackMisc
{
    namespace Private
    {
        //! \private
        template <typename T, typename>
        void maybeRegisterMetaListConvert(int)
        {
            if (QMetaType::hasRegisteredConverterFunction(qMetaTypeId<T>(), qMetaTypeId<QVector<CVariant>>())) { return; }

            QMetaType::registerConverter<T, QVector<CVariant>>([](const T &list) -> QVector<CVariant>
            {
                return list.transform([](const typename T::value_type &v) { return CVariant::from(v); });
            });
            QMetaType::registerConverter<QVector<CVariant>, T>([](const QVector<CVariant> &list) -> T
            {
                return makeRange(list).transform([](const CVariant &v) { return v.to<typename T::value_type>(); });
            });
        }
    }
} // namespace

#endif
