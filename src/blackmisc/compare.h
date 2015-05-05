/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_COMPARE_H
#define BLACKMISC_COMPARE_H

#include "blackmisc/tuple.h"
#include "blackmisc/inheritance_traits.h"

namespace BlackMisc
{
    namespace Mixin
    {

        /*!
         * CRTP class template from which a derived class can inherit operator== implemented using its compare function.
         *
         * \tparam Derived Must overload a function bool compare(const Derived &, const Derived &) which can be found by ADL.
         */
        template <class Derived>
        class EqualsByCompare
        {
        public:
            //! Equals
            friend bool operator ==(const Derived &a, const Derived &b) { return compare(a, b) == 0; }

            //! Not equal
            friend bool operator !=(const Derived &a, const Derived &b) { return compare(a, b) != 0; }
        };

        /*!
         * CRTP class template from which a derived class can inherit operator== implemented by metatuple.
         */
        template <class Derived>
        class EqualsByTuple : private Private::EncapsulationBreaker
        {
        public:
            //! Equals
            friend bool operator ==(const Derived &a, const Derived &b) { return equals(a, b); }

            //! Not equal
            friend bool operator !=(const Derived &a, const Derived &b) { return ! equals(a, b); }

        private:
            static bool equals(const Derived &a, const Derived &b)
            {
                return toMetaTuple(a) == toMetaTuple(b) && baseEquals(static_cast<const BaseOfT<Derived> *>(&a), static_cast<const BaseOfT<Derived> *>(&b));
            }
            template <typename T> static bool baseEquals(const T *a, const T *b) { return *a == *b; }
            static bool baseEquals(const void *, const void *) { return true; }
        };

        /*!
         * CRTP class template from which a derived class can inherit operator< implemented using its compare function.
         *
         * \tparam Derived Must overload a function bool compare(const Derived &, const Derived &) which can be found by ADL.
         */
        template <class Derived>
        class LessThanByCompare
        {
        public:
            //! Less than
            friend bool operator <(const Derived &a, const Derived &b) { return compare(a, b) < 0; }

            //! Greater than
            friend bool operator >(const Derived &a, const Derived &b) { return compare(a, b) > 0; }

            //! Less than or equal
            friend bool operator <=(const Derived &a, const Derived &b) { return compare(a, b) <= 0; }

            //! Greater than or equal
            friend bool operator >=(const Derived &a, const Derived &b) { return compare(a, b) >= 0; }
        };

        /*!
         * CRTP class template from which a derived class can inherit operator< implemented by metatuple.
         */
        template <class Derived>
        class LessThanByTuple : private Private::EncapsulationBreaker
        {
        public:
            //! Less than
            friend bool operator <(const Derived &a, const Derived &b) { return less(a, b); }

            //! Greater than
            friend bool operator >(const Derived &a, const Derived &b) { return less(b, a); }

            //! Less than or equal
            friend bool operator <=(const Derived &a, const Derived &b) { return ! less(b, a); }

            //! Greater than or equal
            friend bool operator >=(const Derived &a, const Derived &b) { return ! less(a, b); }

        private:
            static bool less(const Derived &a, const Derived &b)
            {
                if (baseLess(static_cast<const BaseOfT<Derived> *>(&a), static_cast<const BaseOfT<Derived> *>(&b))) { return true; }
                return toMetaTuple(a) < toMetaTuple(b);
            }
            template <typename T> static bool baseLess(const T *a, const T *b) { return *a < *b; }
            static bool baseLess(const void *, const void *) { return false; }
        };

        /*!
         * CRTP class template from which a derived class can inherit non-member compare() implemented by metatuple.
         *
         * \tparam Derived Must be registered with BLACK_DECLARE_TUPLE_CONVERSION.
         */
        template <class Derived>
        class CompareByTuple : private Private::EncapsulationBreaker
        {
        public:
            //! Return negative, zero, or positive if a is less than, equal to, or greater than b.
            friend int compare(const Derived &a, const Derived &b) { return compareImpl(a, b); }

        private:
            static int compareImpl(const Derived &a, const Derived &b)
            {
                int baseCmp = baseCompare(static_cast<const BaseOfT<Derived> *>(&a), static_cast<const BaseOfT<Derived> *>(&b));
                if (baseCmp) { return baseCmp; }
                return BlackMisc::compare(toMetaTuple(a), toMetaTuple(b));
            }
            template <typename T> static int baseCompare(const T *a, const T *b) { return compare(*a, *b); }
            static int baseCompare(const void *, const void *) { return 0; }
        };

    } // Mixin
} // BlackMisc

#endif
