#ifndef BLACKCORE_CONTEXT_H
#define BLACKCORE_CONTEXT_H

#include "blackcore/context_runtime_config.h"
#include "blackcore/context_runtime.h"
#include <QObject>
#include <QDateTime>

namespace BlackCore
{
    /*!
     * \brief Base for all context classes
     */
    class CContext : public QObject
    {
    public:
        //! Destructor
        ~CContext() {}

        //! Using local objects?
        bool usingLocalObjects() const
        {
            return m_mode == CRuntimeConfig::Local || m_mode == CRuntimeConfig::LocalInDbusServer;
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

        //! Context for network
        IContextNetwork *getIContextNetwork();

        //! Context for network
        const IContextNetwork *getIContextNetwork() const;

        //! Context for network
        IContextAudio *getIContextAudio();

        //! Context for network
        const IContextAudio *getIContextAudio() const;

        //! Settings
        IContextSettings *getIContextSettings();

        //! Settings
        const IContextSettings *getIContextSettings() const;

        //! Context for application
        const IContextApplication *getIContextApplication() const;

        //! Application
        IContextApplication *getIContextApplication();

        //! Context for simulator
        const IContextSimulator *getIContextSimulator() const;

        //! Simulator
        IContextSimulator *getIContextSimulator();

    protected:
        //! Constructor
        CContext(CRuntimeConfig::ContextMode mode, QObject *parent) :
            QObject(parent), m_mode(mode), m_contextId(QDateTime::currentMSecsSinceEpoch())
        {}

        CRuntimeConfig::ContextMode m_mode; //!< How context is used
        qint64 m_contextId; //!< unique identifer, avoid redirection rountrips
    };
}
#endif // guard
