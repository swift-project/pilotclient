#ifndef BLACKCORE_CONTEXT_H
#define BLACKCORE_CONTEXT_H

#include <QObject>
#include "blackcore/context_runtime_config.h"
#include "blackcore/context_runtime.h"

namespace BlackCore
{
    /*!
     * \brief Base for all context classes
     */
    class CContext : public QObject
    {
    private:
        CRuntimeConfig::ContextMode m_mode;

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

    protected:
        //! Constructor
        CContext(CRuntimeConfig::ContextMode mode, QObject *parent) : QObject(parent), m_mode(mode)
        {}

    };
}
#endif // guard
