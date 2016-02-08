/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_FREEFUNCTIONS_H
#define BLACKMISC_FREEFUNCTIONS_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/tuple.h"
#include "blackmisc/inheritancetraits.h"
#include <QDir> // for Q_INIT_RESOURCE
#include <QList>
#include <QVariant>
#include <QMetaType>
#include <memory>

/*!
 * Workaround, to call initResource from namespace. Used in BlackMisc::initResources().
 * Q_INIT_RESOURCE adds resource, here the translation files.
 */
inline void initBlackMiscResources()
{
    // cannot be declared within namespace, see docu
    // hence BlackMisc::initResources() calls this inline function
    Q_INIT_RESOURCE(blackmisc);
}

//! Free functions in BlackMisc
namespace BlackMisc
{
    //! Init resources
    BLACKMISC_EXPORT void initResources();

    namespace Mixin
    {
        /*!
         * CRTP class template from which a derived class can inherit common methods dealing with hashing instances by metatuple.
         *
         * \tparam Derived Must be registered with BLACK_DECLARE_TUPLE_CONVERSION.
         */
        template <class Derived>
        class HashByTuple : private Private::EncapsulationBreaker
        {
        public:
            //! qHash overload, needed for storing value in a QSet.
            friend uint qHash(const Derived &value, uint seed = 0)
            {
                return ::qHash(hashImpl(value), seed);
            }

        private:
            static uint hashImpl(const Derived &value)
            {
                return BlackMisc::qHash(toMetaTuple(value)) ^ baseHash(static_cast<const BaseOfT<Derived> *>(&value));
            }

            template <typename T> static uint baseHash(const T *base) { return qHash(*base); }
            static uint baseHash(const void *) { return 0; }
        };
    } // Mixin

    /*!
     * \brief Calculate a single hash value based on a list of individual hash values
     * \param values
     * \param className   add a hash value for class name on top
     * \return
     */
    BLACKMISC_EXPORT uint calculateHash(const QList<uint> &values, const char *className);

    //! Hash value, but with int list
    BLACKMISC_EXPORT uint calculateHash(const QList<int> &values, const char *className);

    //! Own implementation of std::make_unique, a C++14 feature not provided by GCC in C++11 mode
    template<typename T, typename... Args>
    std::unique_ptr<T> make_unique(Args &&... args)
    {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }
} // ns

#endif // guard
