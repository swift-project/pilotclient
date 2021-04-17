/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
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
            this->setSorting(CStatusMessage::IndexUtcTimestamp, Qt::DescendingOrder);
            m_sortTieBreakers.push_front(CStatusMessage::IndexMessage);
            m_sortTieBreakers.push_front(CStatusMessage::IndexSeverity);
            this->setMode(Detailed);

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

        void CStatusMessageListModel::adjustOrderColumn(const CStatusMessageList &messages)
        {
            this->setMode(m_mode, messages);
        }

        bool CStatusMessageListModel::isSortedByTimestampOrOrder() const
        {
            const CPropertyIndex p = this->getSortProperty();
            return sortedByTimestampOrOrder(p);
        }

        bool CStatusMessageListModel::sortedByTimestampOrOrder(const CPropertyIndex &p)
        {
            if (p.isEmpty()) { return false; }
            const int last = p.indexVector().last();
            return IOrderable::isAnyOrderIndex(last) || ITimestampBased::isAnyTimestampIndex(last);
        }

        void CStatusMessageListModel::setMode(CStatusMessageListModel::Mode mode)
        {
            const Qt::SortOrder  oldOrder = this->getSortOrder();
            const CPropertyIndex oldIndex = this->getSortProperty();
            m_columns.clear();
            m_mode = mode;

            switch (mode)
            {
            case DetailedWithOrder:
                m_columns.addColumn(CColumn::orderColumn());
                [[fallthrough]];
            case Detailed:
                {
                    m_columns.addColumn(CColumn("time", CStatusMessage::IndexUtcTimestamp, new CDateTimeFormatter(CDateTimeFormatter::formatHmsz())));
                    CColumn col = CColumn("severity", CStatusMessage::IndexSeverityAsIcon);
                    col.setSortPropertyIndex(CStatusMessage::IndexSeverityAsString);
                    m_columns.addColumn(col);
                    m_columns.addColumn(CColumn::standardString("message", CStatusMessage::IndexMessage));
                    m_columns.addColumn(CColumn::standardString("category", CStatusMessage::IndexCategoriesAsString));
                }
                break;
            case SimplifiedWithOrder:
                m_columns.addColumn(CColumn::orderColumn());
                [[fallthrough]];
            case Simplified:
                {
                    m_columns.addColumn(CColumn("time", CStatusMessage::IndexUtcTimestamp, new CDateTimeFormatter(CDateTimeFormatter::formatHmsz())));
                    CColumn col = CColumn("severity", CStatusMessage::IndexSeverityAsIcon);
                    col.setSortPropertyIndex(CStatusMessage::IndexSeverityAsString);
                    m_columns.addColumn(col);
                    m_columns.addColumn(CColumn::standardString("message", CStatusMessage::IndexMessage));
                }
                break;
            }
            // m_columns.insertEmptyColumn();

            // sorting
            if (CStatusMessageListModel::sortedByTimestampOrOrder(oldIndex))
            {
                if (mode == DetailedWithOrder || mode == SimplifiedWithOrder)
                {
                    this->setSorting(CStatusMessage::IndexOrder, oldOrder);
                }
                else
                {
                    this->setSorting(CStatusMessage::IndexUtcTimestamp, oldOrder);
                }
            }
            else
            {
                // restore sorting
                this->setSorting(oldIndex, oldOrder);
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
