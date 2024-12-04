// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_HTMLUTILS_H
#define SWIFT_MISC_HTMLUTILS_H

#include <QStringBuilder>

#include "misc/swiftmiscexport.h"

//! Free functions in swift::misc
namespace swift::misc
{
    //! Values as HTML table
    SWIFT_MISC_EXPORT QString toHtmTable(const QStringList &values, int columns);

    //! As hyperlink
    SWIFT_MISC_EXPORT QString asHyperlink(const QString &url, const QString &text = "");

} // namespace swift::misc

#endif // SWIFT_MISC_HTMLUTILS_H
