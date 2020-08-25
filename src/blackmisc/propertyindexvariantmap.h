/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_PROPERTYINDEXVARIANTMAP_H
#define BLACKMISC_PROPERTYINDEXVARIANTMAP_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/compare.h"
#include "blackmisc/dbus.h"
#include "blackmisc/inheritancetraits.h"
#include "blackmisc/predicates.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/propertyindexlist.h"
#include "blackmisc/stringutils.h"
#include "blackmisc/variant.h"

#include <QDBusArgument>
#include <QList>
#include <QMap>
#include <QMetaType>
#include <QString>
#include <QVariant>
#include <QtDebug>
#include <QtGlobal>
#include <type_traits>

namespace BlackMisc
{
    class CPropertyIndexVariantMap;

    namespace Mixin
    {
        /*!
         * CRTP class template from which a derived class can inherit property indexing functions.
         *
         * This is only a placeholder for future support of implementing property indexing through the tuple system.
         * At the moment, it just implements the default properties: String, Icon, and Pixmap.
         */
        template <class Derived>
        class Index
        {
        public:
            //! Base class enums
            enum ColumnIndex
            {
                IndexPixmap = 10, // manually set to avoid circular dependencies
                IndexIcon,
                IndexString
            };

            //! Update by variant map
            //! \return number of values changed, with skipEqualValues equal values will not be changed
            CPropertyIndexList apply(const CPropertyIndexVariantMap &indexMap, bool skipEqualValues = false);

            //! Set property by index
            void setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant);

            //! Property by index
            CVariant propertyByIndex(const CPropertyIndex &index) const;

            //! Property by index as String
            QString propertyByIndexAsString(const CPropertyIndex &index, bool i18n = false) const;

            //! Compare for index
            int comparePropertyByIndex(const CPropertyIndex &index, const Derived &compareValue) const;

            //! Is given variant equal to value of property index?
            bool equalsPropertyByIndex(const CVariant &compareValue, const CPropertyIndex &index) const;

        private:
            const Derived *derived() const { return static_cast<const Derived *>(this); }
            Derived *derived() { return static_cast<Derived *>(this); }

            template <typename T, std::enable_if_t<std::is_default_constructible<T>::value, int> = 0>
            CVariant myself() const { return CVariant::from(*derived()); }
            template <typename T, std::enable_if_t<std::is_default_constructible<T>::value, int> = 0>
            void myself(const CVariant &variant) { *derived() = variant.to<T>(); }

            template <typename T, std::enable_if_t<! std::is_default_constructible<T>::value, int> = 0>
            CVariant myself() const { qFatal("isMyself should have been handled before reaching here"); return {}; }
            template <typename T, std::enable_if_t<! std::is_default_constructible<T>::value, int> = 0>
            void myself(const CVariant &) { qFatal("isMyself should have been handled before reaching here"); }

            template <typename T>
            CVariant basePropertyByIndex(const T *base, const CPropertyIndex &index) const { return base->propertyByIndex(index); }
            template <typename T>
            void baseSetPropertyByIndex(T *base, const CVariant &var, const CPropertyIndex &index) { base->setPropertyByIndex(index, var); }

            CVariant basePropertyByIndex(const void *, const CPropertyIndex &index) const
            {
                qFatal("%s", qPrintable("Property by index not found, index: " + index.toQString())); return {};
            }

            void baseSetPropertyByIndex(void *, const CVariant &, const CPropertyIndex &index)
            {
                qFatal("%s", qPrintable("Property by index not found (setter), index: " + index.toQString()));
            }
        };

        /*!
         * When a derived class and a base class both inherit from Mixin::Index,
         * the derived class uses this macro to disambiguate the inherited members.
         */
        // *INDENT-OFF*
#       define BLACKMISC_DECLARE_USING_MIXIN_INDEX(DERIVED)                     \
            using ::BlackMisc::Mixin::Index<DERIVED>::apply;                    \
            using ::BlackMisc::Mixin::Index<DERIVED>::setPropertyByIndex;       \
            using ::BlackMisc::Mixin::Index<DERIVED>::propertyByIndex;          \
            using ::BlackMisc::Mixin::Index<DERIVED>::propertyByIndexAsString;  \
            using ::BlackMisc::Mixin::Index<DERIVED>::comparePropertyByIndex;   \
            using ::BlackMisc::Mixin::Index<DERIVED>::equalsPropertyByIndex;
        // *INDENT-ON*

    } // Mixin

    /*!
     * Specialized value object compliant map for variants,
     * based on indexes
     */
    class BLACKMISC_EXPORT CPropertyIndexVariantMap :
        public Mixin::MetaType<CPropertyIndexVariantMap>,
        public Mixin::DBusOperators<CPropertyIndexVariantMap>,
        public Mixin::DataStreamOperators<CPropertyIndexVariantMap>,
        public Mixin::String<CPropertyIndexVariantMap>
    {
    public:
        //! Constructor
        //! \param wildcard when used in search, for setting values irrelevant
        CPropertyIndexVariantMap(bool wildcard = false);

        //! Single value constructor
        CPropertyIndexVariantMap(const CPropertyIndex &index, const CVariant &value);

        //! Add a value
        void addValue(const CPropertyIndex &index, const CVariant &value);

        //! Add a value
        void addValue(const CPropertyIndex &index, const QVariant &value) { this->addValue(index, CVariant(value)); }

        //! Add QString as literal, disambiguate as I want to add QString
        void addValue(const CPropertyIndex &index, const char *str);

        //! Add a value as non CVariant
        template<class T> void addValue(const CPropertyIndex &index, const T &value) { m_values.insert(index, CVariant::fromValue(value)); }

        //! Prepend index to all property indexes
        void prependIndex(int index);

        //! Is empty?
        bool isEmpty() const { return m_values.isEmpty(); }

        //! Value
        CVariant value(const CPropertyIndex &index) const { return m_values.value(index); }

        //! Set value
        void value(const CPropertyIndex &index, const CVariant &value) { m_values.value(index, value); }

        //! Indexes
        CPropertyIndexList indexes() const;

        //! Contains index?
        bool contains(const CPropertyIndex &index) const { return m_values.contains(index); }

        //! values
        QList<CVariant> values() const { return m_values.values(); }

        //! Wildcard, only relevant when used in search
        bool isWildcard() const { return m_wildcard; }

        //! Wildcard, only relevant when used in search
        void setWildcard(bool wildcard) { m_wildcard = wildcard; }

        //! clear
        void clear() { m_values.clear(); }

        //! Number of elements
        int size() const;

        //! Equal operator, required if maps are directly compared, not with CValueObject
        BLACKMISC_EXPORT friend bool operator ==(const CPropertyIndexVariantMap &a, const CPropertyIndexVariantMap &b);

        //! Equal operator, required if maps are directly compared, not with CValueObject
        BLACKMISC_EXPORT friend bool operator !=(const CPropertyIndexVariantMap &a, const CPropertyIndexVariantMap &b);

        //! True if this map matches the value contained in the variant
        bool matchesVariant(const CVariant &value) const;

        //! True if this map matches the value
        template <typename T, typename = std::enable_if_t<!std::is_same<T, CVariant>::value>>
        bool matches(const T &value) const { return matchesVariant(CVariant::from(value)); }

        //! Map
        const QMap<CPropertyIndex, CVariant> &map() const { return m_values; }

        //! Hash value
        uint getValueHash() const;

        //! \copydoc CValueObject::qHash
        friend uint qHash(const CPropertyIndexVariantMap &vm) { return vm.getValueHash(); }

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

    protected:
        QMap<CPropertyIndex, CVariant> m_values; //!< values
        bool m_wildcard; //!< wildcard

    public:
        //! \copydoc BlackMisc::Mixin::DBusByMetaClass::marshallToDbus
        void marshallToDbus(QDBusArgument &argument) const;

        //! \copydoc BlackMisc::Mixin::DBusByMetaClass::unmarshallFromDbus
        void unmarshallFromDbus(const QDBusArgument &argument);

        //! \copydoc BlackMisc::Mixin::DataStreamByMetaClass::marshalToDataStream
        void marshalToDataStream(QDataStream &stream) const;

        //! \copydoc BlackMisc::Mixin::DataStreamByMetaClass::unmarshalFromDataStream
        void unmarshalFromDataStream(QDataStream &stream);
    };

    namespace Mixin
    {
        template <class Derived>
        CPropertyIndexList Index<Derived>::apply(const BlackMisc::CPropertyIndexVariantMap &indexMap, bool skipEqualValues)
        {
            if (indexMap.isEmpty()) return {};

            CPropertyIndexList changed;
            const auto &map = indexMap.map();
            for (auto it = map.begin(); it != map.end(); ++it)
            {
                const CVariant value = it.value();
                const CPropertyIndex index = it.key();
                if (skipEqualValues)
                {
                    const bool equal = derived()->equalsPropertyByIndex(value, index);
                    if (equal) { continue; }
                }
                derived()->setPropertyByIndex(index, value);
                changed.push_back(index);
            }
            return changed;
        }

        template <class Derived>
        void Index<Derived>::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself())
            {
                myself<Derived>(variant);
            }
            else
            {
                baseSetPropertyByIndex(static_cast<TIndexBaseOfT<Derived> *>(derived()), variant, index);
            }
        }

        template <class Derived>
        CVariant Index<Derived>::propertyByIndex(const CPropertyIndex &index) const
        {
            if (index.isMyself()) { return myself<Derived>(); }
            const auto i = index.frontCasted<ColumnIndex>(); // keep that "auto", otherwise I won's compile
            switch (i)
            {
            case IndexIcon: return CVariant::from(derived()->toIcon());
            case IndexPixmap: return CVariant::from(derived()->toPixmap());
            case IndexString: return CVariant(derived()->toQString());
            default: return basePropertyByIndex(static_cast<const TIndexBaseOfT<Derived> *>(derived()), index);
            }
        }

        template <class Derived>
        QString Index<Derived>::propertyByIndexAsString(const CPropertyIndex &index, bool i18n) const
        {
            return derived()->propertyByIndex(index).toQString(i18n);
        }

        template <class Derived>
        bool Index<Derived>::equalsPropertyByIndex(const CVariant &compareValue, const CPropertyIndex &index) const
        {
            return derived()->propertyByIndex(index) == compareValue;
        }

        template<class Derived>
        int Index<Derived>::comparePropertyByIndex(const CPropertyIndex &index, const Derived &compareValue) const
        {
            if (this == &compareValue) { return 0; }
            if (index.isMyself()) {
                // slow, only last resort
                return derived()->toQString().compare(compareValue.toQString());
            }

            const auto i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexIcon:
            case IndexPixmap:
            case IndexString:
            default:
                break; // also covers
            }

            // slow, only last resort
            return derived()->toQString().compare(compareValue.toQString());
        }
    } // Mixin
} // ns

Q_DECLARE_METATYPE(BlackMisc::CPropertyIndexVariantMap)

#endif // guard
