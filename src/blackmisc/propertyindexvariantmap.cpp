// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \cond PRIVATE

#include "blackmisc/propertyindexvariantmap.h"
#include "blackmisc/dictionary.h"

#include <QHash>

namespace BlackMisc
{
    CPropertyIndexVariantMap::CPropertyIndexVariantMap(bool wildcard) : m_wildcard(wildcard) {}

    CPropertyIndexVariantMap::CPropertyIndexVariantMap(const CPropertyIndex &index, const CVariant &value)
        : m_wildcard(false)
    {
        this->addValue(index, value);
    }

    bool operator==(const CPropertyIndexVariantMap &a, const CPropertyIndexVariantMap &b)
    {
        return a.m_wildcard == b.m_wildcard && a.m_values == b.m_values;
    }

    bool operator!=(const CPropertyIndexVariantMap &a, const CPropertyIndexVariantMap &b)
    {
        return !(b == a);
    }

    bool CPropertyIndexVariantMap::matchesVariant(const CVariant &variant) const
    {
        if (this->isEmpty()) { return this->isWildcard(); }
        const auto &map = this->map();
        for (auto it = map.begin(); it != map.end(); ++it)
        {
            // QVariant cannot be compared directly
            const CVariant p = variant.propertyByIndex(it.key()); // from value object
            const CVariant v = it.value(); // from map
            if (p != v) return false;
        }
        return true;
    }

    QString CPropertyIndexVariantMap::convertToQString(bool i18n) const
    {
        if (this->isEmpty()) return QStringLiteral("{wildcard: %1}").arg(m_wildcard ? "true" : "false");
        QString s;
        for (const CPropertyIndex &index : makeKeysRange(m_values))
        {
            CVariant v = m_values.value(index);

            s.isEmpty() ?
                s.append("{wildcard: ").append(m_wildcard ? "true" : "false").append(" ") :
                s.append(", ");

            s.append('{').append(index.toQString(i18n)).append(": ");
            s.append("(").append(QString::number(v.userType())).append(") ");
            QString vs = v.toQString(i18n);
            s.append(vs);
            s.append('}');
        }
        s = s.append("}");
        return s;
    }

    void CPropertyIndexVariantMap::marshallToDbus(QDBusArgument &argument) const
    {
        argument << m_values.keys();
        argument << m_values.values();
    }

    void CPropertyIndexVariantMap::unmarshallFromDbus(const QDBusArgument &argument)
    {
        QList<CPropertyIndex> indexes;
        QList<CVariant> values;
        argument >> indexes;
        argument >> values;
        Q_ASSERT(indexes.size() == values.size());
        QMap<CPropertyIndex, CVariant> newMap;
        for (int i = 0; i < indexes.size(); i++)
        {
            newMap.insert(indexes[i], values[i]);
        }
        // replace values in one step
        m_values.swap(newMap);
    }

    void CPropertyIndexVariantMap::marshalToDataStream(QDataStream &stream) const
    {
        stream << m_values.keys();
        stream << m_values.values();
    }

    void CPropertyIndexVariantMap::unmarshalFromDataStream(QDataStream &stream)
    {
        QList<CPropertyIndex> indexes;
        QList<CVariant> values;
        stream >> indexes;
        stream >> values;
        Q_ASSERT(indexes.size() == values.size());
        QMap<CPropertyIndex, CVariant> newMap;
        for (int i = 0; i < indexes.size(); i++)
        {
            newMap.insert(indexes[i], values[i]);
        }
        // replace values in one step
        m_values.swap(newMap);
    }

    void CPropertyIndexVariantMap::addValue(const CPropertyIndex &index, const CVariant &value)
    {
        m_values.insert(index, value);
    }

    void CPropertyIndexVariantMap::addValue(const CPropertyIndex &index, const char *str)
    {
        this->addValue(index, QString(str));
    }

    void CPropertyIndexVariantMap::prependIndex(int index)
    {
        QMap<CPropertyIndex, CVariant> newMap;
        for (const CPropertyIndex &pi : this->indexes())
        {
            CPropertyIndex newPi(pi);
            newPi.prepend(index);
            newMap.insert(newPi, m_values[pi]);
        }
        m_values = newMap;
    }

    CPropertyIndexList CPropertyIndexVariantMap::indexes() const
    {
        return m_values.keys();
    }

    int CPropertyIndexVariantMap::size() const
    {
        return m_values.size();
    }

    uint CPropertyIndexVariantMap::getValueHash() const
    {
        // there is no hash for map, so I use this workaround here
        return qHash(this->toQString(false));
    }
} // namespace

//! \endcond
