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
#include "valueobject.h"
#include "propertyindex.h"
#include "blackmiscexport.h"

// a) "propertyindex.h" needed for QMap below, despite forward declaration
// b) "propertyindexlist.h" here causes circular dependencies

#include <QVariantMap>
#include <QDBusArgument>

namespace BlackMisc
{
    // forward declaration
    class CPropertyIndex;
    class CPropertyIndexList;
    class CPropertyIndexVariantMap;

    //! \private
    template <> struct CValueObjectPolicy<CPropertyIndexVariantMap> : public CValueObjectLegacy {};

    /*!
     * Specialized value object compliant map for variants,
     * based on indexes
     */
    class BLACKMISC_EXPORT CPropertyIndexVariantMap : public CValueObject<CPropertyIndexVariantMap>
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
        template <class T, class = typename std::enable_if<IsValueObject<T>::value>::type>
        friend bool operator ==(const CPropertyIndexVariantMap &valueMap, const T &valueObject) { return valueMap == CVariant::from(valueObject); }

        //! Operator != with CValueObject
        //! \todo Still needed?
        template <class T, class = typename std::enable_if<IsValueObject<T>::value>::type>
        friend bool operator !=(const CPropertyIndexVariantMap &valueMap, const T &valueObject) { return valueMap != CVariant::from(valueObject); }

        //! Operator == with CValueObject
        //! \todo Still needed?
        template <class T, class = typename std::enable_if<IsValueObject<T>::value>::type>
        friend bool operator ==(const T &valueObject, const CPropertyIndexVariantMap &valueMap) { return valueMap == CVariant::from(valueObject); }

        //! Operator != with CValueObject
        //! \todo Still needed?
        template <class T, class = typename std::enable_if<IsValueObject<T>::value>::type>
        friend bool operator !=(const T &valueObject, const CPropertyIndexVariantMap &valueMap) { return valueMap != CVariant::from(valueObject); }

        //! Map
        const QMap<CPropertyIndex, CVariant> &map() const { return this->m_values; }

        //! Hash value
        uint getValueHash() const;

        //! \copydoc CValueObject::qHash
        friend uint qHash(const CPropertyIndexVariantMap &vm) { return vm.getValueHash(); }

        //! \copydoc CValueObject::toQVariant
        virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

        //! \copydoc CValueObject::convertFromQVariant
        virtual void convertFromQVariant(const QVariant &variant) override { BlackMisc::setFromQVariant(this, variant); }

        //! \copydoc CValueObject::convertToQString
        virtual QString convertToQString(bool i18n = false) const override;

    protected:
        QMap<CPropertyIndex, CVariant> m_values; //!< values
        bool m_wildcard; //!< wildcard

    public:
        //! \copydoc CValueObject::marshallToDbus
        void marshallToDbus(QDBusArgument &argument) const;

        //! \copydoc CValueObject::unmarshallFromDbus
        void unmarshallFromDbus(const QDBusArgument &argument);
    };
}

Q_DECLARE_METATYPE(BlackMisc::CPropertyIndexVariantMap)

#endif // guard
