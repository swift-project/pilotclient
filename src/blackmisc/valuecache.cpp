/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \cond PRIVATE

#include "blackmisc/valuecache.h"
#include "blackmisc/atomicfile.h"
#include "blackmisc/swiftdirectories.h"
#include "blackmisc/directoryutils.h"
#include "blackmisc/identifier.h"
#include "blackmisc/lockfree.h"
#include "blackmisc/logcategories.h"
#include "blackmisc/logmessage.h"

#include <QByteArray>
#include <QCoreApplication>
#include <QDBusMetaType>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QFlags>
#include <QIODevice>
#include <QJsonDocument>
#include <QList>
#include <QMetaMethod>
#include <QMutexLocker>
#include <QStandardPaths>
#include <QThread>
#include <Qt>
#include <algorithm>
#include <atomic>
#include <exception>
#include <functional>
#include <limits>

namespace BlackMisc
{

    using Private::CValuePage;

    //! Used in asserts to protect against signed integer overflow.
    template <typename T>
    bool isSafeToIncrement(const T &value) { return value < std::numeric_limits<T>::max(); }

    //! \private
    std::pair<QString &, std::atomic<bool> &> getCacheRootDirectoryMutable()
    {
        static QString dir = CSwiftDirectories::normalizedApplicationDataDirectory();
        static std::atomic<bool> frozen { false };
        return { dir, frozen };
    }

    void setMockCacheRootDirectory(const QString &dir)
    {
        Q_ASSERT_X(! getCacheRootDirectoryMutable().second, Q_FUNC_INFO, "Too late to call this function");
        getCacheRootDirectoryMutable().first = dir;
    }

    const QString &CValueCache::getCacheRootDirectory()
    {
        getCacheRootDirectoryMutable().second = true;
        return getCacheRootDirectoryMutable().first;
    }


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

    QString CValueCachePacket::toTimestampMapString(const QStringList &keys) const
    {
        QStringList result;
        for (const auto &key : keys)
        {
            QString time = contains(key) ? QDateTime::fromMSecsSinceEpoch(value(key).second, Qt::UTC).toString(Qt::ISODate) : "no timestamp";
            result.push_back(key + " (" + time + ")");
        }
        return result.join(",");
    }

    void CValueCachePacket::setTimestamps(const QMap<QString, qint64> &times)
    {
        for (auto it = times.cbegin(); it != times.cend(); ++it)
        {
            if (! contains(it.key())) { continue; }
            (*this)[it.key()].second = it.value();
        }
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
        qRegisterMetaTypeStreamOperators<value_type>();
    }

    ////////////////////////////////
    // CValueCache
    ////////////////////////////////

    const QStringList &CValueCache::getLogCategories()
    {
        static const QStringList cats({ "swift.valuecache" , CLogCategories::services()} );
        return cats;
    }

    CValueCache::CValueCache(int fileSplitDepth, QObject *parent) : QObject(parent), m_fileSplitDepth(fileSplitDepth)
    {
        Q_ASSERT_X(fileSplitDepth >= 0, Q_FUNC_INFO, "Negative value not allowed, use 0 for maximum split depth");
        Q_ASSERT_X(QThread::currentThread() == qApp->thread(), Q_FUNC_INFO, "Cache constructed in wrong thread");
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
        return getElement(key, std::as_const(m_elements).lowerBound(key));
    }

    CValueCache::Element &CValueCache::getElement(const QString &key, QMap<QString, ElementPtr>::const_iterator pos)
    {
        QMutexLocker lock(&m_mutex);
        if (pos != m_elements.cend() && pos.key() == key) { return **pos; }
        Q_ASSERT(pos == std::as_const(m_elements).lowerBound(key));
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
            if (! element->m_value.isValid()) { continue; }
            implementationOf(map).insert(map.cend(), element->m_key, element->m_value);
        }
        return map;
    }

    CVariantMap CValueCache::getAllValues(const QStringList &keys) const
    {
        QMutexLocker lock(&m_mutex);
        CVariantMap map;
        for (const auto &key : keys)
        {
            auto it = m_elements.constFind(key);
            if (it == m_elements.cend()) { continue; }
            if (! (*it)->m_value.isValid()) { continue; }
            map.insert(key, (*it)->m_value);
        }
        return map;
    }

    CValueCachePacket CValueCache::getAllValuesWithTimestamps(const QString &keyPrefix) const
    {
        QMutexLocker lock(&m_mutex);
        CValueCachePacket map;
        for (const auto &element : elementsStartingWith(keyPrefix))
        {
            if (! element->m_value.isValid()) { continue; }
            map.insert(element->m_key, element->m_value, element->m_timestamp);
        }
        return map;
    }

    QStringList CValueCache::getAllUnsavedKeys(const QString &keyPrefix) const
    {
        QMutexLocker lock(&m_mutex);
        QStringList keys;
        for (const auto &element : elementsStartingWith(keyPrefix))
        {
            if (element->m_value.isValid() && ! element->m_saved) { keys.push_back(element->m_key); }
        }
        return keys;
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
        m_elements.detach(); //! \fixme see http://doc.qt.io/qt-5/containers.html#implicit-sharing-iterator-problem
        auto out = std::as_const(m_elements).lowerBound(values.cbegin().key());
        auto end = std::as_const(m_elements).upperBound((values.cend() - 1).key());
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

        if (! isSignalConnected(QMetaMethod::fromSignal(&CValueCache::valuesChangedByLocal)))
        {
            changeValuesFromRemote(values, CIdentifier());
        }
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
        CValueCachePacket ackedChanges(values.isSaved());
        m_elements.detach(); //! \fixme see http://doc.qt.io/qt-5/containers.html#implicit-sharing-iterator-problem
        auto out = std::as_const(m_elements).lowerBound(values.cbegin().key());
        auto end = std::as_const(m_elements).upperBound((values.cend() - 1).key());
        for (auto in = values.cbegin(); in != values.cend(); ++in)
        {
            while (out != end && out.key() < in.key()) { ++out; }
            auto &element = getElement(in.key(), out);

            if (originator.hasApplicationProcessId()) // round trip
            {
                element.m_pendingChanges--;
                Q_ASSERT(element.m_pendingChanges >= 0);
                ackedChanges.insert(in.key(), in.value(), in.timestamp());
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
        if (! ackedChanges.empty() && ackedChanges.isSaved()) { emit valuesSaveRequested(ackedChanges); }
    }

    QJsonObject CValueCache::saveToJson(const QString &keyPrefix) const
    {
        return getAllValues(keyPrefix).toMemoizedJson();
    }

    void CValueCache::loadFromJson(const QJsonObject &json)
    {
        CVariantMap map;
        map.convertFromMemoizedJson(json);
        if (! map.isEmpty()) { insertValues({ map, QDateTime::currentMSecsSinceEpoch() }); }
    }

    CStatusMessageList CValueCache::loadFromJsonNoThrow(const QJsonObject &json, const CLogCategoryList &categories, const QString &prefix)
    {
        CVariantMap map;
        auto messages = map.convertFromMemoizedJsonNoThrow(json, categories, prefix);
        if (! map.isEmpty()) { insertValues({ map, QDateTime::currentMSecsSinceEpoch() }); }
        return messages;
    }

    CStatusMessage CValueCache::saveToFiles(const QString &dir, const QString &keyPrefix)
    {
        QMutexLocker lock(&m_mutex);
        auto values = getAllValues(keyPrefix);
        auto status = saveToFiles(dir, values);
        if (status.isSuccess()) { markAllAsSaved(keyPrefix); }
        return status;
    }

    CStatusMessage CValueCache::saveToFiles(const QString &dir, const QStringList &keys)
    {
        QMutexLocker lock(&m_mutex);
        auto values = getAllValues(keys);
        auto status = saveToFiles(dir, values);
        if (status.isSuccess()) { markAllAsSaved(keys); }
        return status;
    }

    CStatusMessage CValueCache::saveToFiles(const QString &dir, const CVariantMap &values, const QString &keysMessage) const
    {
        QMap<QString, CVariantMap> namespaces;
        for (auto it = values.cbegin(); it != values.cend(); ++it)
        {
            namespaces[it.key().section('/', 0, m_fileSplitDepth - 1)].insert(it.key(), it.value());
        }
        if (! QDir::root().mkpath(dir))
        {
            return CStatusMessage(this).error(u"Failed to create directory '%1'") << dir;
        }
        for (auto it = namespaces.cbegin(); it != namespaces.cend(); ++it)
        {
            CAtomicFile file(dir + "/" + it.key() + ".json");
            if (! QDir::root().mkpath(QFileInfo(file).path()))
            {
                return CStatusMessage(this).error(u"Failed to create directory '%1'") << QFileInfo(file).path();
            }
            if (! file.open(QFile::ReadWrite | QFile::Text))
            {
                return CStatusMessage(this).error(u"Failed to open %1: %2") << file.fileName() << file.errorString();
            }
            auto json = QJsonDocument::fromJson(file.readAll());
            if (json.isArray() || (json.isNull() && ! json.isEmpty()))
            {
                return CStatusMessage(this).error(u"Invalid JSON format in %1") << file.fileName();
            }
            auto object = json.object();
            json.setObject(it->mergeToMemoizedJson(object));

            if (!(file.seek(0) && file.resize(0) && file.write(json.toJson()) > 0 && file.checkedClose()))
            {
                return CStatusMessage(this).error(u"Failed to write to %1: %2") << file.fileName() << file.errorString();
            }
        }
        return CStatusMessage(this).info(u"Written '%1' to value cache in '%2'") <<
            (keysMessage.isEmpty() ? values.keys().to<QStringList>().join(",") : keysMessage) << dir;
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

    CStatusMessage CValueCache::loadFromFiles(const QString &dir, const QSet<QString> &keys, const CVariantMap &currentValues, CValueCachePacket &o_values, const QString &keysMessage, bool keysOnly) const
    {
        if (! QDir(dir).exists())
        {
            return CStatusMessage(this).warning(u"No such directory '%1'") << dir;
        }
        if (! QDir(dir).isReadable())
        {
            return CStatusMessage(this).error(u"Failed to read from directory '%1'") << dir;
        }

        QMap<QString, QStringList> keysInFiles;
        for (const auto &key : keys)
        {
            keysInFiles[key.section('/', 0, m_fileSplitDepth - 1) + ".json"].push_back(key);
        }
        if (keys.isEmpty())
        {
            QDirIterator iter(dir, { "*.json" }, QDir::Files, QDirIterator::Subdirectories);
            while (iter.hasNext())
            {
                keysInFiles.insert(QDir(dir).relativeFilePath(iter.next()), {});
            }
        }
        bool ok = true;
        for (auto it = keysInFiles.cbegin(); it != keysInFiles.cend(); ++it)
        {
            QFile file(QDir(dir).absoluteFilePath(it.key()));
            if (! file.exists())
            {
                continue;
            }
            if (! file.open(QFile::ReadOnly | QFile::Text))
            {
                return CStatusMessage(this).error(u"Failed to open %1: %2") << file.fileName() << file.errorString();
            }
            auto json = QJsonDocument::fromJson(file.readAll());
            if (json.isArray() || (json.isNull() && ! json.isEmpty()))
            {
                return CStatusMessage(this).error(u"Invalid JSON format in %1") << file.fileName();
            }

            CVariantMap temp;
            if (keysOnly)
            {
                for (const auto &key : json.object().keys()) { temp.insert(key, {}); } // clazy:exclude=range-loop
            }
            else
            {
                const QString messagePrefix = QStringLiteral("Parsing %1").arg(it.key());
                auto messages = temp.convertFromMemoizedJsonNoThrow(json.object(), it.value(), this, messagePrefix);
                if (it.value().isEmpty()) { messages.push_back(temp.convertFromMemoizedJsonNoThrow(json.object(), this, messagePrefix)); }
                if (! messages.isEmpty())
                {
                    ok = false;
                    backupFile(file);
                    CLogMessage::preformatted(messages);
                }
            }
            temp.removeDuplicates(currentValues);
            o_values.insert(temp, QFileInfo(file).lastModified().toMSecsSinceEpoch());
        }
        return CStatusMessage(this).info(u"Loaded cache values '%1' from '%2' '%3'") <<
            (keysMessage.isEmpty() ? o_values.keys().to<QStringList>().join(",") : keysMessage) << dir << (ok ? "successfully" : "with errors");
    }

    void CValueCache::backupFile(QFile &file) const
    {
        QDir dir = getCacheRootDirectory();
        QString relative = "backups/" + dir.relativeFilePath(file.fileName());
        QString absolute = dir.absoluteFilePath(relative);
        absolute += "." + QDateTime::currentDateTime().toString(QStringLiteral("yyMMddhhmmss"));
        if (QFile::exists(absolute)) { return; }
        if (! dir.mkpath(QFileInfo(relative).path()))
        {
            CLogMessage(this).error(u"Failed to create %1") << QFileInfo(absolute).path();
            return;
        }
        if (! file.copy(absolute))
        {
            CLogMessage(this).error(u"Failed to back up %1: %2") << QFileInfo(file).fileName() << file.errorString();
            return;
        }
        CLogMessage(this).info(u"Backed up %1 to %2") << QFileInfo(file).fileName() << dir.absoluteFilePath("backups");
    }

    void CValueCache::markAllAsSaved(const QString &keyPrefix)
    {
        QMutexLocker lock(&m_mutex);
        for (const auto &element : elementsStartingWith(keyPrefix))
        {
            element->m_saved = true;
        }
    }

    void CValueCache::markAllAsSaved(const QStringList &keys)
    {
        QMutexLocker lock(&m_mutex);
        for (const auto &key : keys)
        {
            getElement(key).m_saved = true;
        }
    }

    QString CValueCache::filenameForKey(const QString &key) const
    {
        return key.section('/', 0, m_fileSplitDepth - 1) + ".json";
    }

    QStringList CValueCache::enumerateFiles(const QString &dir) const
    {
        auto values = getAllValues();
        QSet<QString> files;
        for (auto it = values.begin(); it != values.end(); ++it) { files.insert(dir + "/" + filenameForKey(it.key())); }
        return files.values();
    }

    void CValueCache::clearAllValues(const QString &keyPrefix)
    {
        QMutexLocker lock(&m_mutex);
        auto values = getAllValues(keyPrefix);
        for (auto it = values.begin(); it != values.end(); ++it) { it.value() = CVariant(); }
        changeValues({ values, 0 });
    }

    QString CValueCache::getHumanReadableName(const QString &key) const
    {
        QMutexLocker lock(&m_mutex);
        return m_humanReadable.value(key, key);
    }

    QString CValueCache::getHumanReadableWithKey(const QString &key) const
    {
        QMutexLocker lock(&m_mutex);
        QString hr = m_humanReadable.value(key);
        return hr.isEmpty() ? key : QStringLiteral("%1 (%2)").arg(hr, key);
    }

    void CValueCache::setHumanReadableName(const QString &key, const QString &name)
    {
        QMutexLocker lock(&m_mutex);
        if (! m_humanReadable.contains(key)) { m_humanReadable.insert(key, name); }
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
            if (std::uncaught_exceptions() > 0) { m_page->abandonBatch(); }
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

    const QStringList &CValuePage::getLogCategories()
    {
        return CValueCache::getLogCategories();
    }

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
        Element(const QString &key, const QString &name, int metaType, const Validator &validator, const CVariant &defaultValue) :
            m_key(key), m_name(name), m_metaType(metaType), m_validator(validator), m_default(defaultValue)
        {}
        const QString m_key;
        const QString m_name;
        const QString m_nameWithKey = m_name.isEmpty() ? m_key : QStringLiteral("%1 (%2)").arg(m_name, m_key);
        LockFree<CVariant> m_value;
        std::atomic<qint64> m_timestamp { 0 };
        const int m_metaType = QMetaType::UnknownType;
        const Validator m_validator;
        const CVariant m_default;
        NotifySlot m_notifySlot;
        int m_pendingChanges = 0;
        bool m_saved = false;
    };

    CValuePage::Element &CValuePage::createElement(const QString &keyTemplate, const QString &name, int metaType, const Validator &validator, const CVariant &defaultValue)
    {
        if (parent()->objectName().isEmpty() && keyTemplate.contains("%OwnerName%"))
        {
            static Element dummy("", "", QMetaType::UnknownType, nullptr, {});
            return dummy;
        }

        QString key = keyTemplate;
        key.replace("%Application%", QFileInfo(QCoreApplication::applicationFilePath()).completeBaseName(), Qt::CaseInsensitive);
        key.replace("%OwnerClass%", QString(parent()->metaObject()->className()).replace("::", "/"), Qt::CaseInsensitive);
        key.replace("%OwnerName%", parent()->objectName(), Qt::CaseInsensitive);

        QString unused;
        Q_ASSERT_X(! m_elements.contains(key), "CValuePage", "Can't have two CCached in the same object referring to the same value");
        Q_ASSERT_X(defaultValue.isValid() ? defaultValue.userType() == metaType : true, "CValuePage", "Metatype mismatch for default value");
        Q_ASSERT_X(defaultValue.isValid() && validator ? validator(defaultValue, unused) : true, "CValuePage", "Validator rejects default value");
        Q_UNUSED(unused)

        auto &element = *(m_elements[key] = ElementPtr(new Element(key, name, metaType, validator, defaultValue)));
        std::forward_as_tuple(element.m_value.uniqueWrite(), element.m_timestamp, element.m_saved) = m_cache->getValue(key);

        auto status = validate(element, element.m_value.read(), CStatusMessage::SeverityDebug);
        if (!status.isEmpty()) // intentionally kept !empty here, debug message supposed to write default value
        {
            element.m_value.uniqueWrite() = defaultValue;

            if (status.getSeverity() == CStatusMessage::SeverityDebug)
            {
                QMutexLocker lock(&m_cache->m_warnedKeysMutex);
                if (! m_cache->m_warnedKeys.contains(key))
                {
                    m_cache->m_warnedKeys.insert(key);
                    CLogMessage::preformatted(status);
                }
            }
            else
            {
                CLogMessage::preformatted(status);
            }
        }

        return element;
    }

    void CValuePage::setNotifySlot(Element &element, const NotifySlot &slot)
    {
        element.m_notifySlot = slot;
    }

    bool CValuePage::isInitialized(const Element &element) const
    {
        return ! element.m_key.isEmpty();
    }

    bool CValuePage::isValid(const Element &element, int typeId) const
    {
        auto reader = element.m_value.read();
        return reader->isValid() && reader->userType() == typeId;
    }

    const CVariant &CValuePage::getValue(const Element &element) const
    {
        Q_ASSERT_X(! element.m_key.isEmpty(), Q_FUNC_INFO, "Empty key suggests an attempt to use value before objectName available for %%OwnerName%%");
        Q_ASSERT(QThread::currentThread() == thread());

        return element.m_value.read();
    }

    CVariant CValuePage::getValueCopy(const Element &element) const
    {
        Q_ASSERT_X(! element.m_key.isEmpty(), Q_FUNC_INFO, "Empty key suggests an attempt to use value before objectName available for %%OwnerName%%");
        return element.m_value.read();
    }

    CStatusMessage CValuePage::setValue(Element &element, CVariant value, qint64 timestamp, bool save, bool ignoreValue)
    {
        Q_ASSERT_X(! element.m_key.isEmpty(), Q_FUNC_INFO, "Empty key suggests an attempt to use value before objectName available for %%OwnerName%%");
        Q_ASSERT(QThread::currentThread() == thread());

        if (timestamp == 0) { timestamp = QDateTime::currentMSecsSinceEpoch(); }
        bool changed = element.m_timestamp != timestamp || element.m_value.read() != value;
        if (! changed && ! save && ! ignoreValue)
        {
            return CStatusMessage(this).info(u"Value '%1' not set, same timestamp and value") << element.m_nameWithKey;
        }

        if (ignoreValue) { value = element.m_value.read(); }
        else { ignoreValue = ! changed; }

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

                element.m_timestamp = timestamp;
                element.m_value.uniqueWrite() = value;
                emit valuesWantToCache({ { { element.m_key, value } }, timestamp, save });
            }
            // All good info
            status = CStatusMessage(this).info(u"Set value '%1'") << element.m_nameWithKey;
        }
        return status;
    }

    const QString &CValuePage::getKey(const Element &element) const
    {
        return element.m_key;
    }

    qint64 CValuePage::getTimestamp(const Element &element) const
    {
        Q_ASSERT_X(! element.m_key.isEmpty(), Q_FUNC_INFO, "Empty key suggests an attempt to use value before objectName available for %%OwnerName%%");
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

        CSequence<NotifySlot *> notifySlots;

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
                    if (element->m_notifySlot.first && (! element->m_notifySlot.second || ! notifySlots.containsBy([ & ](auto slot) { return slot->second == element->m_notifySlot.second; })))
                    {
                        notifySlots.push_back(&element->m_notifySlot);
                    }
                }
                else
                {
                    CLogMessage::preformatted(error);
                }
            }
        });

        for (auto slot : notifySlots) { slot->first(parent()); }
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
        QString reason;
        if (! value.isValid())
        {
            return CStatusMessage(this, invalidSeverity, u"Empty cache value %1", true) << element.m_nameWithKey;
        }
        else if (value.userType() != element.m_metaType)
        {
            return CStatusMessage(this).error(u"Expected %1 but got %2 for %3") << QMetaType::typeName(element.m_metaType) << value.typeName() << element.m_nameWithKey;
        }
        else if (element.m_validator && ! element.m_validator(value, reason))
        {
            if (reason.isEmpty())
            {
                return CStatusMessage(this).error(u"%1 is not valid for %2") << value.toQString() << element.m_nameWithKey;
            }
            else
            {
                return CStatusMessage(this).error(u"%1 (%2 for %3)") << reason << value.toQString() << element.m_nameWithKey;
            }
        }
        else
        {
            return {};
        }
    }

}

//! \endcond
