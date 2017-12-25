/* Copyright (C) 2016
* swift project Community / Contributors
*
* This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
* directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
* including this file, may be copied, modified, propagated, or distributed except according to the terms
* contained in the LICENSE file.
*/

#include "blackmisc/applicationinfolist.h"

namespace BlackMisc
{
    CApplicationInfoList::CApplicationInfoList() = default;

    CApplicationInfoList::CApplicationInfoList(const CSequence<CApplicationInfo> &other) :
        CSequence<CApplicationInfo>(other)
    {}

    bool CApplicationInfoList::containsApplication(CApplicationInfo::Application application) const
    {
        return this->contains(&CApplicationInfo::application, application);
    }

    int CApplicationInfoList::removeApplication(CApplicationInfo::Application application)
    {
        return this->removeIf(&CApplicationInfo::application, application);
    }

    QStringList CApplicationInfoList::processNames() const
    {
        QStringList names;
        names.reserve(size());
        for (const CApplicationInfo &info : *this)
        {
            names.append(info.getProcessInfo().processName());
        }
        return names;
    }
}
