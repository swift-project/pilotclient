/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_DICTIONARY_H
#define BLACKMISC_DICTIONARY_H

#include "valueobject.h"
#include "iterator.h"
#include "range.h"
#include "containerbase.h"
#include <QHash>
#include <utility>
#include <initializer_list>

namespace BlackMisc
{
    namespace Private
    {
        //! \cond PRIVATE

        namespace ADL
        {
            struct NotFound {};
            struct FromAny { template <class T> FromAny(const T &); };
            NotFound operator <(const FromAny &, const FromAny &);
            NotFound operator ==(const FromAny &, const FromAny &);
            NotFound qHash(...);
            using ::BlackMisc::qHash; // bring hidden name into scope

            template <class Key>
            struct SupportsQHash : std::integral_constant<bool,
                ! std::is_same<decltype(std::declval<Key>() == std::declval<Key>()), NotFound>::value &&
                ! std::is_same<decltype(qHash(std::declval<Key>())), NotFound>::value
            > {};
            template <class Key>
            struct SupportsQMap : std::integral_constant<bool,
                ! std::is_same<decltype(std::declval<Key>() < std::declval<Key>()), NotFound>::value
            > {};
        }

        template <bool KeySupportsQHash /* = true */, bool KeySupportsQMap>
        struct AssociativityTraits
        {
            template <class Key, class Value>
            using DefaultType = QHash<Key, Value>;
        };
        template <>
        struct AssociativityTraits<false, true>
        {
            template <class Key, class Value>
            using DefaultType = QMap<Key, Value>;
        };
        template <>
        struct AssociativityTraits<false, false>
        {
            template <class Key, class>
            struct DefaultType { static_assert(std::is_void<Key>::value, "Key does not support either QHash or QMap"); };
        };

        //! \endcond
    } // namespace Private


    //! Trait to select the appropriate default associative container type depending on what the key type supports
    template <class Key>
    struct AssociativityTraits : public Private::AssociativityTraits<Private::ADL::SupportsQHash<Key>::value, Private::ADL::SupportsQMap<Key>::value>
    {};

    // forward declaration
    template<class Key, class Value, template <class...> class Impl = AssociativityTraits<Key>::template DefaultType>
    class CDictionary;

    //! \private
    template <class Key, class Value, template <class...> class Impl>
    struct CValueObjectPolicy<CDictionary<Key, Value, Impl>> : public CValueObjectLegacy
    {};

    //! Associative container with value semantics, chooses a sensible default implementation container type
    template<class Key, class Value, template <class...> class Impl /*= AssociativityTraits<Key>::template DefaultType*/>
    class CDictionary : public CValueObject<CDictionary<Key, Value, Impl>>
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
        typedef Impl<Key,Value> impl_type;

        //! STL compatibility
        //! @{
        typedef Key key_type;
        typedef Value value_type;
        typedef Value &reference;
        typedef const Value &const_reference;
        typedef typename Impl<Key,Value>::size_type size_type;

        typedef typename Impl<Key,Value>::iterator iterator;
        typedef typename Impl<Key,Value>::const_iterator const_iterator;
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

        /*!
         * \brief Return a copy containing only those elements which key matches a particular pair.
         * \param pairs Pairs of { pointer to member function of Value, return value to compare it against }.
         */
        template <class... Pairs >
        CDictionary findKeyBy(Pairs... pairs) const
        {
            return findKeyBy(BlackMisc::Predicates::MemberEqual(pairs...));
        }

        /*!
         * \brief Return a copy containing only those elements for which a given predicate returns true.
         */
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

        /*!
         * \brief Return a copy containing only those elements which value matches a particular pair.
         * \param pairs Pairs of { pointer to member function of Value, return value to compare it against }.
         */
        template <class... Pairs >
        CDictionary findValueBy(Pairs... pairs) const
        {
            return findValueBy(BlackMisc::Predicates::MemberEqual(pairs...));
        }

        /*!
         * \brief Return true if there is an element for which a given predicate returns true.
         */
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

        //! \copydoc CValueObject::toQVariant()
        virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

        //! \copydoc CValueObject::convertFromQVariant
        virtual void convertFromQVariant(const QVariant &variant) override { BlackMisc::setFromQVariant(this, variant); }

        //! \copydoc CValueObject::toJson
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

        //! \copydoc CValueObject::convertFromJson
        void convertFromJson(const QJsonObject &json)
        {
            QJsonArray array = json.value("associativecontainerbase").toArray();
            for (auto it = array.begin(); it != array.end(); ++it)
            {
                QJsonValueRef jsonKey = (*it);
                ++it;
                QJsonValueRef jsonValue = (*it);
                Key key;
                Value value;
                jsonKey >> key;
                jsonValue >> value;
                m_impl.insert(key, value);
            }
        }

        //! Default constructor.
        CDictionary() {}

        //! Initializer list constructor.
        CDictionary(std::initializer_list<std::pair<Key, Value>> il) : m_impl(il) {}

        //! Copy constructor
        CDictionary(const CDictionary &) = default;

        //! Move constructor
        CDictionary(CDictionary &&other) : m_impl(std::move(other.m_impl)) {}

        //! Virtual destructor
        virtual ~CDictionary() {}

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

        //! Removes all items from the dictionary
        void clear() { m_impl.clear(); }

        //! Returns const iterator at the beginning of the dictionary
        const_iterator constBegin() const { return m_impl.constBegin(); }

        //! Returns const iterator at the end of the dictionary
        const_iterator constEnd() const { return m_impl.constEnd(); }

        /*!
         * \brief Returns an const iterator pointing to the item with the key.
         * \param key
         * \return If key is not found, the function returns constEnd()
         */
        const_iterator constFind (const Key &key) const { return m_impl.constFind(key); }

        /*!
         * \brief Returns an const iterator pointing to the item with the key.
         * \param key
         * \return If key is not found, the function returns end()
         */
        const_iterator find(const Key & key) const { return m_impl.find(key); }

        /*!
         * \brief Returns an iterator pointing to the item with the key.
         * \param key
         * \return If key is not found, the function returns end()
         */
        iterator find(const Key &key) { return m_impl.find(key); }

        //! Returns true if dictionary contains an item with key, otherwise false
        bool contains (const Key &key) const {return m_impl.contains(key); }

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

        //! Returns true if dictionary is empty
        bool isEmpty() const { return m_impl.isEmpty(); }

        //! Return key assigned to value
        const Key key(const Value &value) const { return m_impl.key(value); }

        //! Return key assigned to value or if key is not found defaultKey
        const Key key(const Value &value, const Key & defaultKey) const { return m_impl.key(value, defaultKey); }

        //! Return a range of all keys
        CRange<Iterators::KeyIterator<const_iterator>> keys() const { return makeRange(Iterators::makeKeyIterator(begin()), end()); }

        //! Remove all items with key from the dictionary
        int	remove(const Key &key) { return m_impl.remove(key); }

        //! Returns the number of items in the hash.
        int	size() const { return m_impl.size(); }

        //! Swaps hash other with this hash. This operation is very fast and never fails.
        void swap(CDictionary &other) { m_impl.swap(other); }

        //! Returns the value associated with the key.
        const Value	value(const Key &key) const { return m_impl.value(key); }

        //! Returns the value associated with the key or if key is not found defaultValue
        const Value	value(const Key &key, const Value &defaultValue) const { return m_impl.value(key, defaultValue); }

        //! Return a range of all values
        CRange<const_iterator> values() const { return makeRange(begin(), end()); }

        //! Copy assignment.
        CDictionary &operator =(const CDictionary &other) { m_impl = other.m_impl; return *this; }

        //! Move assignment
        CDictionary &operator =(CDictionary && other) { m_impl = std::move(other.m_impl); return *this; }

        /*!
         * \brief Access an element by its key.
         * \note
         * If dictionary does not contain any item with key, a default constructed
         * value will be inserted
         */
        Value &operator [](const Key &key) { return m_impl[key]; }

        /*!
         * \brief Access an element by its key.
         * \note
         * If dictionary does not contain any item with key, a default constructed
         * value will be inserted
         */
        const Value	operator [](const Key &key) const { return m_impl[key]; }

        //! Test for equality.
        bool operator ==(const CDictionary &other) const { return m_impl == other.m_impl; }

        //! Test for inequality.
        bool operator !=(const CDictionary &other) const { return !(*this == other); }

    protected:
        //! \copydoc BlackMisc::CValueObject::convertToQString
        //! \todo Fix brackets
        virtual QString convertToQString(bool i18n = false) const override
        {
            QString str = "{";
            for (auto it = m_impl.cbegin(); it != m_impl.end(); ++it)
            {
                str += "{";
                str += CContainerHelper::stringify(it.key(), i18n) + "," + CContainerHelper::stringify(it.value(), i18n);
                str += "}";
            }
            if (str.isEmpty()) { str = "{"; }
            return str += "}";
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

    private:

        Impl<Key,Value> m_impl;
    };

} // namespace BlackMisc

#endif // BLACKMISC_DICTIONARY_H

