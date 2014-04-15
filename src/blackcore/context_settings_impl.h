/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_CONTEXTSETTINGS_IMPL_H
#define BLACKCORE_CONTEXTSETTINGS_IMPL_H

#include "context_settings.h"
#include "dbus_server.h"
#include "context_runtime.h"

#include "blackmisc/setnetwork.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/hwkeyboardkeylist.h"

namespace BlackCore
{

    /*!
     * \brief Settings context implementation
     */
    class CContextSettings : public IContextSettings
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTSETTINGS_INTERFACENAME)
        friend class CRuntime;

    protected:
        //! \brief Constructor
        CContextSettings(CRuntimeConfig::ContextMode mode, CRuntime *runtime = nullptr);

        //! \brief Register myself in DBus
        CContextSettings *registerWithDBus(CDBusServer *server)
        {
            if (!server || this->m_mode != CRuntimeConfig::LocalInDbusServer) return this;
            server->addObject(IContextSettings::ObjectPath(), this);
            return this;
        }

    public:
        //! Destructor
        virtual ~CContextSettings() {}

        //! \brief settings file
        const QString &getSettingsDirectory() const { return BlackMisc::Settings::CSettingUtilities::getSettingsDirectory(); }

        //! \copydoc IContextSettings::value()
        virtual BlackMisc::CStatusMessageList value(const QString &path, const QString &command, const QVariant &value) override;

    public slots:
        //! \copydoc IContextSettings::getNetworkSettings()
        virtual BlackMisc::Settings::CSettingsNetwork getNetworkSettings() const override;

        //! \copydoc IContextSettings::getHotkeys()
        virtual BlackMisc::Hardware::CKeyboardKeyList getHotkeys() const override;

        /*!
         * \brief DBus version of value method.
         * \remarks Basically an unwanted signature as this is different from the "local" signature and
         * contains explicit DBus types (a: QDbusArgument, b: type for conversion).
         * If this can be removed, fine. -> https://dev.vatsim-germany.org/issues/116
         */
        virtual BlackMisc::CStatusMessageList value(const QString &path, const QString &command, QDBusVariant value, int unifiedBlackMetaType);

        //! \brief read settings
        virtual BlackMisc::CStatusMessage read() override;

        //! \copydoc IContextSettings::write
        virtual BlackMisc::CStatusMessage write() const override;

        //! \copydoc IContextSettings::reset
        virtual BlackMisc::CStatusMessage reset(bool write = true) override;

        //! Settings file name
        virtual QString getSettingsFileName() const override { return BlackMisc::Settings::CSettingUtilities::getSettingsFile(); }

        //! JSON represenation
        virtual QString getSettingsAsJsonString() const override;

    private:
        BlackMisc::Settings::CSettingsNetwork m_settingsNetwork;
        BlackMisc::Hardware::CKeyboardKeyList m_hotkeys;
        QJsonDocument toJsonDocument() const;
        void emitCompletelyChanged();
    };
}

#endif // guard
