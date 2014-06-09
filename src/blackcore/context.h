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
            return QString(path).
                   append(':').
                   append(QString::number(this->getUniqueId()));
        }

    };
}
#endif // guard
