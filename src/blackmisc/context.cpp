//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "blackmisc/debug.h"
#include "blackmisc/context.h"
#include <QFileInfo>
#include <stdexcept>

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

namespace BlackMisc
{

    IContext &IContext::getInstance()
    {
        static CApplicationContext context;
        return context;
    }

    IContext::~IContext()
    {
    }

    CApplicationContext::CApplicationContext()
    {
    }

    QObject *CApplicationContext::singleton(const QString &singletonName)
    {
        TSingletonMap::const_iterator it = m_singletons.find(singletonName);
        if (it != m_singletons.end())
        {
            return it.value();
        }
        throw std::logic_error("Requested singleton not present");
    }

    bool CApplicationContext::hasSingleton(const QString &singletonName) const
    {
        TSingletonMap::const_iterator it = m_singletons.find(singletonName);
        if (it != m_singletons.end())
        {
            return true;
        }
        return false;
    }

    void CApplicationContext::setSingleton(const QString &singletonName, QObject *object)
    {
        m_singletons.insert(singletonName, object);
    }

    void CApplicationContext::releaseSingleton(const QString &singletonName)
    {
        m_singletons.remove(singletonName);
    }

    CDebug *CApplicationContext::getDebug()
    {
        return qobject_cast<CDebug *>(singleton("CDebug"));
    }

    void CApplicationContext::setDebug(CDebug *debug)
    {
        setSingleton("CDebug", debug);
    }

    void CApplicationContext::setDefaultApplicationName()
    {
#ifdef Q_OS_WIN
        //! By default, we use the executables name.
        if (getApplicationName().isEmpty())
        {
            WCHAR name[1024];
            int size = GetModuleFileName(NULL, name, 1023);
            QString applicationPath = QString::fromWCharArray(name, size);
            setApplicationName(QFileInfo(applicationPath).fileName());
        }
#else
        //! Todo: Check if there a corresponding API in Linux and Mac
        //! For the time being, set it to unknown.
        if (m_context.getApplicationName().isEmpty())
        {
            m_context.setApplicationName("<Unknown>");
        }
#endif
    }

} // namespace BlackMisc
