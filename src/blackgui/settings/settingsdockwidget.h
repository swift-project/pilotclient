/* Copyright (C) 2016
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_SETTINGS_DOCKWIDGET_H
#define BLACKGUI_SETTINGS_DOCKWIDGET_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/dictionary.h"
#include "blackmisc/variant.h"

#include <QMap>
#include <QString>
#include <QMetaType>

namespace BlackGui
{
    namespace Settings
    {
        //! Settings for dockwidget
        class BLACKGUI_EXPORT CSettingsDockWidget :
            public BlackMisc::CValueObject<CSettingsDockWidget>
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexFloatingMargins = BlackMisc::CPropertyIndex::GlobalIndexCSettingsDockWidget,
                IndexFloatingFramelessMargins,
                IndexDockedMargins
            };

            //! Default constructor
            CSettingsDockWidget();

            //! Destructor.
            ~CSettingsDockWidget() {}

            //! Set margins for given dock widget
            void setFloatingFramelessMargins(const QMargins &margins);

            //! Margins for given dock widget
            QMargins getFloatingFramelessMargins() const;

            //! Set margins for given dock widget
            void setFloatingMargins(const QMargins &margins);

            //! Margins for given dock widget
            QMargins getFloatingMargins() const;

            //! Set margins for given dock widget
            void setDockedMargins(const QMargins &margins);

            //! Margins for given dock widget
            QMargins getDockedMargins() const;

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

            //! To string
            QString convertToQString(const QString &separator, bool i18n = false) const;

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            BlackMisc::CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const BlackMisc::CPropertyIndex &index, const BlackMisc::CVariant &variant);

        private:
            QString m_floatingMargins {"0:0:0:0"};          //!< margins: when floating
            QString m_floatingFramelessMargins {"0:0:0:0"}; //!< margins, when floating and frameless
            QString m_dockedMargins {"0:0:0:0"};            //!< margins, when floating and frameless

            //! Convert to string
            static QString marginsToString(const QMargins &margins);

            //! Convert from string
            static QMargins stringToMargins(const QString &str);

            BLACK_METACLASS(
                CSettingsDockWidget,
                BLACK_METAMEMBER(floatingMargins),
                BLACK_METAMEMBER(floatingFramelessMargins),
                BLACK_METAMEMBER(dockedMargins)
            );
        };

        //! Needed to compile properly with Q_DECLARE_METATYPE
        using CDockWidgetSettingsDictionary = BlackMisc::CDictionary<QString, CSettingsDockWidget, QMap>;

        //! Settings for all dock widgets
        class BLACKGUI_EXPORT CSettingsDockWidgets :
            public CDockWidgetSettingsDictionary,
            public BlackMisc::Mixin::MetaType<CSettingsDockWidgets>,
            public BlackMisc::Mixin::JsonOperators<CSettingsDockWidgets>
        {
        public:
            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CSettingsDockWidgets)

            //! Default constructor.
            CSettingsDockWidgets() {}

        };

        //! Trait for settings for dock widget
        struct SettingsDockWidgets : public BlackMisc::CSettingTrait<CSettingsDockWidgets>
        {
            //! Key in data cache
            static const char *key() { return "guidockwidget"; }

            //! Default value
            static const CSettingsDockWidgets &defaultValue()
            {
                static const CSettingsDockWidgets defaultValue;
                return defaultValue;
            }
        };
    } // ns
} // ns

Q_DECLARE_METATYPE(BlackGui::Settings::CSettingsDockWidget)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackGui::Settings::CSettingsDockWidget>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackGui::Settings::CSettingsDockWidget>)
Q_DECLARE_METATYPE(BlackGui::Settings::CDockWidgetSettingsDictionary)
Q_DECLARE_METATYPE(BlackGui::Settings::CSettingsDockWidgets)

#endif // guard
