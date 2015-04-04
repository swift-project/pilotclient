/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_CONTEXTAPPLICATION_H
#define BLACKCORE_CONTEXTAPPLICATION_H

#include "blackcore/context.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/audio/voiceroomlist.h"
#include "blackmisc/eveventhotkeyfunction.h"
#include "blackmisc/evoriginator.h"
#include <QObject>
#include <QReadWriteLock>

//! \addtogroup dbus
//! @{

//! DBus interface for context
#define BLACKCORE_CONTEXTAPPLICATION_INTERFACENAME "org.swift.pilotclient.BlackCore.ContextApplication"

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
        //! \note Used with CLogMessage, do not use directly
        void messageLogged(const BlackMisc::CStatusMessage &message, const BlackMisc::Event::COriginator &origin);

        //! Work around for audio context, #382
        void fakedSetComVoiceRoom(const BlackMisc::Audio::CVoiceRoomList &requestedRooms);

    public slots:
        //! Log a log message
        //! \note Not pure because it can be called from the base class constructor.
        //! \note this is the function which relays CLogMessage via DBus
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
