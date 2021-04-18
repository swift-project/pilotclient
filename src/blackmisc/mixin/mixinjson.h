/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_MIXIN_MIXINJSON_H
#define BLACKMISC_MIXIN_MIXINJSON_H

#include "blackmisc/json.h"
#include "blackmisc/inheritancetraits.h"

#include <QByteArray>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonValueRef>
#include <QStringList>
#include <QtGlobal>
#include <type_traits>
#include <utility>

namespace BlackMisc
{
    class CEmpty;

    namespace Mixin
    {
        /*!
         * CRTP class template which will generate marshalling operators for a derived class with its own marshalling implementation.
         *
         * \tparam Must implement public methods QJsonObject toJson() const and void convertFromJson(const QJsonObject &json).
         */
        template <class Derived>
        class JsonOperators
        {
        public:
            //! operator >> for JSON
            friend const QJsonObject &operator>>(const QJsonObject &json, Derived &obj)
            {
                obj.convertFromJson(json);
                return json;
            }

            //! operator >> for JSON
            friend const QJsonValue &operator>>(const QJsonValue &json, Derived &obj)
            {
                obj.convertFromJson(json.toObject());
                return json;
            }

            //! operator >> for JSON
            friend QJsonValueRef operator>>(QJsonValueRef json, Derived &obj)
            {
                obj.convertFromJson(json.toObject());
                return json;
            }

            //! operator << for JSON
            friend QJsonArray &operator<<(QJsonArray &json, const Derived &obj)
            {
                json.append(obj.toJson());
                return json;
            }

            //! operator << for JSON
            friend QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, const Derived &> &value)
            {
                json.insert(value.first, QJsonValue(value.second.toJson()));
                return json;
            }

            //! operator << for JSON
            friend QJsonObject &operator<<(QJsonObject &json, const std::pair<CExplicitLatin1String, const Derived &> &value)
            {
                json[value.first] = QJsonValue(value.second.toJson());
                return json;
            }
        };

        /*!
         * CRTP class template from which a derived class can inherit common methods dealing with JSON by metatuple.
         *
         * \see BLACKMISC_DECLARE_USING_MIXIN_JSON
         */
        template <class Derived>
        class JsonByMetaClass : public JsonOperators<Derived>
        {
        public:
            //! Cast to JSON object
            QJsonObject toJson() const
            {
                QJsonObject json;
                introspect<Derived>().forEachMember([ &, this ](auto member)
                {
                    if constexpr (!decltype(member)::has(MetaFlags<DisabledForJson>()))
                    {
                        json << std::make_pair(CExplicitLatin1String(member.latin1Name()), std::cref(member.in(*this->derived())));
                    }
                });
                return Json::appendJsonObject(json, baseToJson(static_cast<const TBaseOfT<Derived> *>(derived())));
            }

            //! Convenience function JSON as string
            QString toJsonString(QJsonDocument::JsonFormat format = QJsonDocument::Indented) const
            {
                QJsonDocument jsonDoc(toJson());
                return jsonDoc.toJson(format);
            }

            //! Assign from JSON object
            void convertFromJson(const QJsonObject &json)
            {
                baseConvertFromJson(static_cast<TBaseOfT<Derived> *>(derived()), json);
                introspect<Derived>().forEachMember([ &, this ](auto member)
                {
                    if constexpr (!decltype(member)::has(MetaFlags<DisabledForJson>()))
                    {
                        const auto value = json.value(CExplicitLatin1String(member.latin1Name()));
                        if (value.isUndefined())
                        {
                            constexpr bool required = false; //! \fixme add RequiredForJson flag in metaclass system
                            // cppcheck-suppress knownConditionTrueFalse
                            // QLatin1String used instead of QStringLiteral below since the latter causes an internal compiler bug
                            // in GCC 8 and higher
                            if (required) { throw CJsonException(QLatin1String("Missing required member '%1'").arg(member.latin1Name())); }
                        }
                        else
                        {
                            CJsonScope scope(member.latin1Name());
                            Q_UNUSED(scope);
                            value >> member.in(*this->derived());
                        }
                    }
                });
            }

            //! Assign from JSON object string
            void convertFromJson(const QString &jsonString, bool acceptCacheFormat = false)
            {
                const QJsonObject jsonObject = BlackMisc::Json::jsonObjectFromString(jsonString, acceptCacheFormat);
                convertFromJson(jsonObject);
            }

            //! Get object from QJsonObject
            template<class DerivedObj = Derived>
            static DerivedObj fromJson(const QJsonObject &json)
            {
                DerivedObj obj;
                obj.convertFromJson(json);
                return obj;
            }

            //! Get object from JSON string
            template<class DerivedObj = Derived>
            static DerivedObj fromJson(const QString &jsonString, bool acceptCacheJson = false)
            {
                DerivedObj obj;
                if (jsonString.isEmpty()) { return obj; }
                const QJsonObject jsonObj = acceptCacheJson ? Json::swiftDataObjectValue(jsonString) : Json::jsonObjectFromString(jsonString);
                obj.convertFromJson(jsonObj);
                return obj;
            }

            //! Get object from JSON string
            template<class DerivedObj = Derived>
            static Derived fromJsonNoThrow(const QString &jsonString, bool acceptCacheJson, bool &success, QString &errMsg)
            {
                success = false;
                Derived obj;
                try
                {
                    if (jsonString.isEmpty()) { return obj; }
                    const QJsonObject jsonObj = acceptCacheJson ? Json::swiftDataObjectValue(jsonString) : Json::jsonObjectFromString(jsonString);
                    obj.convertFromJson(jsonObj);
                    success = true;
                }
                catch (const CJsonException &ex)
                {
                    errMsg = ex.toString("JSON conversion");
                }
                return obj;
            }

        private:
            const Derived *derived() const { return static_cast<const Derived *>(this); }
            Derived *derived() { return static_cast<Derived *>(this); }

            template <typename T> static QJsonObject baseToJson(const T *base) { return base->toJson(); }
            template <typename T> static void baseConvertFromJson(T *base, const QJsonObject &json) { base->convertFromJson(json); }
            static QJsonObject baseToJson(const void *) { return {}; }
            static void baseConvertFromJson(void *, const QJsonObject &) {}
            static QJsonObject baseToJson(const CEmpty *) { return {}; }
            static void baseConvertFromJson(CEmpty *, const QJsonObject &) {}
        };

        /*!
         * When a derived class and a base class both inherit from Mixin::JsonByTuple,
         * the derived class uses this macro to disambiguate the inherited members.
         */
        #define BLACKMISC_DECLARE_USING_MIXIN_JSON(DERIVED)                      \
            using ::BlackMisc::Mixin::JsonByMetaClass<DERIVED>::toJson;          \
            using ::BlackMisc::Mixin::JsonByMetaClass<DERIVED>::convertFromJson;
    } // Mixin ns
} // guard

#endif // guard
