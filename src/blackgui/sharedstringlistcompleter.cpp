/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "sharedstringlistcompleter.h"
#include <QDateTime>

namespace BlackGui
{
    bool CSharedStringListCompleter::updateData(const QStringList &data, int cacheTimeMs)
    {
        QStringListModel *model = this->getCompleterModel();
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

    bool CSharedStringListCompleter::contains(const QString &value, Qt::CaseSensitivity cs) const
    {
        return this->stringList().contains(value, cs);
    }

    QStringList CSharedStringListCompleter::stringList() const
    {
        const QStringListModel *model = this->getCompleterModel();
        if (!model) { return QStringList(); }
        return model->stringList();
    }

    QStringListModel *CSharedStringListCompleter::getCompleterModel() const
    {
        return qobject_cast<QStringListModel *>(m_completer->model());
    }

    void CompleterUtils::setCompleterParameters(QCompleter *completer)
    {
        Q_ASSERT_X(completer, Q_FUNC_INFO, "Need completer");
        completer->setCaseSensitivity(Qt::CaseInsensitive);
        completer->setWrapAround(true);
        completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
        completer->setCompletionMode(QCompleter::InlineCompletion);
    }
} // ns
