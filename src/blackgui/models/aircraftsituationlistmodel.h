/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_MODELS_AIRCRAFTSITUATIONLISTMODEL_H
#define BLACKGUI_MODELS_AIRCRAFTSITUATIONLISTMODEL_H

#include "blackgui/models/listmodeltimestampobjects.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/aviation/aircraftsituationlist.h"

#include <QVariant>

namespace BlackGui::Models
{
    //! Client list model
    class BLACKGUI_EXPORT CAircraftSituationListModel : public CListModelTimestampWithOffsetObjects<BlackMisc::Aviation::CAircraftSituationList, true>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAircraftSituationListModel(QObject *parent = nullptr);

        //! Destructor
        virtual ~CAircraftSituationListModel() {}
    };
} // namespace
#endif // guard
