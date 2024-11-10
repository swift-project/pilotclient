// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/logcategory.h"

#include <QtGlobal>

BLACK_DEFINE_VALUEOBJECT_MIXINS(swift::misc, CLogCategory)

namespace swift::misc
{
    QString CLogCategory::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        return m_string;
    }
}
