// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_MODELBROWSERCOMPONENT_H
#define BLACKGUI_COMPONENTS_MODELBROWSERCOMPONENT_H

#include <QFrame>
#include <QScopedPointer>

#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/variant.h"

namespace Ui
{
    class CModelBrowserComponent;
}
namespace BlackGui::Components
{
    //! Allow to browse thru the model set
    class CModelBrowserComponent : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CModelBrowserComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CModelBrowserComponent() override;

        //! Browser has been closed
        void close();

    private:
        void onSetRelativePosition();
        void onSetAbsolutePosition();
        void onSetPBH();
        void onSetParts();
        void onModelDblClicked(const BlackMisc::CVariant &object);

        BlackMisc::Aviation::CAircraftParts getParts() const;

        void display();
        void remove();
        void loadModelSet();
        bool hasContexts() const;
        void selectTabParts();
        void onCGChecked(bool checked);
        void updatePartsAndPBH(bool setPbh, bool setParts);
        void fetchSimulatorValues();

        QScopedPointer<Ui::CModelBrowserComponent> ui;
        BlackMisc::Aviation::CAircraftSituation m_situation;
        BlackMisc::Simulation::CSimulatedAircraft m_aircraft;
    };
} // ns

#endif // guard
