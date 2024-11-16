// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_MODELS_SERVERLISTMODEL_H
#define SWIFT_GUI_MODELS_SERVERLISTMODEL_H

#include "gui/models/listmodelbase.h"
#include "gui/swiftguiexport.h"
#include "misc/network/serverlist.h"

class QObject;

namespace swift::gui::models
{
    //! Server list model
    class SWIFT_GUI_EXPORT CServerListModel : public CListModelBase<swift::misc::network::CServerList, true>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CServerListModel(QObject *parent = nullptr);

        //! Destructor
        virtual ~CServerListModel() {}
    };
} // namespace swift::gui::models
#endif // guard
