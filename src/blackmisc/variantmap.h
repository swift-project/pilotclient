/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_VARIANTMAP_H
#define BLACKMISC_VARIANTMAP_H

#include "blackmisc/variant.h"
#include "blackmisc/dictionary.h"

namespace BlackMisc
{

    /*!
     * Map of { QString, CVariant } pairs.
     *
     * Using QMap as implementation type so keys are sorted.
     */
    class BLACKMISC_EXPORT CVariantMap :
        public CDictionary<QString, CVariant, QMap>,
        public Mixin::MetaType<CVariantMap>
    {
    public:
        BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CVariantMap)

        //! Default constructor.
        CVariantMap() {}

        //! Initializer list constructor.
        CVariantMap(std::initializer_list<std::pair<QString, CVariant>> il) : CDictionary(il) {}

        //! Copy constructor.
        CVariantMap(const CVariantMap &) = default;

        //! Move constructor.
        CVariantMap(CVariantMap &&other) noexcept : CDictionary(std::move(other)) {}

        //! Copy assignment operator.
        CVariantMap &operator =(const CVariantMap &other) { CDictionary::operator =(other); return *this; }

        //! Move assignment operator.
        CVariantMap &operator =(CVariantMap &&other) noexcept { CDictionary::operator =(std::move(other)); return *this; }

        //! \copydoc BlackMisc::CValueObject::toJson
        QJsonObject toJson() const;

        //! \copydoc BlackMisc::CValueObject::convertFromJson
        void convertFromJson(const QJsonObject &json);
    };

}

Q_DECLARE_METATYPE(BlackMisc::CVariantMap)

#endif
