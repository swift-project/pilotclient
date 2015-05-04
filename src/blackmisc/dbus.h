/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_DBUS_H
#define BLACKMISC_DBUS_H

#include <QDBusArgument>
#include <type_traits>

/*!
 * Non-member non-friend operator for streaming enums to QDBusArgument.
 *
 * \param argument
 * \param enumType
 * \return
 * \remarks Currently outside namespace for OSX build, see https://dev.vatsim-germany.org/issues/184
 */
template <class ENUM> typename std::enable_if<std::is_enum<ENUM>::value, QDBusArgument>::type const &
operator>>(const QDBusArgument &argument, ENUM &enumType)
{
    uint e;
    argument >> e;
    enumType = static_cast<ENUM>(e);
    return argument;
}
#endif // guard
