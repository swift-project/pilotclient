/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file

#ifndef BLACKCORE_CONTEXTAPPLICATION_H
#define BLACKCORE_CONTEXTAPPLICATION_H

#include "blackcore/context.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/eveventhotkeyfunction.h"
#include "blackmisc/evoriginator.h"
#include <QObject>
#include <QReadWriteLock>

//! \addtogroup dbus
//! @{

//! DBus interface for context
#define BLACKCORE_CONTEXTAPPLICATION_INTERFACENAME "net.vatsim.PilotClient.BlackCore.ContextApplication"

//! DBus object path for context
#define BLACKCORE_CONTEXTAPPLICATION_OBJECTPATH "/Application"

//! @}

namespace BlackCore
{
    class CInputManager;

    /*!
     * Application context interface
     */
    class IContextApplication : public CContext
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTAPPLICATION_INTERFACENAME)

    protected:
        //! Constructor
        IContextApplication(CRuntimeConfig::ContextMode mode, CRuntime *runtime);

    public:
        //! Parts of the application
        enum Application : uint
        {
            ApplicationGui,
            ApplicationCore
        };

        //! State of application
        enum Actions : uint
        {
            ApplicationStarts,
            ApplicationStops
        };

        //! Service name
        static const QString &InterfaceName()
        {
            static QString s(BLACKCORE_CONTEXTAPPLICATION_INTERFACENAME);
            return s;
        }

        //! Service path
        static const QString &ObjectPath()
        {
            static QString s(BLACKCORE_CONTEXTAPPLICATION_OBJECTPATH);
            return s;
        }

        //! \copydoc CContext::getPathAndContextId()
        virtual QString getPathAndContextId() const { return this->buildPathAndContextId(ObjectPath()); }

        //! Factory method
        static IContextApplication *create(CRuntime *parent, CRuntimeConfig::ContextMode mode, CDBusServer *server, QDBusConnection &conn);

        //! Destructor
        virtual ~IContextApplication() {}

    signals:
        //! A component changes
        void componentChanged(uint component, uint action);

        //! A log message was logged
        void messageLogged(const BlackMisc::CStatusMessage &message, const BlackMisc::Event::COriginator &origin);

    public slots:
        //! Log a log message
        //! \note Not pure because it can be called from the base class constructor.
        virtual void logMessage(const BlackMisc::CStatusMessage &message, const BlackMisc::Event::COriginator &origin) { Q_UNUSED(message); Q_UNUSED(origin); }

        //! \brief Ping a token, used to check if application is alive
        virtual qint64 ping(qint64 token) const = 0;

        //! A component has changed its state
        virtual void notifyAboutComponentChange(uint component, uint action) = 0;

        //! Remote enabled version of writing a text file
        virtual bool writeToFile(const QString &fileName, const QString &content) = 0;

        //!  Remote enabled version of reading a text file
        virtual QString readFromFile(const QString &fileName) = 0;

        //!  Remote enabled version of deleting a file
        virtual bool removeFile(const QString &fileName) = 0;

        //!  Remote enabled version of file exists
        virtual bool existsFile(const QString &fileName) = 0;

        //! Process remote event
        virtual void processHotkeyFuncEvent(const BlackMisc::Event::CEventHotkeyFunction &event) = 0;

        //! Change settings
        void changeSettings(uint typeValue);

    };
}

#endif // guard
