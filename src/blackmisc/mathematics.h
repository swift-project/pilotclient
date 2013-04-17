/*  Copyright (C) 2013 VATSIM Community / authors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_MATHEMATICS_H
#define BLACKMISC_MATHEMATICS_H

namespace BlackMisc
{
namespace Math
{

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
    static double hypot(double x, double y);

    /*!
     * \brief Calculates the square of x
     * \param x
     * \return
     */
    static inline double square(const double x)
    {
        return x * x;
    }

    /*!
     * \brief Calculates x to the power of three
     * \param x
     * \return
     */
    static inline double cubic(const double x)
    {
        return x * x * x;
    }

    /*!
     * \brief Calculates the real cubic root
     * \param x
     * \return
     */
    static double cubicRootReal(const double x);

private:
    /*!
     * \brief Avoid object init
     */
    CMath() {}
};

} // namespace
} // namespace
#endif // guard
