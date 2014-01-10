//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "blackmisc/debug.h"
#include "blackmisc/context.h"
#include <QFileInfo>
#include <QCoreApplication>
#include <stdexcept>


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
        // This part was not working before on linux.
        // This class is depricated, but as long as it is not removed, if fixed it anyway.
        setApplicationName(QFileInfo(QCoreApplication::applicationFilePath()).fileName());
    }

} // namespace BlackMisc
