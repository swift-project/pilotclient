/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file

#ifndef BLACKMISC_RANGE_H
#define BLACKMISC_RANGE_H

#include "valueobject.h"
#include "indexvariantmap.h"
#include "predicates.h"
#include "json.h"
#include <algorithm>

namespace BlackMisc
{

    /*!
     * Any container class with begin and end iterators can inherit from this CRTP class
     * to gain some useful algorithms as member functions.
     * \tparam Derived The most derived container class inheriting from this instantiation.
     */
    template <class Derived, class T>
    class CRangeBase
    {
    public:
        /*!
         * \brief Return a copy containing only those elements for which a given predicate returns true.
         */
        template <class Predicate>
        Derived findBy(Predicate p) const
        {
            Derived result = derived();
            result.erase(std::remove_if(result.begin(), result.end(), [ = ](const typename Derived::value_type & value) { return !p(value); }), result.end());
            return result;
        }

        /*!
         * \brief Return a copy containing only those elements matching some particular key/value pair(s).
         * \param k0 A pointer to a member function of T.
         * \param v0 A value to compare against the value returned by k0.
         * \param keysValues Zero or more additional pairs of { pointer to member function of T, value to compare it against }.
         */
        template <class K0, class V0, class... KeysValues>
        Derived findBy(K0 k0, V0 v0, KeysValues... keysValues) const
        {
            return findBy(BlackMisc::Predicates::MemberEqual(k0, v0, keysValues...));
        }

        /*!
         * \brief Return a copy containing only those elements matching a given value map.
         */
        Derived findBy(const CIndexVariantMap &valueMap) const
        {
            return findBy([ & ](const T & value) { return value == valueMap; });
        }

        /*!
         * \brief Return true if there is an element for which a given predicate returns true.
         */
        template <class Predicate>
        bool contains(Predicate p) const
        {
            return std::any_of(derived().cbegin(), derived().cend(), p);
        }

        /*!
         * \brief Return true if there is an element equal to given object. Uses the most efficient implementation available.
         */
        bool contains(const T &object) const
        {
            return derived().find(object) != derived().cend();
        }

        /*!
         * \brief Return a copy containing only those elements matching some particular key/value pair(s).
         * \param k0 A pointer to a member function of T.
         * \param v0 A value to compare against the value returned by k0.
         * \param keysValues Zero or more additional pairs of { pointer to member function of T, value to compare it against }.
         */
        template <class K0, class V0, class... KeysValues>
        bool contains(K0 k0, V0 v0, KeysValues... keysValues) const
        {
            return contains(BlackMisc::Predicates::MemberEqual(k0, v0, keysValues...));
        }

    private:
        Derived &derived() { return static_cast<Derived &>(*this); }
        const Derived &derived() const { return static_cast<const Derived &>(*this); }
    };

}

#endif // guard
