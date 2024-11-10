// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_IDENTIFIERVIEW_H
#define BLACKGUI_IDENTIFIERVIEW_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/identifierlistmodel.h"
#include "blackgui/views/viewbase.h"
#include "misc/identifierlist.h"

namespace swift::misc
{
    class CIdentifier;
}
namespace BlackGui::Views
{
    //! Originator servers
    class BLACKGUI_EXPORT CIdentifierView : public CViewBase<Models::CIdentifierListModel>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CIdentifierView(QWidget *parent = nullptr);
    };
}
#endif // guard
