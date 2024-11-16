// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_CONTEXT_CONTEXT_H
#define SWIFT_CORE_CONTEXT_CONTEXT_H

#include <QDateTime>
#include <QObject>
#include <QString>
#include <QStringBuilder>
#include <QtGlobal>

#include "core/corefacade.h"
#include "core/corefacadeconfig.h"
#include "core/swiftcoreexport.h"
#include "misc/logcategories.h"
#include "misc/logmessage.h"
#include "misc/statusmessage.h"

namespace swift::misc
{
    class CLogCategoryList;
}
namespace swift::core::context
{
    class IContextApplication;
    class IContextAudio;
    class IContextNetwork;
    class IContextOwnAircraft;
    class IContextSimulator;

    //! Base for all context classes
    class SWIFT_CORE_EXPORT IContext : public QObject
    {
        Q_OBJECT

    public:
        //! Destructor
        virtual ~IContext() override {}

        //! Log categories
        static const QStringList &getLogCategories();

        //! Using local implementing object?
        bool isUsingImplementingObject() const
        {
            return m_mode == CCoreFacadeConfig::Local || m_mode == CCoreFacadeConfig::LocalInDBusServer;
        }

        //! Local or remote object?
        bool isLocalObject() const { return isUsingImplementingObject() || m_mode == CCoreFacadeConfig::NotUsed; }

        //! Empty object?
        bool isEmptyObject() const { return m_mode == CCoreFacadeConfig::NotUsed; }

        //! Runtime
        CCoreFacade *getRuntime()
        {
            Q_ASSERT(this->parent());
            return static_cast<CCoreFacade *>(this->parent());
        }

        //! Const runtime
        const CCoreFacade *getRuntime() const
        {
            Q_ASSERT(this->parent());
            return static_cast<CCoreFacade *>(this->parent());
        }

        //! Mode
        CCoreFacadeConfig::ContextMode getMode() const { return this->m_mode; }

        //! Unique id
        qint64 getUniqueId() const { return this->m_contextId; }

        // ---------------------------------------------------------------
        // cross context access
        // ---------------------------------------------------------------

        //! Context for application
        const IContextApplication *getIContextApplication() const;

        //! Application
        IContextApplication *getIContextApplication();

        //! Context for network
        IContextAudio *getIContextAudio();

        //! Context for network
        const IContextAudio *getIContextAudio() const;

        //! Context for network
        IContextNetwork *getIContextNetwork();

        //! Context for network
        const IContextNetwork *getIContextNetwork() const;

        //! Context for own aircraft
        IContextOwnAircraft *getIContextOwnAircraft();

        //! Context for own aircraft
        const IContextOwnAircraft *getIContextOwnAircraft() const;

        //! Context for simulator
        const IContextSimulator *getIContextSimulator() const;

        //! Simulator
        IContextSimulator *getIContextSimulator();

        //! Set debug flag
        void setDebugEnabled(bool debug);

        //! Debug enabled?
        bool isDebugEnabled() const;

        // ---------------------------------------------------------------
        // All context must implement those functions
        // ---------------------------------------------------------------

        //! Id and path name for round trip protection
        virtual QString getPathAndContextId() const = 0;

        //! Parse a given command line
        virtual bool parseCommandLine(const QString &commandLine, const swift::misc::CIdentifier &originator) = 0;

    signals:
        //! Log or debug values changed
        void changedLogOrDebugSettings();

    protected:
        //! Constructor
        IContext(CCoreFacadeConfig::ContextMode mode, QObject *parent);

        //! Path and context id
        QString buildPathAndContextId(const QString &path) const
        {
            return path % u':' % QString::number(this->getUniqueId());
        }

        //! Relay signals from this class
        void relayBaseClassSignals(const QString &serviceName, QDBusConnection &connection, const QString &objectPath,
                                   const QString &interfaceName);

        //! Empty context called
        void logEmptyContextWarning(const QString &functionName) const
        {
            swift::misc::CLogMessage(this, swift::misc::CLogCategories::contextSlot())
                    .warning(u"Empty context called, details: %1")
                << functionName;
        }

        //! Called when shutdown is about to be called
        virtual void onAboutToShutdown() {}

        //! Standard message when status message is returned in empty context
        static const swift::misc::CStatusMessage &statusMessageEmptyContext();

    private:
        CCoreFacadeConfig::ContextMode m_mode; //!< How context is used
        qint64 m_contextId; //!< unique identifer, avoid redirection rountrips
        bool m_debugEnabled = false; //!< debug messages enabled
    };
} // namespace swift::core::context
#endif // guard
