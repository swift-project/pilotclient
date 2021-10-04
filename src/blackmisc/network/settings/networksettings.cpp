/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/network/settings/networksettings.h"
#include "blackmisc/stringutils.h"

#include <QtGlobal>
#include <QStringBuilder>

BLACK_DEFINE_VALUEOBJECT_MIXINS(BlackMisc::Network::Settings, CNetworkSettings)

namespace BlackMisc::Network::Settings
{
    QString CNetworkSettings::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        return QStringLiteral("NOT USED");
    }

    QVariant CNetworkSettings::propertyByIndex(CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        // const ColumnIndex i = index.frontCasted<ColumnIndex>();
        // switch (i)
        // {
        // case IndexDynamicOffsetTime: return CVariant::fromValue(m_dynamicOffsetTimes);
        // default: break;
        // }
        return CValueObject::propertyByIndex(index);
    }

    void CNetworkSettings::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself()) { (*this) = variant.value<CNetworkSettings>(); return; }
        // const ColumnIndex i = index.frontCasted<ColumnIndex>();
        // switch (i)
        // {
        // case IndexDynamicOffsetTime: this->setDynamicOffsetTimes(variant.toBool()); break;
        // default: break;
        // }
        CValueObject::setPropertyByIndex(index, variant);
    }
} // ns
