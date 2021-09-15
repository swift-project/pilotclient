/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_IDENTIFIERVIEW_H
#define BLACKGUI_IDENTIFIERVIEW_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/identifierlistmodel.h"
#include "blackgui/views/viewbase.h"
#include "blackmisc/identifierlist.h"

namespace BlackMisc { class CIdentifier; }
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
