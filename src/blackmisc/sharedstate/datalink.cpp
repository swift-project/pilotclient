/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
