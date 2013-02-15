//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "blackmisc/debug.h"
#include "blackmisc/context.h"

namespace BlackMisc
{

    IContext *IContext::m_context = NULL;

    IContext &IContext::getInstance()
    {
        if (m_context == NULL)
        {
            m_context = new CApplicationContext;
        }
        return *m_context;
    }

    bool IContext::isInitialised()
    {
        return m_context != NULL;
    }


    IContext::~IContext()
    {
        m_context = NULL;
    }

    void	IContext::registerContext()
    {
    #ifdef Q_OS_WIN
        bAssert(m_context == NULL);
    #endif // Q_OS_WIN
        m_context = this;

        m_debug = new CDebug;
    }

    CApplicationContext::CApplicationContext()
    {
        registerContext();
    }

    void *CApplicationContext::singletonPointer(const QString &singletonName)
    {
        TSingletonMap::const_iterator it = m_singletonObjects.find(singletonName);
        if (it != m_singletonObjects.end())
            return it.value();

        return NULL;
    }

    void CApplicationContext::setSingletonPointer(const QString &singletonName, void *object)
    {
        m_singletonObjects.insert(singletonName, object);
    }

    void CApplicationContext::releaseSingletonPointer(const QString &singletonName, void *object)
    {
        bAssert (m_singletonObjects.find(singletonName).value() == object);
        m_singletonObjects.remove(singletonName);
    }

    CDebug *CApplicationContext::getDebug()
    {
        return m_debug;
    }

    void CApplicationContext::setDebug(CDebug *debug)
    {
        m_debug = debug;
    }

    CLibraryContext::CLibraryContext(IContext &applicationContext)
    : m_applicationContext(&applicationContext)
    {
        registerContext();
    }

    void *CLibraryContext::singletonPointer(const QString &singletonName)
    {
        bAssert(m_applicationContext != NULL);
        return m_applicationContext->singletonPointer(singletonName);
    }

    void CLibraryContext::setSingletonPointer(const QString &singletonName, void *object)
    {
        bAssert(m_applicationContext != NULL);
        m_applicationContext->setSingletonPointer(singletonName, object);
    }

    void CLibraryContext::releaseSingletonPointer(const QString &singletonName, void *object)
    {
        bAssert(m_applicationContext != NULL);
        m_applicationContext->releaseSingletonPointer(singletonName, object);
    }

    CDebug *CLibraryContext::getDebug()
    {
       return m_applicationContext->getDebug();
    }

    void CLibraryContext::setDebug(CDebug *debug)
    {
        m_debug = debug;
    }

} // namespace BlackMisc
