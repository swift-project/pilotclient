// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_LIVERYLISTMODEL_H
#define SWIFT_GUI_LIVERYLISTMODEL_H

#include "gui/models/listmodeldbobjects.h"
#include "gui/swiftguiexport.h"

class QObject;

namespace swift::misc::aviation
{
    class CLivery;
    class CLiveryList;
} // namespace swift::misc::aviation

namespace swift::gui::models
{
    //! Distributor list model
    class SWIFT_GUI_EXPORT CLiveryListModel : public CListModelDbObjects<swift::misc::aviation::CLiveryList, int, true>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CLiveryListModel(QObject *parent = nullptr);

        //! Destructor
        virtual ~CLiveryListModel() override {}
    };
} // namespace swift::gui::models

#endif // SWIFT_GUI_LIVERYLISTMODEL_H
