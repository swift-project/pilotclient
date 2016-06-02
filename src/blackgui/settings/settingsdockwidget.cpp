/* Copyright (C) 2016
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "settingsdockwidget.h"
#include <QStringList>

using namespace BlackMisc;

namespace BlackGui
{
    namespace Settings
    {
        CSettingsDockWidget::CSettingsDockWidget()
        { }

        void CSettingsDockWidget::setFloatingFramelessMargins(const QMargins &margins)
        {
            this->m_floatingFramelessMargins = marginsToString(margins);
        }

        QMargins CSettingsDockWidget::getFloatingFramelessMargins() const
        {
            return stringToMargins(this->m_floatingFramelessMargins);
        }

        void CSettingsDockWidget::setFloatingMargins(const QMargins &margins)
        {
            this->m_floatingMargins = marginsToString(margins);
        }

        QMargins CSettingsDockWidget::getFloatingMargins() const
        {
            return stringToMargins(this->m_floatingMargins);
        }

        void CSettingsDockWidget::setDockedMargins(const QMargins &margins)
        {
            this->m_dockedMargins = marginsToString(margins);
        }

        QMargins CSettingsDockWidget::getDockedMargins() const
        {
            return stringToMargins(this->m_dockedMargins);
        }

        QString CSettingsDockWidget::convertToQString(bool i18n) const
        {
            return convertToQString(", ", i18n);
        }

        QString CSettingsDockWidget::convertToQString(const QString &separator, bool i18n) const
        {
            Q_UNUSED(i18n);
            QString s("floating: ");
            s.append(this->m_floatingMargins);
            s.append(separator);
            s.append("floating, frameless: ");
            s.append(this->m_floatingFramelessMargins);
            s.append(separator);
            s.append("docked: ");
            s.append(this->m_dockedMargins);
            return s;
        }

        CVariant CSettingsDockWidget::propertyByIndex(const CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexFloatingMargins:
                return CVariant::fromValue(this->m_floatingMargins);
            case IndexFloatingFramelessMargins:
                return CVariant::fromValue(this->m_floatingFramelessMargins);
            case IndexDockedMargins:
                return CVariant::fromValue(this->m_dockedMargins);
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        void CSettingsDockWidget::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CSettingsDockWidget>(); return; }

            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexFloatingMargins:
                this->m_floatingMargins = variant.toQString();
                break;
            case IndexFloatingFramelessMargins:
                this->m_floatingFramelessMargins = variant.toQString();
                break;
            case IndexDockedMargins:
                this->m_dockedMargins = variant.toQString();
                break;
            default:
                CValueObject::setPropertyByIndex(index, variant);
                break;
            }
        }

        QString CSettingsDockWidget::marginsToString(const QMargins &margins)
        {
            const QString s("%1:%2:%3:%4");
            return s.arg(margins.left()).arg(margins.top()).arg(margins.right()).arg(margins.bottom());
        }

        QMargins CSettingsDockWidget::stringToMargins(const QString &str)
        {
            const QStringList parts = str.split(":");
            Q_ASSERT_X(parts.size() == 4, Q_FUNC_INFO, "malformed");
            bool ok = false;
            const int l = parts.at(0).toInt(&ok);
            Q_ASSERT_X(ok, Q_FUNC_INFO, "malformed number");
            const int t = parts.at(1).toInt(&ok);
            Q_ASSERT_X(ok, Q_FUNC_INFO, "malformed number");
            const int r = parts.at(2).toInt(&ok);
            Q_ASSERT_X(ok, Q_FUNC_INFO, "malformed number");
            const int b = parts.at(3).toInt(&ok);
            Q_ASSERT_X(ok, Q_FUNC_INFO, "malformed number");
            Q_UNUSED(ok);
            return QMargins(l, t, r, b);
        }
    } // ns
} // ns
