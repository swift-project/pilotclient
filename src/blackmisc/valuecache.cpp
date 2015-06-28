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
#include <QThread>

namespace BlackMisc
{

    using Private::CValuePage;

    // Used in asserts to protect against signed integer overflow.
    template <typename T>
    bool isSafeToIncrement(const T &value) { return value < std::numeric_limits<T>::max(); }


    ////////////////////////////////
    // CValueCache
    ////////////////////////////////

    CValueCache::CValueCache(CValueCache::DistributionMode mode, QObject *parent) :
        QObject(parent)
    {
        if (mode == LocalOnly)
        {
            // loopback signal to own slot for local operation
            connect(this, &CValueCache::valuesChangedByLocal, this, [ = ](const CVariantMap &values)
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

    CVariant CValueCache::getValue(const QString &key)
    {
        QMutexLocker lock(&m_mutex);
        return getElement(key).m_value;
    }

    CVariantMap CValueCache::getAllValues(const QString &keyPrefix) const
    {
        QMutexLocker lock(&m_mutex);
        auto begin = m_elements.lowerBound(keyPrefix);
        auto end = m_elements.lowerBound(keyPrefix + QChar(QChar::LastValidCodePoint));
        CVariantMap map;
        for (auto it = begin; it != end; ++it) { implementationOf(map).insert(map.cend(), it.key(), it.value()->m_value); }
        return map;
    }

    void CValueCache::insertValues(const CVariantMap &values)
    {
        QMutexLocker lock(&m_mutex);
        changeValues(values);
    }

    void CValueCache::changeValues(const CVariantMap &values)
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
        }
        emit valuesChanged(values, sender());
        emit valuesChangedByLocal(values);
    }

    void CValueCache::changeValuesFromRemote(const CVariantMap &values, const CIdentifier &originator)
    {
        QMutexLocker lock(&m_mutex);
        if (values.empty()) { return; }
        CVariantMap ratifiedChanges;
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
                ratifiedChanges.insert(in.key(), in.value());
            }
        }
        if (! ratifiedChanges.empty())
        {
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
        insertValues(map);
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
        CVariant m_value;
        const int m_metaType = QMetaType::UnknownType;
        const Validator m_validator;
        const CVariant m_default;
        const NotifySlot m_notifySlot = nullptr;
        int m_pendingChanges = 0;
    };

    CValuePage::Element &CValuePage::createElement(const QString &key, int metaType, Validator validator, const CVariant &defaultValue, NotifySlot slot)
    {
        Q_ASSERT_X(! m_elements.contains(key), "CValuePage", "Can't have two CCached in the same object referring to the same value");
        Q_ASSERT_X(defaultValue.isValid() ? defaultValue.userType() == metaType : true, "CValuePage", "Metatype mismatch for default value");
        Q_ASSERT_X(defaultValue.isValid() && validator ? validator(defaultValue) : true, "CValuePage", "Validator rejects default value");

        auto &element = *(m_elements[key] = ElementPtr(new Element(key, metaType, validator, defaultValue, slot)));
        element.m_value = m_cache->getValue(key);

        auto error = validate(element, element.m_value);
        if (! error.isEmpty()) { element.m_value = defaultValue; }
        return element;
    }

    const CVariant &CValuePage::getValue(const Element &element) const
    {
        Q_ASSERT(QThread::currentThread() == thread());

        return element.m_value;
    }

    CStatusMessage CValuePage::setValue(Element &element, const CVariant &value)
    {
        Q_ASSERT(QThread::currentThread() == thread());

        if (element.m_value == value) { return {}; }

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

                element.m_value = value;
                emit valuesWantToCache({ { element.m_key, value } });
            }
        }
        return error;
    }

    void CValuePage::setValuesFromCache(const CVariantMap &values, QObject *changedBy)
    {
        Q_ASSERT(QThread::currentThread() == thread());

        QList<NotifySlot> notifySlots;

        forEachIntersection(m_elements, values, [changedBy, this, &notifySlots](const QString &, const ElementPtr &element, const CVariant &value)
        {
            if (changedBy == this) // round trip
            {
                element->m_pendingChanges--;
                Q_ASSERT(element->m_pendingChanges >= 0);
            }
            else if (element->m_pendingChanges == 0) // ratify a change only if own change is not pending, to ensure consistency
            {
                auto error = validate(*element, value);
                if (error.isEmpty())
                {
                    element->m_value = value;
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
            forEachIntersection(m_elements, m_batchedValues, [](const QString &, const ElementPtr &element, const CVariant &value)
            {
                Q_ASSERT(isSafeToIncrement(element->m_pendingChanges));
                element->m_pendingChanges++;
                element->m_value = value;
            });
            emit valuesWantToCache(m_batchedValues);
        }
    }

    CStatusMessage CValuePage::validate(const Element &element, const CVariant &value) const
    {
        if (! value.isValid())
        {
            return CLogMessage(this).warning("Uninitialized value for %1") << element.m_key;
        }
        else if (value.userType() != element.m_metaType)
        {
            return CLogMessage(this).error("Expected %1, got %2 for %3") << QMetaType::typeName(element.m_metaType) << value.typeName() << element.m_key;
        }
        else if (element.m_validator && ! element.m_validator(value))
        {
            return CLogMessage(this).error("%1 is not valid for %2") << value << element.m_key;
        }
        else
        {
            return {};
        }
    }

}
