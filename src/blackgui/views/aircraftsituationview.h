// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_VIEWS_AIRCRAFTSITUATIONVIEW_H
#define BLACKGUI_VIEWS_AIRCRAFTSITUATIONVIEW_H

#include "blackgui/views/viewtimestampobjects.h"
#include "blackgui/models/aircraftsituationlistmodel.h"
#include "blackgui/blackguiexport.h"

namespace BlackGui::Views
{
    //! Aircraft situations view
    class BLACKGUI_EXPORT CAircraftSituationView : public CViewWithTimestampWithOffsetObjects<Models::CAircraftSituationListModel>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAircraftSituationView(QWidget *parent = nullptr);

        //! Enable menu
        void setWithMenuRequestElevation(bool enable);

    signals:
        //! Request elevation
        void requestElevation(const swift::misc::aviation::CAircraftSituation &situation);

    protected:
        //! \copydoc CViewBase::customMenu
        virtual void customMenu(Menus::CMenuActions &menuActions) override;

    private:
        //! Request elevation
        void emitRequestElevationForSituation();

        bool m_withMenuRequestElevation = false;
    };
} // ns
#endif // guard
