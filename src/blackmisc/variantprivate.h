/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_VARIANT_PRIVATE_H
#define BLACKMISC_VARIANT_PRIVATE_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/inheritancetraits.h"
#include <QString>
#include <QMetaType>
#include <QDBusMetaType>
#include <QDBusArgument>
#include <QJsonObject>
#include <stdexcept>

namespace BlackMisc
{
    class CEmpty;
    class CVariant;
    class CPropertyIndexRef;
    class CIcon;

    template <typename T>
    class CSequence;

    template <typename T>
    void registerMetaValueType();

    namespace Private
    {
        //! \private Abstract base class representing the set of operations supported by a particular value type.
        struct BLACKMISC_EXPORT IValueObjectMetaInfo
        {
            virtual ~IValueObjectMetaInfo() = default;
            virtual QString toQString(const void *object, bool i18n) const = 0;
            virtual QJsonObject toJson(const void *object) const = 0;
            virtual void convertFromJson(const QJsonObject &json, void *object) const = 0;
            virtual QJsonObject toMemoizedJson(const void *object) const = 0;
            virtual void convertFromMemoizedJson(const QJsonObject &json, void *object, bool allowFallbackToJson) const = 0;
            virtual void unmarshall(const QDBusArgument &arg, void *object) const = 0;
            virtual uint getValueHash(const void *object) const = 0;
            virtual int getMetaTypeId() const = 0;
            virtual const void *upCastTo(const void *object, int metaTypeId) const = 0;
            virtual int compareImpl(const void *lhs, const void *rhs) const = 0;
            virtual void setPropertyByIndex(void *object, const QVariant &variant, CPropertyIndexRef index) const = 0;
            virtual void propertyByIndex(const void *object, QVariant &o_variant, BlackMisc::CPropertyIndexRef index) const = 0;
            virtual bool equalsPropertyByIndex(const void *object, const QVariant &compareValue, CPropertyIndexRef index) const = 0;
            virtual bool matches(const void *object, const CVariant &value) const = 0;
            virtual int toIcon(const void *object) const = 0;
        };

        //! \private Exception to signal that an unsupported operation was requested.
        class CVariantException : public std::invalid_argument
        {
        public:
            template <class T>
            CVariantException(const T &, const QString &opName) : CVariantException(qMetaTypeId<T>(), opName) {}

            CVariantException(int typeId, const QString &opName) : std::invalid_argument((blurb(typeId, opName)).toStdString()), m_operationName(opName) {}

            const QString &operationName() const { return m_operationName; }

            ~CVariantException() {}

        private:
            QString m_operationName;

            static QString blurb(int typeId, const QString &operationName)
            {
                return QString("CVariant requested unsupported operation of contained ") + QMetaType::typeName(typeId) + " object: " + operationName;
            }
        };

        //! \private Helper using expression SFINAE to select the correct method implementations.
        struct CValueObjectMetaInfoHelper
        {
            template <typename T>
            static QJsonObject toJson(const T &object, decltype(static_cast<void>(object.toJson()), 0)) { return object.toJson(); }
            template <typename T>
            static QJsonObject toJson(const T &object, ...) { throw CVariantException(object, "toJson"); }

            template <typename T>
            static void convertFromJson(const QJsonObject &json, T &object, decltype(static_cast<void>(object.convertFromJson(json)), 0)) { object.convertFromJson(json); }
            template <typename T>
            static void convertFromJson(const QJsonObject &, T &object, ...) { throw CVariantException(object, "convertFromJson"); }

            template <typename T>
            static QJsonObject toMemoizedJson(const T &object, decltype(static_cast<void>(object.toMemoizedJson()), 0)) { return object.toMemoizedJson(); }
            template <typename T>
            static QJsonObject toMemoizedJson(const T &object, ...) { return toJson(object, 0); }

            template <typename T>
            static void convertFromMemoizedJson(const QJsonObject &json, T &object, bool allowFallbackToJson, decltype(static_cast<void>(object.convertFromMemoizedJson(json, allowFallbackToJson)), 0)) { object.convertFromMemoizedJson(json, allowFallbackToJson); }
            template <typename T>
            static void convertFromMemoizedJson(const QJsonObject &json, T &object, bool allowFallbackToJson, ...) { convertFromJson(json, object, 0); Q_UNUSED(allowFallbackToJson) }

            template <typename T>
            static uint getValueHash(const T &object, decltype(static_cast<void>(qHash(object)), 0)) { return qHash(object); }
            template <typename T>
            static uint getValueHash(const T &object, ...) { throw CVariantException(object, "getValueHash"); }

            template <typename T>
            static int compareImpl(const T &lhs, const T &rhs, decltype(static_cast<void>(compare(lhs, rhs)), 0)) { return compare(lhs, rhs); }
            template <typename T>
            static int compareImpl(const T &lhs, const T &, ...) { throw CVariantException(lhs, "compare"); }

            template <typename T>
            static void setPropertyByIndex(T &object, const QVariant &variant, CPropertyIndexRef index, decltype(static_cast<void>(object.setPropertyByIndex(index, variant)), 0)) { object.setPropertyByIndex(index, variant); }
            template <typename T>
            static void setPropertyByIndex(T &object, const QVariant &, CPropertyIndexRef, ...) { throw CVariantException(object, "setPropertyByIndex"); }

            template <typename T>
            static void propertyByIndex(QVariant &o_variant, const T &object, CPropertyIndexRef index, decltype(static_cast<void>(object.propertyByIndex(index)), 0)) { o_variant = object.propertyByIndex(index); }
            template <typename T>
            static void propertyByIndex(QVariant &, const T &object, CPropertyIndexRef, ...) { throw CVariantException(object, "propertyByIndex"); }

            template <typename T>
            static bool equalsPropertyByIndex(const T &object, const QVariant &variant, CPropertyIndexRef index, decltype(static_cast<void>(object.equalsPropertyByIndex(variant, index)), 0)) { return object.equalsPropertyByIndex(variant, index); }
            template <typename T>
            static bool equalsPropertyByIndex(const T &object, const QVariant &, CPropertyIndexRef, ...) { throw CVariantException(object, "equalsPropertyByIndex"); }

            template <typename T>
            static int toIcon(const T &object, std::enable_if_t < ! std::is_same_v<T, CVariant>, decltype(static_cast<void>(object.toIcon()), 0) >) { return object.toIcon(); }
            template <typename T>
            static int toIcon(const T &object, ...) { throw CVariantException(object, "toIcon"); }

            template <typename T>
            static bool matches(const T &object, const CVariant &value, decltype(static_cast<void>(object.matches(value)), 0)) { return object.matches(value); }
            template <typename T>
            static bool matches(const T &object, const CVariant &, ...) { throw CVariantException(object, "matches"); }
        };

        //! \private Implementation of IValueObjectMetaInfo representing the set of operations supported by T.
        template <typename T>
        struct CValueObjectMetaInfo : public IValueObjectMetaInfo
        {
            virtual QString toQString(const void *object, bool i18n) const override
            {
                return cast(object).toQString(i18n);
            }
            virtual QJsonObject toJson(const void *object) const override
            {
                return CValueObjectMetaInfoHelper::toJson(cast(object), 0);
            }
            virtual void convertFromJson(const QJsonObject &json, void *object) const override
            {
                CValueObjectMetaInfoHelper::convertFromJson(json, cast(object), 0);
            }
            virtual QJsonObject toMemoizedJson(const void *object) const override
            {
                return CValueObjectMetaInfoHelper::toMemoizedJson(cast(object), 0);
            }
            virtual void convertFromMemoizedJson(const QJsonObject &json, void *object, bool allowFallbackToJson) const override
            {
                CValueObjectMetaInfoHelper::convertFromMemoizedJson(json, cast(object), allowFallbackToJson, 0);
            }
            virtual void unmarshall(const QDBusArgument &arg, void *object) const override
            {
                arg >> cast(object);
            }
            virtual uint getValueHash(const void *object) const override
            {
                return CValueObjectMetaInfoHelper::getValueHash(cast(object), 0);
            }
            virtual int getMetaTypeId() const override
            {
                return maybeGetMetaTypeId(std::bool_constant<QMetaTypeId<T>::Defined> {});
            }
            virtual const void *upCastTo(const void *object, int metaTypeId) const override
            {
                const auto base = static_cast<const void *>(static_cast<const TMetaBaseOfT<T> *>(&cast(object)));
                return metaTypeId == getMetaTypeId() ? object : CValueObjectMetaInfo<TMetaBaseOfT<T>> {} .upCastTo(base, metaTypeId);
            }
            virtual int compareImpl(const void *lhs, const void *rhs) const override
            {
                return CValueObjectMetaInfoHelper::compareImpl(cast(lhs), cast(rhs), 0);
            }
            virtual void setPropertyByIndex(void *object, const QVariant &variant, CPropertyIndexRef index) const override
            {
                CValueObjectMetaInfoHelper::setPropertyByIndex(cast(object), variant, index, 0);
            }
            virtual void propertyByIndex(const void *object, QVariant &o_variant, BlackMisc::CPropertyIndexRef index) const override
            {
                CValueObjectMetaInfoHelper::propertyByIndex(o_variant, cast(object), index, 0);
            }
            virtual bool equalsPropertyByIndex(const void *object, const QVariant &compareValue, CPropertyIndexRef index) const override
            {
                return CValueObjectMetaInfoHelper::equalsPropertyByIndex(cast(object), compareValue, index, 0);
            }
            virtual int toIcon(const void *object) const override
            {
                return CValueObjectMetaInfoHelper::toIcon(cast(object), 0);
            }
            virtual bool matches(const void *object, const CVariant &value) const override
            {
                return CValueObjectMetaInfoHelper::matches(cast(object), value, 0);
            }

            static const T &cast(const void *object) { return *static_cast<const T *>(object); }
            static T &cast(void *object) { return *static_cast<T *>(object); }

            static int maybeGetMetaTypeId(std::true_type) { return qMetaTypeId<T>(); }
            static int maybeGetMetaTypeId(std::false_type) { return QMetaType::UnknownType; }
        };

        //! \private Explicit specialization for the terminating case of the recursive CValueObjectMetaInfo::upCastTo.
        template <>
        struct CValueObjectMetaInfo<void>
        {
            const void *upCastTo(const void *, int) const
            {
                return nullptr;
            }
        };

        //! \private Getter to obtain the IValueObjectMetaInfo which was stored by BlackMisc::registerMetaValueType.
        IValueObjectMetaInfo *getValueObjectMetaInfo(int typeId);

        //! \private Getter to obtain the IValueObjectMetaInfo which was stored by BlackMisc::registerMetaValueType.
        IValueObjectMetaInfo *getValueObjectMetaInfo(const QVariant &);

        //! \private Getter to obtain the IValueObjectMetaInfo which was stored by BlackMisc::registerMetaValueType.
        template <typename T>
        IValueObjectMetaInfo *getValueObjectMetaInfo() { return getValueObjectMetaInfo(qMetaTypeId<T>()); }

        //! \cond PRIVATE
        template <typename T, typename = std::enable_if_t<std::is_base_of_v<CSequence<typename T::value_type>, T> && ! std::is_same_v<typename T::value_type, CVariant>>>
        void maybeRegisterMetaListConvert(int);
        template <typename T>
        void maybeRegisterMetaListConvert(...) {}

        template <typename T, bool IsRegisteredMetaType /* = true */>
        struct MetaTypeHelperImpl
        {
            static constexpr int maybeGetMetaTypeId() { return qMetaTypeId<T>(); }
            static void maybeRegisterMetaType() { qRegisterMetaType<T>(); qDBusRegisterMetaType<T>(); qRegisterMetaTypeStreamOperators<T>(); registerMetaValueType<T>(); maybeRegisterMetaListConvert<T>(0); }
        };

        template <typename T>
        struct MetaTypeHelperImpl<T, /* IsRegisteredMetaType = */ false>
        {
            static constexpr int maybeGetMetaTypeId() { return QMetaType::UnknownType; }
            static void maybeRegisterMetaType() {}
        };

        template <typename T>
        using MetaTypeHelper = MetaTypeHelperImpl<T, QMetaTypeId<T>::Defined>;
        //! \endcond
    }
}

Q_DECLARE_METATYPE(BlackMisc::Private::IValueObjectMetaInfo *)

#endif
