/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "sharedstringlistcompleter.h"
#include <QStringListModel>
#include <QDateTime>

namespace BlackGui
{
    bool CSharedStringListCompleter::updateData(const QStringList &data, int cacheTimeMs)
    {
        QStringListModel *model = qobject_cast<QStringListModel *>(m_completer->model());
        Q_ASSERT_X(model, Q_FUNC_INFO, "Model missing");
        bool updated = false;
        const qint64 now = QDateTime::currentMSecsSinceEpoch();
        if (model->rowCount() != data.size() || (now - m_lastUpdated) > cacheTimeMs)
        {
            // quick check, different size means data changed -> update
            // otherwise rely on time
            m_lastUpdated = now;
            model->setStringList(data);
            updated = true;
        }
        return updated;
    }

    void CSharedStringListCompleter::clearData()
    {
        this->updateData(QStringList(), 0);
    }

    bool CSharedStringListCompleter::wasUpdatedWithinTime(int checkTimeMs) const
    {
        return (QDateTime::currentMSecsSinceEpoch() - m_lastUpdated) <= checkTimeMs;
    }
} // ns
