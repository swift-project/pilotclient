// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#include "blackmisc/sharedstate/datalink.h"
#include "blackmisc/promise.h"
#include "blackmisc/variant.h"

namespace BlackMisc::SharedState
{
    void CDataLinkConnectionWatcher::setStatus(bool status)
    {
        if (status != m_connected)
        {
            m_connected = status;
            if (m_connected) { emit connected(); }
            else { emit disconnected(); }
        }
    }

    IDataLink::~IDataLink() = default;

    IDataLink::IDataLink()
    {
        qRegisterMetaType<CPromise<CVariant>>();
    }

    QString IDataLink::getChannelName(const QObject *object)
    {
        const QMetaObject *meta = object->parent()->metaObject();
        const char *info = meta->classInfo(meta->indexOfClassInfo("SharedStateChannel")).value();
        const QString name = object->parent()->objectName();
        return name.isEmpty() ? QString(info) : (info % QLatin1Char(':') % name);
    }
}
