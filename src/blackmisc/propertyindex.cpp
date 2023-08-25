// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/propertyindex.h"
#include "blackmisc/verify.h"
#include <QStringList>
#include <QtGlobal>

namespace BlackMisc
{
    CPropertyIndex::CPropertyIndex(int singleProperty) : m_indexes { singleProperty }
    {
        Q_ASSERT(singleProperty >= static_cast<int>(CPropertyIndexRef::GlobalIndexCValueObject));
    }

    CPropertyIndex::CPropertyIndex(std::initializer_list<int> il) : m_indexes(il)
    {}

    CPropertyIndex::CPropertyIndex(const QVector<int> &indexes) : m_indexes(indexes)
    {}

    CPropertyIndex::CPropertyIndex(const QList<int> &indexes) : m_indexes(indexes.toVector())
    {}

    CPropertyIndex::CPropertyIndex(const QString &indexes)
    {
        this->parseFromString(indexes);
    }

    CPropertyIndex::operator CPropertyIndexRef() const
    {
        return CPropertyIndexRef(m_indexes);
    }

    CPropertyIndex CPropertyIndex::copyFrontRemoved() const
    {
        BLACK_VERIFY_X(!this->isEmpty(), Q_FUNC_INFO, "Empty index");
        if (this->isEmpty()) { return CPropertyIndex(); }
        CPropertyIndex copy = *this;
        copy.m_indexes.pop_front();
        return copy;
    }

    bool CPropertyIndex::isNested() const
    {
        return m_indexes.size() > 1;
    }

    bool CPropertyIndex::isMyself() const
    {
        return m_indexes.isEmpty();
    }

    bool CPropertyIndex::isEmpty() const
    {
        return m_indexes.isEmpty();
    }

    QString CPropertyIndex::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        QString s;
        for (const int i : m_indexes)
        {
            Q_ASSERT(i >= static_cast<int>(CPropertyIndexRef::GlobalIndexCValueObject));
            if (!s.isEmpty()) { s.append(";"); }
            s.append(QString::number(i));
        }
        return s;
    }

    void CPropertyIndex::parseFromString(const QString &indexes)
    {
        m_indexes.clear();
        if (indexes.isEmpty()) { return; }
        for (const QStringRef &index : indexes.splitRef(';'))
        {
            if (index.isEmpty()) { continue; }
            bool ok;
            int i = index.toInt(&ok);
            Q_ASSERT(ok);
            Q_ASSERT(i >= static_cast<int>(CPropertyIndexRef::GlobalIndexCValueObject));
            m_indexes.append(i);
        }
    }

    QJsonObject CPropertyIndex::toJson() const
    {
        QJsonObject json;
        json.insert(QStringLiteral("indexes"), this->convertToQString());
        return json;
    }

    void CPropertyIndex::convertFromJson(const QJsonObject &json)
    {
        const QJsonValue value = json.value(QLatin1String("indexes"));
        if (!value.isString()) { throw CJsonException("'indexes' missing or not a string"); }
        this->parseFromString(value.toString());
    }

    QVector<int> CPropertyIndex::indexVector() const
    {
        return m_indexes;
    }

    QList<int> CPropertyIndex::indexList() const
    {
        return m_indexes.toList();
    }

    void CPropertyIndex::prepend(int newLeftIndex)
    {
        m_indexes.push_front(newLeftIndex);
    }

    bool CPropertyIndex::contains(int index) const
    {
        return m_indexes.contains(index);
    }

    int CPropertyIndex::frontToInt() const
    {
        Q_ASSERT_X(!this->isEmpty(), Q_FUNC_INFO, "No index");
        return m_indexes.front();
    }

    bool CPropertyIndex::startsWith(int index) const
    {
        if (this->isEmpty()) { return false; }
        return this->frontToInt() == index;
    }
} // namespace
