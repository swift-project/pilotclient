// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_MIXIN_MIXINCOMPARE_H
#define SWIFT_MISC_MIXIN_MIXINCOMPARE_H

#include "misc/metaclass.h"
#include "misc/inheritancetraits.h"
#include "misc/typetraits.h"
#include <QList>
#include <QPair>
#include <QString>

namespace swift::misc
{
    class CEmpty;

    namespace mixin
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
            friend bool operator==(const Derived &a, const Derived &b) { return compare(a, b) == 0; }

            //! Not equal
            friend bool operator!=(const Derived &a, const Derived &b) { return compare(a, b) != 0; }
        };

        /*!
         * CRTP class template from which a derived class can inherit operator== implemented by metaclass.
         */
        template <class Derived>
        class EqualsByMetaClass
        {
        public:
            //! Equals
            friend bool operator==(const Derived &a, const Derived &b) { return equals(a, b); }

            //! Not equal
            friend bool operator!=(const Derived &a, const Derived &b) { return !equals(a, b); }

        private:
            static bool equals(const Derived &a, const Derived &b);
            template <typename T>
            static bool baseEquals(const T *a, const T *b);
            static bool baseEquals(const void *, const void *);
            static bool baseEquals(const CEmpty *, const CEmpty *);
            template <typename T, typename Flags>
            static bool membersEqual(const T &a, const T &b, Flags);
        };

        template <class Derived>
        bool EqualsByMetaClass<Derived>::equals(const Derived &a, const Derived &b)
        {
            bool result = baseEquals(static_cast<const TBaseOfT<Derived> *>(&a), static_cast<const TBaseOfT<Derived> *>(&b));
            introspect<Derived>().forEachMember([&](auto member) {
                if constexpr (!decltype(member)::has(MetaFlags<DisabledForComparison>()))
                {
                    result = result && EqualsByMetaClass::membersEqual(member.in(a), member.in(b), member.m_flags);
                }
            });
            return result;
        }

        template <class Derived>
        template <typename T>
        bool EqualsByMetaClass<Derived>::baseEquals(const T *a, const T *b)
        {
            return *a == *b;
        }

        template <class Derived>
        bool EqualsByMetaClass<Derived>::baseEquals(const void *, const void *)
        {
            return true;
        }

        template <class Derived>
        bool EqualsByMetaClass<Derived>::baseEquals(const CEmpty *, const CEmpty *)
        {
            return true;
        }

        template <class Derived>
        template <typename T, typename Flags>
        bool EqualsByMetaClass<Derived>::membersEqual(const T &a, const T &b, Flags)
        {
            if constexpr (static_cast<bool>(Flags::value & CaseInsensitiveComparison)) { return a.compare(b, Qt::CaseInsensitive) == 0; }
            else { return a == b; }
        }

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
            friend bool operator<(const Derived &a, const Derived &b) { return compare(a, b) < 0; }

            //! Greater than
            friend bool operator>(const Derived &a, const Derived &b) { return compare(a, b) > 0; }

            //! Less than or equal
            friend bool operator<=(const Derived &a, const Derived &b) { return compare(a, b) <= 0; }

            //! Greater than or equal
            friend bool operator>=(const Derived &a, const Derived &b) { return compare(a, b) >= 0; }
        };

        /*!
         * CRTP class template from which a derived class can inherit operator< implemented by metaclass.
         */
        template <class Derived>
        class LessThanByMetaClass
        {
        public:
            //! Less than
            friend bool operator<(const Derived &a, const Derived &b) { return less(a, b); }

            //! Greater than
            friend bool operator>(const Derived &a, const Derived &b) { return less(b, a); }

            //! Less than or equal
            friend bool operator<=(const Derived &a, const Derived &b) { return !less(b, a); }

            //! Greater than or equal
            friend bool operator>=(const Derived &a, const Derived &b) { return !less(a, b); }

        private:
            static bool less(const Derived &a, const Derived &b);
            template <typename T>
            static bool baseLess(const T *a, const T *b);
            static bool baseLess(const void *, const void *);
            static bool baseLess(const CEmpty *, const CEmpty *);
            template <typename T, typename Flags>
            static bool membersLess(bool &io_greaterThan, const T &a, const T &b, Flags);
        };

        template <class Derived>
        bool LessThanByMetaClass<Derived>::less(const Derived &a, const Derived &b)
        {
            bool result = baseLess(static_cast<const TBaseOfT<Derived> *>(&a), static_cast<const TBaseOfT<Derived> *>(&b));
            bool gt = baseLess(static_cast<const TBaseOfT<Derived> *>(&b), static_cast<const TBaseOfT<Derived> *>(&a));
            introspect<Derived>().forEachMember([&](auto member) {
                if constexpr (!decltype(member)::has(MetaFlags<DisabledForComparison>()))
                {
                    result = result || LessThanByMetaClass::membersLess(gt, member.in(a), member.in(b), member.m_flags);
                }
            });
            return result;
        }

        template <class Derived>
        template <typename T>
        bool LessThanByMetaClass<Derived>::baseLess(const T *a, const T *b)
        {
            return *a < *b;
        }

        template <class Derived>
        bool LessThanByMetaClass<Derived>::baseLess(const void *, const void *)
        {
            return false;
        }

        template <class Derived>
        bool LessThanByMetaClass<Derived>::baseLess(const CEmpty *, const CEmpty *)
        {
            return false;
        }

        template <class Derived>
        template <typename T, typename Flags>
        bool LessThanByMetaClass<Derived>::membersLess(bool &io_greaterThan, const T &a, const T &b, Flags)
        {
            if (io_greaterThan) { return false; }
            if constexpr (static_cast<bool>(Flags::value & CaseInsensitiveComparison))
            {
                io_greaterThan = b.compare(a, Qt::CaseInsensitive) < 0;
                return a.compare(b, Qt::CaseInsensitive) < 0;
            }
            else
            {
                io_greaterThan = b < a;
                return a < b;
            }
        }

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
            static int compareImpl(const Derived &a, const Derived &b);
            template <typename T>
            static int baseCompare(const T *a, const T *b);
            static int baseCompare(const void *, const void *);
            static int baseCompare(const CEmpty *, const CEmpty *);
            template <typename T, typename Flags>
            static int membersCompare(const T &a, const T &b, Flags);
        };

        template <class Derived>
        int CompareByMetaClass<Derived>::compareImpl(const Derived &a, const Derived &b)
        {
            int result = baseCompare(static_cast<const TBaseOfT<Derived> *>(&a), static_cast<const TBaseOfT<Derived> *>(&b));
            introspect<Derived>().forEachMember([&](auto member) {
                if constexpr (!decltype(member)::has(MetaFlags<DisabledForComparison>()))
                {
                    result = result ? result : CompareByMetaClass::membersCompare(member.in(a), member.in(b), member.m_flags);
                }
            });
            return result;
        }

        template <class Derived>
        template <typename T>
        int CompareByMetaClass<Derived>::baseCompare(const T *a, const T *b)
        {
            return compare(*a, *b);
        }

        template <class Derived>
        int CompareByMetaClass<Derived>::baseCompare(const void *, const void *)
        {
            return 0;
        }

        template <class Derived>
        int CompareByMetaClass<Derived>::baseCompare(const CEmpty *, const CEmpty *)
        {
            return 0;
        }

        template <class Derived>
        template <typename T, typename Flags>
        int CompareByMetaClass<Derived>::membersCompare(const T &a, const T &b, Flags)
        {
            if constexpr (static_cast<bool>(Flags::value & CaseInsensitiveComparison)) { return a.compare(b, Qt::CaseInsensitive); }
            else if constexpr (THasCompare<T, T>::value) { return compare(a, b); }
            else { return a < b ? -1 : b < a ? 1 :
                                               0; }
        }

    } // Mixin
} // swift::misc

#endif
