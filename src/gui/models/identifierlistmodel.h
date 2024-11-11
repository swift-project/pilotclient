// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_MODELS_IDENTIFIERLISTMODEL_H
#define SWIFT_GUI_MODELS_IDENTIFIERLISTMODEL_H

#include "gui/swiftguiexport.h"
#include "gui/models/listmodelbase.h"
#include "misc/identifier.h"
#include "misc/identifierlist.h"

namespace swift::gui::models
{
    //! Originator list model
    class SWIFT_GUI_EXPORT CIdentifierListModel : public CListModelBase<swift::misc::CIdentifierList>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CIdentifierListModel(QObject *parent = nullptr);

        //! Destructor
        virtual ~CIdentifierListModel() {}
    };
} // ns
#endif // guard
