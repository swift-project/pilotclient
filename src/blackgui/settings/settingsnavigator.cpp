/* Copyright (C) 2016
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "settingsnavigator.h"
#include "blackgui/guiutility.h"
#include "blackmisc/stringutils.h"
#include <QStringList>

using namespace BlackMisc;
using namespace BlackGui;

namespace BlackGui
{
    namespace Settings
    {
        CSettingsNavigator::CSettingsNavigator()
        { }

        void CSettingsNavigator::reset()
        { }

        void CSettingsNavigator::setMargins(const QMargins &margins)
        {
            this->m_margins = CGuiUtility::marginsToString(margins);
        }

        QMargins CSettingsNavigator::getMargins() const
        {
            return CGuiUtility::stringToMargins(this->m_margins);
        }

        QByteArray CSettingsNavigator::getGeometry() const
        {
            return byteArrayFromHexString(this->m_geometry);
        }

        void CSettingsNavigator::setGeometry(const QByteArray &ba)
        {
            this->m_geometry = bytesToHexString(ba);
        }

        QString CSettingsNavigator::convertToQString(bool i18n) const
        {
            return convertToQString(", ", i18n);
        }

        QString CSettingsNavigator::convertToQString(const QString &separator, bool i18n) const
        {
            Q_UNUSED(i18n);
            QString s("margins: ");
            s.append(this->m_margins);
            s.append(separator);
            s.append("frameless: ");
            s.append(boolToTrueFalse(this->m_frameless));
            s.append(separator);
            s.append("columns: ");
            s.append(this->m_columns);
            return s;
        }

        CVariant CSettingsNavigator::propertyByIndex(const CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexMargins:
                return CVariant::fromValue(this->m_margins);
            case IndexFrameless:
                return CVariant::fromValue(this->isFramless());
            case IndexColumns:
                return CVariant::fromValue(this->m_columns);
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        void CSettingsNavigator::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CSettingsNavigator>(); return; }

            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexMargins:
                this->m_margins = variant.toQString();
                break;
            case IndexColumns:
                this->m_columns = variant.toInt();
                break;
            case IndexFrameless:
                this->m_frameless = variant.toBool();
                break;
            default:
                CValueObject::setPropertyByIndex(index, variant);
                break;
            }
        }
    } // ns
} // ns
