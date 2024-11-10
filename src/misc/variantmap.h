// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_VARIANTMAP_H
#define SWIFT_MISC_VARIANTMAP_H

#include "misc/swiftmiscexport.h"
#include "misc/mixin/mixinhash.h"
#include "misc/dictionary.h"
#include "misc/variant.h"

#include <QJsonObject>
#include <QMap>
#include <QMetaType>
#include <QString>
#include <initializer_list>
#include <utility>

namespace swift::misc
{

    class CStatusMessageList;

    /*!
     * Map of { QString, CVariant } pairs.
     *
     * Using QMap as implementation type so keys are sorted.
     */
    class SWIFT_MISC_EXPORT CVariantMap :
        public CDictionary<QString, CVariant, QMap>,
        public mixin::MetaType<CVariantMap>
    {
    public:
        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CVariantMap)
        using CDictionary::CDictionary;

        //! Default constructor.
        CVariantMap() {}

        //! Copy constructor.
        CVariantMap(const CVariantMap &) = default;

        //! Move constructor.
        CVariantMap(CVariantMap &&other) noexcept : CDictionary(std::move(other)) {}

        //! Copy assignment operator.
        CVariantMap &operator=(const CVariantMap &other)
        {
            CDictionary::operator=(other);
            return *this;
        }

        //! Move assignment operator.
        CVariantMap &operator=(CVariantMap &&other) noexcept
        {
            CDictionary::operator=(std::move(other));
            return *this;
        }

        //! Destructor.
        ~CVariantMap() = default;

        //! Insert values from this map into an existing JSON object.
        QJsonObject &mergeToJson(QJsonObject &json) const;

        //! \copydoc swift::misc::CValueObject::toJson
        QJsonObject toJson() const;

        //! \copydoc swift::misc::CValueObject::convertFromJson
        void convertFromJson(const QJsonObject &json);

        //! \copydoc swift::misc::CValueObject::convertFromJson
        //! Convert only keys present in list argument.
        void convertFromJson(const QJsonObject &json, const QStringList &keys);

        //! Call convertFromJson, catch any CJsonException that are thrown and return them as CStatusMessage.
        CStatusMessageList convertFromJsonNoThrow(const QJsonObject &json, const CLogCategoryList &categories, const QString &prefix);

        //! Call convertFromJson, catch any CJsonException that are thrown and return them as CStatusMessage.
        CStatusMessageList convertFromJsonNoThrow(const QJsonObject &json, const QStringList &keys, const CLogCategoryList &categories, const QString &prefix);

        //! Insert values from this map into an existing compact JSON object.
        QJsonObject &mergeToMemoizedJson(QJsonObject &json) const;

        //! To compact JSON format.
        QJsonObject toMemoizedJson() const;

        //! From compact JSON format.
        void convertFromMemoizedJson(const QJsonObject &json);

        //! From compact JSON format.
        //! Convert only keys present in list argument.
        void convertFromMemoizedJson(const QJsonObject &json, const QStringList &keys);

        //! Call convertFromMemoizedJson, catch any CJsonException that are thrown and return them as CStatusMessage.
        CStatusMessageList convertFromMemoizedJsonNoThrow(const QJsonObject &json, const CLogCategoryList &categories, const QString &prefix);

        //! Call convertFromMemoizedJson, catch any CJsonException that are thrown and return them as CStatusMessage.
        CStatusMessageList convertFromMemoizedJsonNoThrow(const QJsonObject &json, const QStringList &keys, const CLogCategoryList &categories, const QString &prefix);
    };

}

Q_DECLARE_METATYPE(swift::misc::CVariantMap)

#endif
