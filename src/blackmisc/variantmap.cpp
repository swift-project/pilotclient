/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/variantmap.h"

#include <QJsonValue>

namespace BlackMisc
{

    QJsonObject CVariantMap::toJson() const
    {
        QJsonObject json;
        for (auto it = cbegin(); it != cend(); ++it)
        {
            json.insert(it.key(), it.value().toJson());
        }
        return json;
    }

    void CVariantMap::convertFromJson(const QJsonObject &json)
    {
        clear();
        for (auto it = json.begin(); it != json.end(); ++it)
        {
            CVariant value;
            value.convertFromJson(it.value().toObject());
            implementationOf(*this).insert(cend(), it.key(), value);
        }
    }

}
