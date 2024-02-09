// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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
            QJsonObject toJson() const;

            //! Convenience function JSON as string
            QString toJsonString(QJsonDocument::JsonFormat format = QJsonDocument::Indented) const;

            //! Assign from JSON object
            void convertFromJson(const QJsonObject &json);

            //! Assign from JSON object string
            void convertFromJson(const QString &jsonString, bool acceptCacheFormat = false);

            //! Get object from QJsonObject
            template <class DerivedObj = Derived>
            static DerivedObj fromJson(const QJsonObject &json);

            //! Get object from JSON string
            template <class DerivedObj = Derived>
            static DerivedObj fromJson(const QString &jsonString, bool acceptCacheJson = false);

            //! Get object from JSON string
            template <class DerivedObj = Derived>
            static DerivedObj fromJsonNoThrow(const QString &jsonString, bool acceptCacheJson, bool &success, QString &errMsg);

        private:
            const Derived *derived() const;
            Derived *derived();

            template <typename T>
            static QJsonObject baseToJson(const T *base);
            template <typename T>
            static void baseConvertFromJson(T *base, const QJsonObject &json);
            static QJsonObject baseToJson(const void *);
            static void baseConvertFromJson(void *, const QJsonObject &);
            static QJsonObject baseToJson(const CEmpty *);
            static void baseConvertFromJson(CEmpty *, const QJsonObject &);
        };

        template <class Derived>
        QJsonObject JsonByMetaClass<Derived>::toJson() const
        {
            QJsonObject json;
            introspect<Derived>().forEachMember([&, this](auto member) {
                if constexpr (!decltype(member)::has(MetaFlags<DisabledForJson>()))
                {
                    json << std::make_pair(CExplicitLatin1String(member.latin1Name()), std::cref(member.in(*this->derived())));
                }
            });
            return Json::appendJsonObject(json, baseToJson(static_cast<const TBaseOfT<Derived> *>(derived())));
        }

        template <class Derived>
        QString JsonByMetaClass<Derived>::toJsonString(QJsonDocument::JsonFormat format) const
        {
            QJsonDocument jsonDoc(toJson());
            return jsonDoc.toJson(format);
        }

        template <class Derived>
        void JsonByMetaClass<Derived>::convertFromJson(const QJsonObject &json)
        {
            baseConvertFromJson(static_cast<TBaseOfT<Derived> *>(derived()), json);
            introspect<Derived>().forEachMember([&, this](auto member) {
                if constexpr (!decltype(member)::has(MetaFlags<DisabledForJson>()))
                {
                    const auto value = json.value(CExplicitLatin1String(member.latin1Name()));
                    if (value.isUndefined())
                    {
                        constexpr bool required = decltype(member)::has(MetaFlags<RequiredForJson>());
                        // QLatin1String used instead of QStringLiteral below since the latter causes an internal compiler bug
                        // in GCC 8 and higher
                        if (required) { throw CJsonException(QLatin1String("Missing required member '%1'").arg(member.latin1Name())); } // cppcheck-suppress knownConditionTrueFalse
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

        template <class Derived>
        void JsonByMetaClass<Derived>::convertFromJson(const QString &jsonString, bool acceptCacheFormat)
        {
            const QJsonObject jsonObject = BlackMisc::Json::jsonObjectFromString(jsonString, acceptCacheFormat);
            convertFromJson(jsonObject);
        }

        template <class Derived>
        template <class DerivedObj>
        DerivedObj JsonByMetaClass<Derived>::fromJson(const QJsonObject &json)
        {
            DerivedObj obj;
            obj.convertFromJson(json);
            return obj;
        }

        template <class Derived>
        template <class DerivedObj>
        DerivedObj JsonByMetaClass<Derived>::fromJson(const QString &jsonString, bool acceptCacheJson)
        {
            DerivedObj obj;
            if (jsonString.isEmpty()) { return obj; }
            const QJsonObject jsonObj = acceptCacheJson ? Json::swiftDataObjectValue(jsonString) : Json::jsonObjectFromString(jsonString);
            obj.convertFromJson(jsonObj);
            return obj;
        }

        template <class Derived>
        template <class DerivedObj>
        DerivedObj JsonByMetaClass<Derived>::fromJsonNoThrow(const QString &jsonString, bool acceptCacheJson, bool &success, QString &errMsg)
        {
            success = false;
            DerivedObj obj;
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

        template <class Derived>
        const Derived *JsonByMetaClass<Derived>::derived() const
        {
            return static_cast<const Derived *>(this);
        }

        template <class Derived>
        Derived *JsonByMetaClass<Derived>::derived()
        {
            return static_cast<Derived *>(this);
        }

        template <class Derived>
        template <typename T>
        QJsonObject JsonByMetaClass<Derived>::baseToJson(const T *base)
        {
            return base->toJson();
        }

        template <class Derived>
        template <typename T>
        void JsonByMetaClass<Derived>::baseConvertFromJson(T *base, const QJsonObject &json)
        {
            base->convertFromJson(json);
        }

        template <class Derived>
        QJsonObject JsonByMetaClass<Derived>::baseToJson(const void *)
        {
            return {};
        }

        template <class Derived>
        void JsonByMetaClass<Derived>::baseConvertFromJson(void *, const QJsonObject &)
        {}

        template <class Derived>
        QJsonObject JsonByMetaClass<Derived>::baseToJson(const CEmpty *)
        {
            return {};
        }

        template <class Derived>
        void JsonByMetaClass<Derived>::baseConvertFromJson(CEmpty *, const QJsonObject &)
        {}

/*!
 * When a derived class and a base class both inherit from Mixin::JsonByTuple,
 * the derived class uses this macro to disambiguate the inherited members.
 */
#define BLACKMISC_DECLARE_USING_MIXIN_JSON(DERIVED)             \
    using ::BlackMisc::Mixin::JsonByMetaClass<DERIVED>::toJson; \
    using ::BlackMisc::Mixin::JsonByMetaClass<DERIVED>::convertFromJson;
    } // Mixin ns
} // guard

#endif // guard
