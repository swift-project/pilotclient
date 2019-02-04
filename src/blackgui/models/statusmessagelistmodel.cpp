/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/models/columnformatters.h"
#include "blackgui/models/columns.h"
#include "blackgui/models/statusmessagelistmodel.h"
#include "blackmisc/propertyindexvariantmap.h"
#include "blackmisc/timestampbased.h"

#include <Qt>
#include <QtGlobal>

using namespace BlackMisc;

namespace BlackGui
{
    namespace Models
    {
        CStatusMessageListModel::CStatusMessageListModel(QObject *parent) :
            CListModelTimestampObjects<CStatusMessageList, true>("ViewStatusMessageList", parent)
        {
            this->setMode(Detailed);
            m_sortTieBreakers.push_front(CStatusMessage::IndexMessage);
            m_sortTieBreakers.push_front(CStatusMessage::IndexSeverity);

            // force strings for translation in resource files
            (void)QT_TRANSLATE_NOOP("ViewStatusMessageList", "time");
            (void)QT_TRANSLATE_NOOP("ViewStatusMessageList", "severity");
            (void)QT_TRANSLATE_NOOP("ViewStatusMessageList", "type");
            (void)QT_TRANSLATE_NOOP("ViewStatusMessageList", "message");
            (void)QT_TRANSLATE_NOOP("ViewStatusMessageList", "all categories");
        }

        void CStatusMessageListModel::setMode(CStatusMessageListModel::Mode mode, const CStatusMessageList &messages)
        {
            switch (mode)
            {
            case DetailedWithOrder:
            case Detailed:
                this->setMode(messages.needsOrder() ? Detailed : DetailedWithOrder);
                break;
            case SimplifiedWithOrder:
            case Simplified:
                this->setMode(messages.needsOrder() ? Simplified : SimplifiedWithOrder);
                break;
            }
        }

        void CStatusMessageListModel::setMode(CStatusMessageListModel::Mode mode)
        {
            m_columns.clear();
            m_sortColumn = CStatusMessage::IndexUtcTimestamp;

            switch (mode)
            {
            case DetailedWithOrder:
                m_columns.addColumn(CColumn::orderColumn());
                m_sortColumn = CStatusMessage::IndexOrder;
                Q_FALLTHROUGH();
            case Detailed:
                {
                    m_columns.addColumn(CColumn("time", CStatusMessage::IndexUtcTimestamp, new CDateTimeFormatter(CDateTimeFormatter::formatHmsz())));
                    CColumn col = CColumn("severity", CStatusMessage::IndexIcon);
                    col.setSortPropertyIndex(CStatusMessage::IndexSeverityAsString);
                    m_columns.addColumn(col);
                    m_columns.addColumn(CColumn::standardString("message", CStatusMessage::IndexMessage));
                    m_columns.addColumn(CColumn::standardString("category", CStatusMessage::IndexCategoryHumanReadableOrTechnicalAsString));

                    m_sortOrder = Qt::DescendingOrder;
                }
                break;
            case SimplifiedWithOrder:
                m_columns.addColumn(CColumn::orderColumn());
                m_sortColumn = CStatusMessage::IndexOrder;
                Q_FALLTHROUGH();
            case Simplified:
                {
                    m_columns.addColumn(CColumn("time", CStatusMessage::IndexUtcTimestamp, new CDateTimeFormatter(CDateTimeFormatter::formatHmsz())));
                    CColumn col = CColumn("severity", CStatusMessage::IndexIcon);
                    col.setSortPropertyIndex(CStatusMessage::IndexSeverityAsString);
                    m_columns.addColumn(col);
                    m_columns.addColumn(CColumn::standardString("message", CStatusMessage::IndexMessage));

                    m_sortColumn = CStatusMessage::IndexUtcTimestamp;
                    m_sortOrder = Qt::DescendingOrder;
                }
                break;
            }
        }

        QVariant CStatusMessageListModel::data(const QModelIndex &index, int role) const
        {
            if (role == Qt::ToolTipRole)
            {
                // the underlying model object as summary
                const CStatusMessage msg(this->at(index));
                return msg.toHtml(false, false);
            }
            return CListModelTimestampObjects::data(index, role);
        }
    } // namespace
} // namespace
