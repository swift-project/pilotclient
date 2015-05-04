/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_PROPERTYINDEXVARIANTMAP_H
#define BLACKMISC_PROPERTYINDEXVARIANTMAP_H

#include "variant.h"
#include "propertyindexlist.h"
#include "blackmiscexport.h"
#include "tuple.h"
#include "inheritance_traits.h"
#include <QVariantMap>
#include <QDBusArgument>

namespace BlackMisc
{

    class CPropertyIndexVariantMap;

    namespace Mixin
    {
        /*!
         * CRTP class template from which a derived class can inherit property indexing functions.
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
            CPropertyIndexList apply(const BlackMisc::CPropertyIndexVariantMap &indexMap, bool skipEqualValues = false);

            //! Set property by index
            void setPropertyByIndex(const CVariant &variant, const CPropertyIndex &index);

            //! Property by index
            CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! Property by index as String
            QString propertyByIndexAsString(const CPropertyIndex &index, bool i18n = false) const;

            //! Is given variant equal to value of property index?
            bool equalsPropertyByIndex(const CVariant &compareValue, const CPropertyIndex &index) const;

        private:
            const Derived *derived() const { return static_cast<const Derived *>(this); }
            Derived *derived() { return static_cast<Derived *>(this); }
        };
    } // Mixin

    /*!
     * Specialized value object compliant map for variants,
     * based on indexes
     */
    class BLACKMISC_EXPORT CPropertyIndexVariantMap :
        public Mixin::MetaType<CPropertyIndexVariantMap>,
        public Mixin::DBusOperators<CPropertyIndexVariantMap>,
        public Mixin::String<CPropertyIndexVariantMap>
    {
    public:
        /*!
         * Constructor
         * \param wildcard when used in search, for setting values irrelevant
         */
        CPropertyIndexVariantMap(bool wildcard = false);

        //! Single value constructor
        CPropertyIndexVariantMap(const CPropertyIndex &index, const CVariant &value);

        //! Destructor
        virtual ~CPropertyIndexVariantMap() {}

        //! Add a value
        void addValue(const CPropertyIndex &index, const CVariant &value);

        //! Add a value
        void addValue(const CPropertyIndex &index, const QVariant &value) { this->addValue(index, CVariant(value)); }

        //! Add QString as literal, disambiguate as I want to add QString
        void addValue(const CPropertyIndex &index, const char *str);

        //! Add a value as non CVariant
        template<class T> void addValue(const CPropertyIndex &index, const T &value) { this->m_values.insert(index, CVariant::fromValue(value)); }

        //! Prepend index to all property indexes
        void prependIndex(int index);

        //! Is empty?
        bool isEmpty() const { return this->m_values.isEmpty(); }

        //! Value
        CVariant value(const CPropertyIndex &index) const { return this->m_values.value(index).toCVariant(); }

        //! Set value
        void value(const CPropertyIndex &index, const CVariant &value) { this->m_values.value(index, value); }

        //! Indexes
        CPropertyIndexList indexes() const;

        //! Contains index?
        bool contains(const CPropertyIndex &index) const { return this->m_values.contains(index); }

        //! values
        QList<CVariant> values() const { return this->m_values.values(); }

        //! Wildcard, only relevant when used in search
        bool isWildcard() const { return this->m_wildcard; }

        //! Wildcard, only relevant when used in search
        void setWildcard(bool wildcard) { this->m_wildcard = wildcard; }

        //! clear
        void clear() { this->m_values.clear(); }

        //! Equal operator, required if maps are directly compared, not with CValueObject
        BLACKMISC_EXPORT friend bool operator ==(const CPropertyIndexVariantMap &a, const CPropertyIndexVariantMap &b);

        //! Equal operator, required if maps are directly compared, not with CValueObject
        BLACKMISC_EXPORT friend bool operator !=(const CPropertyIndexVariantMap &a, const CPropertyIndexVariantMap &b);

        //! Operator == with CVariant
        BLACKMISC_EXPORT friend bool operator ==(const CPropertyIndexVariantMap &valueMap, const CVariant &variant);

        //! Operator != with CVariant
        BLACKMISC_EXPORT friend bool operator !=(const CPropertyIndexVariantMap &valueMap, const CVariant &variant);

        //! Operator == with CVariant
        BLACKMISC_EXPORT friend bool operator ==(const CVariant &variant, const CPropertyIndexVariantMap &valueMap);

        //! Operator != with CVariant
        BLACKMISC_EXPORT friend bool operator !=(const CVariant &variant, const CPropertyIndexVariantMap &valueMap);

        //! Operator == with CValueObject
        //! \todo Still needed?
        template <class T, class = typename std::enable_if<QMetaTypeId<T>::Defined>::type>
        friend bool operator ==(const CPropertyIndexVariantMap &valueMap, const T &valueObject) { return valueMap == CVariant::from(valueObject); }

        //! Operator != with CValueObject
        //! \todo Still needed?
        template <class T, class = typename std::enable_if<QMetaTypeId<T>::Defined>::type>
        friend bool operator !=(const CPropertyIndexVariantMap &valueMap, const T &valueObject) { return valueMap != CVariant::from(valueObject); }

        //! Operator == with CValueObject
        //! \todo Still needed?
        template <class T, class = typename std::enable_if<QMetaTypeId<T>::Defined>::type>
        friend bool operator ==(const T &valueObject, const CPropertyIndexVariantMap &valueMap) { return valueMap == CVariant::from(valueObject); }

        //! Operator != with CValueObject
        //! \todo Still needed?
        template <class T, class = typename std::enable_if<QMetaTypeId<T>::Defined>::type>
        friend bool operator !=(const T &valueObject, const CPropertyIndexVariantMap &valueMap) { return valueMap != CVariant::from(valueObject); }

        //! Map
        const QMap<CPropertyIndex, CVariant> &map() const { return this->m_values; }

        //! Hash value
        uint getValueHash() const;

        //! \copydoc CValueObject::qHash
        friend uint qHash(const CPropertyIndexVariantMap &vm) { return vm.getValueHash(); }

        //! \copydoc CValueObject::toQVariant
        QVariant toQVariant() const { return QVariant::fromValue(*this); }

        //! \copydoc CValueObject::convertFromQVariant
        void convertFromQVariant(const QVariant &variant) { BlackMisc::setFromQVariant(this, variant); }

        //! \copydoc CValueObject::convertToQString
        QString convertToQString(bool i18n = false) const;

    protected:
        QMap<CPropertyIndex, CVariant> m_values; //!< values
        bool m_wildcard; //!< wildcard

    public:
        //! \copydoc CValueObject::marshallToDbus
        void marshallToDbus(QDBusArgument &argument) const;

        //! \copydoc CValueObject::unmarshallFromDbus
        void unmarshallFromDbus(const QDBusArgument &argument);
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
                const CVariant value = it.value().toCVariant();
                const CPropertyIndex index = it.key();
                if (skipEqualValues)
                {
                    bool equal = derived()->equalsPropertyByIndex(value, index);
                    if (equal) { continue; }
                }
                derived()->setPropertyByIndex(value, index);
                changed.push_back(index);
            }
            return changed;
        }
        template <class Derived>
        void Index<Derived>::setPropertyByIndex(const CVariant &variant, const CPropertyIndex &index)
        {
            if (index.isMyself())
            {
                derived()->convertFromCVariant(variant);
                return;
            }

            // not all classes have implemented nesting
            const QString m = QString("Property by index not found (setter), index: ").append(index.toQString());
            qFatal("%s", qPrintable(m));
        }
        template <class Derived>
        CVariant Index<Derived>::propertyByIndex(const CPropertyIndex &index) const
        {
            if (index.isMyself())
            {
                return derived()->toCVariant();
            }
            auto i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexIcon:
                return CVariant::from(derived()->toIcon());
            case IndexPixmap:
                return CVariant::from(derived()->toPixmap());
            case IndexString:
                return CVariant(derived()->toQString());
            default:
                break;
            }

            // not all classes have implemented nesting
            const QString m = QString("Property by index not found, index: ").append(index.toQString());
            qFatal("%s", qPrintable(m));
            return {};
        }
        template <class Derived>
        QString Index<Derived>::propertyByIndexAsString(const CPropertyIndex &index, bool i18n) const
        {
            // default implementation, requires propertyByIndex
            return derived()->propertyByIndex(index).toQString(i18n);
        }
        template <class Derived>
        bool Index<Derived>::equalsPropertyByIndex(const CVariant &compareValue, const CPropertyIndex &index) const
        {
            return derived()->propertyByIndex(index) == compareValue;
        }
    } // Mixin

}

Q_DECLARE_METATYPE(BlackMisc::CPropertyIndexVariantMap)

#endif // guard
