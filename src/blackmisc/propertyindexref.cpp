/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#include "blackmisc/propertyindexref.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/verify.h"
#include <QtGlobal>

namespace BlackMisc
{
    CPropertyIndexRef::CPropertyIndexRef(int index) :
        m_begin(nullptr),
        m_sizeOrIndex(index)
    {}

    CPropertyIndexRef::CPropertyIndexRef(const QVector<int> &indexes) :
        m_begin(indexes.data()),
        m_sizeOrIndex(indexes.size())
    {}

    CPropertyIndexRef CPropertyIndexRef::copyFrontRemoved() const
    {
        BLACK_VERIFY_X(!this->isEmpty(), Q_FUNC_INFO, "Empty index");
        if (this->isEmpty() || !m_begin) { return -1; }
        CPropertyIndexRef copy = *this;
        ++copy.m_begin;
        --copy.m_sizeOrIndex;
        return copy;
    }

    bool CPropertyIndexRef::isNested() const
    {
        return m_begin && m_sizeOrIndex > 1;
    }

    bool CPropertyIndexRef::isMyself() const
    {
        return this->isEmpty();
    }

    bool CPropertyIndexRef::isEmpty() const
    {
        return m_begin ? m_sizeOrIndex < 1 : m_sizeOrIndex < 0;
    }

    int CPropertyIndexRef::frontToInt() const
    {
        Q_ASSERT_X(!this->isEmpty(), Q_FUNC_INFO, "No index");
        return m_begin ? *m_begin : m_sizeOrIndex;
    }

    bool CPropertyIndexRef::startsWith(int index) const
    {
        if (this->isEmpty()) { return false; }
        return this->frontToInt() == index;
    }

    QString CPropertyIndexRef::toQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        QString s;
        if (this->isEmpty()) { return s; }

        auto it = m_begin ? m_begin : &m_sizeOrIndex;
        auto end = it + (m_begin ? m_sizeOrIndex : 1);
        for (; it != end; ++it)
        {
            Q_ASSERT(*it >= static_cast<int>(CPropertyIndex::GlobalIndexCValueObject));
            if (!s.isEmpty()) { s.append(";"); }
            s.append(QString::number(*it));
        }
        return s;
    }
}
