// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_SETTINGS_VIEWUPDATESETTINGS_H
#define SWIFT_GUI_SETTINGS_VIEWUPDATESETTINGS_H

#include <QMap>
#include <QMetaType>
#include <QString>

#include "gui/swiftguiexport.h"
#include "misc/pq/time.h"
#include "misc/propertyindex.h"
#include "misc/settingscache.h"
#include "misc/variant.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::gui::settings, CViewUpdateSettings)

namespace swift::gui::settings
{
    //! Settings about view update rates
    class SWIFT_GUI_EXPORT CViewUpdateSettings :
        public swift::misc::CValueObject<CViewUpdateSettings>
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexAtc = swift::misc::CPropertyIndexRef::GlobalIndexCViewUpdateSettings,
            IndexAircraft,
            IndexRendering,
            IndexUser
        };

        //! Default constructor
        CViewUpdateSettings();

        //! Reset to defaults
        void reset();

        //! Get time
        const swift::misc::physical_quantities::CTime &getAtcUpdateTime() const { return m_updateAtc; }

        //! Set time
        void setAtcUpdateTime(const swift::misc::physical_quantities::CTime &time) { this->m_updateAtc = time; }

        //! Get time
        const swift::misc::physical_quantities::CTime &getAircraftUpdateTime() const { return m_updateAircraft; }

        //! Set time
        void setAircraftUpdateTime(const swift::misc::physical_quantities::CTime &time) { this->m_updateAircraft = time; }

        //! Get time
        const swift::misc::physical_quantities::CTime &getUserUpdateTime() const { return m_updateUser; }

        //! Set time
        void setUserUpdateTime(const swift::misc::physical_quantities::CTime &time) { this->m_updateUser = time; }

        //! Get time
        const swift::misc::physical_quantities::CTime &getRenderingUpdateTime() const { return m_updateRendering; }

        //! Set time
        void setRenderingUpdateTime(const swift::misc::physical_quantities::CTime &time) { this->m_updateRendering = time; }

        //! Valid?
        bool isValid() const;

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(swift::misc::CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(swift::misc::CPropertyIndexRef index, const QVariant &variant);

    private:
        swift::misc::physical_quantities::CTime m_updateAtc { 10.0, swift::misc::physical_quantities::CTimeUnit::s() };
        swift::misc::physical_quantities::CTime m_updateAircraft { 10.0, swift::misc::physical_quantities::CTimeUnit::s() };
        swift::misc::physical_quantities::CTime m_updateRendering { 10.0, swift::misc::physical_quantities::CTimeUnit::s() };
        swift::misc::physical_quantities::CTime m_updateUser { 10.0, swift::misc::physical_quantities::CTimeUnit::s() };

        SWIFT_METACLASS(
            CViewUpdateSettings,
            SWIFT_METAMEMBER(updateAtc),
            SWIFT_METAMEMBER(updateAircraft),
            SWIFT_METAMEMBER(updateRendering),
            SWIFT_METAMEMBER(updateUser));
    };

    //! Trait for settings about update rates
    struct TViewUpdateSettings : public swift::misc::TSettingTrait<CViewUpdateSettings>
    {
        //! \copydoc swift::misc::TSettingTrait::key
        static const char *key() { return "guiviewupdate"; }

        //! \copydoc swift::misc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("View update");
            return name;
        }

        //! \copydoc swift::misc::TSettingTrait::isValid
        static bool isValid(const CViewUpdateSettings &settings, QString &) { return settings.isValid(); }
    };
} // namespace swift::gui::settings

Q_DECLARE_METATYPE(swift::gui::settings::CViewUpdateSettings)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::gui::settings::CViewUpdateSettings>)

#endif // guard
