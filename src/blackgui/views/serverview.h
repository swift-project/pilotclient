/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_VIEWS_SERVERVIEW_H
#define BLACKGUI_VIEWS_SERVERVIEW_H

#include "blackgui/blackguiexport.h"
#include "blackgui/views/viewbase.h"
#include "blackgui/models/serverlistmodel.h"

class QWidget;

namespace BlackGui::Views
{
    //! Network servers
    class BLACKGUI_EXPORT CServerView : public CViewBase<Models::CServerListModel>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CServerView(QWidget *parent = nullptr);
    };
} // ns
#endif // guard
