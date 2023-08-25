// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_SETTINGS_VIEWUPDATESETTINGS_H
#define BLACKGUI_SETTINGS_VIEWUPDATESETTINGS_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/pq/time.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/variant.h"

#include <QMap>
#include <QString>
#include <QMetaType>

BLACK_DECLARE_VALUEOBJECT_MIXINS(BlackGui::Settings, CViewUpdateSettings)

namespace BlackGui::Settings
{
    //! Settings about view update rates
    class BLACKGUI_EXPORT CViewUpdateSettings :
        public BlackMisc::CValueObject<CViewUpdateSettings>
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexAtc = BlackMisc::CPropertyIndexRef::GlobalIndexCViewUpdateSettings,
            IndexAircraft,
            IndexRendering,
            IndexUser
        };

        //! Default constructor
        CViewUpdateSettings();

        //! Reset to defaults
        void reset();

        //! Get time
        const BlackMisc::PhysicalQuantities::CTime &getAtcUpdateTime() const { return m_updateAtc; }

        //! Set time
        void setAtcUpdateTime(const BlackMisc::PhysicalQuantities::CTime &time) { this->m_updateAtc = time; }

        //! Get time
        const BlackMisc::PhysicalQuantities::CTime &getAircraftUpdateTime() const { return m_updateAircraft; }

        //! Set time
        void setAircraftUpdateTime(const BlackMisc::PhysicalQuantities::CTime &time) { this->m_updateAircraft = time; }

        //! Get time
        const BlackMisc::PhysicalQuantities::CTime &getUserUpdateTime() const { return m_updateUser; }

        //! Set time
        void setUserUpdateTime(const BlackMisc::PhysicalQuantities::CTime &time) { this->m_updateUser = time; }

        //! Get time
        const BlackMisc::PhysicalQuantities::CTime &getRenderingUpdateTime() const { return m_updateRendering; }

        //! Set time
        void setRenderingUpdateTime(const BlackMisc::PhysicalQuantities::CTime &time) { this->m_updateRendering = time; }

        //! Valid?
        bool isValid() const;

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        QVariant propertyByIndex(BlackMisc::CPropertyIndexRef index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(BlackMisc::CPropertyIndexRef index, const QVariant &variant);

    private:
        BlackMisc::PhysicalQuantities::CTime m_updateAtc { 10.0, BlackMisc::PhysicalQuantities::CTimeUnit::s() };
        BlackMisc::PhysicalQuantities::CTime m_updateAircraft { 10.0, BlackMisc::PhysicalQuantities::CTimeUnit::s() };
        BlackMisc::PhysicalQuantities::CTime m_updateRendering { 10.0, BlackMisc::PhysicalQuantities::CTimeUnit::s() };
        BlackMisc::PhysicalQuantities::CTime m_updateUser { 10.0, BlackMisc::PhysicalQuantities::CTimeUnit::s() };

        BLACK_METACLASS(
            CViewUpdateSettings,
            BLACK_METAMEMBER(updateAtc),
            BLACK_METAMEMBER(updateAircraft),
            BLACK_METAMEMBER(updateRendering),
            BLACK_METAMEMBER(updateUser)
        );
    };

    //! Trait for settings about update rates
    struct TViewUpdateSettings : public BlackMisc::TSettingTrait<CViewUpdateSettings>
    {
        //! \copydoc BlackMisc::TSettingTrait::key
        static const char *key() { return "guiviewupdate"; }

        //! \copydoc BlackMisc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("View update");
            return name;
        }

        //! \copydoc BlackMisc::TSettingTrait::isValid
        static bool isValid(const CViewUpdateSettings &settings, QString &) { return settings.isValid(); }
    };
} // ns

Q_DECLARE_METATYPE(BlackGui::Settings::CViewUpdateSettings)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackGui::Settings::CViewUpdateSettings>)

#endif // guard
