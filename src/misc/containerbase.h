// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_CONTAINERBASE_H
#define SWIFT_MISC_CONTAINERBASE_H

#include <algorithm>

#include <QStringList>

#include "misc/json.h"
#include "misc/jsonexception.h"
#include "misc/mixin/mixindbus.h"
#include "misc/mixin/mixinjson.h"
#include "misc/mixin/mixinmetatype.h"
#include "misc/mixin/mixinstring.h"
#include "misc/predicates.h"
#include "misc/range.h"

namespace swift::misc
{
    //! Class providing static helper methods for different containers
    class CContainerHelper
    {
    public:
        //! Stringify value object
        template <class U>
        static QString stringify(const U &obj, bool i18n)
        {
            return obj.toQString(i18n);
        }
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
        //! Stringify pair
        template <class A, class B>
        static QString stringify(const std::pair<A, B> &pair, bool i18n)
        {
            return stringify(pair.first, i18n) + ":" + stringify(pair.second, i18n);
        }
    };

    /*!
     * Base class for CCollection and CSequence adding mutating operations and CValueObject facility on top of
     * CRangeBase.
     */
    template <class Derived>
    class CContainerBase :
        public CRangeBase<Derived>,
        public mixin::MetaType<Derived>,
        public mixin::DBusOperators<Derived>,
        public mixin::JsonOperators<Derived>,
        public mixin::String<Derived>
    {
    public:
        //! \copydoc swift::misc::CValueObject::compare
        friend int compare(const Derived &a, const Derived &b)
        {
            for (auto i = a.cbegin(), j = b.cbegin(); i != a.cend() && j != b.cend(); ++i, ++j)
            {
                if (*i < *j) { return -1; }
                if (*j < *i) { return 1; }
            }
            if (a.size() < b.size()) { return -1; }
            if (b.size() < a.size()) { return 1; }
            return 0;
        }

        //! Return a new container of a different type, containing the same elements as this one.
        //! \tparam Other the type of the new container.
        template <template <class> class Other>
        auto to() const
        {
            return to(Other<typename Derived::value_type>());
        }

        //! Return a new container of a different type, containing the same elements as this one.
        //! \tparam Other the type of the new container.
        //! \tparam T element type of the new container.
        //! \param other an initial value for the new container; will be copied.
        template <template <class> class Other, class T>
        Other<T> to(Other<T> other) const
        {
            for (auto it = derived().cbegin(); it != derived().cend(); ++it) { other.push_back(*it); }
            return other;
        }

        //! Remove elements matching some particular key/value pair(s).
        //! \param k0 A pointer to a member function of T.
        //! \param v0 A value to compare against the value returned by k0.
        //! \param keysValues Zero or more additional pairs of { pointer to member function of T, value to compare it
        //! against }. \return The number of elements removed.
        template <class K0, class V0, class... KeysValues>
        int removeIf(K0 k0, V0 v0, KeysValues... keysValues)
        {
            return derived().removeIf(swift::misc::predicates::MemberEqual(k0, v0, keysValues...));
        }

        //! Simplifies composition, returns 0 for performance
        friend size_t qHash(const Derived &) { return 0; } // clazy:exclude=qhash-namespace

        //! \copydoc swift::misc::mixin::JsonByMetaClass::toJson
        QJsonObject toJson() const
        {
            QJsonArray array;
            QJsonObject json;
            for (auto it = derived().cbegin(); it != derived().cend(); ++it) { array << (*it); }
            json.insert("containerbase", array);
            return json;
        }

        //! Convenience function JSON as string
        QString toJsonString(QJsonDocument::JsonFormat format = QJsonDocument::Indented) const
        {
            QJsonDocument jsonDoc(toJson());
            return jsonDoc.toJson(format);
        }

        //! \copydoc swift::misc::mixin::JsonByMetaClass::convertFromJson
        //! \throws CJsonException
        void convertFromJson(const QJsonObject &json)
        {
            derived().clear();
            QJsonValue value = json.value("containerbase");
            if (value.isUndefined()) { throw CJsonException("Missing 'containerbase'"); }
            QJsonArray array = value.toArray();
            int index = 0;
            for (auto i = array.begin(); i != array.end(); ++i)
            {
                CJsonScope scope("containerbase", index++);
                Q_UNUSED(scope);
                QJsonValueRef ref = (*i);
                typename Derived::value_type val;
                ref >> val;
                derived().push_back(std::move(val));
            }
        }

        //! Assign from JSON object string
        void convertFromJson(const QString &jsonString, bool acceptCacheFormat = false)
        {
            if (jsonString.isEmpty()) { return; }
            const QJsonObject jsonObject = json::jsonObjectFromString(jsonString, acceptCacheFormat);
            this->convertFromJson(jsonObject);
        }

        //! Static version of convertFromJson
        static Derived fromJson(const QJsonObject &json)
        {
            Derived derived;
            derived.convertFromJson(json);
            return derived;
        }

        //! Static version of convertFromJson
        static Derived fromJson(const QString &jsonString, bool acceptCacheJson = false)
        {
            Derived obj;
            if (jsonString.isEmpty()) { return obj; }
            const QJsonObject jsonObj =
                acceptCacheJson ? json::swiftDataObjectValue(jsonString) : json::jsonObjectFromString(jsonString);
            obj.convertFromJson(jsonObj);
            return obj;
        }

        //! Static version of convertFromJson
        static Derived fromJsonNoThrow(const QString &jsonString, bool acceptCacheJson, bool &success, QString &errMsg)
        {
            success = false;
            Derived obj;
            try
            {
                if (jsonString.isEmpty()) { return obj; }
                const QJsonObject jsonObj =
                    acceptCacheJson ? json::swiftDataObjectValue(jsonString) : json::jsonObjectFromString(jsonString);
                obj.convertFromJson(jsonObj);
                success = true;
            }
            catch (const CJsonException &ex)
            {
                errMsg = ex.toString("JSON conversion");
            }
            return obj;
        }

        //! Call convertFromJson, catch any CJsonException that is thrown and return it as CStatusMessage.
        CStatusMessage convertFromJsonNoThrow(const QJsonObject &json, const CLogCategoryList &categories,
                                              const QString &prefix); // implemented in statusmessage.h

        //! Call convertFromJson, catch any CJsonException that is thrown and return it as CStatusMessage.
        CStatusMessage convertFromJsonNoThrow(const QString &jsonString, const CLogCategoryList &categories,
                                              const QString &prefix); // implemented in statusmessage.h

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const
        {
            QString str;
            for (const auto &value : derived())
            {
                str += (str.isEmpty() ? "{" : ", ") + CContainerHelper::stringify(value, i18n);
            }
            if (str.isEmpty()) { str = "{"; }
            return str += "}";
        }

        //! To string list
        QStringList toStringList(bool i18n = false) const
        {
            QStringList sl;
            for (const auto &obj : this->derived()) { sl.append(obj.toQString(i18n)); }
            return sl;
        }

    protected:
        //! \copydoc swift::misc::CValueObject::getMetaTypeId
        int getMetaTypeId() const { return qMetaTypeId<Derived>(); }

    public:
        //! \copydoc swift::misc::CValueObject::marshallToDbus
        void marshallToDbus(QDBusArgument &argument) const
        {
            argument.beginArray(qMetaTypeId<typename Derived::value_type>());
            std::for_each(derived().cbegin(), derived().cend(), [&](const auto &value) { argument << value; });
            argument.endArray();
        }

        //! \copydoc swift::misc::CValueObject::unmarshallFromDbus
        void unmarshallFromDbus(const QDBusArgument &argument)
        {
            derived().clear();
            argument.beginArray();
            while (!argument.atEnd())
            {
                typename Derived::value_type value;
                argument >> value;
                derived().push_back(value);
            }
            argument.endArray();
        }

    private:
        Derived &derived() { return static_cast<Derived &>(*this); }
        const Derived &derived() const { return static_cast<const Derived &>(*this); }
    };
} // namespace swift::misc

#endif // SWIFT_MISC_CONTAINERBASE_H
