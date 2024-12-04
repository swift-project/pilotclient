// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_MODELBROWSERCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_MODELBROWSERCOMPONENT_H

#include <QFrame>
#include <QScopedPointer>

#include "misc/aviation/aircraftsituation.h"
#include "misc/simulation/simulatedaircraft.h"
#include "misc/variant.h"

namespace Ui
{
    class CModelBrowserComponent;
}
namespace swift::gui::components
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
        void onModelDblClicked(const swift::misc::CVariant &object);

        swift::misc::aviation::CAircraftParts getParts() const;

        void display();
        void remove();
        void loadModelSet();
        bool hasContexts() const;
        void onCGChecked(bool checked);
        void updatePartsAndPBH(bool setPbh, bool setParts);
        void fetchSimulatorValues();

        QScopedPointer<Ui::CModelBrowserComponent> ui;
        swift::misc::aviation::CAircraftSituation m_situation;
        swift::misc::simulation::CSimulatedAircraft m_aircraft;
    };
} // namespace swift::gui::components

#endif // SWIFT_GUI_COMPONENTS_MODELBROWSERCOMPONENT_H
