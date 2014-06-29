/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_DICTIONARY_H
#define BLACKMISC_DICTIONARY_H

#include "valueobject.h"
#include "sequence.h"
#include "collection.h"
#include <QHash>

namespace BlackMisc
{
    //! Associative container with value semantics, chooses a sensible default implementation container type
    template<class Key, class Value, template <class...> class Impl>
    class CDictionary : public CValueObject
    {

    public:

        //! \brief STL compatibility
        //! @{
        typedef Key key_type;
        typedef Value value_type;
        typedef Value &reference;
        typedef const Value &const_reference;
        typedef typename Impl<Key,Value>::size_type size_type;

        typedef typename Impl<Key,Value>::iterator iterator;
        typedef typename Impl<Key,Value>::const_iterator const_iterator;
        //! @}


        //! Return keys as collection
        CCollection<Key> keysCollection() const
        {
            CCollection<Key> collection;
            for (auto it = m_impl.begin(); it != m_impl.end(); ++it)
            {
                collection.push_back(it.key());
            }
            return collection;
        }

        //! Return values as collection
        CCollection<Value> valuesCollection() const
        {
            CCollection<Value> collection;
            for (auto it = m_impl.begin(); it != m_impl.end(); ++it)
            {
                collection.push_back(it.value());
            }
            return collection;
        }

        //! Return keys as sequence
        CSequence<Key> keysSequence() const
        {
            CSequence<Key> sequence;
            for (auto it = m_impl.begin(); it != m_impl.end(); ++it)
            {
                sequence.push_back(it.key());
            }
            return sequence;
        }

        //! Return values as sequence
        CSequence<Value> valuesSequence() const
        {
            CSequence<Value> sequence;
            for (auto it = m_impl.begin(); it != m_impl.end(); ++it)
            {
                sequence.push_back(it.value());
            }
            return sequence;
        }


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
            return findKeyBy(BlackMisc::Predicates::MemberEqual<Key>(pairs...));
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
            return findValueBy(BlackMisc::Predicates::MemberEqual<Value>(pairs...));
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
            return containsByKey(BlackMisc::Predicates::MemberEqual<Key>(membFunc, returnValue));
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
            return containsByValue(BlackMisc::Predicates::MemberEqual<Value>(membFunc, returnValue));
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
            removeByKeyIf(BlackMisc::Predicates::MemberEqual<Key>(membFunc, returnValue));
        }

        //! Remove elements for which value matches a particular pair.
        template <class MembFunc, class ReturnValue>
        void removeByValueIf(MembFunc membFunc, ReturnValue returnValue)
        {
            removeByValueIf(BlackMisc::Predicates::MemberEqual<Value>(membFunc, returnValue));
        }

        //! \copydoc BlackMisc::CValueObject::toQVariant
        virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

        //! \copydoc BlackMisc::CValueObject::getValueHash
        virtual uint getValueHash() const override { return qHash(this); }

        //! \copydoc CValueObject::toJson
        virtual QJsonObject toJson() const override
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

        //! \copydoc CValueObject::fromJson
        void fromJson(const QJsonObject &json) override
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

        //! Copy constructor
        CDictionary(const CDictionary &) = default;

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
        bool contains (const Key &key) const {return m_impl.contains(); }

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

        //! Return a collection of all keys
        CCollection<Key>	keys() const { return this->keysCollection(); }

        //! Remove all items with key from the dictionary
        int	remove(const Key &key) { return m_impl.remove(key); }

        //! Returns the number of items in the hash.
        int	size() const { return m_impl.size(); }

        //! Swaps hash other with this hash. This operation is very fast and never fails.
        void swap(CDictionary &other) { m_impl.swap(other); }

        //! Returns the value associated with the key.
        const Value	value(const Key &key) const { return m_impl.value(key); }

        //! Returns the value associated with the key or if key is not found defaultValue
        const Value	value(const Key &key, const Value &defaultValue) const { return m_impl.value(key); }

        //! Return a collection of all values
        CCollection<Value> values() const { return this->valuesCollection(); }

        //! Copy assignment.
        CDictionary &operator =(const CDictionary &other) { m_impl = other.m_impl; return *this; }

        //! Move assignment
        CDictionary &operator =(CDictionary && other) { m_impl = other.m_impl; return *this; }

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

        //! \copydoc BlackMisc::CValueObject::getMetaTypeId
        virtual int getMetaTypeId() const override { return qMetaTypeId<CDictionary>(); }

        //! \copydoc BlackMisc::CValueObject::isA
        virtual bool isA(int metaTypeId) const override
        {
            if (metaTypeId == qMetaTypeId<CDictionary>()) { return true; }
            return CValueObject::isA(metaTypeId);
        }

        //! \copydoc BlackMisc::CValueObject::compareImpl
        virtual int compareImpl(const CValueObject &other) const override
        {
            const auto &o = static_cast<const CDictionary &>(other);
            if (m_impl.size() < o.m_impl.size()) { return -1; }
            if (m_impl.size() > o.m_impl.size()) { return 1; }
            return 0;
        }

        //! \copydoc BlackMisc::CValueObject::marshallToDbus
        virtual void marshallToDbus(QDBusArgument &argument) const override
        {
           argument << m_impl;
        }

        //! \copydoc BlackMisc::CValueObject::unmarshallFromDbus
        virtual void unmarshallFromDbus(const QDBusArgument &argument) override
        {
            argument >> m_impl;
        }

    private:

        Impl<Key,Value> m_impl;
    };

} // namespace BlackMisc

#endif // BLACKMISC_DICTIONARY_H

