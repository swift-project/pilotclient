// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_SIMULATION_SETTINGS_FGSWIFTBUSSETTINGS_H
#define BLACKMISC_SIMULATION_SETTINGS_FGSWIFTBUSSETTINGS_H

#include <QString>
#include "blackmisc/dbusserver.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/valueobject.h"

BLACK_DECLARE_VALUEOBJECT_MIXINS(BlackMisc::Simulation::Settings, CFGSwiftBusSettings)

namespace BlackMisc::Simulation::Settings
{
    //! FGSwiftBus settings
    class BLACKMISC_EXPORT CFGSwiftBusSettings final :
        public CValueObject<CFGSwiftBusSettings>
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexDBusServerAddress = CPropertyIndexRef::GlobalIndexCFGSwiftBusSettings,
        };

        //! Default constructor
        CFGSwiftBusSettings() = default;

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        QVariant propertyByIndex(BlackMisc::CPropertyIndexRef index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(BlackMisc::CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! DBus server
        QString getDBusServerAddress() const { return m_dBusServerAddress; }

        //! Set DBus server
        void setDBusServerAddress(const QString &dBusServer) { m_dBusServerAddress = dBusServer; }

    private:
        QString m_dBusServerAddress { "tcp:host=127.0.0.1,port=45003" }; //!< DBus server

        BLACK_METACLASS(
            CFGSwiftBusSettings,
            BLACK_METAMEMBER(dBusServerAddress)
        );
    };

    /*!
     * Setting for FGSwiftBus.
     */
    struct TFGSwiftBusServer : public BlackMisc::TSettingTrait<CFGSwiftBusSettings>
    {
        //! \copydoc BlackMisc::TSettingTrait::key
        static const char *key() { return "fgswiftbus/settings"; }

        //! \copydoc BlackCore::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("FGSwiftBus");
            return name;
        }

        //! \copydoc BlackMisc::TSettingTrait::defaultValue
        static CFGSwiftBusSettings defaultValue() { return CFGSwiftBusSettings(); }

        //! \copydoc BlackMisc::TSettingTrait::isValid
        static bool isValid(const CFGSwiftBusSettings &settings, QString &) { return BlackMisc::CDBusServer::isSessionOrSystemAddress(settings.getDBusServerAddress()) || BlackMisc::CDBusServer::isQtDBusAddress(settings.getDBusServerAddress()); }
    };
} // ns

Q_DECLARE_METATYPE(BlackMisc::Simulation::Settings::CFGSwiftBusSettings)

#endif // guard
