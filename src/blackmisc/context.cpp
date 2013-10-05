//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "blackmisc/debug.h"
#include "blackmisc/context.h"
#include <QFileInfo>

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

namespace BlackMisc
{

    IContext *&instancePointer()
    {
        static CApplicationContext context;
        static IContext *ptr = &context;
        return ptr;
    }

    void IContext::setInstance(IContext &context)
    {
        instancePointer() = &context;
    }

    IContext &IContext::getInstance()
    {
        return *instancePointer();
    }

    IContext::~IContext()
    {
    }

    CApplicationContext::CApplicationContext()
    {
    }

    const QObject *CApplicationContext::getQObjectNothrow(const QString &name) const
    {
        auto it = m_map.find(name);
        return it == m_map.end() ? nullptr : it.value();
    }

    void CApplicationContext::setQObject(QString name, QObject &object)
    {
        m_map.insert(name, &object);
    }

    void CApplicationContext::removeQObject(const QString &name)
    {
        m_map.remove(name);
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
