// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_MODELS_APPLICATIONINFOLISTMODEL_H
#define SWIFT_GUI_MODELS_APPLICATIONINFOLISTMODEL_H

#include "gui/swiftguiexport.h"
#include "gui/models/listmodelbase.h"
#include "misc/applicationinfolist.h"
#include <QObject>

class QStandardItemModel;

namespace swift::gui::models
{
    //! Application info list model
    class SWIFT_GUI_EXPORT CApplicationInfoListModel : public CListModelBase<swift::misc::CApplicationInfoList, true>
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
