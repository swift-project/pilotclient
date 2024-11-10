// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_VALUECACHE_H
#define SWIFT_MISC_VALUECACHE_H

#include "misc/swiftmiscexport.h"
#include "misc/mixin/mixincompare.h"
#include "misc/mixin/mixindbus.h"
#include "misc/mixin/mixindatastream.h"
#include "misc/mixin/mixinhash.h"
#include "misc/dictionary.h"
#include "misc/identifier.h"
#include "misc/metaclass.h"
#include "misc/propertyindexref.h"
#include "misc/range.h"
#include "misc/slot.h"
#include "misc/statusmessagelist.h"
#include "misc/valuecacheprivate.h"
#include "misc/mixin/mixinmetatype.h"
#include "misc/variantmap.h"

#include <QChar>
#include <QDBusArgument>
#include <QDateTime>
#include <QFile>
#include <QJsonObject>
#include <QMap>
#include <QMetaType>
#include <QMutex>
#include <QObject>
#include <QSet>
#include <QSharedPointer>
#include <QString>
#include <QStringList>
#include <QThread>
#include <QVariant>
#include <QtGlobal>
#include <stdexcept>
#include <cstddef>
#include <tuple>
#include <utility>
#include <memory>

namespace swift::misc
{
    class CLogCategoryList;

    /*!
     * Overwrite the default root directory for cache and settings, for testing purposes.
     *
     * May not be called after any cache or settings objects have been constructed.
     */
    SWIFT_MISC_EXPORT void setMockCacheRootDirectory(const QString &path);

    /*!
     * Value class used for signalling changed values in the cache.
     */
    class SWIFT_MISC_EXPORT CValueCachePacket :
        public CDictionary<QString, std::pair<CVariant, qint64>, QMap>,
        public mixin::MetaType<CValueCachePacket>,
        public mixin::DBusByMetaClass<CValueCachePacket>,
        public mixin::DataStreamByMetaClass<CValueCachePacket>,
        public mixin::EqualsByMetaClass<CValueCachePacket>
    {
    public:
        SWIFT_MISC_DECLARE_USING_MIXIN_DBUS(CValueCachePacket)
        SWIFT_MISC_DECLARE_USING_MIXIN_DATASTREAM(CValueCachePacket)

        //! \copydoc swift::misc::CValueObject::base_type
        using base_type = CDictionary;

        //! Constructor.
        CValueCachePacket(bool saved = false, bool valuesChanged = true) : m_saved(saved), m_valuesChanged(valuesChanged)
        {}

        //! Construct from CVariantMap and a timestamp.
        CValueCachePacket(const CVariantMap &values, qint64 timestamp, bool saved = false, bool valuesChanged = true);

        //! Values have been changed.
        bool valuesChanged() const { return m_valuesChanged; }

        //! @{
        //! Values are to be saved.
        bool isSaved() const { return m_saved; }
        void setSaved(bool saved = true) { m_saved = saved; }
        //! @}

        //! Insert a key/value pair with a timestamp.
        void insert(const QString &key, const CVariant &value, qint64 timestamp);

        //! Insert a CVariantMap with a timestamp.
        void insert(const CVariantMap &values, qint64 timestamp);

        //! Insert values from another packet.
        void insert(const CValueCachePacket &other) { CDictionary::insert(other); }

        //! Discard timestamps and return as variant map.
        CVariantMap toVariantMap() const;

        //! Discard values and return as map of timestamps.
        QMap<QString, qint64> toTimestampMap() const;

        //! Return map of timestamps converted to string.
        QString toTimestampMapString(const QStringList &keys) const;

        //! Change the timestamps of values.
        void setTimestamps(const QMap<QString, qint64> &);

        //! Remove value matching the given key, and return it in a separate packet.
        CValueCachePacket takeByKey(const QString &key);

        //! \copydoc swift::misc::mixin::MetaType::registerMetadata
        static void registerMetadata();

        //! \private Iterator behaves like a CVariantMap::const_iterator with an additional timestamp() method.
        struct const_iterator : public CDictionary::const_iterator
        {
            using value_type = CVariant;
            using pointer = const value_type *;
            using reference = const value_type &;
            const_iterator(CDictionary::const_iterator base) : CDictionary::const_iterator(base) {}
            reference value() const { return CDictionary::const_iterator::value().first; }
            reference operator*() const { return value(); }
            pointer operator->() const { return &value(); }
            qint64 timestamp() const { return CDictionary::const_iterator::value().second; }
        };

        //! @{
        //! Iterators.
        const_iterator cbegin() const { return CDictionary::cbegin(); }
        const_iterator cend() const { return CDictionary::cend(); }
        const_iterator begin() const { return CDictionary::cbegin(); }
        const_iterator end() const { return CDictionary::cend(); }
        //! @}

    private:
        bool m_saved = false;
        bool m_valuesChanged = true;

        BLACK_METACLASS(
            CValueCachePacket,
            BLACK_METAMEMBER(saved),
            BLACK_METAMEMBER(valuesChanged)
        );
    };

    /*!
     * Manages a map of { QString, CVariant } pairs, which can be distributed among multiple processes.
     */
    class SWIFT_MISC_EXPORT CValueCache : public QObject
    {
        Q_OBJECT

    public:
        class BatchGuard;

        //! Log categories
        static const QStringList &getLogCategories();

        //! Constructor.
        explicit CValueCache(int fileSplitDepth, QObject *parent = nullptr);

        //! Return map containing all values in the cache.
        //! If prefix is provided then only those values whose keys start with that prefix.
        //! \threadsafe
        swift::misc::CVariantMap getAllValues(const QString &keyPrefix = {}) const;

        //! Return map containing all given values in the cache.
        //! \threadsafe
        swift::misc::CVariantMap getAllValues(const QStringList &keys) const;

        //! Return map containing all values in the cache, and timestamps when they were modified.
        //! \threadsafe
        swift::misc::CValueCachePacket getAllValuesWithTimestamps(const QString &keyPrefix = {}) const;

        //! Return keys of all values which have been changed but not saved.
        //! \threadsafe
        QStringList getAllUnsavedKeys(const QString &keyPrefix = {}) const;

        //! Add some values to the cache.
        //! Values already in the cache will remain in the cache unless they are overwritten.
        //! \threadsafe
        void insertValues(const swift::misc::CValueCachePacket &values);

        //! Save values in Json format.
        //! If prefix is provided then only those values whose keys start with that prefix.
        //! \threadsafe
        QJsonObject saveToJson(const QString &keyPrefix = {}) const;

        //! Load all values in Json format.
        //! Values already in the cache will remain in the cache unless they are overwritten.
        //! \throws swift::misc::CJsonException if JSON schema validation fails.
        //! \threadsafe
        void loadFromJson(const QJsonObject &json);

        //! Call loadFromJson, catch any CJsonException that are thrown and return them as CStatusMessage.
        //! \threadsafe
        CStatusMessageList loadFromJsonNoThrow(const QJsonObject &json, const CLogCategoryList &categories, const QString &prefix);

        //! Save values to Json files in a given directory.
        //! If prefix is provided then only those values whose keys start with that prefix.
        //! \threadsafe
        CStatusMessage saveToFiles(const QString &directory, const QString &keyPrefix = {});

        //! Save values to Json files in a given directory.
        //! \threadsafe
        CStatusMessage saveToFiles(const QString &directory, const QStringList &keys);

        //! Load all values from Json files in a given directory.
        //! Values already in the cache will remain in the cache unless they are overwritten.
        //! \threadsafe
        CStatusMessage loadFromFiles(const QString &directory);

        //! Return the (relative) filename that may is (or would be) used to save the value with the given key.
        //! The file may or may not exist (because it might not have been saved yet).
        //! \threadsafe
        QString filenameForKey(const QString &key) const;

        //! List the Json files which are (or would be) used to save the current values.
        //! The files may or may not exist (because they might not have been saved yet).
        //! \threadsafe
        QStringList enumerateFiles(const QString &directory) const;

        //! Clear all values from the cache.
        //! \threadsafe
        void clearAllValues(const QString &keyPrefix = {});

        //! Return the human readable name of the given key, or the raw key string if there is none.
        QString getHumanReadableName(const QString &key) const;

        //! Return the human readable name of the given key, with the raw key string appended.
        QString getHumanReadableWithKey(const QString &key) const;

        //! \private
        void setHumanReadableName(const QString &key, const QString &name);

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
        //! \see swift::misc::CValueCache::valuesChangedByLocal.
        //! \param values The values that were changed.
        //! \param originator Identifier of the process which made the change. Can be this very process, or a different one.
        void changeValuesFromRemote(const swift::misc::CValueCachePacket &values, const swift::misc::CIdentifier &originator);

    signals:
        //! Emitted when values in the cache are changed by an object in this very process.
        //! The interprocess communication (e.g. DBus) should arrange for this signal to call the slot changeValueFromRemote
        //! of CValueCache instances in all processes including this one. The slot will do its own round-trip detection.
        void valuesChangedByLocal(const swift::misc::CValueCachePacket &values);

        //! Emitted when this cache has ratified a change which included a request to save (i.e. via CCaches::setAndSave).
        //! If the derived class does not handle such requests, the signal can be ignored.
        void valuesSaveRequested(const swift::misc::CValueCachePacket &values);

    protected:
        //! @{
        //! Returns a range referring to all elements which start with the given prefix.
        auto elementsStartingWith(const QString &keyPrefix)
        {
            return makeRange(m_elements.lowerBound(keyPrefix), m_elements.lowerBound(keyPrefix + QChar(QChar::LastValidCodePoint)));
        }
        auto elementsStartingWith(const QString &keyPrefix) const
        {
            return makeRange(m_elements.lowerBound(keyPrefix), m_elements.lowerBound(keyPrefix + QChar(QChar::LastValidCodePoint)));
        }
        //! @}

        //! Save specific values to Json files in a given directory.
        //! \threadsafe
        CStatusMessage saveToFiles(const QString &directory, const CVariantMap &values, const QString &keysMessage = {}) const;

        //! Load from Json files in a given directory any values which differ from the current ones, and insert them in o_values.
        //! \threadsafe
        CStatusMessage loadFromFiles(const QString &directory, const QSet<QString> &keys, const CVariantMap &current, CValueCachePacket &o_values, const QString &keysMessage = {}, bool keysOnly = false) const;

        //! Mark all values with keys that start with the given prefix as having been saved.
        //! \threadsafe
        void markAllAsSaved(const QString &keyPrefix);

        //! Mark all values with given keys as having been saved.
        //! \threadsafe
        void markAllAsSaved(const QStringList &keys);

        //! Mutex protecting operations which are critical on m_elements.
        mutable QRecursiveMutex m_mutex;

    protected:
        //! Synchronously return a current value.
        //! \threadsafe
        CVariant getValueSync(const QString &key) { return std::get<0>(getValue(key)); }

        //! Synchronously return a current timestamp.
        //! \threadsafe
        qint64 getTimestampSync(const QString &key) { return std::get<1>(getValue(key)); }

        //! \private
        static const QString &getCacheRootDirectory();

    private:
        friend class private_ns::CValuePage;
        struct Element;

        using ElementPtr = QSharedPointer<Element>; // QMap doesn't support move-only types

        QMap<QString, ElementPtr> m_elements;
        QMap<QString, QString> m_humanReadable;
        const int m_fileSplitDepth = 1; //!< How many levels of subdirectories to split JSON files

        Element &getElement(const QString &key);
        Element &getElement(const QString &key, QMap<QString, ElementPtr>::const_iterator pos);
        std::tuple<CVariant, qint64, bool> getValue(const QString &key);
        void backupFile(QFile &file) const;

        virtual void connectPage(private_ns::CValuePage *page);

        // only used by CValuePage::createElement
        QSet<QString> m_warnedKeys;
        QMutex m_warnedKeysMutex;

    signals:
        //! \private
        void valuesChanged(const swift::misc::CValueCachePacket &values, QObject *changedBy);

    protected slots:
        //! \private
        void changeValues(const swift::misc::CValueCachePacket &values);
    };

    /*!
     * Provides access to one of the values stored in a CValueCache.
     * \tparam T The expected value type of the cached value.
     */
    template <typename T>
    class CCached
    {
    public:
        //! Constructor.
        //! \param cache The CValueCache object which manages the value.
        //! \param key The key string which identifies the value.
        //! \param name Human readable name corresponding to the key.
        //! \param owner Will be the parent of the internal QObject used for signal/slot connections.
        template <typename U>
        CCached(CValueCache *cache, const QString &key, const QString &name, U *owner) : CCached(cache, key, name, nullptr, T {}, owner)
        {}

        //! Constructor.
        //! \param cache The CValueCache object which manages the value.
        //! \param key The key string which identifies the value.
        //! \param name Human readable name corresponding to the key.
        //! \param validator A functor which tests the validity of a value and returns true if it is valid.
        //! \param defaultValue A value which will be used as default if the value is invalid.
        //! \param owner Will be the parent of the internal QObject used for signal/slot connections.
        template <typename U, typename F>
        CCached(CValueCache *cache, const QString &key, const QString &name, F validator, const T &defaultValue, U *owner) : m_page(&private_ns::CValuePage::getPageFor(owner, cache)),
                                                                                                                             m_element(&m_page->createElement(key, name, qMetaTypeId<T>(), wrap(validator), CVariant::from(defaultValue)))
        {
            if (isInitialized()) { cache->setHumanReadableName(getKey(), name); }
        }

        //! Set a callback to be called when the value is changed by another source.
        template <typename F>
        void setNotifySlot(F slot)
        {
            if (!isInitialized())
            {
                onOwnerNameChanged([this, slot] { setNotifySlot(slot); });
                return;
            }
            using U = typename private_ns::TClassOfPointerToMember<F>::type;
            Q_ASSERT_X(m_page->parent()->metaObject()->inherits(&U::staticMetaObject), Q_FUNC_INFO, "Slot is member function of wrong class");
            m_page->setNotifySlot(*m_element, { [slot](QObject *obj) { private_ns::invokeSlot(slot, static_cast<U *>(obj)); }, makeId(slot) });
        }

        //! Read the current value.
        const T &getThreadLocal() const
        {
            static const T empty {};
            return *(isValid() ? static_cast<const T *>(getVariant().data()) : &empty);
        }

        //! Get a copy of the current value.
        //! \threadsafe
        T get() const { return isValid() ? getVariantCopy().template value<T>() : T {}; }

        //! Write a new value. Must be called from the thread in which the owner lives.
        CStatusMessage set(const T &value, qint64 timestamp = 0) { return m_page->setValue(*m_element, CVariant::from(value), timestamp); }

        //! Write and save in the same step. Must be called from the thread in which the owner lives.
        CStatusMessage setAndSave(const T &value, qint64 timestamp = 0) { return m_page->setValue(*m_element, CVariant::from(value), timestamp, true); }

        //! Save using the currently set value. Must be called from the thread in which the owner lives.
        CStatusMessage save() { return m_page->setValue(*m_element, {}, 0, true); }

        //! Write a property of the value. Must be called from the thread in which the owner lives.
        CStatusMessage setProperty(CPropertyIndexRef index, const CVariant &value, qint64 timestamp = 0)
        {
            auto v = get();
            v.setPropertyByIndex(index, value);
            return set(v, timestamp);
        }

        //! Write a property and save in the same step. Must be called from the thread in which the owner lives.
        CStatusMessage setAndSaveProperty(CPropertyIndexRef index, const CVariant &value, qint64 timestamp = 0)
        {
            auto v = get();
            v.setPropertyByIndex(index, value);
            return setAndSave(v, timestamp);
        }

        //! Is current thread the owner thread, so CCached::set is safe
        bool isOwnerThread() const { return QThread::currentThread() == m_page->thread(); }

        //! Get the key string of this value.
        const QString &getKey() const { return m_page->getKey(*m_element); }

        //! Return the time when this value was updated.
        //! \threadsafe with tiny risk of mismatch of value and timestamp
        QDateTime getTimestamp() const { return QDateTime::fromMSecsSinceEpoch(m_page->getTimestamp(*m_element)); }

        //! Return the time when this value was updated.
        //! \threadsafe with tiny risk of mismatch of value and timestamp
        qint64 getTimestampMsSinceEpoch() const { return this->getTimestamp().toMSecsSinceEpoch(); }

        //! How old is that cache (ms)?
        qint64 lastUpdatedAge() const { return QDateTime::currentMSecsSinceEpoch() - this->getTimestampMsSinceEpoch(); }

        //! Return true if this value was already saved.
        bool isSaved() const { return m_page->isSaved(*m_element); }

        //! Return true if this value is currently saving.
        bool isSaving() const { return m_page->isSaving(*m_element); }

        //! Can be false if key contains %OwnerName% and owner's objectName was empty.
        bool isInitialized() const { return m_page->isInitialized(*m_element); }

        //! Deleted copy constructor.
        CCached(const CCached &) = delete;

        //! Deleted copy assignment operator.
        CCached &operator=(const CCached &) = delete;

    private:
        template <typename F>
        static private_ns::CValuePage::Validator wrap(F func)
        {
            return [func](const CVariant &value, QString &reason) -> bool { return func(value.to<T>(), reason); };
        }
        static private_ns::CValuePage::Validator wrap(std::nullptr_t) { return {}; }

        template <typename F>
        static auto makeId(F &&)
        {
            return nullptr;
        }
        template <typename U, typename M>
        static auto makeId(M U::*slot)
        {
            return static_cast<std::tuple_element_t<1, private_ns::CValuePage::NotifySlot>>(slot);
        }

        const QVariant &getVariant() const { return m_page->getValue(*m_element).getQVariant(); }
        QVariant getVariantCopy() const { return m_page->getValueCopy(*m_element).getQVariant(); }
        bool isValid() const { return m_page->isValid(*m_element, qMetaTypeId<T>()); }

    protected:
        //! \private Connect a function to be called (only once) when the owner's objectName changes.
        void onOwnerNameChanged(const std::function<void()> &function)
        {
            connectOnce(m_page->parent(), &QObject::objectNameChanged, [function](const QString &) { function(); });
        }

        private_ns::CValuePage *m_page = (throw std::logic_error("Uninitialized member"), nullptr); //!< \private
        private_ns::CValuePage::Element *m_element = (throw std::logic_error("Uninitialized member"), nullptr); //!< \private
    };

    /*!
     * RAII object returned by CValueCache::batchChanges. Applies deferred changes when it is destroyed.
     */
    class SWIFT_MISC_EXPORT CValueCache::BatchGuard
    {
    public:
        //! Destructor. Applies deferred changes.
        ~BatchGuard();

        //! Deleted copy constructor. Class is move-only.
        BatchGuard(const BatchGuard &) = delete;

        //! Deleted copy assignment operator. Class is move-only.
        BatchGuard &operator=(const BatchGuard &) = delete;

        //! Move constructor.
        BatchGuard(BatchGuard &&other) noexcept : m_page(other.m_page) { other.m_page = nullptr; }

        //! Move assignment operator.
        BatchGuard &operator=(BatchGuard &&other) noexcept
        {
            std::swap(m_page, other.m_page);
            return *this;
        }

    private:
        friend class CValueCache;
        BatchGuard(private_ns::CValuePage &page) : m_page(&page) {}
        private_ns::CValuePage *m_page;
    };

} // namespace

Q_DECLARE_METATYPE(swift::misc::CValueCachePacket)

#endif
