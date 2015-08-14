/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_CONTEXTSETTINGS_H
#define BLACKCORE_CONTEXTSETTINGS_H

#include "blackcoreexport.h"
#include "blackcore/context.h"
#include "blackcore/dbus_server.h"
#include "blackinput/keyboard.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/dbus.h"
#include "blackmisc/variant.h"
#include "blackcore/settingsallclasses.h"
#include <QObject>

//! \addtogroup dbus
//! @{

//! DBus interface for context
#define BLACKCORE_CONTEXTSETTINGS_INTERFACENAME "org.swift_project.blackcore.contextsettings"

//! DBus object path for context
#define BLACKCORE_CONTEXTSETTINGS_OBJECTPATH "/settings"

//! @}

namespace BlackCore
{
    /*!
     * Context settings interface
     */
    class BLACKCORE_EXPORT IContextSettings : public CContext
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTSETTINGS_INTERFACENAME)

    public:
        //! Settings type
        enum SettingsType
        {
            SettingsNetwork,
            SettingsAudio,
            SettingsSimulator
        };

    protected:
        //! DBus version constructor
        IContextSettings(CRuntimeConfig::ContextMode mode, CRuntime *runtime = nullptr) : CContext(mode, runtime)
        {}

    public:
        //! Service name
        static const QString &InterfaceName()
        {
            static QString s(BLACKCORE_CONTEXTSETTINGS_INTERFACENAME);
            return s;
        }

        //! Service path
        static const QString &ObjectPath()
        {
            static QString s(BLACKCORE_CONTEXTSETTINGS_OBJECTPATH);
            return s;
        }

        //! \copydoc CContext::getPathAndContextId()
        virtual QString getPathAndContextId() const { return this->buildPathAndContextId(ObjectPath()); }

        //! Path for audio settings
        static const QString &PathAudioSettings()
        {
            static QString s("audio");
            return s;
        }

        //! Root path
        static const QString &PathRoot()
        {
            static QString s("root");
            return s;
        }

        //! Factory method
        static IContextSettings *create(CRuntime *parent, CRuntimeConfig::ContextMode mode, CDBusServer *server, QDBusConnection &conn);

        //! Destructor
        virtual ~IContextSettings() {}

    signals:
        //! Settings have been changed
        //! \sa IContextSettings::SettingsType
        void changedSettings(uint type);

    public slots:
        //! Handle value
        virtual BlackMisc::CStatusMessageList value(const QString &path, const QString &command, const BlackMisc::CVariant &value) = 0;

        //! Audio settings
        virtual BlackMisc::Settings::CSettingsAudio getAudioSettings() const = 0;

        //! Save settings
        virtual BlackMisc::CStatusMessage write() const = 0;

        //! Read settings
        virtual BlackMisc::CStatusMessage read() = 0;

        //! Reset settings
        virtual BlackMisc::CStatusMessage reset(bool write = true) = 0;

        //! Read settings
        virtual QString getSettingsFileName() const = 0;

        //! Settings as JSON string
        virtual QString getSettingsAsJsonString() const = 0;
    };
}

#endif // guard
