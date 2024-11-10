// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_MODELS_APPLICATIONINFOLISTMODEL_H
#define BLACKGUI_MODELS_APPLICATIONINFOLISTMODEL_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/listmodelbase.h"
#include "misc/applicationinfolist.h"
#include <QObject>

class QStandardItemModel;

namespace BlackGui::Models
{
    //! Application info list model
    class BLACKGUI_EXPORT CApplicationInfoListModel : public CListModelBase<swift::misc::CApplicationInfoList, true>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CApplicationInfoListModel(QObject *parent = nullptr);

        //! Destructor
        virtual ~CApplicationInfoListModel() {}

        //! \copydoc swift::misc::CApplicationInfoList::fromOtherSwiftVersionsFromDataDirectories
        void otherSwiftVersionsFromDataDirectories(bool reinit);
    };
} // ns
#endif // guard
