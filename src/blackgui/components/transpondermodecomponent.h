// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_TRANSPONDERMODECOMPONENT_H
#define BLACKGUI_COMPONENTS_TRANSPONDERMODECOMPONENT_H

#include "misc/aviation/transponder.h"
#include "misc/identifiable.h"
#include <QFrame>
#include <QScopedPointer>

namespace Ui
{
    class CTransponderModeComponent;
}
namespace swift::misc
{
    class CIdentifier;
    namespace simulation
    {
        class CSimulatedAircraft;
    }
}
namespace BlackGui::Components
{
    /*!
     * Small component to display XPDR mode
     */
    class CTransponderModeComponent : public QFrame, swift::misc::CIdentifiable
    {
        Q_OBJECT

    public:
        //! Ctor
        explicit CTransponderModeComponent(QWidget *parent = nullptr);

        //! Dtor
        virtual ~CTransponderModeComponent() override;

    signals:
        //! value has been changed
        void changed();

    private:
        QScopedPointer<Ui::CTransponderModeComponent> ui;
        swift::misc::aviation::CTransponder m_transponder;

        //! Init
        void init();

        //! Clicked
        void onClicked();

        //! Changed cockpit data
        void onChangedAircraftCockpit(const swift::misc::simulation::CSimulatedAircraft &aircraft, const swift::misc::CIdentifier &originator);
    };
} // ns

#endif // guard
