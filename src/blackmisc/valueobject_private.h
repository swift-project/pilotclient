/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_VALUEOBJECT_PRIVATE_H
#define BLACKMISC_VALUEOBJECT_PRIVATE_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/inheritance_traits.h"
#include <QString>
#include <QMetaType>
#include <QDBusMetaType>
#include <QDBusArgument>
#include <QJsonObject>

namespace BlackMisc
{
    class CEmpty;
    class CVariant;
    class CPropertyIndex;
    class CIcon;

    template <typename T>
    void registerMetaValueType();

    namespace Private
    {
        //! \private Needed so we can copy forward-declared CVariant.
        void assign(CVariant &, const CVariant &);

        //! \private Needed so we can copy forward-declared CIcon.
        void assign(CIcon &, const CIcon &);

        //! \private Abstract base class representing the set of operations supported by a particular value type.
        struct BLACKMISC_EXPORT IValueObjectMetaInfo
        {
            virtual ~IValueObjectMetaInfo() = default;
            virtual QString toQString(const void *object, bool i18n) const = 0;
            virtual QJsonObject toJson(const void *object) const = 0;
            virtual void convertFromJson(const QJsonObject &json, void *object) const = 0;
            virtual void unmarshall(const QDBusArgument &arg, void *object) const = 0;
            virtual uint getValueHash(const void *object) const = 0;
            virtual int getMetaTypeId() const = 0;
            virtual const void *upCastTo(const void *object, int metaTypeId) const = 0;
            virtual int compareImpl(const void *lhs, const void *rhs) const = 0;
            virtual void setPropertyByIndex(void *object, const CVariant &variant, const CPropertyIndex &index) const = 0;
            virtual void propertyByIndex(const void *object, CVariant &o_variant, const BlackMisc::CPropertyIndex &index) const = 0;
            virtual QString propertyByIndexAsString(const void *object, const CPropertyIndex &index, bool i18n) const = 0;
            virtual bool equalsPropertyByIndex(const void *object, const CVariant &compareValue, const CPropertyIndex &index) const = 0;
            virtual void toIcon(const void *object, CIcon &o_icon) const = 0;
        };

        //! \private Fallback in case qHash is not defined for T.
        template <typename T>
        uint qHash(const T &) { return 0; }

        //! \private Fallback in case compare is not defined for T.
        template <typename T>
        int compare(const T &, const T &) { return 0; }

        //! \private Implementation of IValueObjectMetaInfo representing the set of operations supported by T.
        template <typename T>
        struct CValueObjectMetaInfo : public IValueObjectMetaInfo
        {
            // http://en.wikibooks.org/wiki/More_C++_Idioms/Member_Detector
            struct Fallback { int toJson, convertFromJson, setPropertyByIndex, propertyByIndex, propertyByIndexAsString, equalsPropertyByIndex, toIcon; };
            template <int Fallback:: *> struct int_t { typedef int type; };
            template <typename U> struct Derived : public U, public Fallback {};
#           define DISABLE_IF_HAS(MEMBER) typename int_t<&Derived<U>::MEMBER>::type

            template <typename U> static QJsonObject toJsonHelper(const U &, DISABLE_IF_HAS(toJson)) { return {}; }
            template <typename U> static QJsonObject toJsonHelper(const U &object, ...) { return object.toJson(); }
            template <typename U> static void convertFromJsonHelper(const QJsonObject &, U &, DISABLE_IF_HAS(convertFromJson)) {}
            template <typename U> static void convertFromJsonHelper(const QJsonObject &json, U &object, ...) { object.convertFromJson(json); }
            template <typename U> static void setPropertyByIndexHelper(U &, const CVariant &, const CPropertyIndex &, DISABLE_IF_HAS(setPropertyByIndex)) {}
            template <typename U> static void setPropertyByIndexHelper(U &object, const CVariant &variant, const CPropertyIndex &index, ...) { object.setPropertyByIndex(variant, index); }
            template <typename U> static void propertyByIndexHelper(CVariant &, const U &, const CPropertyIndex &, DISABLE_IF_HAS(propertyByIndex)) {}
            template <typename U> static void propertyByIndexHelper(CVariant &o_variant, const U &object, const CPropertyIndex &index, ...) { assign(o_variant, object.propertyByIndex(index)); }
            template <typename U> static QString propertyByIndexAsStringHelper(const U &, const CPropertyIndex &, bool, DISABLE_IF_HAS(propertyByIndexAsString)) { return {}; }
            template <typename U> static QString propertyByIndexAsStringHelper(const U &object, const CPropertyIndex &index, bool i18n, ...) { return object.propertyByIndexAsString(index, i18n); }
            template <typename U> static bool equalsPropertyByIndexHelper(const U &, const CVariant &, const CPropertyIndex &, DISABLE_IF_HAS(equalsPropertyByIndex)) { return false; }
            template <typename U> static bool equalsPropertyByIndexHelper(const U &object, const CVariant &variant, const CPropertyIndex &index, ...) { return object.equalsPropertyByIndex(variant, index); }
            template <typename U> static void toIconHelper(const U &, CIcon &, DISABLE_IF_HAS(toIcon)) {}
            template <typename U> static void toIconHelper(const U &, CIcon &, typename std::enable_if<std::is_same<U, CVariant>::value, int>::type) {} // CIcon is incomplete when CValueObjectMetaInfo<CVariant> is instantiated
            template <typename U> static void toIconHelper(const U &object, CIcon &o_icon, ...) { assign(o_icon, object.toIcon()); }

#           undef DISABLE_IF_HAS

            virtual QString toQString(const void *object, bool i18n) const override
            {
                return cast(object).toQString(i18n);
            }
            virtual QJsonObject toJson(const void *object) const override
            {
                return toJsonHelper(cast(object), 0);
            }
            virtual void convertFromJson(const QJsonObject &json, void *object) const override
            {
                convertFromJsonHelper(json, cast(object), 0);
            }
            virtual void unmarshall(const QDBusArgument &arg, void *object) const override
            {
                cast(object).unmarshallFromDbus(arg);
            }
            virtual uint getValueHash(const void *object) const override
            {
                return qHash(cast(object));
            }
            virtual int getMetaTypeId() const override
            {
                return maybeGetMetaTypeId(std::integral_constant<bool, QMetaTypeId<T>::Defined>{});
            }
            virtual const void *upCastTo(const void *object, int metaTypeId) const override
            {
                const auto base = static_cast<const void *>(static_cast<const MetaBaseOfT<T> *>(&cast(object)));
                return metaTypeId == getMetaTypeId() ? object : CValueObjectMetaInfo<MetaBaseOfT<T>>{}.upCastTo(base, metaTypeId);
            }
            virtual int compareImpl(const void *lhs, const void *rhs) const override
            {
                return compare(cast(lhs), cast(rhs));
            }
            virtual void setPropertyByIndex(void *object, const CVariant &variant, const CPropertyIndex &index) const override
            {
                setPropertyByIndexHelper(cast(object), variant, index, 0);
            }
            virtual void propertyByIndex(const void *object, CVariant &o_variant, const BlackMisc::CPropertyIndex &index) const override
            {
                propertyByIndexHelper(o_variant, cast(object), index, 0);
            }
            virtual QString propertyByIndexAsString(const void *object, const CPropertyIndex &index, bool i18n) const override
            {
                return propertyByIndexAsStringHelper(cast(object), index, i18n, 0);
            }
            virtual bool equalsPropertyByIndex(const void *object, const CVariant &compareValue, const CPropertyIndex &index) const override
            {
                return equalsPropertyByIndexHelper(cast(object), compareValue, index, 0);
            }
            virtual void toIcon(const void *object, CIcon &o_icon) const override
            {
                toIconHelper(cast(object), o_icon, 0);
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

        //! \private
        template <typename T, bool IsRegisteredMetaType /* = true */>
        struct MetaTypeHelperImpl
        {
            static Q_DECL_CONSTEXPR int maybeGetMetaTypeId() { return qMetaTypeId<T>(); }
            static void maybeRegisterMetaType() { qRegisterMetaType<T>(); qDBusRegisterMetaType<T>(); registerMetaValueType<T>(); }
            static QVariant maybeToQVariant(const T &obj) { return QVariant::fromValue(obj); }
            static void maybeConvertFromQVariant(T &obj, const QVariant &var) { BlackMisc::setFromQVariant(&obj, var); }
        };

        //! \private
        template <typename T>
        struct MetaTypeHelperImpl<T, /* IsRegisteredMetaType = */ false>
        {
            static Q_DECL_CONSTEXPR int maybeGetMetaTypeId() { return QMetaType::UnknownType; }
            static void maybeRegisterMetaType() {}
            static QVariant maybeToQVariant(const T &) { return {}; }
            static void maybeConvertFromQVariant(T &, const QVariant &) {}
        };

        //! \private
        template <typename T>
        using MetaTypeHelper = MetaTypeHelperImpl<T, QMetaTypeId<T>::Defined>;
    }
}

Q_DECLARE_METATYPE(BlackMisc::Private::IValueObjectMetaInfo *)

#endif
