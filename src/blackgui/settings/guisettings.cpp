/* Copyright (C) 2016
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "guisettings.h"
#include <QStyleFactory>

using namespace BlackMisc;

namespace BlackGui::Settings
{
    CGeneralGuiSettings::CGeneralGuiSettings()
    { }

    void CGeneralGuiSettings::setWidgetStyle(const QString &widgetStyle)
    {
        if (this->m_widgetStyle == widgetStyle) { return; }
        const auto availableStyles = QStyleFactory::keys();
        if (availableStyles.contains(widgetStyle))
        {
            m_widgetStyle = widgetStyle;
        }
    }

    bool CGeneralGuiSettings::isDifferentValidWidgetStyle(const QString &style) const
    {
        if (!QStyleFactory::keys().contains(style)) { return false; }
        return style != this->m_widgetStyle;
    }

    QAbstractItemView::SelectionMode CGeneralGuiSettings::getPreferredSelection() const
    {
        return static_cast<QAbstractItemView::SelectionMode>(m_preferredSelection);
    }

    void CGeneralGuiSettings::setPreferredSelection(QAbstractItemView::SelectionMode selection)
    {
        this->m_preferredSelection = static_cast<int>(selection);
    }

    QString CGeneralGuiSettings::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        return QStringLiteral("Widget style: %1").arg(this->m_widgetStyle);
    }

    QVariant CGeneralGuiSettings::propertyByIndex(BlackMisc::CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexWidgetStyle:
            return QVariant::fromValue(this->m_widgetStyle);
        case IndexPreferredSelection:
            return QVariant::fromValue(this->m_preferredSelection);
        default:
            return CValueObject::propertyByIndex(index);
        }
    }

    void CGeneralGuiSettings::setPropertyByIndex(BlackMisc::CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself()) { (*this) = variant.value<CGeneralGuiSettings>(); return; }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexWidgetStyle:
            this->setWidgetStyle(variant.toString());
            break;
        case IndexPreferredSelection:
            this->m_preferredSelection = variant.toInt();
            break;
        default:
            CValueObject::setPropertyByIndex(index, variant);
            break;
        }
    }
} // ns
