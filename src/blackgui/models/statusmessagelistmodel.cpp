/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "statusmessagelistmodel.h"
#include "blackmisc/icon.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include <QMetaProperty>
#include <QBrush>
#include <QIcon>

using namespace BlackMisc;

namespace BlackGui
{
    namespace Models
    {
        /*
         * Constructor
         */
        CStatusMessageListModel::CStatusMessageListModel(QObject *parent) :
            CListModelBase<BlackMisc::CStatusMessage, BlackMisc::CStatusMessageList>("ViewStatusMessageList", parent)
        {
            this->m_columns.addColumn(CColumn("time", CStatusMessage::IndexTimestamp, new CDateTimeFormatter(CDateTimeFormatter::formatHms())));
            this->m_columns.addColumn(CColumn::standardString("category", CStatusMessage::IndexCategoryHumanReadable));
            this->m_columns.addColumn(CColumn("severity", CStatusMessage::IndexIcon));
            this->m_columns.addColumn(CColumn::standardString("message", CStatusMessage::IndexMessage));
            this->m_columns.addColumn(CColumn::standardString("all categories", CStatusMessage::IndexCategories));

            this->m_sortedColumn = CStatusMessage::IndexTimestamp;
            this->m_sortOrder = Qt::DescendingOrder;

            // force strings for translation in resource files
            (void)QT_TRANSLATE_NOOP("ViewStatusMessageList", "time");
            (void)QT_TRANSLATE_NOOP("ViewStatusMessageList", "severity");
            (void)QT_TRANSLATE_NOOP("ViewStatusMessageList", "type");
            (void)QT_TRANSLATE_NOOP("ViewStatusMessageList", "message");
            (void)QT_TRANSLATE_NOOP("ViewStatusMessageList", "all categories");
        }

    } // namespace
} // namespace
