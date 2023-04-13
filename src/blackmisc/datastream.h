/* Copyright (C) 2019
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
