// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#include "misc/propertyindexref.h"
#include "misc/verify.h"
#include <QtGlobal>

namespace swift::misc
{
    CPropertyIndexRef::CPropertyIndexRef(int index) : m_begin(nullptr),
                                                      m_sizeOrIndex(index)
    {}

    CPropertyIndexRef::CPropertyIndexRef(const QVector<int> &indexes) : m_begin(indexes.data()),
                                                                        m_sizeOrIndex(indexes.size())
    {}

    CPropertyIndexRef CPropertyIndexRef::copyFrontRemoved() const
    {
        SWIFT_VERIFY_X(!this->isEmpty(), Q_FUNC_INFO, "Empty index");
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
            Q_ASSERT(*it >= static_cast<int>(CPropertyIndexRef::GlobalIndexCValueObject));
            if (!s.isEmpty()) { s.append(";"); }
            s.append(QString::number(*it));
        }
        return s;
    }
}
