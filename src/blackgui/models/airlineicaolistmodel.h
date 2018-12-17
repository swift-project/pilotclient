/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_AIRLINEICAOLISTMODEL_H
#define BLACKGUI_AIRLINEICAOLISTMODEL_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/listmodeldbobjects.h"

class QObject;

namespace BlackMisc
{
    namespace Aviation
    {
        class CAirlineIcaoCode;
        class CAirlineIcaoCodeList;
    }
}

namespace BlackGui
{
    namespace Models
    {
        //! Airport list model
        class BLACKGUI_EXPORT CAirlineIcaoCodeListModel :
            public CListModelDbObjects<BlackMisc::Aviation::CAirlineIcaoCode, BlackMisc::Aviation::CAirlineIcaoCodeList, int, true>
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CAirlineIcaoCodeListModel(QObject *parent = nullptr);

            //! Destructor
            virtual ~CAirlineIcaoCodeListModel() {}
        };
    }
}
#endif // guard
