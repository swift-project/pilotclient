// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_MODELS_IDENTIFIERLISTMODEL_H
#define BLACKGUI_MODELS_IDENTIFIERLISTMODEL_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/listmodelbase.h"
#include "misc/identifier.h"
#include "misc/identifierlist.h"

namespace BlackGui::Models
{
    //! Originator list model
    class BLACKGUI_EXPORT CIdentifierListModel : public CListModelBase<swift::misc::CIdentifierList>
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
