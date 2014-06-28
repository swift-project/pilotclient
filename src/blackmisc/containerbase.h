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
#include "indexvariantmap.h"
#include "predicates.h"
#include "json.h"
#include <algorithm>

#define _SCL_SECURE_NO_WARNINGS // suppress MSVC unchecked iterator warning for std::transform

namespace BlackMisc
{

    //! Class providing static helper methods for different containers
    class CContainerHelper
    {
    public:
        //! Stringify value object
        template <class U> static QString stringify(const U &obj, bool i18n) { return obj.toQString(i18n); }
        //! Stringify int
        static QString stringify(int n, bool i18n) { return i18n ? QLocale().toString(n) : QString::number(n); }
        //! Stringify uint
        static QString stringify(uint n, bool i18n) { return i18n ? QLocale().toString(n) : QString::number(n); }
        //! Stringify qlonglong
        static QString stringify(qlonglong n, bool i18n) { return i18n ? QLocale().toString(n) : QString::number(n); }
        //! Stringify qulonglong
        static QString stringify(qulonglong n, bool i18n) { return i18n ? QLocale().toString(n) : QString::number(n); }
        //! Stringify double
        static QString stringify(double n, bool i18n) { return i18n ? QLocale().toString(n) : QString::number(n); }
        //! Stringify QString
        static QString stringify(QString str, bool /*i18n*/) { return str; }
    };

    /*!
     * \brief Base class for CCollection and CSequence implementing their algorithms.
     */
    template <template <class> class C, class T>
    class CContainerBase : public CValueObject
    {
    public:
        /*!
         * \brief Return a new container of a different type, containing the same elements as this one.
         * \tparam Other the type of the new container.
         * \param other an optional initial value for the new container; will be copied.
         */
        template <template <class> class Other>
        Other<T> to(Other<T> other = Other<T>()) const
        {
            for (auto it = derived().cbegin(); it != derived().cend(); ++it) { other.push_back(*it); }
            return other;
        }

        /*!
         * \brief Return a copy containing only those elements for which a given predicate returns true.
         */
        template <class Predicate>
        C<T> findBy(Predicate p) const
        {
            C<T> result = derived();
            result.erase(std::remove_if(result.begin(), result.end(), [ = ](const T & value) { return !p(value); }), result.end());
            return result;
        }

        /*!
         * \brief Return a copy containing only those elements matching some particular key/value pair(s).
         * \param k0 A pointer to a member function of T.
         * \param v0 A value to compare against the value returned by k0.
         * \param keysValues Zero or more additional pairs of { pointer to member function of T, value to compare it against }.
         */
        template <class K0, class V0, class... KeysValues>
        C<T> findBy(K0 k0, V0 v0, KeysValues... keysValues) const
        {
            return findBy(BlackMisc::Predicates::MemberEqual<T>(k0, v0, keysValues...));
        }

        /*!
         * \brief Return a copy containing only those elements matching a given value map.
         */
        C<T> findBy(const CIndexVariantMap &valueMap) const
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
            return contains(BlackMisc::Predicates::MemberEqual<T>(k0, v0, keysValues...));
        }

        /*!
         * \brief Remove elements for which a given predicate returns true.
         * \pre The sequence must be initialized.
         */
        template <class Predicate>
        void removeIf(Predicate p)
        {
            for (auto it = derived().begin(); it != derived().end();)
            {
                if (p(*it)) { it = derived().erase(it); }
                else { ++it; }
            }
        }

        /*!
         * \brief Remove elements matching some particular key/value pair(s).
         * \param k0 A pointer to a member function of T.
         * \param v0 A value to compare against the value returned by k0.
         * \param keysValues Zero or more additional pairs of { pointer to member function of T, value to compare it against }.
         */
        template <class K0, class V0, class... KeysValues>
        void removeIf(K0 k0, V0 v0, KeysValues... keysValues)
        {
            removeIf(BlackMisc::Predicates::MemberEqual<T>(k0, v0, keysValues...));
        }

    public:
        //! \copydoc BlackMisc::CValueObject::toQVariant
        virtual QVariant toQVariant() const override { return QVariant::fromValue(derived()); }

        //! \copydoc BlackMisc::CValueObject::getValueHash
        virtual uint getValueHash() const override { return qHash(&derived()); }

        //! \copydoc CValueObject::toJson
        virtual QJsonObject toJson() const override
        {
            QJsonArray array;
            QJsonObject json;
            for (auto it = derived().cbegin(); it != derived().cend(); ++it)
            {
                array << (*it);
            }
            json.insert("containerbase", array);
            return json;
        }

        //! \copydoc CValueObject::fromJson
        void fromJson(const QJsonObject &json) override
        {
            QJsonArray array = json.value("containerbase").toArray();
            for (auto i = array.begin(); i != array.end(); ++i)
            {
                QJsonValueRef ref = (*i);
                T value;
                ref >> value;
                derived().insert(value);
            }
        }

    protected: // CValueObject overrides
        //! \copydoc BlackMisc::CValueObject::convertToQString
        virtual QString convertToQString(bool i18n = false) const override
        {
            QString str;
            // qualifying stringify with this-> to workaround bug in GCC 4.7.2 http://gcc.gnu.org/bugzilla/show_bug.cgi?id=56402
            std::for_each(derived().cbegin(), derived().cend(), [ & ](const T & value) { str += (str.isEmpty() ? "{" : ", ") + CContainerHelper::stringify(value, i18n); });
            if (str.isEmpty()) { str = "{"; }
            return str += "}";
        }

        //! \copydoc BlackMisc::CValueObject::getMetaTypeId
        virtual int getMetaTypeId() const override { return qMetaTypeId<C<T>>(); }

        //! \copydoc BlackMisc::CValueObject::isA
        virtual bool isA(int metaTypeId) const override
        {
            if (metaTypeId == qMetaTypeId<C<T>>()) { return true; }
            return CValueObject::isA(metaTypeId);
        }

        //! \copydoc BlackMisc::CValueObject::compareImpl
        virtual int compareImpl(const CValueObject &other) const override
        {
            const auto &o = static_cast<const CContainerBase &>(other);
            if (derived().size() < o.derived().size()) { return -1; }
            if (derived().size() > o.derived().size()) { return 1; }
            //for (auto i1 = derived().cbegin(), i2 = o.derived().cbegin(); i1 != derived().cend() && i2 != o.derived().cend(); ++i1, ++i2)
            //{
            //    if (*i1 < *i2) { return -1; }
            //    if (*i1 > *i2) { return 1; }
            //}
            return 0;
        }

        //! \copydoc BlackMisc::CValueObject::marshallToDbus
        virtual void marshallToDbus(QDBusArgument &argument) const override
        {
            argument.beginArray(qMetaTypeId<T>());
            std::for_each(derived().cbegin(), derived().cend(), [ & ](const T & value) { argument << value; });
            argument.endArray();
        }

        //! \copydoc BlackMisc::CValueObject::unmarshallFromDbus
        virtual void unmarshallFromDbus(const QDBusArgument &argument) override
        {
            argument.beginArray();
            while (!argument.atEnd()) { T value; argument >> value; derived().insert(value); }
            argument.endArray();
        }

    private:
        C<T> &derived() { return static_cast<C<T> &>(*this); }
        const C<T> &derived() const { return static_cast<const C<T> &>(*this); }
    };

}

#endif // guard
