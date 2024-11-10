// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_SETTINGS_VIEWUPDATESETTINGS_H
#define BLACKGUI_SETTINGS_VIEWUPDATESETTINGS_H

#include "blackgui/blackguiexport.h"
#include "misc/settingscache.h"
#include "misc/pq/time.h"
#include "misc/propertyindex.h"
#include "misc/variant.h"

#include <QMap>
#include <QString>
#include <QMetaType>

BLACK_DECLARE_VALUEOBJECT_MIXINS(BlackGui::Settings, CViewUpdateSettings)

namespace BlackGui::Settings
{
    //! Settings about view update rates
    class BLACKGUI_EXPORT CViewUpdateSettings :
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

        BLACK_METACLASS(
            CViewUpdateSettings,
            BLACK_METAMEMBER(updateAtc),
            BLACK_METAMEMBER(updateAircraft),
            BLACK_METAMEMBER(updateRendering),
            BLACK_METAMEMBER(updateUser)
        );
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
} // ns

Q_DECLARE_METATYPE(BlackGui::Settings::CViewUpdateSettings)
Q_DECLARE_METATYPE(swift::misc::CCollection<BlackGui::Settings::CViewUpdateSettings>)

#endif // guard
