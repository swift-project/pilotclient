// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_VIEWS_AIRCRAFTSITUATIONVIEW_H
#define SWIFT_GUI_VIEWS_AIRCRAFTSITUATIONVIEW_H

#include "gui/models/aircraftsituationlistmodel.h"
#include "gui/swiftguiexport.h"
#include "gui/views/viewtimestampobjects.h"

namespace swift::gui::views
{
    //! Aircraft situations view
    class SWIFT_GUI_EXPORT CAircraftSituationView :
        public CViewWithTimestampWithOffsetObjects<models::CAircraftSituationListModel>
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
        void customMenu(menus::CMenuActions &menuActions) override;

    private:
        //! Request elevation
        void emitRequestElevationForSituation();

        bool m_withMenuRequestElevation = false;
    };
} // namespace swift::gui::views
#endif // SWIFT_GUI_VIEWS_AIRCRAFTSITUATIONVIEW_H
