// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/models/textmessagelistmodel.h"

#include <Qt>
#include <QtDebug>
#include <QtGlobal>

#include "gui/models/columnformatters.h"
#include "gui/models/columns.h"
#include "misc/aviation/callsign.h"
#include "misc/network/textmessage.h"
#include "misc/propertyindexvariantmap.h"
#include "misc/timestampbased.h"

using namespace swift::misc::physical_quantities;
using namespace swift::misc::network;
using namespace swift::misc::aviation;

namespace swift::gui::models
{
    CTextMessageListModel::CTextMessageListModel(TextMessageMode mode, QObject *parent)
        : CListModelTimestampObjects("ModelTextMessageList", parent), m_textMessageMode(NotSet)
    {
        this->setTextMessageMode(mode);

        // force strings for translation in resource files
        (void)QT_TRANSLATE_NOOP("ModelTextMessageList", "time");
        (void)QT_TRANSLATE_NOOP("ModelTextMessageList", "from");
        (void)QT_TRANSLATE_NOOP("ModelTextMessageList", "to");
        (void)QT_TRANSLATE_NOOP("ModelTextMessageList", "message");
    }

    void CTextMessageListModel::setTextMessageMode(CTextMessageListModel::TextMessageMode mode)
    {
        if (m_textMessageMode == mode) return;
        m_textMessageMode = mode;
        m_columns.clear();
        switch (mode)
        {
        case NotSet:
        case FromTo:
        {
            CColumn col = CColumn("type", CTextMessage::IndexIcon);
            col.setSortPropertyIndex({ CTextMessage::IndexSenderCallsign, CCallsign::IndexSuffix });
            m_columns.addColumn(col);
            m_columns.addColumn(CColumn("time", "received", CTextMessage::IndexUtcTimestamp,
                                        new CDateTimeFormatter(CDateTimeFormatter::formatHms())));
            m_columns.addColumn(
                CColumn::standardString("from", { CTextMessage::IndexSenderCallsign, CCallsign::IndexCallsignString }));
            m_columns.addColumn(CColumn::standardString("to", CTextMessage::IndexRecipientCallsignOrFrequency));
            m_columns.addColumn(CColumn::standardString("message", CTextMessage::IndexMessage));

            // default sort order
            this->setSortColumnByPropertyIndex(CTextMessage::IndexUtcTimestamp);
            m_sortOrder = Qt::DescendingOrder;
        }
        break;

        case From:
        {
            m_columns.addColumn(CColumn("time", "received", CTextMessage::IndexUtcTimestamp,
                                        new CDateTimeFormatter(CDateTimeFormatter::formatHms())));
            m_columns.addColumn(
                CColumn::standardString("from", { CTextMessage::IndexSenderCallsign, CCallsign::IndexCallsignString }));
            m_columns.addColumn(CColumn::standardString("message", CTextMessage::IndexMessage));

            // default sort order
            this->setSortColumnByPropertyIndex(CTextMessage::IndexUtcTimestamp);
            m_sortOrder = Qt::DescendingOrder;
        }
        break;

        default: qFatal("Wrong mode"); break;
        }
    }

    QVariant CTextMessageListModel::data(const QModelIndex &index, int role) const
    {
        if (role == Qt::ToolTipRole)
        {
            // the underlying model object as summary
            const CTextMessage model(this->at(index));
            return model.asHtmlSummary("<br>");
        }
        return CListModelBase::data(index, role);
    }
} // namespace swift::gui::models
