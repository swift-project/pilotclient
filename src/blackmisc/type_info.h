//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef TYPE_INFO_H
#define TYPE_INFO_H

#include <typeinfo>

namespace BlackMisc
{

    class CTypeInfo
    {
    public:
        explicit CTypeInfo(const std::type_info& info);

        bool operator < (const CTypeInfo& rhs) const;

    private:
        const std::type_info& m_typeinfo;
    };

} // namespace BlackMisc

#endif // TYPE_INFO_H
