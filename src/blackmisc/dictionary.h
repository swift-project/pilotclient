/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_DICTIONARY_H
#define BLACKMISC_DICTIONARY_H

#include "blackmisc/containerbase.h"
#include "blackmisc/mixin/mixindbus.h"
#include "blackmisc/mixin/mixindatastream.h"
#include "blackmisc/datastream.h"
#include "blackmisc/inheritancetraits.h"
#include "blackmisc/iterator.h"
#include "blackmisc/mixin/mixinjson.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/predicates.h"
#include "blackmisc/range.h"
#include "blackmisc/mixin/mixinstring.h"
#include "blackmisc/typetraits.h"
#include "blackmisc/mixin/mixinhash.h"
#include "blackmisc/blackmiscexport.h"

#include <QDBusArgument>
#include <QHash>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonValueRef>
#include <QList>
#include <QMap>
#include <QString>
#include <QtDebug>
#include <QtGlobal>
#include <algorithm>
#include <initializer_list>
#include <type_traits>
#include <utility>

namespace BlackMisc
{
    namespace Private
    {
        //! \cond PRIVATE
        template <bool KeySupportsQHash /* = true */, bool KeySupportsQMap>
        struct TAssociativityTraits
        {
            template <class Key, class Value>
            using DefaultType = QHash<Key, Value>;
        };
        template <>
        struct TAssociativityTraits<false, true>
        {
            template <class Key, class Value>
            using DefaultType = QMap<Key, Value>;
        };
        template <>
        struct TAssociativityTraits<false, false>
        {
            template <class Key, class>
            struct DefaultType { static_assert(std::is_void_v<Key>, "Key does not support either QHash or QMap"); };
        };
        //! \endcond
    }

    /*!
     * Trait to select the appropriate default associative container type depending on what the key type supports.
     */
    template <typename K, typename V>
    using TDefaultAssociativeType = typename Private::TAssociativityTraits<TModelsQHashKey<K>::value, TModelsQMapKey<K>::value>::template DefaultType<K, V>;

    /*!
     * Associative container with value semantics, chooses a sensible default implementation container type.
     */
    // *INDENT-OFF*
    template<class Key, class Value, template <class...> class Impl = TDefaultAssociativeType>
    class CDictionary :
        public Mixin::DBusOperators<CDictionary<Key, Value, Impl>>,
        public Mixin::DataStreamOperators<CDictionary<Key, Value, Impl>>,
        public Mixin::JsonOperators<CDictionary<Key, Value, Impl>>,
        public Mixin::String<CDictionary<Key, Value, Impl>>
    // *INDENT-ON*
    {
        //! \copydoc BlackMisc::CValueObject::compare
        friend int compare(const CDictionary &a, const CDictionary &b)
        {
            if (a.m_impl.size() < b.m_impl.size()) { return -1; }
            if (a.m_impl.size() > b.m_impl.size()) { return 1; }
            return 0;
        }

    public:
        //! The implementation container
        using impl_type = Impl<Key, Value>;

        //! STL compatibility
        //! @{
        typedef Key key_type;
        typedef Value value_type;
        typedef Value &reference;
        typedef const Value &const_reference;
        typedef typename Impl<Key, Value>::size_type size_type;

        typedef typename Impl<Key, Value>::iterator iterator;
        typedef typename Impl<Key, Value>::const_iterator const_iterator;
        //! @}

        //! Return a copy containing only those elements for which the dictionary keys return true for a given predicate.
        template <class Predicate>
        CDictionary findKeyBy(Predicate p) const
        {
            CDictionary result = *this;
            for (auto it = result.begin(); it != result.end();)
            {
                if (!p(it.key())) { it = result.erase(it); }
                else { ++it; }
            }
            return result;
        }

        //! Return a copy containing only those elements which key matches a particular pair.
        //! \param pairs Pairs of { pointer to member function of Value, return value to compare it against }.
        template <class... Pairs >
        CDictionary findKeyBy(Pairs... pairs) const
        {
            return findKeyBy(BlackMisc::Predicates::MemberEqual(pairs...));
        }

        //! Return a copy containing only those elements for which a given predicate returns true.
        template <class Predicate>
        CDictionary findValueBy(Predicate p) const
        {
            CDictionary result = *this;
            for (auto it = result.begin(); it != result.end();)
            {
                if (!p(it.value())) { it = result.erase(it); }
                else { ++it; }
            }
            return result;
        }

        //! Return a copy containing only those elements which value matches a particular pair.
        //! \param pairs Pairs of { pointer to member function of Value, return value to compare it against }.
        template <class... Pairs >
        CDictionary findValueBy(Pairs... pairs) const
        {
            return findValueBy(BlackMisc::Predicates::MemberEqual(pairs...));
        }

        //! Return true if there is an element for which a given predicate returns true.
        template <class Predicate>
        bool containsByKey(Predicate p) const
        {
            auto keys = m_impl.keys();
            return std::any_of(keys.cbegin(), keys.cend(), p);
        }

        //! Return true if there is an element which key matches a given pair.
        template <class MembFunc, class ReturnValue>
        bool containsByKey(MembFunc membFunc, ReturnValue returnValue) const
        {
            return containsByKey(BlackMisc::Predicates::MemberEqual(membFunc, returnValue));
        }

        //! Return true if there is an element for which a given predicate returns true.
        template <class Predicate>
        bool containsByValue(Predicate p) const
        {
            return std::any_of(m_impl.cbegin(), m_impl.cend(), p);
        }

        //! Return true if there is an element which value matches a given pair.
        template <class MembFunc, class ReturnValue>
        bool containsByValue(MembFunc membFunc, ReturnValue returnValue) const
        {
            return containsByValue(BlackMisc::Predicates::MemberEqual(membFunc, returnValue));
        }

        //! Remove elements for which a given predicate for value returns true.
        template <class Predicate>
        void removeByKeyIf(Predicate p)
        {
            for (auto it = m_impl.begin(); it != m_impl.end();)
            {
                if (p(it.key())) { it = m_impl.erase(it); }
                else { ++it; }
            }
        }

        //! Remove elements for which a given predicate for key returns true.
        template <class Predicate>
        void removeByValueIf(Predicate p)
        {
            for (auto it = m_impl.begin(); it != m_impl.end();)
            {
                if (p(it.value())) { it = m_impl.erase(it); }
                else { ++it; }
            }
        }

        //! Remove elements for which key matches a particular pair.
        template <class MembFunc, class ReturnValue>
        void removeByKeyIf(MembFunc membFunc, ReturnValue returnValue)
        {
            removeByKeyIf(BlackMisc::Predicates::MemberEqual(membFunc, returnValue));
        }

        //! Remove elements for which value matches a particular pair.
        template <class MembFunc, class ReturnValue>
        void removeByValueIf(MembFunc membFunc, ReturnValue returnValue)
        {
            removeByValueIf(BlackMisc::Predicates::MemberEqual(membFunc, returnValue));
        }

        //! Remove elements for which the same key/value pair is present in an other dictionary.
        void removeDuplicates(const CDictionary &other)
        {
            for (auto it = begin(); it != end();)
            {
                auto it2 = other.find(it.key());
                if (it2 != other.end() && it.value() == it2.value()) { it = erase(it); }
                else { ++it; }
            }
        }

        //! \copydoc BlackMisc::Mixin::JsonByMetaClass::toJson
        QJsonObject toJson() const
        {
            QJsonArray array;
            QJsonObject json;

            for (auto it = m_impl.cbegin(); it != m_impl.cend(); ++it)
            {
                array << it.key() << it.value();
            }
            json.insert("associativecontainerbase", array);
            return json;
        }

        //! \copydoc BlackMisc::Mixin::JsonByMetaClass::convertFromJson
        void convertFromJson(const QJsonObject &json)
        {
            QJsonValue value = json.value("associativecontainerbase");
            if (value.isUndefined()) { throw CJsonException("Missing 'associativecontainerbase'"); }
            QJsonArray array = value.toArray();
            int index = 0;
            for (auto it = array.begin(); it != array.end(); ++it)
            {
                QJsonValueRef jsonKey = (*it);
                ++it;
                if (it == array.end()) { qWarning("Odd number of elements in CDictionary::convertFromJson"); return; }
                QJsonValueRef jsonValue = (*it);
                Key key;
                Value val;
                {
                    CJsonScope scope("associativecontainerbase", 2 * index);
                    Q_UNUSED(scope);
                    jsonKey >> key;
                }
                {
                    CJsonScope scope("associativecontainerbase", 2 * index++ + 1);
                    Q_UNUSED(scope);
                    jsonValue >> val;
                }
                m_impl.insert(std::move(key), std::move(val));
            }
        }

        //! Default constructor.
        CDictionary() {}

        //! Initializer list constructor.
        CDictionary(std::initializer_list<std::pair<Key, Value>> il) : m_impl(il) {}

        //! Copy constructor
        CDictionary(const CDictionary &) = default;

        //! Move constructor
        CDictionary(CDictionary &&other) noexcept : m_impl(std::move(other.m_impl)) {}

        //! Destructor
        ~CDictionary() = default;

        //! Returns iterator at the beginning of the dictionary
        iterator begin() { return m_impl.begin(); }

        //! Returns const iterator at the beginning of the dictionary
        const_iterator begin() const { return m_impl.begin(); }

        //! Returns const iterator at the beginning of the dictionary
        const_iterator cbegin() const { return m_impl.cbegin(); }

        //! Returns iterator at the end of the dictionary
        iterator end() { return m_impl.end(); }

        //! Returns const iterator at the end of the dictionary
        const_iterator end() const { return m_impl.end(); }

        //! Returns const iterator at the end of the dictionary
        const_iterator cend() const { return m_impl.cend(); }

        //! Returns const iterator for iterating over keys
        //! @{
        auto keyBegin() const { return m_impl.keyBegin(); }
        auto keyEnd() const { return m_impl.keyEnd(); }
        //! @}

        //! Returns iterator for iterating over keys and values together
        //! @{
        auto keyValueBegin() { return m_impl.keyValueBegin(); }
        auto keyValueEnd() { return m_impl.keyValueEnd(); }
        //! @}

        //! Returns const iterator for iterating over keys and values together
        //! @{
        auto keyValueBegin() const { return m_impl.keyValueBegin(); }
        auto constKeyValueBegin() const { return m_impl.constKeyValueBegin(); }
        auto keyValueEnd() const { return m_impl.keyValueEnd(); }
        auto constKeyValueEnd() const { return m_impl.constKeyValueEnd(); }
        //! @}

        //! Removes all items from the dictionary
        void clear() { m_impl.clear(); }

        //! Returns const iterator at the beginning of the dictionary
        const_iterator constBegin() const { return m_impl.constBegin(); }

        //! Returns const iterator at the end of the dictionary
        const_iterator constEnd() const { return m_impl.constEnd(); }

        //! Returns an const iterator pointing to the item with the key.
        //! \return If key is not found, the function returns constEnd()
        const_iterator constFind(const Key &key) const { return m_impl.constFind(key); }

        //! Returns an const iterator pointing to the item with the key.
        //! \return If key is not found, the function returns end()
        const_iterator find(const Key &key) const { return m_impl.find(key); }

        //! Returns an iterator pointing to the item with the key.
        //! \return If key is not found, the function returns end()
        iterator find(const Key &key) { return m_impl.find(key); }

        //! Returns true if dictionary contains an item with key, otherwise false
        bool contains(const Key &key) const {return m_impl.contains(key); }

        //! Returns the number of items with key
        int count(const Key &key) const { return m_impl.count(key); }

        //! Returns the size of the dictionary
        int count() const { return m_impl.count(); }

        //! Returns true if the
        bool empty() const { return m_impl.empty(); }

        //! Removes the key/value pair iterator is currently pointing to and returns an iterator to the next item.
        iterator erase(iterator pos) { return m_impl.erase(pos); }

        //! Insert new item with key and value
        iterator insert(const Key &key, const Value &value) { return m_impl.insert(key, value); }

        //! Insert all items of other dictionary into this dictionary
        void insert(const CDictionary &other) { for (auto i = other.cbegin(); i != other.cend(); ++i) { insert(i.key(), i.value()); } }

        //! Returns true if dictionary is empty
        bool isEmpty() const { return m_impl.isEmpty(); }

        //! Return key assigned to value
        const Key key(const Value &value) const { return m_impl.key(value); }

        //! Return key assigned to value or if key is not found defaultKey
        const Key key(const Value &value, const Key &defaultKey) const { return m_impl.key(value, defaultKey); }

        //! Return a range of all keys (does not allocate a temporary container)
        auto keys() const { return makeRange(keyBegin(), keyEnd()); }

        //! Remove all items with key from the dictionary
        int remove(const Key &key) { return m_impl.remove(key); }

        //! Returns the number of items in the hash.
        int size() const { return m_impl.size(); }

        //! Swaps hash other with this hash. This operation is very fast and never fails.
        void swap(CDictionary &other) noexcept { m_impl.swap(other.m_impl); }

        //! Returns the value associated with the key.
        const Value value(const Key &key) const { return m_impl.value(key); }

        //! Returns the value associated with the key or if key is not found defaultValue
        const Value value(const Key &key, const Value &defaultValue) const { return m_impl.value(key, defaultValue); }

        //! Return a range of all values (does not allocate a temporary container)
        CRange<const_iterator> values() const { return makeRange(begin(), end()); }

        //! Copy assignment.
        CDictionary &operator =(const CDictionary &other) { m_impl = other.m_impl; return *this; }

        //! Move assignment
        CDictionary &operator =(CDictionary &&other) noexcept { m_impl = std::move(other.m_impl); return *this; }

        //! Return reference to the internal implementation object.
        friend impl_type &implementationOf(CDictionary &dict) { return dict.m_impl; }

        //! Return reference to the internal implementation object.
        friend const impl_type &implementationOf(const CDictionary &dict) { return dict.m_impl; }

        //! Access an element by its key.
        //! \note If dictionary does not contain any item with key, a default constructed value will be inserted.
        Value &operator [](const Key &key) { return m_impl[key]; }

        //! Access an element by its key.
        const Value operator [](const Key &key) const { return m_impl[key]; }

        //! Test for equality.
        friend bool operator ==(const CDictionary &a, const CDictionary &b) { return a.m_impl == b.m_impl; }

        //! Test for inequality.
        friend bool operator !=(const CDictionary &a, const CDictionary &b) { return !(a == b); }

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const
        {
            QString str;
            for (auto it = m_impl.cbegin(); it != m_impl.end(); ++it)
            {
                str += "{";
                str += CContainerHelper::stringify(it.key(), i18n) + "," + CContainerHelper::stringify(it.value(), i18n);
                str += "}";
            }
            return "{" + str + "}";
        }

    public:
        //! \copydoc BlackMisc::CValueObject::marshallToDbus
        void marshallToDbus(QDBusArgument &argument) const
        {
            argument << m_impl;
        }

        //! \copydoc BlackMisc::CValueObject::unmarshallFromDbus
        void unmarshallFromDbus(const QDBusArgument &argument)
        {
            argument >> m_impl;
        }

        //! \copydoc BlackMisc::Mixin::DataStreamByMetaClass::marshalToDataStream
        void marshalToDataStream(QDataStream &stream) const { stream << m_impl; }

        //! \copydoc BlackMisc::Mixin::DataStreamByMetaClass::unmarshalFromDataStream
        void unmarshalFromDataStream(QDataStream &stream) { stream >> m_impl; }

    private:
        Impl<Key, Value> m_impl;
    };

    /*!
     * Identity function for API consistency with CDictionary::implementationOf.
     */
    template <class Key, class Value>
    QMap<Key, Value> &implementationOf(QMap<Key, Value> &dict) { return dict; }

    /*!
     * Identity function for API consistency with CDictionary::implementationOf.
     */
    template <class Key, class Value>
    const QMap<Key, Value> &implementationOf(const QMap<Key, Value> &dict) { return dict; }

    /*!
     * Call a functor for each {key,value1,value2} triple in the keywise intersection of two maps.
     */
    template <class Map1, class Map2, class F>
    void forEachIntersection(const Map1 &map1, const Map2 &map2, F functor)
    {
        static_assert(std::is_same_v<typename Map1::key_type, typename Map2::key_type>, "Maps must have the same key type");
        if (map1.empty() || map2.empty()) { return; }
        auto it1 = implementationOf(map1).lowerBound(map2.cbegin().key());
        auto end1 = implementationOf(map1).upperBound((map2.cend() - 1).key());
        auto it2 = implementationOf(map2).lowerBound(map1.cbegin().key());
        auto end2 = implementationOf(map2).upperBound((map1.cend() - 1).key());
        while (it1 != end1 && it2 != end2)
        {
            if (it1.key() < it2.key()) { ++it1; }
            else if (it2.key() < it1.key()) { ++it2; }
            else { functor(it1.key(), it1.value(), it2); ++it1; ++it2; }
        }
    }
} // namespace BlackMisc

#endif // BLACKMISC_DICTIONARY_H

