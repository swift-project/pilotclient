// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_DATASTREAM_H
#define BLACKMISC_DATASTREAM_H

#include <QDataStream>
#include <utility>

/*!
 * Operator for marshalling pairs with QDataStream.
 */
template <typename T, typename U>
QDataStream &operator<<(QDataStream &stream, const std::pair<T, U> &pair)
{
    return stream << pair.first << pair.second;
}

/*!
 * Operator for unmarshalling pairs with QDataStream.
 */
template <typename T, typename U>
QDataStream &operator>>(QDataStream &stream, std::pair<T, U> &pair)
{
    return stream >> pair.first >> pair.second;
}

#endif
