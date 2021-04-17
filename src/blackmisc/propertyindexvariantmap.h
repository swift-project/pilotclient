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
#include "blackmisc/mixin/mixincompare.h"
#include "blackmisc/mixin/mixindbus.h"
#include "blackmisc/mixin/mixinindex.h"
#include "blackmisc/inheritancetraits.h"
#include "blackmisc/predicates.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/propertyindexlist.h"
#include "blackmisc/mixin/mixinstring.h"
#include "blackmisc/mixin/mixinmetatype.h"
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
        template <typename T, typename = std::enable_if_t<!std::is_same_v<T, CVariant>>>
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
    }
} // ns

Q_DECLARE_METATYPE(BlackMisc::CPropertyIndexVariantMap)

#endif // guard
