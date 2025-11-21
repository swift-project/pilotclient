// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_SETTINGS_GUI_H
#define SWIFT_GUI_SETTINGS_GUI_H

#include <QAbstractItemView>
#include <QString>

#include "gui/swiftguiexport.h"
#include "misc/settingscache.h"
#include "misc/simulation/aircraftmodel.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::gui::settings, CGeneralGuiSettings)

namespace swift::gui::settings
{
    //! General GUI settings
    class SWIFT_GUI_EXPORT CGeneralGuiSettings : public swift::misc::CValueObject<CGeneralGuiSettings>
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexWidgetStyle = swift::misc::CPropertyIndexRef::GlobalIndexCGeneralGuiSettings,
            IndexPreferredSelection
        };

        //! Default constructor
        CGeneralGuiSettings() = default;

        //! Widget style
        const QString &getWidgetStyle() const { return m_widgetStyle; }

        //! Widget style
        void setWidgetStyle(const QString &widgetStyle);

        //! Has changed widget style
        bool isDifferentValidWidgetStyle(const QString &style) const;

        //! Preferred selection
        QAbstractItemView::SelectionMode getPreferredSelection() const;

        //! Preferred selection
        void setPreferredSelection(QAbstractItemView::SelectionMode selection);

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(swift::misc::CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(swift::misc::CPropertyIndexRef index, const QVariant &variant);

    private:
        QString m_widgetStyle { "Fusion" };
        int m_preferredSelection = static_cast<int>(QAbstractItemView::ExtendedSelection);

        SWIFT_METACLASS(
            CGeneralGuiSettings,
            SWIFT_METAMEMBER(widgetStyle),
            SWIFT_METAMEMBER(preferredSelection));
    };

    //! General GUI settings
    struct TGeneralGui : public swift::misc::TSettingTrait<CGeneralGuiSettings>
    {
        //! \copydoc swift::misc::TSettingTrait::key
        static const char *key() { return "guigeneral"; }

        //! \copydoc swift::misc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("General GUI");
            return name;
        }
    };

    //! Settings for last manual entries of own aircraft mode
    struct TBackgroundConsolidation : public swift::misc::TSettingTrait<int>
    {
        //! \copydoc swift::misc::TSettingTrait::key
        static const char *key() { return "backgroundconsolidation"; }

        //! \copydoc swift::misc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("Background consolidation");
            return name;
        }

        //! \copydoc swift::misc::TSettingTrait::isValid
        static bool isValid(const int &valueInSeconds, QString &)
        {
            return valueInSeconds == -1 || (valueInSeconds >= minSecs() && valueInSeconds <= maxSecs());
        }

        //! \copydoc swift::misc::TSettingTrait::defaultValue
        static const int &defaultValue()
        {
            static const int i = 60;
            return i;
        }

        //! Minimum
        static int minSecs() { return 30; }

        //! Maximum
        static int maxSecs() { return 3600; }
    };
} // namespace swift::gui::settings

Q_DECLARE_METATYPE(swift::gui::settings::CGeneralGuiSettings)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::gui::settings::CGeneralGuiSettings>)

#endif // SWIFT_GUI_SETTINGS_GUI_H
