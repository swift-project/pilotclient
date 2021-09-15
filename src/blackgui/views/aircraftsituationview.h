/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
        void requestElevation(const BlackMisc::Aviation::CAircraftSituation &situation);

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
