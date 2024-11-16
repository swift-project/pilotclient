// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_MODELS_WINDLAYERLISTMODEL_H
#define SWIFT_GUI_MODELS_WINDLAYERLISTMODEL_H

#include "gui/models/listmodelbase.h"
#include "gui/swiftguiexport.h"
#include "misc/weather/windlayer.h"
#include "misc/weather/windlayerlist.h"

class QObject;

namespace swift::gui::models
{
    //! Wind layer list model
    class SWIFT_GUI_EXPORT CWindLayerListModel : public CListModelBase<swift::misc::weather::CWindLayerList, false>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CWindLayerListModel(QObject *parent = nullptr);

        //! Destructor
        virtual ~CWindLayerListModel() {}
    };
} // namespace swift::gui::models
#endif // guard
