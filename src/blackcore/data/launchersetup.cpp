/* Copyright (C) 2017
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "launchersetup.h"
#include "blackmisc/stringutils.h"

using namespace BlackMisc;

namespace BlackCore
{
    namespace Data
    {
        QString CLauncherSetup::convertToQString(bool i18n) const
        {
            Q_UNUSED(i18n);
            return QStringLiteral("DBus: %1 frameless: %2 mode: %3").arg(m_dBusAddress, boolToYesNo(m_windowFrameless)).arg(m_coreMode);
        }

        CVariant CLauncherSetup::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexDBusAddress:
                return CVariant::fromValue(this->m_dBusAddress);
            case IndexFramelessWindow:
                return CVariant::fromValue(this->m_windowFrameless);
            case IndexCoreMode:
                return CVariant::fromValue(this->m_coreMode);
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        void CLauncherSetup::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CLauncherSetup>(); return; }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexDBusAddress:
                this->setDBusAddress(variant.toQString());
                break;
            case IndexFramelessWindow:
                this->m_windowFrameless = variant.toBool();
                break;
            case IndexCoreMode:
                this->m_coreMode = variant.toInt();
                break;
            default:
                CValueObject::setPropertyByIndex(index, variant);
                break;
            }
        }
    } // ns
} // ns
