/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_MATHEMATICS_H
#define BLACKMISC_MATHEMATICS_H
#include <QtCore/qmath.h>

namespace BlackMisc
{
namespace Math
{

// Some namespace wide constant values

//! Mathematical constant Pi
const qreal PI = 4.0 * qAtan(1.0);

//! 2 * Pi
const qreal TwoPI  = 2.0 * PI;

/*!
 * \brief Math utils
 */
class CMath
{
public:

    /*!
     * \brief Calculates the hypotenuse of x and y without overflow
     * \param x
     * \param y
     * \return
     */
    static qreal hypot(qreal x, qreal y);

    /*!
     * \brief Calculates the square of x
     * \param x
     * \return
     */
    static inline qreal square(qreal x)
    {
        return x * x;
    }

    /*!
     * \brief Calculates x to the power of three
     * \param x
     * \return
     */
    static inline qreal cubic(const qreal x)
    {
        return x * x * x;
    }

    /*!
     * \brief Calculates the real cubic root
     * \param x
     * \return
     */
    static qreal cubicRootReal(qreal x);

private:
    /*!
     * \brief Avoid object init
     */
    CMath() {}
};

} // namespace
} // namespace
#endif // guard
