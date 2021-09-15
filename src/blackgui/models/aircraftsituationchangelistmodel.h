/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_MODELS_AIRCRAFTSITUATIONCHANGELISTMODEL_H
#define BLACKGUI_MODELS_AIRCRAFTSITUATIONCHANGELISTMODEL_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/listmodeltimestampobjects.h"
#include "blackmisc/aviation/aircraftsituationchangelist.h"
#include <QVariant>

class QModelIndex;
class QObject;

namespace BlackGui::Models
{
    //! Aircraft situation changes list model
    class BLACKGUI_EXPORT CAircraftSituationChangeListModel : public CListModelTimestampWithOffsetObjects<BlackMisc::Aviation::CAircraftSituationChangeList, true>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAircraftSituationChangeListModel(QObject *parent = nullptr);

        //! Destructor
        virtual ~CAircraftSituationChangeListModel() {}
    };
} // namespace

#endif // guard
