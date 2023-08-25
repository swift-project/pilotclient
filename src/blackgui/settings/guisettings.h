// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_SETTINGS_GUI_H
#define BLACKGUI_SETTINGS_GUI_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/simulation/aircraftmodel.h"

#include <QString>
#include <QAbstractItemView>

BLACK_DECLARE_VALUEOBJECT_MIXINS(BlackGui::Settings, CGeneralGuiSettings)

namespace BlackGui::Settings
{
    //! General GUI settings
    class BLACKGUI_EXPORT CGeneralGuiSettings :
        public BlackMisc::CValueObject<CGeneralGuiSettings>
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexWidgetStyle = BlackMisc::CPropertyIndexRef::GlobalIndexCGeneralGuiSettings,
            IndexPreferredSelection
        };

        //! Default constructor
        CGeneralGuiSettings();

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

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        QVariant propertyByIndex(BlackMisc::CPropertyIndexRef index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(BlackMisc::CPropertyIndexRef index, const QVariant &variant);

    private:
        QString m_widgetStyle { "Fusion" };
        int m_preferredSelection = static_cast<int>(QAbstractItemView::ExtendedSelection);

        BLACK_METACLASS(
            CGeneralGuiSettings,
            BLACK_METAMEMBER(widgetStyle),
            BLACK_METAMEMBER(preferredSelection)
        );
    };

    //! General GUI settings
    struct TGeneralGui : public BlackMisc::TSettingTrait<CGeneralGuiSettings>
    {
        //! \copydoc BlackMisc::TSettingTrait::key
        static const char *key() { return "guigeneral"; }

        //! \copydoc BlackMisc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("General GUI");
            return name;
        }
    };

    //! Settings for last manual entries of own aircraft mode
    struct TBackgroundConsolidation : public BlackMisc::TSettingTrait<int>
    {
        //! \copydoc BlackMisc::TSettingTrait::key
        static const char *key() { return "backgroundconsolidation"; }

        //! \copydoc BlackMisc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("Background consolidation");
            return name;
        }

        //! \copydoc BlackMisc::TSettingTrait::isValid
        static bool isValid(const int &valueInSeconds, QString &) { return valueInSeconds == -1 || (valueInSeconds >= minSecs() && valueInSeconds <= maxSecs()); }

        //! \copydoc BlackMisc::TSettingTrait::defaultValue
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
} // ns

Q_DECLARE_METATYPE(BlackGui::Settings::CGeneralGuiSettings)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackGui::Settings::CGeneralGuiSettings>)

#endif // guard
