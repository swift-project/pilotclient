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

#include "blackmisc/metaclass.h"
#include "blackmisc/inheritancetraits.h"
#include "blackmisc/typetraits.h"

namespace BlackMisc
{
    class CEmpty;

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
                bool result = baseEquals(static_cast<const TBaseOfT<Derived> *>(&a), static_cast<const TBaseOfT<Derived> *>(&b));
                meta.forEachMemberPair(a, b, [ & ](auto &&... args) { result = result && EqualsByMetaClass::membersEqual(std::forward<decltype(args)>(args)...); });
                return result;
            }
            template <typename T> static bool baseEquals(const T *a, const T *b) { return *a == *b; }
            static bool baseEquals(const void *, const void *) { return true; }
            static bool baseEquals(const CEmpty *, const CEmpty *) { return true; }

            template <typename T, typename Flags>
            static bool membersEqual(const T &a, const T &b, Flags)
            {
                return membersEqual(a, b, std::integral_constant<bool, static_cast<bool>(Flags::value & CaseInsensitiveComparison)>());
            }
            template <typename T>
            static bool membersEqual(const T &a, const T &b, std::true_type) { return a.compare(b, Qt::CaseInsensitive) == 0; }
            template <typename T>
            static bool membersEqual(const T &a, const T &b, std::false_type) { return a == b; }
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
                auto meta = introspect<Derived>().without(MetaFlags<DisabledForComparison>());
                bool result = baseLess(static_cast<const TBaseOfT<Derived> *>(&a), static_cast<const TBaseOfT<Derived> *>(&b));
                bool gt = baseLess(static_cast<const TBaseOfT<Derived> *>(&b), static_cast<const TBaseOfT<Derived> *>(&a));
                meta.forEachMemberPair(a, b, [ & ](auto &&... args) { result = result || LessThanByMetaClass::membersLess(gt, std::forward<decltype(args)>(args)...); });
                return result;
            }
            template <typename T> static bool baseLess(const T *a, const T *b) { return *a < *b; }
            static bool baseLess(const void *, const void *) { return false; }
            static bool baseLess(const CEmpty *, const CEmpty *) { return false; }

            template <typename T, typename Flags>
            static bool membersLess(bool &io_greaterThan, const T &a, const T &b, Flags)
            {
                using CaseInsensitive = std::integral_constant<bool, static_cast<bool>(Flags::value & CaseInsensitiveComparison)>;
                if (io_greaterThan) { return false; }
                io_greaterThan = membersLess(b, a, CaseInsensitive());
                return membersLess(a, b, CaseInsensitive());
            }
            template <typename T>
            static bool membersLess(const T &a, const T &b, std::true_type) { return a.compare(b, Qt::CaseInsensitive) < 0; }
            template <typename T>
            static bool membersLess(const T &a, const T &b, std::false_type) { return a < b; }
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
                auto meta = introspect<Derived>().without(MetaFlags<DisabledForComparison>());
                int result = baseCompare(static_cast<const TBaseOfT<Derived> *>(&a), static_cast<const TBaseOfT<Derived> *>(&b));
                meta.forEachMemberPair(a, b, [ & ](auto &&... args) { result = result ? result : CompareByMetaClass::membersCompare(std::forward<decltype(args)>(args)...); });
                return result;
            }
            template <typename T> static int baseCompare(const T *a, const T *b) { return compare(*a, *b); }
            static int baseCompare(const void *, const void *) { return 0; }
            static int baseCompare(const CEmpty *, const CEmpty *) { return 0; }

            template <typename T, typename Flags>
            static int membersCompare(const T &a, const T &b, Flags)
            {
                using CaseInsensitive = std::integral_constant<bool, static_cast<bool>(Flags::value & CaseInsensitiveComparison)>;
                return membersCompare(a, b, CaseInsensitive(), THasCompare<T, T>());
            }
            template <typename T, typename U>
            static int membersCompare(const T &a, const T &b, std::true_type, U) { return a.compare(b, Qt::CaseInsensitive); }
            template <typename T>
            static int membersCompare(const T &a, const T &b, std::false_type, std::true_type) { return compare(a, b); }
            template <typename T>
            static int membersCompare(const T &a, const T &b, std::false_type, std::false_type) { return a < b ? -1 : b < a ? 1 : 0; }
        };

    } // Mixin
} // BlackMisc

#endif
