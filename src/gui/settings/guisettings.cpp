// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "guisettings.h"

#include <QStyleFactory>

using namespace swift::misc;

SWIFT_DEFINE_VALUEOBJECT_MIXINS(swift::gui::settings, CGeneralGuiSettings)

namespace swift::gui::settings
{
    void CGeneralGuiSettings::setWidgetStyle(const QString &widgetStyle)
    {
        if (this->m_widgetStyle == widgetStyle) { return; }
        const auto availableStyles = QStyleFactory::keys();
        if (availableStyles.contains(widgetStyle)) { m_widgetStyle = widgetStyle; }
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

    void CGeneralGuiSettings::setUiLanguage(const QString &locale)
    {
        // Normalize: accept "zh_CN", "de_DE", "de", "DE" etc.
        m_uiLanguage = locale.trimmed();
    }

    QString CGeneralGuiSettings::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        return QStringLiteral("Widget style: %1, Language: %2").arg(m_widgetStyle, m_uiLanguage);
    }

    QVariant CGeneralGuiSettings::propertyByIndex(swift::misc::CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        const auto i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexWidgetStyle: return QVariant::fromValue(this->m_widgetStyle);
        case IndexPreferredSelection: return QVariant::fromValue(this->m_preferredSelection);
        case IndexUiLanguage: return QVariant::fromValue(this->m_uiLanguage);
        default: return CValueObject::propertyByIndex(index);
        }
    }

    void CGeneralGuiSettings::setPropertyByIndex(swift::misc::CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<CGeneralGuiSettings>();
            return;
        }
        const auto i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexWidgetStyle: this->setWidgetStyle(variant.toString()); break;
        case IndexPreferredSelection: this->m_preferredSelection = variant.toInt(); break;
        case IndexUiLanguage: this->setUiLanguage(variant.toString()); break;
        default: CValueObject::setPropertyByIndex(index, variant); break;
        }
    }
} // namespace swift::gui::settings
