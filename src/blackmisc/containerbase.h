/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKMISC_CONTAINERBASE_H
#define BLACKMISC_CONTAINERBASE_H

#include "valueobject.h"
#include "valuemap.h"
#include "predicates.h"
#include <algorithm>

#define _SCL_SECURE_NO_WARNINGS // suppress MSVC unchecked iterator warning for std::transform

namespace BlackMisc
{

    /*!
     * \brief Base class for CCollection and CSequence implementing their algorithms.
     */
    template <template <class> class C, class T>
    class CContainerBase : public CValueObject
    {
    public:
        /*!
         * \brief Return a copy containing only those elements for which a given predicate returns true.
         * \param p
         * \return
         */
        template <class Predicate>
        C<T> findBy(Predicate p) const
        {
            C<T> result = derived();
            result.erase(std::remove_if(result.begin(), result.end(), [ = ](const T &value) { return !p(value); }), result.end());
            return result;
        }

        /*!
         * \brief Return a copy containing only those elements matching a particular key/value pair.
         * \param key1 A pointer to a member function of T.
         * \param value1 Will be compared to the return value of key1.
         * \return
         */
        template <class K1, class V1>
        C<T> findBy(K1 key1, V1 value1) const
        {
            return findBy(BlackMisc::Predicates::MemberEqual<T>(key1, value1));
        }

        /*!
         * \brief Return a copy containing only those elements matching some particular key/value pairs.
         * \param key1 A pointer to a member function of T.
         * \param value1 Will be compared to the return value of key1.
         * \param key2 A pointer to a member function of T.
         * \param value2 Will be compared to the return value of key2.
         * \return
         */
        template <class K1, class V1, class K2, class V2>
        C<T> findBy(K1 key1, V1 value1, K2 key2, V2 value2) const
        {
            return findBy(BlackMisc::Predicates::MemberEqual<T>(key1, value1, key2, value2));
        }

        /*!
         * \brief Return a copy containing only those elements matching some particular key/value pairs.
         * \param key1 A pointer to a member function of T.
         * \param value1 Will be compared to the return value of key1.
         * \param key2 A pointer to a member function of T.
         * \param value2 Will be compared to the return value of key2.
         * \param key3 A pointer to a member function of T.
         * \param value3 Will be compared to the return value of key3.
         * \return
         */
        template <class K1, class V1, class K2, class V2, class K3, class V3>
        C<T> findBy(K1 key1, V1 value1, K2 key2, V2 value2, K3 key3, V3 value3) const
        {
            return findBy(BlackMisc::Predicates::MemberEqual<T>(key1, value1, key2, value2, key3, value3));
        }

        /*!
         * \brief Return a copy containing only those elements matching a given value map.
         * \param valueMap
         * \return
         */
        C<T> findBy(const CValueMap &valueMap) const
        {
            return findBy([ & ](const T &value) { return value == valueMap; });
        }

        /*!
         * \brief Return true if there is an element for which a given predicate returns true
         * \param p
         * \return
         */
        template <class Predicate>
        bool contains(Predicate p) const
        {
            return std::any_of(derived().begin(), derived().end(), p);
        }

        /*!
         * \brief Return a copy containing only those elements matching a particular key/value pair.
         * \param key1 A pointer to a member function of T.
         * \param value1 Will be compared to the return value of key1.
         * \return
         */
        template <class K1, class V1>
        bool contains(K1 key1, V1 value1) const
        {
            return contains(BlackMisc::Predicates::MemberEqual<T>(key1, value1));
        }

        /*!
         * \brief Remove elements for which a given predicate returns true.
         * \param p
         */
        template <class Predicate>
        void removeIf(Predicate p)
        {
            std::remove_if(derived().begin(), derived().end(), p);
        }

        /*!
         * \brief Remove elements matching a particular key/value pair.
         * \param key1 A pointer to a member function of T.
         * \param value1 Will be compared to the return value of key1.
         */
        template <class K1, class V1>
        void removeIf(K1 key1, V1 value1)
        {
            removeIf(BlackMisc::Predicates::MemberEqual<T>(key1, value1));
        }

    public: // CValueObject overrides
        virtual QVariant toQVariant() const { return QVariant::fromValue(derived()); }

        // comparing containers by hash will only compare their addresses
        virtual uint getValueHash() const { return qHash(&derived()); }

    protected: // CValueObject overrides
        virtual QString convertToQString(bool i18n = false) const
        {
            QString str;
            // qualifying stringify with this-> to workaround bug in GCC 4.7.2 http://gcc.gnu.org/bugzilla/show_bug.cgi?id=56402
            std::for_each(derived().begin(), derived().end(), [ & ](const T &value) { str += (str.isEmpty() ? "{" : ",") + this->stringify(value, i18n); });
            if (str.isEmpty()) { str = "{"; }
            return str += "}";
        }

        virtual void marshallToDbus(QDBusArgument &argument) const
        {
            argument.beginArray();
            std::for_each(derived().begin(), derived().end(), [ & ](const T &value) { argument << value; });
            argument.endArray();
        }

        virtual void unmarshallFromDbus(const QDBusArgument &argument)
        {
            argument.beginArray();
            while (!argument.atEnd()) { T value; argument >> value; derived().insert(value); }
            argument.endArray();
        }

    private:
        C<T> &derived() { return static_cast<C<T> &>(*this); }
        const C<T> &derived() const { return static_cast<const C<T> &>(*this); }

        template <class U> static QString stringify(const U &obj, bool i18n) { return obj.toQString(i18n); }
        static QString stringify(int n, bool i18n) { return i18n ? QLocale().toString(n) : QString::number(n); }
        static QString stringify(uint n, bool i18n) { return i18n ? QLocale().toString(n) : QString::number(n); }
        static QString stringify(qlonglong n, bool i18n) { return i18n ? QLocale().toString(n) : QString::number(n); }
        static QString stringify(qulonglong n, bool i18n) { return i18n ? QLocale().toString(n) : QString::number(n); }
        static QString stringify(double n, bool i18n) { return i18n ? QLocale().toString(n) : QString::number(n); }
    };

}

#endif // guard