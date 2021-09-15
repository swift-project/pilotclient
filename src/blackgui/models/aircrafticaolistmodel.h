/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_MODELS_AIRCRAFTICAOLISTMODEL_H
#define BLACKGUI_MODELS_AIRCRAFTICAOLISTMODEL_H

#include "blackmisc/aviation/aircrafticaocodelist.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackgui/models/listmodeldbobjects.h"
#include "blackgui/blackguiexport.h"

namespace BlackGui::Models
{
    //! Airport list model
    class BLACKGUI_EXPORT CAircraftIcaoCodeListModel :
        public CListModelDbObjects<BlackMisc::Aviation::CAircraftIcaoCodeList, int, true>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAircraftIcaoCodeListModel(QObject *parent = nullptr);

        //! Destructor
        virtual ~CAircraftIcaoCodeListModel() {}
    };
} // ns

#endif // guard
