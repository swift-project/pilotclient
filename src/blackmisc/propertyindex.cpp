/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "propertyindex.h"
#include "predicates.h"
#include "blackmiscfreefunctions.h"

namespace BlackMisc
{
    /*
     * Constructor
     */
    CPropertyIndex::CPropertyIndex() {}

    /*
     * Non nested index
     */
    CPropertyIndex::CPropertyIndex(int singleProperty)
    {
        Q_ASSERT(singleProperty >= static_cast<int>(GlobalIndexCValueObject));
        this->m_indexes.append(singleProperty);
        this->listToString();
    }

    /*
     * Construct from initializer list
     */
    CPropertyIndex::CPropertyIndex(std::initializer_list<int> il) :
        m_indexes(il)
    {
        this->listToString();
    }

    /*
     * Construct from QList
     */
    CPropertyIndex::CPropertyIndex(const QList<int> &indexes) :
        m_indexes(indexes)
    {
        this->listToString();
    }

    /*
     * From string
     */
    CPropertyIndex::CPropertyIndex(const QString &indexes) : m_indexString(indexes)
    {
        this->parseFromString(indexes);
    }

    /*
     * Current property index, front element removed
     */
    CPropertyIndex CPropertyIndex::copyFrontRemoved() const
    {
        Q_ASSERT(!this->m_indexes.isEmpty());
        if (this->m_indexes.isEmpty()) return CPropertyIndex();
        QList<int> c = this->m_indexes;
        c.removeAt(0);
        CPropertyIndex pi(c);
        return pi;
    }

    /*
     * Nested index
     */
    bool CPropertyIndex::isNested() const
    {
        return this->m_indexes.size() > 1;
    }

    /*
     * Register metadata
     */
    void CPropertyIndex::registerMetadata()
    {
        qRegisterMetaType<CPropertyIndex>();
        qDBusRegisterMetaType<CPropertyIndex>();
    }

    /*
     * Convert to string
     */
    QString CPropertyIndex::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        return this->m_indexString;
    }

    /*
     * metaTypeId
     */
    int CPropertyIndex::getMetaTypeId() const
    {
        return qMetaTypeId<CPropertyIndex>();
    }

    /*
     * is a
     */
    bool CPropertyIndex::isA(int metaTypeId) const
    {
        if (metaTypeId == qMetaTypeId<CPropertyIndex>()) { return true; }
        return this->CValueObject::isA(metaTypeId);
    }

    /*
     * Compare
     */
    int CPropertyIndex::compareImpl(const CValueObject &otherBase) const
    {
        const auto &other = static_cast<const CPropertyIndex &>(otherBase);
        return compare(TupleConverter<CPropertyIndex>::toTuple(*this), TupleConverter<CPropertyIndex>::toTuple(other));
    }

    /*
     * Marshall to DBus
     */
    void CPropertyIndex::marshallToDbus(QDBusArgument &argument) const
    {
        argument << TupleConverter<CPropertyIndex>::toTuple(*this);
    }

    /*
     * Unmarshall from DBus
     */
    void CPropertyIndex::unmarshallFromDbus(const QDBusArgument &argument)
    {
        argument >> TupleConverter<CPropertyIndex>::toTuple(*this);
        this->stringToList();
    }

    /*
     * Parse from string
     */
    void CPropertyIndex::parseFromString(const QString &indexes)
    {
        this->m_indexString = indexes.simplified().replace(" ", ";").replace(",", ";");
        this->stringToList();
    }

    /*
     * To string
     */
    void CPropertyIndex::listToString()
    {
        QString l;
        foreach(int i, this->m_indexes)
        {
            Q_ASSERT(i >= static_cast<int>(GlobalIndexCValueObject));
            if (!l.isEmpty()) { l.append(";"); }
            l.append(QString::number(i));
        }
        this->m_indexString = l;
    }

    /*
     * To int list
     */
    void CPropertyIndex::stringToList()
    {
        this->m_indexes.clear();
        if (this->m_indexString.isEmpty()) { return; }
        QStringList indexes = this->m_indexString.split(';');
        foreach(QString index, indexes)
        {
            if (index.isEmpty()) continue;
            bool ok;
            int i = index.toInt(&ok);
            Q_ASSERT(ok);
            Q_ASSERT(i >= static_cast<int>(GlobalIndexCValueObject));
            this->m_indexes.append(i);
        }
    }

    /*
     * Equal?
     */
    bool CPropertyIndex::operator ==(const CPropertyIndex &other) const
    {
        if (this == &other) return true;
        return TupleConverter<CPropertyIndex>::toTuple(*this) == TupleConverter<CPropertyIndex>::toTuple(other);
    }

    /*
     * Unequal?
     */
    bool CPropertyIndex::operator !=(const CPropertyIndex &other) const
    {
        return !((*this) == other);
    }

    /*
     * Hash
     */
    uint CPropertyIndex::getValueHash() const
    {
        return qHash(TupleConverter<CPropertyIndex>::toTuple(*this));
    }

    /*
     * To JSON
     */
    QJsonObject CPropertyIndex::toJson() const
    {
        return BlackMisc::serializeJson(CPropertyIndex::jsonMembers(), TupleConverter<CPropertyIndex>::toTuple(*this));
    }

    /*
     * From JSON
     */
    void CPropertyIndex::convertFromJson(const QJsonObject &json)
    {
        BlackMisc::deserializeJson(json, CPropertyIndex::jsonMembers(), TupleConverter<CPropertyIndex>::toTuple(*this));
        this->stringToList();
    }

    /*
     * Members
     */
    const QStringList &CPropertyIndex::jsonMembers()
    {
        return TupleConverter<CPropertyIndex>::jsonMembers();
    }

} // namespace
