// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_SETTINGS_NAVIGATOR_H
#define SWIFT_GUI_SETTINGS_NAVIGATOR_H

#include "gui/swiftguiexport.h"
#include "misc/settingscache.h"
#include "misc/propertyindex.h"
#include "misc/variant.h"

#include <QMap>
#include <QString>
#include <QMetaType>

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::gui::settings, CNavigatorSettings)

namespace swift::gui::settings
{
    //! Settings for navigator bar
    class SWIFT_GUI_EXPORT CNavigatorSettings :
        public swift::misc::CValueObject<CNavigatorSettings>
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexMargins = swift::misc::CPropertyIndexRef::GlobalIndexCNavigatorSettings,
            IndexFrameless,
            IndexColumns
        };

        //! Default constructor
        CNavigatorSettings();

        //! Destructor.
        ~CNavigatorSettings() {}

        //! Reset to defaults
        void reset();

        //! Set margins
        void setMargins(const QMargins &margins);

        //! Margins
        QMargins getMargins() const;

        //! Frameless?
        bool isFramless() const { return m_frameless; }

        //! Frameless
        void setFrameless(bool frameless) { m_frameless = frameless; }

        //! Number pf columns
        int getColumns() const { return m_columns; }

        //! Set columns
        void setColumns(int columns) { this->m_columns = columns; }

        //! Geometry
        QByteArray getGeometry() const;

        //! Set geometry
        void setGeometry(const QByteArray &ba);

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! To string
        QString convertToQString(const QString &separator, bool i18n = false) const;

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(swift::misc::CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(swift::misc::CPropertyIndexRef index, const QVariant &variant);

    private:
        QString m_margins { "0:0:0:0" }; //!< margins
        QString m_geometry; //!< geometry as HEX values
        bool m_frameless = false; //!< frameless
        int m_columns = 1; //!< number of columns

        SWIFT_METACLASS(
            CNavigatorSettings,
            SWIFT_METAMEMBER(margins),
            SWIFT_METAMEMBER(frameless),
            SWIFT_METAMEMBER(columns),
            SWIFT_METAMEMBER(geometry));
    };

    //! Trait for settings for navigator
    struct TNavigator : public swift::misc::TSettingTrait<CNavigatorSettings>
    {
        //! \copydoc swift::misc::TSettingTrait::key
        static const char *key() { return "guinavigator"; }

        //! \copydoc swift::misc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("Navigator");
            return name;
        }
    };
} // ns

Q_DECLARE_METATYPE(swift::gui::settings::CNavigatorSettings)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::gui::settings::CNavigatorSettings>)

#endif // guard
