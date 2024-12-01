// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_PROPERTYINDEXVARIANTMAP_H
#define SWIFT_MISC_PROPERTYINDEXVARIANTMAP_H

#include <type_traits>

#include <QDBusArgument>
#include <QList>
#include <QMap>
#include <QMetaType>
#include <QString>
#include <QVariant>
#include <QtGlobal>

#include "misc/mixin/mixindbus.h"
#include "misc/mixin/mixinindex.h"
#include "misc/mixin/mixinmetatype.h"
#include "misc/mixin/mixinstring.h"
#include "misc/propertyindex.h"
#include "misc/propertyindexlist.h"
#include "misc/swiftmiscexport.h"
#include "misc/variant.h"

namespace swift::misc
{
    /*!
     * Specialized value object compliant map for variants,
     * based on indexes
     */
    class SWIFT_MISC_EXPORT CPropertyIndexVariantMap :
        public mixin::MetaType<CPropertyIndexVariantMap>,
        public mixin::DBusOperators<CPropertyIndexVariantMap>,
        public mixin::DataStreamOperators<CPropertyIndexVariantMap>,
        public mixin::String<CPropertyIndexVariantMap>
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
        template <class T>
        void addValue(const CPropertyIndex &index, const T &value)
        {
            m_values.insert(index, CVariant::fromValue(value));
        }

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
        SWIFT_MISC_EXPORT friend bool operator==(const CPropertyIndexVariantMap &a, const CPropertyIndexVariantMap &b);

        //! Equal operator, required if maps are directly compared, not with CValueObject
        SWIFT_MISC_EXPORT friend bool operator!=(const CPropertyIndexVariantMap &a, const CPropertyIndexVariantMap &b);

        //! True if this map matches the value contained in the variant
        bool matchesVariant(const CVariant &value) const;

        //! True if this map matches the value
        template <typename T, typename = std::enable_if_t<!std::is_same_v<T, CVariant>>>
        bool matches(const T &value) const
        {
            return matchesVariant(CVariant::from(value));
        }

        //! Map
        const QMap<CPropertyIndex, CVariant> &map() const { return m_values; }

        //! Hash value
        size_t getValueHash() const;

        //! \copydoc CValueObject::qHash
        friend size_t qHash(const CPropertyIndexVariantMap &vm) { return vm.getValueHash(); }

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

    protected:
        QMap<CPropertyIndex, CVariant> m_values; //!< values
        bool m_wildcard; //!< wildcard

    public:
        //! \copydoc swift::misc::mixin::DBusByMetaClass::marshallToDbus
        void marshallToDbus(QDBusArgument &argument) const;

        //! \copydoc swift::misc::mixin::DBusByMetaClass::unmarshallFromDbus
        void unmarshallFromDbus(const QDBusArgument &argument);

        //! \copydoc swift::misc::mixin::DataStreamByMetaClass::marshalToDataStream
        void marshalToDataStream(QDataStream &stream) const;

        //! \copydoc swift::misc::mixin::DataStreamByMetaClass::unmarshalFromDataStream
        void unmarshalFromDataStream(QDataStream &stream);
    };

    namespace mixin
    {
        template <class Derived>
        CPropertyIndexList Index<Derived>::apply(const swift::misc::CPropertyIndexVariantMap &indexMap,
                                                 bool skipEqualValues)
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
    } // namespace mixin
} // namespace swift::misc

Q_DECLARE_METATYPE(swift::misc::CPropertyIndexVariantMap)

#endif // guard
