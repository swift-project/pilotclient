/* Copyright (C) 2017
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/data/launchersetup.h"
#include "blackmisc/stringutils.h"

using namespace BlackMisc;

BLACK_DEFINE_VALUEOBJECT_MIXINS(BlackCore::Data, CLauncherSetup)

namespace BlackCore::Data
{
    QString CLauncherSetup::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n)
        return QStringLiteral("DBus: %1 frameless: %2 mode: %3").arg(m_dBusAddress, boolToYesNo(m_windowFrameless)).arg(m_coreMode);
    }

    QVariant CLauncherSetup::propertyByIndex(BlackMisc::CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexDBusAddress:     return QVariant::fromValue(m_dBusAddress);
        case IndexFramelessWindow: return QVariant::fromValue(m_windowFrameless);
        case IndexCoreMode:        return QVariant::fromValue(m_coreMode);
        case IndexAudioMode:       return QVariant::fromValue(m_audioMode);
        default:                   return CValueObject::propertyByIndex(index);
        }
    }

    void CLauncherSetup::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself()) { (*this) = variant.value<CLauncherSetup>(); return; }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexDBusAddress:       this->setDBusAddress(variant.toString());   break;
        case IndexFramelessWindow:   m_windowFrameless = variant.toBool();       break;
        case IndexCoreMode:          m_coreMode  = variant.toInt();              break;
        case IndexAudioMode:         m_audioMode = variant.toInt();              break;
        default:
            CValueObject::setPropertyByIndex(index, variant);
            break;
        }
    }
} // ns
