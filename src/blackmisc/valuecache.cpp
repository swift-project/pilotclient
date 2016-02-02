/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/valuecache.h"
#include "blackmisc/identifier.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/algorithm.h"
#include "blackmisc/lockfree.h"
#include "blackmisc/atomicfile.h"
#include <QThread>
#include <QJsonDocument>

namespace BlackMisc
{

    using Private::CValuePage;

    // Used in asserts to protect against signed integer overflow.
    template <typename T>
    bool isSafeToIncrement(const T &value) { return value < std::numeric_limits<T>::max(); }


    ////////////////////////////////
    // CValueCachePacket
    ////////////////////////////////

    CValueCachePacket::CValueCachePacket(const CVariantMap &values, qint64 timestamp, bool saved) :
        m_saved(saved)
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

    void CValueCachePacket::registerMetadata()
    {
        MetaType::registerMetadata();
        qDBusRegisterMetaType<value_type>();
    }

    ////////////////////////////////
    // CValueCache
    ////////////////////////////////

    CValueCache::CValueCache(CValueCache::DistributionMode mode, QObject *parent) :
        QObject(parent)
    {
        if (mode == LocalOnly)
        {
            // loopback signal to own slot for local operation
            connect(this, &CValueCache::valuesChangedByLocal, this, [ = ](const CValueCachePacket &values)
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
        std::atomic<qint64> m_timestamp { QDateTime::currentMSecsSinceEpoch() };
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

            Q_ASSERT(isSafeToIncrement(element.m_pendingChanges));
            element.m_pendingChanges++;
            element.m_value = in.value();
            element.m_timestamp = in.timestamp();
            element.m_saved = values.isSaved();
        }
        emit valuesChanged(values, sender());
        emit valuesChangedByLocal(values);
    }

    void CValueCache::changeValuesFromRemote(const CValueCachePacket &values, const CIdentifier &originator)
    {
        QMutexLocker lock(&m_mutex);
        if (values.empty()) { return; }
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
        if (! status.isEmpty()) { markAllAsSaved(keyPrefix); }
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
            return CLogMessage(this).error("Failed to create directory %1") << dir;
        }
        for (auto it = namespaces.cbegin(); it != namespaces.cend(); ++it)
        {
            QFile readFile(dir + "/" + it.key() + ".json");
            if (! readFile.open(QFile::ReadWrite | QFile::Text))
            {
                return CLogMessage(this).error("Failed to open %1: %2") << readFile.fileName() << readFile.errorString();
            }
            auto json = QJsonDocument::fromJson(readFile.readAll());
            if (json.isArray() || (json.isNull() && ! json.isEmpty()))
            {
                return CLogMessage(this).error("Invalid JSON format in %1") << readFile.fileName();
            }
            CVariantMap storedValues;
            storedValues.convertFromJson(json.object());
            storedValues.insert(*it);
            json.setObject(storedValues.toJson());
            readFile.close();
            CAtomicFile writeFile(readFile.fileName());
            if (! (writeFile.open(QFile::WriteOnly | QFile::Text) && writeFile.write(json.toJson()) > 0 && writeFile.checkedClose()))
            {
                return CLogMessage(this).error("Failed to write to %1: %2") << writeFile.fileName() << writeFile.errorString();
            }
        }
        return {};
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
            return CLogMessage(this).error("Failed to read directory %1") << dir;
        }
        for (const auto &filename : QDir(dir).entryList({ "*.json" }, QDir::Files))
        {
            QFile file(dir + "/" + filename);
            if (! file.open(QFile::ReadOnly | QFile::Text))
            {
                return CLogMessage(this).error("Failed to open %1 : %2") << file.fileName() << file.errorString();
            }
            auto json = QJsonDocument::fromJson(file.readAll());
            if (json.isArray() || (json.isNull() && ! json.isEmpty()))
            {
                return CLogMessage(this).error("Invalid JSON format in %1") << file.fileName();
            }
            CVariantMap temp;
            temp.convertFromJson(json.object());
            temp.removeByKeyIf([&keys](const QString &key) { return keys.contains(key); }); // TODO optimize by skipping files
            temp.removeDuplicates(currentValues);
            o_values.insert(temp, QFileInfo(file).lastModified().toMSecsSinceEpoch());
        }
        return {};
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
        changeValues({ values, QDateTime::currentMSecsSinceEpoch() });
    }

    CValueCache::BatchGuard CValueCache::batchChanges(QObject *owner)
    {
        Q_ASSERT(QThread::currentThread() == owner->thread());

        auto &page = CValuePage::getPageFor(owner, this);
        page.beginBatch();
        return page;
    }

    CValueCache::BatchGuard::~BatchGuard() Q_DECL_NOEXCEPT
    {
        if (m_page)
        {
            if (std::uncaught_exception()) { m_page->abandonBatch(); }
            else { m_page->endBatch(); }
        }
    }


    ////////////////////////////////
    // Private :: CValuePage
    ////////////////////////////////

    CValuePage::CValuePage(QObject *parent, CValueCache *cache) :
        QObject(parent),
        m_cache(cache)
    {
        connect(this, &CValuePage::valuesWantToCache, cache, &CValueCache::changeValues);
        connect(cache, &CValueCache::valuesChanged, this, &CValuePage::setValuesFromCache);
    }

    CValuePage &CValuePage::getPageFor(QObject *parent, CValueCache *cache)
    {
        auto pages = parent->findChildren<CValuePage *>();
        auto it = std::find_if(pages.cbegin(), pages.cend(), [cache](CValuePage *page) { return page->m_cache == cache; });
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
        std::atomic<qint64> m_timestamp { QDateTime::currentMSecsSinceEpoch() };
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

        auto error = validate(element, element.m_value.read());
        if (! error.isEmpty())
        {
            CLogMessage::preformatted(error);
            element.m_value.uniqueWrite() = defaultValue;
        }
        return element;
    }

    const CVariant &CValuePage::getValue(const Element &element) const
    {
        return element.m_value.read();
    }

    CStatusMessage CValuePage::setValue(Element &element, const CVariant &value, qint64 timestamp, bool save)
    {
        Q_ASSERT(QThread::currentThread() == thread());

        if (timestamp == 0) { timestamp = QDateTime::currentMSecsSinceEpoch(); }
        if (element.m_value.read() == value && element.m_timestamp == timestamp) { return {}; }

        auto error = validate(element, value);
        if (error.isEmpty())
        {
            if (m_batchMode > 0)
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
        }
        else
        {
            CLogMessage::preformatted(error);
        }
        return error;
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

        QList<NotifySlot> notifySlots;

        forEachIntersection(m_elements, values, [changedBy, this, &notifySlots, &values](const QString &, const ElementPtr &element, CValueCachePacket::const_iterator it)
        {
            if (changedBy == this) // round trip
            {
                element->m_pendingChanges--;
                Q_ASSERT(element->m_pendingChanges >= 0);
            }
            else if (element->m_pendingChanges == 0) // ratify a change only if own change is not pending, to ensure consistency
            {
                auto error = validate(*element, it.value());
                if (error.isEmpty())
                {
                    element->m_value.uniqueWrite() = it.value();
                    element->m_timestamp = it.timestamp();
                    element->m_saved = values.isSaved();
                    if (element->m_notifySlot && ! notifySlots.contains(element->m_notifySlot)) { notifySlots.push_back(element->m_notifySlot); }
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
            forEachIntersection(m_elements, m_batchedValues, [timestamp](const QString &, const ElementPtr &element, CVariantMap::const_iterator it)
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

    CStatusMessage CValuePage::validate(const Element &element, const CVariant &value) const
    {
        if (! value.isValid())
        {
            return CStatusMessage(this, CStatusMessage::SeverityDebug, "Uninitialized value for " + element.m_key);
        }
        else if (value.userType() != element.m_metaType)
        {
            return CStatusMessage(this, CStatusMessage::SeverityError, QString("Expected ") + QMetaType::typeName(element.m_metaType) + " but got " + value.typeName() + " for " + element.m_key);
        }
        else if (element.m_validator && ! element.m_validator(value))
        {
            return CStatusMessage(this, CStatusMessage::SeverityError, value.toQString() + " is not valid for " + element.m_key);
        }
        else
        {
            return {};
        }
    }

}
