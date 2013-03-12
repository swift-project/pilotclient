//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "blackmisc/type_info.h"

namespace BlackMisc
{

    CTypeInfo::CTypeInfo(const type_info& info)
        :  m_typeinfo(info)
    {
    }

    bool CTypeInfo::operator <(const CTypeInfo& rhs) const
    {
        return m_typeinfo.before(rhs.m_typeinfo) != 0;
    }

} // namespace BlackMisc
