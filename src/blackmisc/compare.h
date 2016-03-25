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
#include "blackmisc/metaclass.h"
#include "blackmisc/inheritancetraits.h"
#include "blackmisc/typetraits.h"

namespace BlackMisc
{
    //! \cond PRIVATE
    namespace Private
    {
        template <typename T, typename U>
        int compareImpl(const T &a, const U &b, std::true_type)
        {
            return compare(a, b);
        }

        template <typename T, typename U>
        int compareImpl(const T &a, const U &b, std::false_type)
        {
            return a < b ? -1 : b < a ? 1 : 0;
        }

        template <size_t C, size_t I = 0>
        struct CompareHelper
        {
            template <typename T, typename U>
            static int compareTuples(const T &a, const U &b)
            {
                int cmp = compareImpl(std::get<I>(a), std::get<I>(b), HasCompare<std::tuple_element_t<I, T>, std::tuple_element_t<I, U>>());
                return cmp ? cmp : CompareHelper<C, I + 1>::compareTuples(a, b);
            }
        };

        template <size_t C>
        struct CompareHelper<C, C>
        {
            template <typename T, typename U>
            static int compareTuples(const T &, const U &)
            {
                return 0;
            }
        };
    }
    //! \endcond

    /*!
     * Lexicographically compare two tuples and return negative, positive, or zero,
     * if a is less than, greater than, or equal to b.
     * Each element is compared with compare(a', b') if supported by the element type, operator less-than otherwise.
     * \todo Rename to compare() after removing tuple.h, which contains a function with the same name and signature.
     */
    template <typename... Ts, typename... Us>
    int compareTuples(const std::tuple<Ts...> &a, const std::tuple<Us...> &b)
    {
        static_assert(sizeof...(Ts) == sizeof...(Us), "tuples must be same size");
        return Private::CompareHelper<sizeof...(Ts)>::compareTuples(a, b);
    }

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
            static bool baseEquals(const CEmpty *, const CEmpty *) { return true; }
        };

        /*!
         * CRTP class template from which a derived class can inherit operator== implemented by metaclass.
         */
        template <class Derived>
        class EqualsByMetaClass
        {
        public:
            //! Equals
            friend bool operator ==(const Derived &a, const Derived &b) { return equals(a, b); }

            //! Not equal
            friend bool operator !=(const Derived &a, const Derived &b) { return ! equals(a, b); }

        private:
            static bool equals(const Derived &a, const Derived &b)
            {
                auto meta = introspect<Derived>().without(MetaFlags<DisabledForComparison>());
                return meta.toCaseAwareTuple(a) == meta.toCaseAwareTuple(b) && baseEquals(static_cast<const BaseOfT<Derived> *>(&a), static_cast<const BaseOfT<Derived> *>(&b));
            }
            template <typename T> static bool baseEquals(const T *a, const T *b) { return *a == *b; }
            static bool baseEquals(const void *, const void *) { return true; }
            static bool baseEquals(const CEmpty *, const CEmpty *) { return true; }
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
            static bool baseLess(const CEmpty *, const CEmpty *) { return false; }
        };

        /*!
         * CRTP class template from which a derived class can inherit operator< implemented by metaclass.
         */
        template <class Derived>
        class LessThanByMetaClass
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
                auto meta = introspect<Derived>().without(MetaFlags<DisabledForComparison>());
                return meta.toCaseAwareTuple(a) < meta.toCaseAwareTuple(b);
            }
            template <typename T> static bool baseLess(const T *a, const T *b) { return *a < *b; }
            static bool baseLess(const void *, const void *) { return false; }
            static bool baseLess(const CEmpty *, const CEmpty *) { return false; }
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
            static int baseCompare(const CEmpty *, const CEmpty *) { return 0; }
        };

        /*!
         * CRTP class template from which a derived class can inherit non-member compare() implemented by metaclass.
         */
        template <class Derived>
        class CompareByMetaClass
        {
        public:
            //! Return negative, zero, or positive if a is less than, equal to, or greater than b.
            friend int compare(const Derived &a, const Derived &b) { return compareImpl(a, b); }

        private:
            static int compareImpl(const Derived &a, const Derived &b)
            {
                int baseCmp = baseCompare(static_cast<const BaseOfT<Derived> *>(&a), static_cast<const BaseOfT<Derived> *>(&b));
                if (baseCmp) { return baseCmp; }
                auto meta = introspect<Derived>().without(MetaFlags<DisabledForComparison>());
                return BlackMisc::compareTuples(meta.toCaseAwareTuple(a), meta.toCaseAwareTuple(b));
            }
            template <typename T> static int baseCompare(const T *a, const T *b) { return compare(*a, *b); }
            static int baseCompare(const void *, const void *) { return 0; }
            static int baseCompare(const CEmpty *, const CEmpty *) { return 0; }
        };

    } // Mixin
} // BlackMisc

#endif
