/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKCORE_CONTEXT_H
#define BLACKCORE_CONTEXT_H

#include "blackcore/context_runtime_config.h"
#include "blackcore/context_runtime.h"
#include "blackmisc/logmessage.h"
#include <QObject>
#include <QDateTime>

namespace BlackCore
{
    //! Base for all context classes
    class CContext : public QObject
    {
    public:
        //! Destructor
        ~CContext() {}

        //! Log category
        static const CLogCategoryList &getLogCategories()
        {
            static const CLogCategoryList cats { CLogCategory::context() };
            return cats;
        }

        //! Using local implementing object?
        bool isUsingImplementingObject() const
        {
            return m_mode == CRuntimeConfig::Local || m_mode == CRuntimeConfig::LocalInDbusServer;
        }

        //! Local or remote object?
        bool isLocalObject() const
        {
            return isUsingImplementingObject() || m_mode == CRuntimeConfig::NotUsed;
        }

        //! Empty object?
        bool isEmptyObject() const
        {
            return m_mode == CRuntimeConfig::NotUsed;
        }

        //! Runtime
        CRuntime *getRuntime()
        {
            Q_ASSERT(this->parent());
            return static_cast<CRuntime *>(this->parent());
        }

        //! Const runtime
        const CRuntime *getRuntime() const
        {
            Q_ASSERT(this->parent());
            return static_cast<CRuntime *>(this->parent());
        }

        //! Mode
        CRuntimeConfig::ContextMode getMode() const { return this->m_mode; }

        //! Unique id
        qint64 getUniqueId() const { return this->m_contextId; }

        //
        // cross context access
        //

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

        //! Settings
        IContextSettings *getIContextSettings();

        //! Settings
        const IContextSettings *getIContextSettings() const;

        //! Context for simulator
        const IContextSimulator *getIContextSimulator() const;

        //! Simulator
        IContextSimulator *getIContextSimulator();

        //! Id and path name for round trip protection
        virtual QString getPathAndContextId() const = 0;

    protected:
        //! Constructor
        CContext(CRuntimeConfig::ContextMode mode, QObject *parent) :
            QObject(parent), m_mode(mode), m_contextId(QDateTime::currentMSecsSinceEpoch())
        {}

        CRuntimeConfig::ContextMode m_mode; //!< How context is used
        qint64 m_contextId; //!< unique identifer, avoid redirection rountrips

        //! Path and context id
        QString buildPathAndContextId(const QString &path) const
        {
            return QString(path) + ":" + QString::number(this->getUniqueId());
        }

        //! Empty context called
        void logEmptyContextWarning(const QString &functionName) const
        {
            CLogMessage(this, CLogCategory::contextSlot()).warning("Empty context called, details: %1") << functionName;
        }

        //! Standard message when status message is returned in empty context
        static const BlackMisc::CStatusMessage &statusMessageEmptyContext();
    };
}
#endif // guard
