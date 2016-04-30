/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \cond PRIVATE

#include "blackmisc/valuecache.h"
#include "blackmisc/identifier.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/algorithm.h"
#include "blackmisc/lockfree.h"
#include "blackmisc/atomicfile.h"
#include <QThread>
#include <QJsonDocument>
#include <QDir>

namespace BlackMisc
{

    using Private::CValuePage;

    //! Used in asserts to protect against signed integer overflow.
    template <typename T>
    bool isSafeToIncrement(const T &value) { return value < std::numeric_limits<T>::max(); }


    ////////////////////////////////
    // CValueCachePacket
    ////////////////////////////////

    CValueCachePacket::CValueCachePacket(const CVariantMap &values, qint64 timestamp, bool saved, bool valuesChanged) :
        m_saved(saved), m_valuesChanged(valuesChanged)
    {
        for (auto it = values.cbegin(); it != values.cend(); ++it)
        {
            implementationOf(*this).insert(CDictionary::cend(), it.key(), std::make_pair(it.value(), timestamp));
        }
    }

    void CValueCachePacket::insert(const QString &key, const CVariant &value, qint64 timestamp)
    {
        CDictionary::insert(key, std::make_pair(value, timestamp));
    }

    void CValueCachePacket::insert(const CVariantMap &values, qint64 timestamp)
    {
        for (auto it = values.cbegin(); it != values.cend(); ++it)
        {
            CDictionary::insert(it.key(), std::make_pair(it.value(), timestamp));
        }
    }

    CVariantMap CValueCachePacket::toVariantMap() const
    {
        CVariantMap result;
        for (auto it = cbegin(); it != cend(); ++it)
        {
            implementationOf(result).insert(result.cend(), it.key(), it.value());
        }
        return result;
    }

    QMap<QString, qint64> CValueCachePacket::toTimestampMap() const
    {
        QMap<QString, qint64> result;
        for (auto it = cbegin(); it != cend(); ++it)
        {
            implementationOf(result).insert(result.cend(), it.key(), it.timestamp());
        }
        return result;
    }

    CValueCachePacket CValueCachePacket::takeByKey(const QString &key)
    {
        auto copy = *this;
        remove(key);
        copy.removeByKeyIf([ = ](const QString &key2) { return key2 != key; });
        return copy;
    }

    void CValueCachePacket::registerMetadata()
    {
        MetaType::registerMetadata();
        qDBusRegisterMetaType<value_type>();
    }

    ////////////////////////////////
    // CValueCache
    ////////////////////////////////

    const CLogCategoryList &CValueCache::getLogCategories()
    {
        static const CLogCategoryList cats({ CLogCategory("swift.valuecache") , CLogCategory::services()} );
        return cats;
    }

    CValueCache::CValueCache(CValueCache::DistributionMode mode, QObject *parent) :
        QObject(parent)
    {
        if (mode == LocalOnly)
        {
            // loopback signal to own slot for local operation
            connect(this, &CValueCache::valuesChangedByLocal, this, [ = ](const CValueCachePacket & values)
            {
                changeValuesFromRemote(values, CIdentifier());
            });
        }
    }

    struct CValueCache::Element
    {
        Element(const QString &key) : m_key(key) {}
        const QString m_key;
        CVariant m_value;
        int m_pendingChanges = 0;
        bool m_saved = false;
        std::atomic<qint64> m_timestamp { 0 };
    };

    CValueCache::Element &CValueCache::getElement(const QString &key)
    {
        QMutexLocker lock(&m_mutex);
        return getElement(key, m_elements.lowerBound(key));
    }

    CValueCache::Element &CValueCache::getElement(const QString &key, QMap<QString, ElementPtr>::const_iterator pos)
    {
        QMutexLocker lock(&m_mutex);
        if (pos != m_elements.end() && pos.key() == key) { return **pos; }
        Q_ASSERT(pos == m_elements.lowerBound(key));
        return **m_elements.insert(pos, key, ElementPtr(new Element(key)));
    }

    std::tuple<CVariant, qint64, bool> CValueCache::getValue(const QString &key)
    {
        QMutexLocker lock(&m_mutex);
        const auto &element = getElement(key);
        return std::make_tuple(element.m_value, element.m_timestamp.load(), element.m_saved);
    }

    CVariantMap CValueCache::getAllValues(const QString &keyPrefix) const
    {
        QMutexLocker lock(&m_mutex);
        CVariantMap map;
        for (const auto &element : elementsStartingWith(keyPrefix))
        {
            implementationOf(map).insert(map.cend(), element->m_key, element->m_value);
        }
        return map;
    }

    CValueCachePacket CValueCache::getAllValuesWithTimestamps(const QString &keyPrefix) const
    {
        QMutexLocker lock(&m_mutex);
        CValueCachePacket map;
        for (const auto &element : elementsStartingWith(keyPrefix))
        {
            map.insert(element->m_key, element->m_value, element->m_timestamp);
        }
        return map;
    }

    void CValueCache::insertValues(const CValueCachePacket &values)
    {
        QMutexLocker lock(&m_mutex);
        changeValues(values);
    }

    void CValueCache::changeValues(const CValueCachePacket &values)
    {
        QMutexLocker lock(&m_mutex);
        if (values.empty()) { return; }
        auto out = m_elements.lowerBound(values.cbegin().key());
        auto end = m_elements.upperBound((values.cend() - 1).key());
        for (auto in = values.cbegin(); in != values.cend(); ++in)
        {
            while (out != end && out.key() < in.key()) { ++out; }
            auto &element = getElement(in.key(), out);

            if (values.valuesChanged())
            {
                Q_ASSERT(isSafeToIncrement(element.m_pendingChanges));
                element.m_pendingChanges++;
                element.m_value = in.value();
                element.m_timestamp = in.timestamp();
            }
            element.m_saved = values.isSaved();
        }
        if (values.valuesChanged()) { emit valuesChanged(values, sender()); }
        emit valuesChangedByLocal(values);
    }

    void CValueCache::changeValuesFromRemote(const CValueCachePacket &values, const CIdentifier &originator)
    {
        QMutexLocker lock(&m_mutex);
        if (values.empty()) { return; }
        if (! values.valuesChanged())
        {
            if (values.isSaved()) { emit valuesSaveRequested(values); }
            return;
        }
        CValueCachePacket ratifiedChanges(values.isSaved());
        auto out = m_elements.lowerBound(values.cbegin().key());
        auto end = m_elements.upperBound((values.cend() - 1).key());
        for (auto in = values.cbegin(); in != values.cend(); ++in)
        {
            while (out != end && out.key() < in.key()) { ++out; }
            auto &element = getElement(in.key(), out);

            if (originator.isFromSameProcess()) // round trip
            {
                element.m_pendingChanges--;
                Q_ASSERT(element.m_pendingChanges >= 0);
            }
            else if (element.m_pendingChanges == 0) // ratify a change only if own change is not pending, to ensure consistency
            {
                element.m_value = in.value();
                element.m_timestamp = in.timestamp();
                element.m_saved = values.isSaved();
                ratifiedChanges.insert(in.key(), in.value(), in.timestamp());
            }
        }
        if (! ratifiedChanges.empty())
        {
            if (ratifiedChanges.isSaved()) { emit valuesSaveRequested(ratifiedChanges); }
            emit valuesChanged(ratifiedChanges, nullptr);
        }
    }

    QJsonObject CValueCache::saveToJson(const QString &keyPrefix) const
    {
        return getAllValues(keyPrefix).toJson();
    }

    void CValueCache::loadFromJson(const QJsonObject &json)
    {
        CVariantMap map;
        map.convertFromJson(json);
        insertValues({ map, QDateTime::currentMSecsSinceEpoch() });
    }

    CStatusMessage CValueCache::saveToFiles(const QString &dir, const QString &keyPrefix)
    {
        QMutexLocker lock(&m_mutex);
        auto values = getAllValues(keyPrefix);
        auto status = saveToFiles(dir, values);
        if (status.isSuccess()) { markAllAsSaved(keyPrefix); }
        return status;
    }

    CStatusMessage CValueCache::saveToFiles(const QString &dir, const CVariantMap &values) const
    {
        QMap<QString, CVariantMap> namespaces;
        for (auto it = values.cbegin(); it != values.cend(); ++it)
        {
            namespaces[it.key().section('/', 0, 0)].insert(it.key(), it.value());
        }
        if (! QDir::root().mkpath(dir))
        {
            return CStatusMessage(this, CStatusMessage::SeverityError, "Failed to create directory " + dir);
        }
        for (auto it = namespaces.cbegin(); it != namespaces.cend(); ++it)
        {
            CAtomicFile file(dir + "/" + it.key() + ".json");
            if (! file.open(QFile::ReadWrite | QFile::Text))
            {
                return CStatusMessage(this, CStatusMessage::SeverityError, QString("Failed to open %1: %2").arg(file.fileName()).arg(file.errorString()));
            }
            auto json = QJsonDocument::fromJson(file.readAll());
            if (json.isArray() || (json.isNull() && ! json.isEmpty()))
            {
                return CStatusMessage(this, CStatusMessage::SeverityError, "Invalid JSON format in " + file.fileName());
            }
            CVariantMap storedValues;
            storedValues.convertFromJson(json.object());
            storedValues.insert(*it);
            json.setObject(storedValues.toJson());

            if (!(file.seek(0) && file.resize(0) && file.write(json.toJson()) > 0 && file.checkedClose()))
            {
                return CStatusMessage(this, CStatusMessage::SeverityError,
                                      QString("Failed to write to %1: %2").arg(file.fileName()).arg(file.errorString()));
            }
        }
        return CStatusMessage(this, CStatusMessage::SeverityInfo,
                              QString("Written %1 files for value cache in %2").arg(namespaces.size()).arg(dir));
    }

    CStatusMessage CValueCache::loadFromFiles(const QString &dir)
    {
        QMutexLocker lock(&m_mutex);
        CValueCachePacket values;
        auto status = loadFromFiles(dir, {}, getAllValues(), values);
        values.setSaved();
        insertValues(values);
        return status;
    }

    CStatusMessage CValueCache::loadFromFiles(const QString &dir, const QSet<QString> &keys, const CVariantMap &currentValues, CValueCachePacket &o_values) const
    {
        if (! QDir(dir).isReadable())
        {
            return CStatusMessage(this, CStatusMessage::SeverityError, "Failed to create directory " + dir);
        }

        const QStringList entries(QDir(dir).entryList({ "*.json" }, QDir::Files));
        for (const auto &filename : entries)
        {
            QFile file(dir + "/" + filename);
            if (! file.open(QFile::ReadOnly | QFile::Text))
            {
                return CStatusMessage(this, CStatusMessage::SeverityError, QString("Failed to open %1: %2").arg(file.fileName()).arg(file.errorString()));
            }
            auto json = QJsonDocument::fromJson(file.readAll());
            if (json.isArray() || (json.isNull() && ! json.isEmpty()))
            {
                return CStatusMessage(this, CStatusMessage::SeverityError, "Invalid JSON format in " + file.fileName());
            }
            CVariantMap temp;
            temp.convertFromJson(json.object());
            if (! keys.isEmpty())
            {
                temp.removeByKeyIf([&keys](const QString & key) { return ! keys.contains(key); }); // TODO optimize by skipping files
            }
            temp.removeDuplicates(currentValues);
            o_values.insert(temp, QFileInfo(file).lastModified().toMSecsSinceEpoch());
        }
        return CStatusMessage(this, CStatusMessage::SeverityInfo,
                              QString("Loaded value cache from %1 files in %2").arg(entries.size()).arg(dir));
    }

    void CValueCache::markAllAsSaved(const QString &keyPrefix)
    {
        QMutexLocker lock(&m_mutex);
        for (const auto &element : elementsStartingWith(keyPrefix))
        {
            element->m_saved = true;
        }
    }

    QString CValueCache::filenameForKey(const QString &key)
    {
        return key.section('/', 0, 0) + ".json";
    }

    QStringList CValueCache::enumerateFiles(const QString &dir) const
    {
        auto values = getAllValues();
        QSet<QString> files;
        for (auto it = values.begin(); it != values.end(); ++it) { files.insert(dir + "/" + filenameForKey(it.key())); }
        return files.toList();
    }

    void CValueCache::clearAllValues(const QString &keyPrefix)
    {
        QMutexLocker lock(&m_mutex);
        auto values = getAllValues(keyPrefix);
        for (auto it = values.begin(); it != values.end(); ++it) { it.value() = CVariant(); }
        changeValues({ values, 0 });
    }

    CValueCache::BatchGuard CValueCache::batchChanges(QObject *owner)
    {
        Q_ASSERT(QThread::currentThread() == owner->thread());

        auto &page = CValuePage::getPageFor(owner, this);
        page.beginBatch();
        return page;
    }

    CValueCache::BatchGuard::~BatchGuard()
    {
        if (m_page)
        {
            if (std::uncaught_exception()) { m_page->abandonBatch(); }
            else { m_page->endBatch(); }
        }
    }

    void CValueCache::connectPage(CValuePage *page)
    {
        connect(page, &CValuePage::valuesWantToCache, this, &CValueCache::changeValues);
        connect(this, &CValueCache::valuesChanged, page, &CValuePage::setValuesFromCache);
    }


    ////////////////////////////////
    // Private :: CValuePage
    ////////////////////////////////

    CValuePage::CValuePage(QObject *parent, CValueCache *cache) :
        QObject(parent),
        m_cache(cache)
    {
        m_cache->connectPage(this);
    }

    CValuePage &CValuePage::getPageFor(QObject *parent, CValueCache *cache)
    {
        auto pages = parent->findChildren<CValuePage *>("", Qt::FindDirectChildrenOnly);
        auto it = std::find_if(pages.cbegin(), pages.cend(), [cache](CValuePage * page) { return page->m_cache == cache; });
        if (it == pages.cend()) { return *new CValuePage(parent, cache); }
        else { return **it; }
    }

    struct CValuePage::Element
    {
        Element(const QString &key, int metaType, Validator validator, const CVariant &defaultValue, NotifySlot slot) :
            m_key(key), m_metaType(metaType), m_validator(validator), m_default(defaultValue), m_notifySlot(slot)
        {}
        const QString m_key;
        LockFree<CVariant> m_value;
        std::atomic<qint64> m_timestamp { 0 };
        const int m_metaType = QMetaType::UnknownType;
        const Validator m_validator;
        const CVariant m_default;
        const NotifySlot m_notifySlot = nullptr;
        int m_pendingChanges = 0;
        bool m_saved = false;
    };

    CValuePage::Element &CValuePage::createElement(const QString &key, int metaType, Validator validator, const CVariant &defaultValue, NotifySlot slot)
    {
        Q_ASSERT_X(! m_elements.contains(key), "CValuePage", "Can't have two CCached in the same object referring to the same value");
        Q_ASSERT_X(defaultValue.isValid() ? defaultValue.userType() == metaType : true, "CValuePage", "Metatype mismatch for default value");
        Q_ASSERT_X(defaultValue.isValid() && validator ? validator(defaultValue) : true, "CValuePage", "Validator rejects default value");

        auto &element = *(m_elements[key] = ElementPtr(new Element(key, metaType, validator, defaultValue, slot)));
        std::forward_as_tuple(element.m_value.uniqueWrite(), element.m_timestamp, element.m_saved) = m_cache->getValue(key);

        auto status = validate(element, element.m_value.read(), CStatusMessage::SeverityDebug);
        if (!status.isEmpty()) // intentionally kept !empty here, debug message supposed to write default value
        {
            element.m_value.uniqueWrite() = defaultValue;
            CLogMessage::preformatted(status);
        }

        return element;
    }

    bool CValuePage::isValid(const Element &element, int typeId) const
    {
        auto reader = element.m_value.read();
        return reader->isValid() && reader->userType() == typeId;
    }

    const CVariant &CValuePage::getValue(const Element &element) const
    {
        Q_ASSERT(QThread::currentThread() == thread());

        return element.m_value.read();
    }

    CVariant CValuePage::getValueCopy(const Element &element) const
    {
        return element.m_value.read();
    }

    CStatusMessage CValuePage::setValue(Element &element, CVariant value, qint64 timestamp, bool save, bool ignoreValue)
    {
        Q_ASSERT(QThread::currentThread() == thread());

        if (timestamp == 0) { timestamp = QDateTime::currentMSecsSinceEpoch(); }
        if (element.m_value.read() == value && element.m_timestamp == timestamp && ! ignoreValue) { return {}; }

        if (ignoreValue) { value = element.m_value.read(); }

        auto status = validate(element, value, CStatusMessage::SeverityError);
        if (status.isSuccess())
        {
            if (ignoreValue)
            {
                element.m_saved = save;
                emit valuesWantToCache({ { { element.m_key, {} } }, 0, save, false });
            }
            else if (m_batchMode > 0)
            {
                m_batchedValues[element.m_key] = value;
            }
            else
            {
                Q_ASSERT(isSafeToIncrement(element.m_pendingChanges));
                element.m_pendingChanges++;
                element.m_saved = save;

                element.m_value.uniqueWrite() = value;
                emit valuesWantToCache({ { { element.m_key, value } }, timestamp, save });
            }
            // All good info
            status = CStatusMessage(this, CStatusMessage::SeverityInfo, "set values in cache");
        }
        return status;
    }

    const QString &CValuePage::getKey(const Element &element) const
    {
        return element.m_key;
    }

    qint64 CValuePage::getTimestamp(const Element &element) const
    {
        return element.m_timestamp;
    }

    bool CValuePage::isSaved(const Element &element) const
    {
        return element.m_saved && ! element.m_pendingChanges;
    }

    bool CValuePage::isSaving(const Element &element) const
    {
        return element.m_saved && element.m_pendingChanges;
    }

    void CValuePage::setValuesFromCache(const CValueCachePacket &values, QObject *changedBy)
    {
        Q_ASSERT(QThread::currentThread() == thread());
        Q_ASSERT_X(values.valuesChanged(), Q_FUNC_INFO, "packet with unchanged values should not reach here");

        QList<NotifySlot> notifySlots;

        forEachIntersection(m_elements, values, [changedBy, this, &notifySlots, &values](const QString &, const ElementPtr & element, CValueCachePacket::const_iterator it)
        {
            if (changedBy == this) // round trip
            {
                element->m_pendingChanges--;
                Q_ASSERT(element->m_pendingChanges >= 0);
            }
            else if (element->m_pendingChanges == 0) // ratify a change only if own change is not pending, to ensure consistency
            {
                auto error = validate(*element, it.value(), CStatusMessage::SeverityError);
                if (error.isSuccess())
                {
                    element->m_value.uniqueWrite() = it.value();
                    element->m_timestamp = it.timestamp();
                    element->m_saved = values.isSaved();
                    if (element->m_notifySlot && ! notifySlots.contains(element->m_notifySlot))
                    {
                        notifySlots.push_back(element->m_notifySlot);
                    }
                }
                else
                {
                    CLogMessage::preformatted(error);
                }
            }
        });

        for (auto slot : notifySlots) { (parent()->*slot)(); }
    }

    void CValuePage::beginBatch()
    {
        Q_ASSERT(QThread::currentThread() == thread());

        Q_ASSERT(isSafeToIncrement(m_batchMode));
        if (m_batchMode <= 0) { m_batchedValues.clear(); }
        m_batchMode++;
    }

    void CValuePage::abandonBatch()
    {
        Q_ASSERT(QThread::currentThread() == thread());

        Q_ASSERT(m_batchMode >= 0);
        m_batchMode--;
    }

    void CValuePage::endBatch()
    {
        Q_ASSERT(QThread::currentThread() == thread());

        Q_ASSERT(m_batchMode >= 0);
        m_batchMode--;

        if (m_batchMode <= 0 && ! m_batchedValues.empty())
        {
            qint64 timestamp = QDateTime::currentMSecsSinceEpoch();
            forEachIntersection(m_elements, m_batchedValues, [timestamp](const QString &, const ElementPtr & element, CVariantMap::const_iterator it)
            {
                Q_ASSERT(isSafeToIncrement(element->m_pendingChanges));
                element->m_pendingChanges++;
                element->m_value.uniqueWrite() = it.value();
                element->m_timestamp = timestamp;
                element->m_saved = false;
            });
            emit valuesWantToCache({ m_batchedValues, timestamp });
        }
    }

    CStatusMessage CValuePage::validate(const Element &element, const CVariant &value, CStatusMessage::StatusSeverity invalidSeverity) const
    {
        if (! value.isValid())
        {
            return CStatusMessage(this, invalidSeverity, "Empty cache value " + element.m_key, true);
        }
        else if (value.userType() != element.m_metaType)
        {
            return CStatusMessage(this, CStatusMessage::SeverityError, QString("Expected ") + QMetaType::typeName(element.m_metaType) + " but got " + value.typeName() + " for " + element.m_key, true);
        }
        else if (element.m_validator && ! element.m_validator(value))
        {
            return CStatusMessage(this, CStatusMessage::SeverityError, value.toQString() + " is not valid for " + element.m_key, true);
        }
        else
        {
            return {};
        }
    }

}

//! \endcond
