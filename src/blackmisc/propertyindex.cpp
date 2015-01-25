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
    CPropertyIndex::CPropertyIndex(int singleProperty)
    {
        Q_ASSERT(singleProperty >= static_cast<int>(GlobalIndexCValueObject));
        this->setIndexStringByList({singleProperty});
    }

    CPropertyIndex::CPropertyIndex(std::initializer_list<int> il)
    {
        this->setIndexStringByList(il);
    }

    CPropertyIndex::CPropertyIndex(const QList<int> &indexes)
    {
        this->setIndexStringByList(indexes);
    }

    CPropertyIndex::CPropertyIndex(const QString &indexes) : m_indexString(indexes)
    {
        this->parseFromString(indexes);
    }

    CPropertyIndex CPropertyIndex::copyFrontRemoved() const
    {
        Q_ASSERT(!this->isEmpty());
        if (this->isEmpty()) { return CPropertyIndex(); }
        QList<int> l = this->indexList();
        l.removeAt(0);
        CPropertyIndex pi(l);
        return pi;
    }

    bool CPropertyIndex::isNested() const
    {
        return this->m_indexString.contains(';');
    }

    bool CPropertyIndex::isMyself() const
    {
        return this->m_indexString.isEmpty();
    }

    bool CPropertyIndex::isEmpty() const
    {
        return this->m_indexString.isEmpty();
    }

    QString CPropertyIndex::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        return this->m_indexString;
    }

    void CPropertyIndex::parseFromString(const QString &indexes)
    {
        if (indexes.isEmpty())
        {
            this->m_indexString.clear();
        }
        else
        {
            QString is(indexes.simplified().replace(" ", ";").replace(",", ";"));
            if (is.endsWith(';'))
            {
                this->m_indexString = is.left(is.length() - 1);
            }
            else
            {
                this->m_indexString = is;
            }
        }
    }

    void CPropertyIndex::setIndexStringByList(const QList<int> &list)
    {
        QString l;
        for (const int &i : list)
        {
            Q_ASSERT(i >= static_cast<int>(GlobalIndexCValueObject));
            if (!l.isEmpty()) { l.append(";"); }
            l.append(QString::number(i));
        }
        this->m_indexString = l;
    }

    QList<int> CPropertyIndex::indexList() const
    {
        QList<int> list;
        if (this->m_indexString.isEmpty()) { return list; }
        QStringList indexes = this->m_indexString.split(';');
        foreach(QString index, indexes)
        {
            if (index.isEmpty()) { continue; }
            bool ok;
            int i = index.toInt(&ok);
            Q_ASSERT(ok);
            Q_ASSERT(i >= static_cast<int>(GlobalIndexCValueObject));
            list.append(i);
        }
        return list;
    }

} // namespace
