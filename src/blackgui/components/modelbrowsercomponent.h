/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_MODELBROWSERCOMPONENT_H
#define BLACKGUI_COMPONENTS_MODELBROWSERCOMPONENT_H

#include <QFrame>
#include <QScopedPointer>

#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/variant.h"

namespace Ui { class CModelBrowserComponent; }
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

        QScopedPointer <Ui::CModelBrowserComponent> ui;
        BlackMisc::Aviation::CAircraftSituation   m_situation;
        BlackMisc::Simulation::CSimulatedAircraft m_aircraft;
    };
} // ns

#endif // guard
