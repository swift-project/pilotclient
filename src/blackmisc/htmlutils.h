// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_HTMLUTILS_H
#define BLACKMISC_HTMLUTILS_H

#include "blackmisc/blackmiscexport.h"
#include <QStringBuilder>

//! Free functions in BlackMisc
namespace BlackMisc
{
    //! Values as HTML table
    BLACKMISC_EXPORT QString toHtmTable(const QStringList &values, int columns);

    //! As hyperlink
    BLACKMISC_EXPORT QString asHyperlink(const QString &url, const QString &text = "");

} // ns

#endif // guard
