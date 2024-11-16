// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_SETTINGS_DOCKWIDGET_H
#define SWIFT_GUI_SETTINGS_DOCKWIDGET_H

#include <QMap>
#include <QMetaType>
#include <QString>

#include "gui/swiftguiexport.h"
#include "misc/dictionary.h"
#include "misc/propertyindex.h"
#include "misc/settingscache.h"
#include "misc/variant.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::gui::settings, CDockWidgetSettings)

namespace swift::gui::settings
{
    //! Settings for dockwidget
    class SWIFT_GUI_EXPORT CDockWidgetSettings :
        public swift::misc::CValueObject<CDockWidgetSettings>
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexFloatingMargins = swift::misc::CPropertyIndexRef::GlobalIndexCDockWidgetSettings,
            IndexFloatingFramelessMargins,
            IndexDockedMargins,
            IndexFrameless,
            IndexFloating
        };

        //! Default constructor
        CDockWidgetSettings();

        //! Reset margins to default
        void resetMarginsToDefault();

        //! Reset to defaults
        void reset();

        //! Set margins for given dock widget
        void setMarginsWhenFramelessFloating(const QMargins &margins);

        //! Margins for given dock widget
        QMargins getMarginsWhenFramelessFloating() const;

        //! Set margins for given dock widget
        void setMarginsWhenFloating(const QMargins &margins);

        //! Margins for given dock widget
        QMargins getMarginsWhenFloating() const;

        //! Set margins for given dock widget
        void setMarginsWhenDocked(const QMargins &margins);

        //! Margins for given dock widget
        QMargins getMarginsWhenDocked() const;

        //! Floating?
        bool isFloating() const { return m_floating; }

        //! Frameless?
        bool isFramless() const { return m_frameless; }

        //! Floating
        void setFloating(bool floating) { m_floating = floating; }

        //! Frameless
        void setFrameless(bool frameless) { m_frameless = frameless; }

        //! Geometry
        QByteArray getGeometry() const;

        //! Set geometry
        void setGeometry(const QByteArray &ba);

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(const QString &separator, bool i18n = false) const;

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(swift::misc::CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(swift::misc::CPropertyIndexRef index, const QVariant &variant);

    private:
        QString m_floatingMargins { "0:0:0:0" }; //!< margins: when floating
        QString m_floatingFramelessMargins { "0:0:0:0" }; //!< margins, when floating and frameless
        QString m_dockedMargins { "0:0:0:0" }; //!< margins, when docked
        QString m_geometry; //!< geometry as HEX values
        bool m_floating = false; //!< floating
        bool m_frameless = false; //!< frameless

        SWIFT_METACLASS(
            CDockWidgetSettings,
            SWIFT_METAMEMBER(floatingMargins),
            SWIFT_METAMEMBER(floatingFramelessMargins),
            SWIFT_METAMEMBER(dockedMargins),
            SWIFT_METAMEMBER(floating),
            SWIFT_METAMEMBER(frameless),
            SWIFT_METAMEMBER(geometry));
    };

    //! Trait for settings for dock widget
    //! \details All settings will go in one file, separated by application and object name
    //!          (of the parent object using this setting). If the object name is not set, it will not work correctly
    struct TDockWidget : public swift::misc::TSettingTrait<CDockWidgetSettings>
    {
        //! \copydoc swift::misc::TSettingTrait::key
        static const char *key() { return "guidockwidget/%Application%/%OwnerName%"; } // Key in data cache https://dev.vatsim-germany.org/issues/776

        //! \copydoc swift::misc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("Dockable widgets");
            return name;
        }
    };
} // namespace swift::gui::settings

Q_DECLARE_METATYPE(swift::gui::settings::CDockWidgetSettings)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::gui::settings::CDockWidgetSettings>)

#endif // guard
