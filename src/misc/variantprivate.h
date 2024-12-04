// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_VARIANT_PRIVATE_H
#define SWIFT_MISC_VARIANT_PRIVATE_H

#include <stdexcept>

#include <QDBusArgument>
#include <QDBusMetaType>
#include <QJsonObject>
#include <QMetaType>
#include <QString>

#include "misc/inheritancetraits.h"
#include "misc/propertyindexref.h"
#include "misc/swiftmiscexport.h"

namespace swift::misc
{
    class CEmpty;
    class CVariant;
    class CPropertyIndexRef;
    class CIcon;

    template <typename T>
    class CSequence;

    template <typename T>
    void registerMetaValueType();

    namespace private_ns
    {
        //! \private Abstract base class representing the set of operations supported by a particular value type.
        struct SWIFT_MISC_EXPORT IValueObjectMetaInfo
        {
            virtual ~IValueObjectMetaInfo() = default;
            virtual QString toQString(const void *object, bool i18n) const = 0;
            virtual QJsonObject toJson(const void *object) const = 0;
            virtual void convertFromJson(const QJsonObject &json, void *object) const = 0;
            virtual QJsonObject toMemoizedJson(const void *object) const = 0;
            virtual void convertFromMemoizedJson(const QJsonObject &json, void *object,
                                                 bool allowFallbackToJson) const = 0;
            virtual void unmarshall(const QDBusArgument &arg, void *object) const = 0;
            virtual size_t getValueHash(const void *object) const = 0;
            virtual int getMetaTypeId() const = 0;
            virtual const void *upCastTo(const void *object, int metaTypeId) const = 0;
            virtual int compareImpl(const void *lhs, const void *rhs) const = 0;
            virtual void setPropertyByIndex(void *object, const QVariant &variant, CPropertyIndexRef index) const = 0;
            virtual void propertyByIndex(const void *object, QVariant &o_variant,
                                         swift::misc::CPropertyIndexRef index) const = 0;
            virtual bool equalsPropertyByIndex(const void *object, const QVariant &compareValue,
                                               CPropertyIndexRef index) const = 0;
            virtual bool matches(const void *object, const CVariant &value) const = 0;
            virtual int toIcon(const void *object) const = 0;
        };

        //! \private Exception to signal that an unsupported operation was requested.
        class CVariantException : public std::invalid_argument
        {
        public:
            template <class T>
            CVariantException(const T &, const QString &opName) : CVariantException(qMetaTypeId<T>(), opName)
            {}

            CVariantException(int typeId, const QString &opName)
                : std::invalid_argument((blurb(typeId, opName)).toStdString()), m_operationName(opName)
            {}

            const QString &operationName() const { return m_operationName; }

            ~CVariantException() {}

        private:
            QString m_operationName;

            static QString blurb(int typeId, const QString &operationName)
            {
                return QString("CVariant requested unsupported operation of contained ") + QMetaType::typeName(typeId) +
                       " object: " + operationName;
            }
        };

        //! \private Helper using expression SFINAE to select the correct method implementations.
        struct CValueObjectMetaInfoHelper
        {
            template <typename T>
            static QJsonObject toJson(const T &object, decltype(static_cast<void>(object.toJson()), 0))
            {
                return object.toJson();
            }
            template <typename T>
            static QJsonObject toJson(const T &object, ...)
            {
                throw CVariantException(object, "toJson");
            }

            template <typename T>
            static void convertFromJson(const QJsonObject &json, T &object,
                                        decltype(static_cast<void>(object.convertFromJson(json)), 0))
            {
                object.convertFromJson(json);
            }
            template <typename T>
            static void convertFromJson(const QJsonObject &, T &object, ...)
            {
                throw CVariantException(object, "convertFromJson");
            }

            template <typename T>
            static QJsonObject toMemoizedJson(const T &object, decltype(static_cast<void>(object.toMemoizedJson()), 0))
            {
                return object.toMemoizedJson();
            }
            template <typename T>
            static QJsonObject toMemoizedJson(const T &object, ...)
            {
                return toJson(object, 0);
            }

            template <typename T>
            static void convertFromMemoizedJson(
                const QJsonObject &json, T &object, bool allowFallbackToJson,
                decltype(static_cast<void>(object.convertFromMemoizedJson(json, allowFallbackToJson)), 0))
            {
                object.convertFromMemoizedJson(json, allowFallbackToJson);
            }
            template <typename T>
            static void convertFromMemoizedJson(const QJsonObject &json, T &object, bool allowFallbackToJson, ...)
            {
                convertFromJson(json, object, 0);
                Q_UNUSED(allowFallbackToJson)
            }

            template <typename T>
            static size_t getValueHash(const T &object, decltype(static_cast<void>(qHash(object)), 0))
            {
                return qHash(object);
            }
            template <typename T>
            static size_t getValueHash(const T &object, ...)
            {
                throw CVariantException(object, "getValueHash");
            }

            template <typename T>
            static int compareImpl(const T &lhs, const T &rhs, decltype(static_cast<void>(compare(lhs, rhs)), 0))
            {
                return compare(lhs, rhs);
            }
            template <typename T>
            static int compareImpl(const T &lhs, const T &, ...)
            {
                throw CVariantException(lhs, "compare");
            }

            template <typename T>
            static void setPropertyByIndex(T &object, const QVariant &variant, CPropertyIndexRef index,
                                           decltype(static_cast<void>(object.setPropertyByIndex(index, variant)), 0))
            {
                object.setPropertyByIndex(index, variant);
            }
            template <typename T>
            static void setPropertyByIndex(T &object, const QVariant &, CPropertyIndexRef, ...)
            {
                throw CVariantException(object, "setPropertyByIndex");
            }

            template <typename T>
            static void propertyByIndex(QVariant &o_variant, const T &object, CPropertyIndexRef index,
                                        decltype(static_cast<void>(object.propertyByIndex(index)), 0))
            {
                o_variant = object.propertyByIndex(index);
            }
            template <typename T>
            static void propertyByIndex(QVariant &, const T &object, CPropertyIndexRef, ...)
            {
                throw CVariantException(object, "propertyByIndex");
            }

            template <typename T>
            static bool equalsPropertyByIndex(const T &object, const QVariant &variant, CPropertyIndexRef index,
                                              decltype(static_cast<void>(object.equalsPropertyByIndex(variant, index)),
                                                       0))
            {
                return object.equalsPropertyByIndex(variant, index);
            }
            template <typename T>
            static bool equalsPropertyByIndex(const T &object, const QVariant &, CPropertyIndexRef, ...)
            {
                throw CVariantException(object, "equalsPropertyByIndex");
            }

            template <typename T>
            static int
            toIcon(const T &object,
                   std::enable_if_t<!std::is_same_v<T, CVariant>, decltype(static_cast<void>(object.toIcon()), 0)>)
            {
                return object.toIcon();
            }
            template <typename T>
            static int toIcon(const T &object, ...)
            {
                throw CVariantException(object, "toIcon");
            }

            template <typename T>
            static bool matches(const T &object, const CVariant &value,
                                decltype(static_cast<void>(object.matches(value)), 0))
            {
                return object.matches(value);
            }
            template <typename T>
            static bool matches(const T &object, const CVariant &, ...)
            {
                throw CVariantException(object, "matches");
            }
        };

        //! \private Implementation of IValueObjectMetaInfo representing the set of operations supported by T.
        template <typename T>
        struct CValueObjectMetaInfo : public IValueObjectMetaInfo
        {
            CValueObjectMetaInfo();
            virtual ~CValueObjectMetaInfo() override;
            static CValueObjectMetaInfo *instance(const T &);

            CValueObjectMetaInfo(const CValueObjectMetaInfo &) = delete;
            CValueObjectMetaInfo &operator=(const CValueObjectMetaInfo &) = delete;

            virtual QString toQString(const void *object, bool i18n) const override;
            virtual QJsonObject toJson(const void *object) const override;
            virtual void convertFromJson(const QJsonObject &json, void *object) const override;
            virtual QJsonObject toMemoizedJson(const void *object) const override;
            virtual void convertFromMemoizedJson(const QJsonObject &json, void *object,
                                                 bool allowFallbackToJson) const override;
            virtual void unmarshall(const QDBusArgument &arg, void *object) const override;
            virtual size_t getValueHash(const void *object) const override;
            virtual int getMetaTypeId() const override;
            virtual const void *upCastTo(const void *object, int metaTypeId) const override;
            virtual int compareImpl(const void *lhs, const void *rhs) const override;
            virtual void setPropertyByIndex(void *object, const QVariant &variant,
                                            CPropertyIndexRef index) const override;
            virtual void propertyByIndex(const void *object, QVariant &o_variant,
                                         swift::misc::CPropertyIndexRef index) const override;
            virtual bool equalsPropertyByIndex(const void *object, const QVariant &compareValue,
                                               CPropertyIndexRef index) const override;
            virtual int toIcon(const void *object) const override;
            virtual bool matches(const void *object, const CVariant &value) const override;

            static const T &cast(const void *object);
            static T &cast(void *object);
        };

        template <typename T>
        CValueObjectMetaInfo<T>::CValueObjectMetaInfo()
        {}
        template <typename T>
        CValueObjectMetaInfo<T>::~CValueObjectMetaInfo()
        {}
        template <typename T>
        CValueObjectMetaInfo<T> *CValueObjectMetaInfo<T>::instance(const T &)
        {
            static CValueObjectMetaInfo mi;
            return &mi;
        }
        template <typename T>
        QString CValueObjectMetaInfo<T>::toQString(const void *object, bool i18n) const
        {
            return cast(object).toQString(i18n);
        }
        template <typename T>
        QJsonObject CValueObjectMetaInfo<T>::toJson(const void *object) const
        {
            return CValueObjectMetaInfoHelper::toJson(cast(object), 0);
        }
        template <typename T>
        void CValueObjectMetaInfo<T>::convertFromJson(const QJsonObject &json, void *object) const
        {
            CValueObjectMetaInfoHelper::convertFromJson(json, cast(object), 0);
        }
        template <typename T>
        QJsonObject CValueObjectMetaInfo<T>::toMemoizedJson(const void *object) const
        {
            return CValueObjectMetaInfoHelper::toMemoizedJson(cast(object), 0);
        }
        template <typename T>
        void CValueObjectMetaInfo<T>::convertFromMemoizedJson(const QJsonObject &json, void *object,
                                                              bool allowFallbackToJson) const
        {
            CValueObjectMetaInfoHelper::convertFromMemoizedJson(json, cast(object), allowFallbackToJson, 0);
        }
        template <typename T>
        void CValueObjectMetaInfo<T>::unmarshall(const QDBusArgument &arg, void *object) const
        {
            arg >> cast(object);
        }
        template <typename T>
        size_t CValueObjectMetaInfo<T>::getValueHash(const void *object) const
        {
            return CValueObjectMetaInfoHelper::getValueHash(cast(object), 0);
        }
        template <typename T>
        int CValueObjectMetaInfo<T>::getMetaTypeId() const
        {
            if constexpr (QMetaTypeId<T>::Defined) { return qMetaTypeId<T>(); }
            else { return QMetaType::UnknownType; }
        }
        template <typename T>
        const void *CValueObjectMetaInfo<T>::upCastTo(const void *object, int metaTypeId) const
        {
            if constexpr (THasMetaBaseV<T>)
            {
                const auto base = static_cast<const void *>(static_cast<const TMetaBaseOfT<T> *>(&cast(object)));
                return metaTypeId == getMetaTypeId() ?
                           object :
                           CValueObjectMetaInfo<TMetaBaseOfT<T>>::instance(cast(object))->upCastTo(base, metaTypeId);
            }
            else
            {
                Q_UNUSED(metaTypeId);
                return object;
            }
        }
        template <typename T>
        int CValueObjectMetaInfo<T>::compareImpl(const void *lhs, const void *rhs) const
        {
            return CValueObjectMetaInfoHelper::compareImpl(cast(lhs), cast(rhs), 0);
        }
        template <typename T>
        void CValueObjectMetaInfo<T>::setPropertyByIndex(void *object, const QVariant &variant,
                                                         CPropertyIndexRef index) const
        {
            CValueObjectMetaInfoHelper::setPropertyByIndex(cast(object), variant, index, 0);
        }
        template <typename T>
        void CValueObjectMetaInfo<T>::propertyByIndex(const void *object, QVariant &o_variant,
                                                      swift::misc::CPropertyIndexRef index) const
        {
            CValueObjectMetaInfoHelper::propertyByIndex(o_variant, cast(object), index, 0);
        }
        template <typename T>
        bool CValueObjectMetaInfo<T>::equalsPropertyByIndex(const void *object, const QVariant &compareValue,
                                                            CPropertyIndexRef index) const
        {
            return CValueObjectMetaInfoHelper::equalsPropertyByIndex(cast(object), compareValue, index, 0);
        }
        template <typename T>
        int CValueObjectMetaInfo<T>::toIcon(const void *object) const
        {
            return CValueObjectMetaInfoHelper::toIcon(cast(object), 0);
        }
        template <typename T>
        bool CValueObjectMetaInfo<T>::matches(const void *object, const CVariant &value) const
        {
            return CValueObjectMetaInfoHelper::matches(cast(object), value, 0);
        }
        template <typename T>
        const T &CValueObjectMetaInfo<T>::cast(const void *object)
        {
            return *static_cast<const T *>(object);
        }
        template <typename T>
        T &CValueObjectMetaInfo<T>::cast(void *object)
        {
            return *static_cast<T *>(object);
        }

        //! \private Getter to obtain the IValueObjectMetaInfo which was stored by swift::misc::registerMetaValueType.
        IValueObjectMetaInfo *getValueObjectMetaInfo(int typeId);

        //! \private Getter to obtain the IValueObjectMetaInfo which was stored by swift::misc::registerMetaValueType.
        IValueObjectMetaInfo *getValueObjectMetaInfo(const QVariant &);

        //! \private Getter to obtain the IValueObjectMetaInfo which was stored by swift::misc::registerMetaValueType.
        template <typename T>
        IValueObjectMetaInfo *getValueObjectMetaInfo()
        {
            return getValueObjectMetaInfo(qMetaTypeId<T>());
        }

        //! \cond PRIVATE
        template <typename T, typename = std::enable_if_t<std::is_base_of_v<CSequence<typename T::value_type>, T> &&
                                                          !std::is_same_v<typename T::value_type, CVariant>>>
        constexpr bool canConvertVariantList(int)
        {
            return true;
        }
        template <typename T>
        constexpr bool canConvertVariantList(...)
        {
            return false;
        }

        template <typename T>
        struct MetaTypeHelper
        {
            static constexpr int maybeGetMetaTypeId()
            {
                if constexpr (QMetaTypeId<T>::Defined) { return qMetaTypeId<T>(); }
                else { return QMetaType::UnknownType; }
            }
            static void maybeRegisterMetaType();
            static void maybeRegisterMetaList();
        };

        template <typename T>
        void MetaTypeHelper<T>::maybeRegisterMetaType()
        {
            if constexpr (QMetaTypeId<T>::Defined)
            {
                qRegisterMetaType<T>();
                qDBusRegisterMetaType<T>();
                registerMetaValueType<T>();
                maybeRegisterMetaList();
            }
        };
        //! \endcond
    } // namespace private_ns
} // namespace swift::misc

Q_DECLARE_METATYPE(swift::misc::private_ns::IValueObjectMetaInfo *)

#endif // SWIFT_MISC_VARIANT_PRIVATE_H
