// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "dockwidgetsettings.h"

#include <QStringList>

#include "gui/guiutility.h"
#include "misc/stringutils.h"

using namespace swift::misc;
using namespace swift::gui;

SWIFT_DEFINE_VALUEOBJECT_MIXINS(swift::gui::settings, CDockWidgetSettings)

namespace swift::gui::settings
{
    CDockWidgetSettings::CDockWidgetSettings() {}

    void CDockWidgetSettings::resetMarginsToDefault()
    {
        // this->setMarginsWhenFloating(QMargins(0, 3, 15, 35)); // found by trial and error on windows
        this->setMarginsWhenFloating(QMargins(0, 0, 0, 0));
        this->setMarginsWhenFramelessFloating(QMargins(0, 0, 0, 0));
        this->setMarginsWhenDocked(QMargins(0, 0, 0, 0));
    }

    void CDockWidgetSettings::reset()
    {
        this->resetMarginsToDefault();
        m_geometry = "";
        m_frameless = false;
        m_floating = false;
    }

    void CDockWidgetSettings::setMarginsWhenFramelessFloating(const QMargins &margins)
    {
        m_floatingFramelessMargins = CGuiUtility::marginsToString(margins);
    }

    QMargins CDockWidgetSettings::getMarginsWhenFramelessFloating() const
    {
        return CGuiUtility::stringToMargins(m_floatingFramelessMargins);
    }

    void CDockWidgetSettings::setMarginsWhenFloating(const QMargins &margins)
    {
        m_floatingMargins = CGuiUtility::marginsToString(margins);
    }

    QMargins CDockWidgetSettings::getMarginsWhenFloating() const
    {
        return CGuiUtility::stringToMargins(m_floatingMargins);
    }

    void CDockWidgetSettings::setMarginsWhenDocked(const QMargins &margins)
    {
        m_dockedMargins = CGuiUtility::marginsToString(margins);
    }

    QMargins CDockWidgetSettings::getMarginsWhenDocked() const { return CGuiUtility::stringToMargins(m_dockedMargins); }

    QByteArray CDockWidgetSettings::getGeometry() const { return QByteArray::fromHex(m_geometry.toLatin1()); }

    void CDockWidgetSettings::setGeometry(const QByteArray &ba) { m_geometry = ba.toHex(); }

    QString CDockWidgetSettings::convertToQString(bool i18n) const { return convertToQString(", ", i18n); }

    QString CDockWidgetSettings::convertToQString(const QString &separator, bool i18n) const
    {
        Q_UNUSED(i18n);
        QString s("floating: ");
        s.append(m_floatingMargins);
        s.append(separator);
        s.append("floating, frameless: ");
        s.append(m_floatingFramelessMargins);
        s.append(separator);
        s.append("docked: ");
        s.append(m_dockedMargins);
        s.append(separator);
        s.append("frameless: ");
        s.append(boolToTrueFalse(m_frameless));
        s.append(separator);
        s.append("floating: ");
        s.append(boolToTrueFalse(m_floating));
        return s;
    }

    QVariant CDockWidgetSettings::propertyByIndex(CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexFloatingMargins: return QVariant::fromValue(m_floatingMargins);
        case IndexFloatingFramelessMargins: return QVariant::fromValue(m_floatingFramelessMargins);
        case IndexDockedMargins: return QVariant::fromValue(m_dockedMargins);
        case IndexFrameless: return QVariant::fromValue(m_frameless);
        case IndexFloating: return QVariant::fromValue(m_floating);
        default: return CValueObject::propertyByIndex(index);
        }
    }

    void CDockWidgetSettings::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<CDockWidgetSettings>();
            return;
        }

        ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexFloatingMargins: m_floatingMargins = variant.toString(); break;
        case IndexFloatingFramelessMargins: m_floatingFramelessMargins = variant.toString(); break;
        case IndexDockedMargins: m_dockedMargins = variant.toString(); break;
        case IndexFloating: m_floating = variant.toBool(); break;
        case IndexFrameless: m_frameless = variant.toBool(); break;
        default: CValueObject::setPropertyByIndex(index, variant); break;
        }
    }
} // namespace swift::gui::settings
