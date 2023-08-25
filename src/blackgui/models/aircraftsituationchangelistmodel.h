// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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
