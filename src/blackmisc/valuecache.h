/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_VALUECACHE_H
#define BLACKMISC_VALUECACHE_H

#include "blackmisc/valuecacheprivate.h"
#include "blackmisc/range.h"

namespace BlackMisc
{

    /*!
     * Value class used for signalling changed values in the cache.
     */
    class BLACKMISC_EXPORT CValueCachePacket :
        public CDictionary<QString, std::pair<CVariant, qint64>, QMap>,
        public Mixin::MetaType<CValueCachePacket>,
        public Mixin::DBusByTuple<CValueCachePacket>,
        public Mixin::EqualsByTuple<CValueCachePacket>
    {
    public:
        BLACKMISC_DECLARE_USING_MIXIN_DBUS(CValueCachePacket)

        //! \copydoc BlackMisc::CValueObject::base_type
        using base_type = CDictionary;

        //! Constructor.
        CValueCachePacket(bool saved = false) : m_saved(saved) {}

        //! Construct from CVariantMap and a timestamp.
        CValueCachePacket(const CVariantMap &values, qint64 timestamp, bool saved = false);

        //! Values are to be saved.
        //! @{
        bool isSaved() const { return m_saved; }
        void setSaved(bool saved = true) { m_saved = saved; }
        //! @}

        //! Insert a key/value pair with a timestamp.
        void insert(const QString &key, const CVariant &value, qint64 timestamp);

        //! Insert a CVariantMap with a timestamp.
        void insert(const CVariantMap &values, qint64 timestamp);

        //! Insert values from another packet.
        void insert(const CValueCachePacket &other) { CDictionary::insert(other); }

        //! Add a page owner and key to the list of inhibited notifications.
        void inhibit(QObject *pageOwner, const QString &key) { m_inhibitions.push_back(std::make_pair(reinterpret_cast<quintptr>(pageOwner), key)); }

        //! Query whether a particular owner/key pair should have its notification inhibited.
        bool isInhibited(QObject *pageOwner, const QString &key) const { return m_inhibitions.contains(std::make_pair(reinterpret_cast<quintptr>(pageOwner), key)); }

        //! Discard timestamps and return as variant map.
        CVariantMap toVariantMap() const;

        //! Discard values and return as map of timestamps.
        QMap<QString, qint64> toTimestampMap() const;

        //! \copydoc BlackMisc::Mixin::MetaType::registerMetadata
        static void registerMetadata();

        //! \private Iterator behaves like a CVariantMap::const_iterator with an additional timestamp() method.
        struct const_iterator : public CDictionary::const_iterator
        {
            using value_type = CVariant;
            using pointer = const value_type *;
            using reference = const value_type &;
            const_iterator(CDictionary::const_iterator base) : CDictionary::const_iterator(base) {}
            reference value() const { return CDictionary::const_iterator::value().first; }
            reference operator *() const { return value(); }
            pointer operator ->() const { return &value(); }
            qint64 timestamp() const { return CDictionary::const_iterator::value().second; }
        };

        //! Iterators.
        //! @{
        const_iterator cbegin() const { return CDictionary::cbegin(); }
        const_iterator cend() const { return CDictionary::cend(); }
        const_iterator begin() const { return CDictionary::cbegin(); }
        const_iterator end() const { return CDictionary::cend(); }
        //! @}

    private:
        BLACK_ENABLE_TUPLE_CONVERSION(CValueCachePacket)
        bool m_saved = false;
        CSequence<std::pair<quintptr, QString>> m_inhibitions;
    };

    /*!
     * Manages a map of { QString, CVariant } pairs, which can be distributed among multiple processes.
     */
    class BLACKMISC_EXPORT CValueCache : public QObject
    {
        Q_OBJECT

    public:
        class BatchGuard;

        //! Whether or not the cache can be distributed among multiple processes.
        enum DistributionMode
        {
            LocalOnly,      //!< Not distributed.
            Distributed     //!< Distributed among multiple processes.
        };

        //! Constructor.
        //! \param mode Whether or not the cache can be distributed among multiple processes.
        //! \param parent The parent of the QObject.
        explicit CValueCache(DistributionMode mode, QObject *parent = nullptr);

        //! Return map containing all values in the cache.
        //! If prefix is provided then only those values whose keys start with that prefix.
        //! \threadsafe
        BlackMisc::CVariantMap getAllValues(const QString &keyPrefix = {}) const;

        //! Return map containing all values in the cache, and timestamps when they were modified.
        //! \threadsafe
        BlackMisc::CValueCachePacket getAllValuesWithTimestamps(const QString &keyPrefix = {}) const;

        //! Add some values to the cache.
        //! Values already in the cache will remain in the cache unless they are overwritten.
        //! \threadsafe
        void insertValues(const BlackMisc::CValueCachePacket &values);

        //! Save values in Json format.
        //! If prefix is provided then only those values whose keys start with that prefix.
        //! \threadsafe
        QJsonObject saveToJson(const QString &keyPrefix = {}) const;

        //! Load all values in Json format.
        //! Values already in the cache will remain in the cache unless they are overwritten.
        //! \threadsafe
        void loadFromJson(const QJsonObject &json);

        //! Save values to Json files in a given directory.
        //! If prefix is provided then only those values whose keys start with that prefix.
        //! \threadsafe
        CStatusMessage saveToFiles(const QString &directory, const QString &keyPrefix = {});

        //! Load all values from Json files in a given directory.
        //! Values already in the cache will remain in the cache unless they are overwritten.
        //! \threadsafe
        CStatusMessage loadFromFiles(const QString &directory);

        //! Return the (relative) filename that may is (or would be) used to save the value with the given key.
        //! The file may or may not exist (because it might not have been saved yet).
        //! \threadsafe
        static QString filenameForKey(const QString &key);

        //! List the Json files which are (or would be) used to save the current values.
        //! The files may or may not exist (because they might not have been saved yet).
        //! \threadsafe
        QStringList enumerateFiles(const QString &directory) const;

        //! Clear all values from the cache.
        //! \threadsafe
        void clearAllValues(const QString &keyPrefix = {});

        //! Begins a batch of changes to be made through CCached instances owned by owner.
        //! \details All changes made through those CCached instances will be deferred until the returned RAII object is
        //! destroyed. If the destruction happens during stack unwinding due to an exception being thrown, the changes are
        //! abandoned, otherwise they are applied in one single change signal instead of lots of individual ones.
        //! Can be called multiple times; the batch will be applied (or abandoned) when the last RAII object is destroyed.
        //! CCached::getValue will continue to return the old value until the batched changes are applied.
        //! \note Must be called in the thread where owner lives.
        BatchGuard batchChanges(QObject *owner);

    public slots:
        //! Notify this cache that values have been changed by one of the duplicate caches in the multi-process environment.
        //! \see BlackMisc::CValueCache::valuesChangedByLocal.
        //! \param values The values that were changed.
        //! \param originator Identifier of the process which made the change. Can be this very process, or a different one.
        void changeValuesFromRemote(const BlackMisc::CValueCachePacket &values, const BlackMisc::CIdentifier &originator);

    signals:
        //! Emitted when values in the cache are changed by an object in this very process.
        //! The interprocess communication (e.g. DBus) should arrange for this signal to call the slot changeValueFromRemote
        //! of CValueCache instances in all processes including this one. The slot will do its own round-trip detection.
        void valuesChangedByLocal(const BlackMisc::CValueCachePacket &values);

        //! Emitted when this cache has ratified a change which included a request to save (i.e. via CCaches::setAndSave).
        //! If the derived class does not handle such requests, the signal can be ignored.
        void valuesSaveRequested(const BlackMisc::CValueCachePacket &values);

    private:
        struct Element; // remove forward declaration (and uncomment the one below) when elementsStartingWith uses C++14 auto deduced return type

    protected:
        //! Returns a range referring to all elements which start with the given prefix.
        //! \todo Use C++14 auto deduced return type.
        //! @{
        CRange<QMap<QString, QSharedPointer<Element>>::iterator> elementsStartingWith(const QString &keyPrefix)
        {
            return makeRange(m_elements.lowerBound(keyPrefix), m_elements.lowerBound(keyPrefix + QChar(QChar::LastValidCodePoint)));
        }
        CRange<QMap<QString, QSharedPointer<Element>>::const_iterator> elementsStartingWith(const QString &keyPrefix) const
        {
            return makeRange(m_elements.lowerBound(keyPrefix), m_elements.lowerBound(keyPrefix + QChar(QChar::LastValidCodePoint)));
        }
        //! @}

        //! Save specific values to Json files in a given directory.
        //! \threadsafe
        CStatusMessage saveToFiles(const QString &directory, const CVariantMap &values) const;

        //! Load from Json files in a given directory any values which differ from the current ones, and insert them in o_values.
        //! \threadsafe
        CStatusMessage loadFromFiles(const QString &directory, const QSet<QString> &keys, const CVariantMap &current, CValueCachePacket &o_values) const;

        //! Mark all values with keys that start with the given prefix as having been saved.
        //! \threadsafe
        void markAllAsSaved(const QString &keyPrefix);

        //! Mutex protecting operations which are critical on m_elements.
        mutable QMutex m_mutex { QMutex::Recursive };

    protected:
        //! Synchronously return a current value.
        //! \threadsafe
        CVariant getValueSync(const QString &key) { return std::get<0>(getValue(key)); }

        //! Synchronously return a current timestamp.
        //! \threadsafe
        qint64 getTimestampSync(const QString &key) { return std::get<1>(getValue(key)); }

    private:
        friend class Private::CValuePage;
        // struct Element; // to be uncommented when the forward declaration above elementsStartingWith is removed
        using ElementPtr = QSharedPointer<Element>; // QMap doesn't support move-only types

        QMap<QString, ElementPtr> m_elements;

        Element &getElement(const QString &key);
        Element &getElement(const QString &key, QMap<QString, ElementPtr>::const_iterator pos);
        std::tuple<CVariant, qint64, bool> getValue(const QString &key);

    signals:
        //! \private
        void valuesChanged(const BlackMisc::CValueCachePacket &values, QObject *changedBy);

    private slots:
        void changeValues(const BlackMisc::CValueCachePacket &values);
    };

    /*!
     * Provides access to one of the values stored in a CValueCache.
     * \tparam T The expected value type of the cached value.
     */
    template <typename T>
    class CCached
    {
    public:
        //! Type of pointer to non-const member function of U taking no arguments and returning void,
        //! for slot parameter of CCached constructor.
        template <typename U>
        using NotifySlot = void (U::*)();

        //! Constructor.
        //! \param cache The CValueCache object which manages the value.
        //! \param key The key string which identifies the value.
        //! \param owner Will be the parent of the internal QObject used for signal/slot connections.
        //! \param slot A member function of owner which will be called when the value is changed by another source.
        template <typename U>
        CCached(CValueCache *cache, const QString &key, U *owner, NotifySlot<U> slot = nullptr) :
            CCached(cache, key, nullptr, T{}, owner, slot)
        {}

        //! Constructor.
        //! \param cache The CValueCache object which manages the value.
        //! \param key The key string which identifies the value.
        //! \param validator A functor which tests the validity of a value and returns true if it is valid.
        //! \param defaultValue A value which will be used as default if the value is invalid.
        //! \param owner Will be the parent of the internal QObject used for signal/slot connections.
        //! \param slot A member function of owner which will be called when the value is changed by another source.
        template <typename U, typename F>
        CCached(CValueCache *cache, const QString &key, F validator, const T &defaultValue, U *owner, NotifySlot<U> slot = nullptr) :
            m_page(Private::CValuePage::getPageFor(owner, cache)),
            m_element(m_page.createElement(key, qMetaTypeId<T>(), wrap(validator), CVariant::from(defaultValue), slot_cast(slot)))
        {}

        //! Read the current value.
        //! \threadsafe
        const T &get() const { static const T empty {}; return *(isValid() ? static_cast<const T *>(getVariant().data()) : &empty); }

        //! Write a new value. Must be called from the thread in which the owner lives.
        CStatusMessage set(const T &value, qint64 timestamp = 0) { return m_page.setValue(m_element, CVariant::from(value), timestamp); }

        //! Write and save in the same step. Must be called from the thread in which the owner lives.
        CStatusMessage setAndSave(const T &value, qint64 timestamp = 0) { return m_page.setValue(m_element, CVariant::from(value), timestamp, true); }

        //! Get the key string of this value.
        const QString &getKey() const { return m_page.getKey(m_element); }

        //! Return the time when this value was updated.
        QDateTime getTimestamp() const { return m_page.getTimestamp(m_element); }

        //! Return true if this value was already saved.
        bool isSaved() const { return m_page.isSaved(m_element); }

        //! Return true if this value is currently saving.
        bool isSaving() const { return m_page.isSaving(m_element); }

        //! Deleted copy constructor.
        CCached(const CCached &) = delete;

        //! Deleted copy assignment operator.
        CCached &operator =(const CCached &) = delete;

    private:
        template <typename F>
        static Private::CValuePage::Validator wrap(F func) { return [func](const CVariant &value)->bool { return func(value.to<T>()); }; }
        static Private::CValuePage::Validator wrap(std::nullptr_t) { return {}; }

        template <typename F>
        static Private::CValuePage::NotifySlot slot_cast(F slot) { return static_cast<Private::CValuePage::NotifySlot>(slot); }

        const QVariant &getVariant() const { return m_page.getValue(m_element).getQVariant(); }
        bool isValid() const { return getVariant().isValid() && getVariant().userType() == qMetaTypeId<T>(); }

        Private::CValuePage &m_page;
        Private::CValuePage::Element &m_element;
    };

    /*!
     * RAII object returned by CValueCache::batchChanges. Applies deferred changes when it is destroyed.
     */
    class BLACKMISC_EXPORT CValueCache::BatchGuard
    {
    public:
        //! Destructor. Applies deferred changes.
        ~BatchGuard() Q_DECL_NOEXCEPT;

        //! Deleted copy constructor. Class is move-only.
        BatchGuard(const BatchGuard &) = delete;

        //! Deleted copy assignment operator. Class is move-only.
        BatchGuard &operator =(const BatchGuard &) = delete;

        //! Move constructor.
        BatchGuard(BatchGuard &&other) : m_page(other.m_page) { other.m_page = nullptr; }

        //! Move assignment operator.
        BatchGuard &operator =(BatchGuard &&other) { std::swap(m_page, other.m_page); return *this; }

    private:
        friend class CValueCache;
        BatchGuard(Private::CValuePage &page) : m_page(&page) {}
        Private::CValuePage *m_page;
    };

} // namespace

Q_DECLARE_METATYPE(BlackMisc::CValueCachePacket)
BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::CValueCachePacket, (attr(o.m_saved), attr(o.m_inhibitions)))

#endif
