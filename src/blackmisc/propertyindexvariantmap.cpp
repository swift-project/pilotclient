/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "propertyindexvariantmap.h"
#include "propertyindexlist.h"
#include "blackmiscfreefunctions.h"

namespace BlackMisc
{
    /*
     * Constructor
     */
    CPropertyIndexVariantMap::CPropertyIndexVariantMap(bool wildcard) : m_wildcard(wildcard) {}

    /*
     * Constructor single value
     */
    CPropertyIndexVariantMap::CPropertyIndexVariantMap(const CPropertyIndex &index, const QVariant &value)
        : m_wildcard(false)
    {
        this->addValue(index, value);
    }

    /*
     * ==
     */
    bool CPropertyIndexVariantMap::operator ==(const CPropertyIndexVariantMap &other) const
    {
        return this->m_wildcard == other.m_wildcard && this->m_values == other.m_values;
    }

    /*
     * !=
     */
    bool CPropertyIndexVariantMap::operator !=(const CPropertyIndexVariantMap &other) const
    {
        return !(*this == other);
    }

    /*
     * Convert to string
     */
    QString CPropertyIndexVariantMap::convertToQString(bool i18n) const
    {
        if (this->isEmpty()) return QString("{wildcard: %1}").arg(this->m_wildcard ? "true" : "false");
        QString s;
        foreach(CPropertyIndex index, this->m_values.keys())
        {
            CVariant v = this->m_values.value(index);

            s.isEmpty() ?
            s.append("{wildcard: ").append(this->m_wildcard ? "true" : "false").append(" ") :
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

    /*
     * metaTypeId
     */
    int CPropertyIndexVariantMap::getMetaTypeId() const
    {
        return qMetaTypeId<CPropertyIndexVariantMap>();
    }

    /*
     * is a
     */
    bool CPropertyIndexVariantMap::isA(int metaTypeId) const
    {
        if (metaTypeId == qMetaTypeId<CPropertyIndexVariantMap>()) { return true; }
        return this->CValueObject::isA(metaTypeId);
    }

    /*
     * Compare
     */
    int CPropertyIndexVariantMap::compareImpl(const CValueObject &/*otherBase*/) const
    {
        qFatal("not implemented");
        return 0;
    }

    /*
     * Marshall to DBus
     */
    void CPropertyIndexVariantMap::marshallToDbus(QDBusArgument &argument) const
    {
        argument << this->m_values.keys();
        argument << this->m_values.values();
    }

    /*
     * Unmarshall from DBus
     */
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
        this->m_values.swap(newMap);
    }

    /*
     * Add value
     */
    void CPropertyIndexVariantMap::addValue(const CPropertyIndex &index, const QVariant &value)
    {
        this->m_values.insert(index, value);
    }

    /*
     * Add string by literal
     */
    void CPropertyIndexVariantMap::addValue(const CPropertyIndex &index, const char *str)
    {
        this->addValue(index, QString(str));
    }

    /*
     * Indexes
     */
    CPropertyIndexList CPropertyIndexVariantMap::indexes() const
    {
        return CPropertyIndexList::fromImpl(this->m_values.keys());
    }

    /*
     * Register metadata
     */
    void CPropertyIndexVariantMap::registerMetadata()
    {
        qRegisterMetaType<CPropertyIndexVariantMap>();
        qDBusRegisterMetaType<CPropertyIndexVariantMap>();
    }

    /*
     * Hash
     */
    uint CPropertyIndexVariantMap::getValueHash() const
    {
        // there is no hash for map, so I use this workaround here
        const QString s = this->toQString(false);
        QList<uint> h;
        h << qHash(s);
        return BlackMisc::calculateHash(h, "CIndexVariantMap");
    }
} // namespace
