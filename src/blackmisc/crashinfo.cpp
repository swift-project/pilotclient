/* Copyright (C) 2018
* swift project Community / Contributors
*
* This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
* directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
* including this file, may be copied, modified, propagated, or distributed except according to the terms
* contained in the LICENSE file.
*/

#include "blackmisc/crashinfo.h"
#include <QStringBuilder>

namespace BlackMisc
{
    CCrashInfo::CCrashInfo() {}

    QString CCrashInfo::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        return QStringLiteral("{ %1, %2 }").arg(this->getInfo(), this->getUserName());
    }

    CVariant CCrashInfo::propertyByIndex(const CPropertyIndex &index) const
    {
        if (index.isMyself()) { return CVariant::from(*this); }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexUserName: return CVariant::fromValue(m_userName);
        case IndexInfo: return CVariant::fromValue(m_info);
        default: break;
        }
        return CValueObject::propertyByIndex(index);
    }

    void CCrashInfo::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
    {
        if (index.isMyself()) { (*this) = variant.to<CCrashInfo>(); return; }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexUserName: this->setUserName(variant.toQString()); break;
        case IndexInfo: this->setUserName(variant.toQString()); break;
        default: CValueObject::setPropertyByIndex(index, variant); break;
        }
    }

    int CCrashInfo::comparePropertyByIndex(const CPropertyIndex &index, const CCrashInfo &compareValue) const
    {
        if (index.isMyself()) { return this->getInfo().compare(compareValue.getInfo()); }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexUserName: return this->getUserName().compare(compareValue.getUserName());
        case IndexInfo: return this->getInfo().compare(compareValue.getInfo());
        default: return CValueObject::comparePropertyByIndex(index.copyFrontRemoved(), compareValue);
        }
    }
} // ns
