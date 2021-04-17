/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_DBUS_H
#define BLACKMISC_DBUS_H

#include "blackmisc/blackmiscexport.h"

#include <QFlags>
#include <QDBusArgument>
#include <type_traits>
#include <utility>
#include <string>

/*!
 * Non member non-friend streaming for std::string
 */
QDBusArgument &operator <<(QDBusArgument &arg, const std::string &s);

/*!
 * Operator for std::string from QDBusArgument.
 */
const QDBusArgument &operator >>(const QDBusArgument &arg, std::string &s);

// *INDENT-OFF*
/*!
 * Operator for streaming enums to QDBusArgument.
 */
template <class E, std::enable_if_t<std::is_enum_v<E>, int> = 0>
QDBusArgument &operator <<(QDBusArgument &arg, const E &value)
{
    arg.beginStructure();
    arg << static_cast<int>(value);
    arg.endStructure();
    return arg;
}

/*!
 * Operator for streaming enums from QDBusArgument.
 */
template <class E, std::enable_if_t<std::is_enum_v<E>, int> = 0>
const QDBusArgument &operator >>(const QDBusArgument &arg, E &value)
{
    int temp;
    arg.beginStructure();
    arg >> temp;
    arg.endStructure();
    value = static_cast<E>(temp);
    return arg;
}

/*!
 * Operator for streaming QFlags to QDBusArgument.
 */
template <class T>
QDBusArgument &operator <<(QDBusArgument &arg, const QFlags<T> &value)
{
    arg.beginStructure();
    arg << static_cast<typename QFlags<T>::Int>(value);
    arg.endStructure();
    return arg;
}

/*!
 * Operator for streaming QFlags from QDBusArgument.
 */
template <class T>
const QDBusArgument &operator >>(const QDBusArgument &arg, QFlags<T> &value)
{
    typename QFlags<T>::Int temp = 0;
    arg.beginStructure();
    arg >> temp;
    arg.endStructure();
    value = static_cast<QFlags<T>>(temp);
    return arg;
}

/*!
 * Operator for streaming pairs to QDBusArgument.
 */
template <class A, class B>
QDBusArgument &operator <<(QDBusArgument &arg, const std::pair<A, B> &pair)
{
    arg.beginStructure();
    arg << pair.first << pair.second;
    arg.endStructure();
    return arg;
}

/*!
 * Operator for streaming pairs from QDBusArgument.
 */
template <class A, class B>
const QDBusArgument &operator >>(const QDBusArgument &arg, std::pair<A, B> &pair)
{
    arg.beginStructure();
    arg >> pair.first >> pair.second;
    arg.endStructure();
    return arg;
}
// *INDENT-ON*

//! Windows: prevents unloading of QtDBus shared library until the process is terminated.
//! QtDBus must have been loaded already by the calling process.
//! Does nothing on non-Windows platforms.
BLACKMISC_EXPORT void preventQtDBusDllUnload();

#endif // guard
