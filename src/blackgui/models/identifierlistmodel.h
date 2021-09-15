/* Copyright (C) 2015
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_MODELS_IDENTIFIERLISTMODEL_H
#define BLACKGUI_MODELS_IDENTIFIERLISTMODEL_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/listmodelbase.h"
#include "blackmisc/identifier.h"
#include "blackmisc/identifierlist.h"

namespace BlackGui::Models
{
    //! Originator list model
    class BLACKGUI_EXPORT CIdentifierListModel : public CListModelBase<BlackMisc::CIdentifierList>
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
