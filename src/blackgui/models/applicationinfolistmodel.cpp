/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/models/applicationinfolistmodel.h"
#include "blackgui/models/columnformatters.h"
#include "blackgui/models/columns.h"

#include <QModelIndex>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QString>
#include <Qt>
#include <QtDebug>
#include <QtGlobal>

using namespace BlackMisc;

namespace BlackGui::Models
{
    CApplicationInfoListModel::CApplicationInfoListModel(QObject *parent) : CListModelBase("ModelApplicationInfoListModel", parent)
    {
        m_columns.addColumn(CColumn::standardString("version", CApplicationInfo::IndexVersionString));
        m_columns.addColumn(CColumn::standardString("OS", CApplicationInfo::IndexPlatformInfo));
        m_columns.addColumn(CColumn::standardString("exe.path", CApplicationInfo::IndexExecutablePath));
        m_columns.addColumn(CColumn("e.?", "existing?", CApplicationInfo::IndexExecutablePathExisting, new CBoolIconFormatter("directory existing", "directory not existing")));
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
} // namespace
