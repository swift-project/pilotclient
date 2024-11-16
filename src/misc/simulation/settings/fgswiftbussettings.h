// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_SETTINGS_FGSWIFTBUSSETTINGS_H
#define SWIFT_MISC_SIMULATION_SETTINGS_FGSWIFTBUSSETTINGS_H

#include <QString>

#include "misc/dbusserver.h"
#include "misc/settingscache.h"
#include "misc/valueobject.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::simulation::Settings, CFGSwiftBusSettings)

namespace swift::misc::simulation::settings
{
    //! FGSwiftBus settings
    class SWIFT_MISC_EXPORT CFGSwiftBusSettings final :
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

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(swift::misc::CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(swift::misc::CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! DBus server
        QString getDBusServerAddress() const { return m_dBusServerAddress; }

        //! Set DBus server
        void setDBusServerAddress(const QString &dBusServer) { m_dBusServerAddress = dBusServer; }

    private:
        QString m_dBusServerAddress { "tcp:host=127.0.0.1,port=45003" }; //!< DBus server

        SWIFT_METACLASS(
            CFGSwiftBusSettings,
            SWIFT_METAMEMBER(dBusServerAddress));
    };

    /*!
     * Setting for FGSwiftBus.
     */
    struct TFGSwiftBusServer : public swift::misc::TSettingTrait<CFGSwiftBusSettings>
    {
        //! \copydoc swift::misc::TSettingTrait::key
        static const char *key() { return "fgswiftbus/settings"; }

        //! \copydoc swift::core::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("FGSwiftBus");
            return name;
        }

        //! \copydoc swift::misc::TSettingTrait::defaultValue
        static CFGSwiftBusSettings defaultValue() { return CFGSwiftBusSettings(); }

        //! \copydoc swift::misc::TSettingTrait::isValid
        static bool isValid(const CFGSwiftBusSettings &settings, QString &) { return swift::misc::CDBusServer::isSessionOrSystemAddress(settings.getDBusServerAddress()) || swift::misc::CDBusServer::isQtDBusAddress(settings.getDBusServerAddress()); }
    };
} // namespace swift::misc::simulation::settings

Q_DECLARE_METATYPE(swift::misc::simulation::settings::CFGSwiftBusSettings)

#endif // guard
