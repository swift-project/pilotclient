// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/models/applicationinfolistmodel.h"

#include <QModelIndex>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QString>
#include <Qt>
#include <QtDebug>
#include <QtGlobal>

#include "gui/models/columnformatters.h"
#include "gui/models/columns.h"

using namespace swift::misc;

namespace swift::gui::models
{
    CApplicationInfoListModel::CApplicationInfoListModel(QObject *parent)
        : CListModelBase("ModelApplicationInfoListModel", parent)
    {
        m_columns.addColumn(CColumn::standardString("version", CApplicationInfo::IndexVersionString));
        m_columns.addColumn(CColumn::standardString("OS", CApplicationInfo::IndexPlatformInfo));
        m_columns.addColumn(CColumn::standardString("exe.path", CApplicationInfo::IndexExecutablePath));
        m_columns.addColumn(CColumn("e.?", "existing?", CApplicationInfo::IndexExecutablePathExisting,
                                    new CBoolIconFormatter("directory existing", "directory not existing")));
        m_columns.addColumn(CColumn::standardString("data.path", CApplicationInfo::IndexApplicationDataPath));
        m_columns.setWidthPercentages({ 20, 15, 30, 5, 30 });

        // default sort order
        this->setSortColumnByPropertyIndex(CApplicationInfo::IndexVersionString);
        m_sortOrder = Qt::DescendingOrder;
    }

    void CApplicationInfoListModel::otherSwiftVersionsFromDataDirectories(bool reinit)
    {
        this->update(CApplicationInfoList::fromOtherSwiftVersionsFromDataDirectories(reinit));
    }
} // namespace swift::gui::models
