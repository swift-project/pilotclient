/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_CONTEXTAPPLICATION_H
#define BLACKCORE_CONTEXTAPPLICATION_H

#include "blackcore/context.h"
#include "blackmisc/statusmessage.h"
#include <QObject>

#define BLACKCORE_CONTEXTAPPLICATION_INTERFACENAME "net.vatsim.PilotClient.BlackCore.ContextApplication"
#define BLACKCORE_CONTEXTAPPLICATION_OBJECTPATH "/Application"

namespace BlackCore
{
    /*!
     * \brief Application context interface
     */
    class IContextApplication : public CContext
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTAPPLICATION_INTERFACENAME)

    protected:
        friend class CRuntime;

        //! Constructor
        IContextApplication(CRuntimeConfig::ContextMode mode, CRuntime *runtime) : CContext(mode, runtime) {}

    public:

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

        //! Destructor
        virtual ~IContextApplication() {}

    signals:
        //! \brief Status message
        void statusMessage(const BlackMisc::CStatusMessage &message);

        //! Widget GUI is about to start
        void widgetGuiStarting() const;

        //! Widget GUI is about to terminate
        void widgetGuiTerminating() const;

    public slots:

        //! \brief Ping a token, used to check if application is alive
        virtual qint64 ping(qint64 token) const = 0;
    };
}

#endif // guard
