// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/models/audiodeviceinfolistmodel.h"
#include "gui/models/columnformatters.h"
#include "gui/models/columns.h"

#include <Qt>
#include <QtGlobal>

using namespace swift::misc;
using namespace swift::misc::audio;

namespace swift::gui::models
{
    CAudioDeviceInfoListModel::CAudioDeviceInfoListModel(QObject *parent) : CListModelBase("AudioDeviceInfoListModel", parent)
    {
        m_columns.addColumn(CColumn::standardString("type", CAudioDeviceInfo::IndexDeviceTypeAsString));
        m_columns.addColumn(CColumn::standardString("name", CAudioDeviceInfo::IndexName));
        m_columns.addColumn(CColumn::standardString("machine", { CAudioDeviceInfo::IndexIdentifier, CIdentifier::IndexMachineName }));
        m_columns.addColumn(CColumn::standardString("process", { CAudioDeviceInfo::IndexIdentifier, CIdentifier::IndexProcessName }));

        // default sort order
        this->setSortColumnByPropertyIndex(CAudioDeviceInfo::IndexName);
        m_sortOrder = Qt::AscendingOrder;
    }
} // ns
