// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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
