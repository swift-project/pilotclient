/* Copyright (C) 2019
 * Swift Project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/models/audiodeviceinfolistmodel.h"
#include "blackgui/models/columnformatters.h"
#include "blackgui/models/columns.h"

#include <Qt>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::Audio;

namespace BlackGui::Models
{
    CAudioDeviceInfoListModel::CAudioDeviceInfoListModel(QObject *parent) :
        CListModelBase("AudioDeviceInfoListModel", parent)
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
