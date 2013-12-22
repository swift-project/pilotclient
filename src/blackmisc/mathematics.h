/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_MATHEMATICS_H
#define BLACKMISC_MATHEMATICS_H
#include <QtCore/qmath.h>
#include <cmath>

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
            static inline double square(double x)
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
            static double cubicRootReal(double x);

            /*!
             * \brief Utility round method
             * \param value
             * \param digits
             * \return
             */
            static double round(double value, int digits);

            /*!
             * \brief Round by given epsilon, e.g.
             * \param value
             * \param epsilon
             * \return
             */
            static double roundEpsilon(double value, double epsilon);

            /*!
             * \brief Nearest integer not greater in magnitude than value, correcting for epsilon
             * \param value
             * \param epsilon
             */
            static inline double trunc(double value, double epsilon = 1e-10)
            {
                return value < 0 ? ceil(value - epsilon) : floor(value + epsilon);
            }

            /*!
             * \brief Fractional part of value
             * \param value
             */
            static inline double fract(double value)
            {
                double unused;
                return modf(value, &unused);
            }

            /*!
             * \brief PI
             * \return
             */
            static const double &PIHALF()
            {
                static double pi = 2.0 * qAtan(1.0);
                return pi;
            }

            /*!
             * \brief PI
             * \return
             */
            static const double &PI()
            {
                static double pi = 4.0 * qAtan(1.0);
                return pi;
            }

            /*!
             * \brief PI * 2
             * \return
             */
            static const double &PI2()
            {
                static double pi2 = 8.0 * qAtan(1.0);
                return pi2;
            }

        private:
            /*!
             * \brief No objects, just static
             */
            CMath();
        };

    } // namespace
} // namespace
#endif // guard
