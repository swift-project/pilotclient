// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_TRANSPONDERMODECOMPONENT_H
#define SWIFT_GUI_COMPONENTS_TRANSPONDERMODECOMPONENT_H

#include <QFrame>
#include <QScopedPointer>

#include "misc/aviation/transponder.h"
#include "misc/identifiable.h"

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
} // namespace swift::misc
namespace swift::gui::components
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
} // namespace swift::gui::components

#endif // guard
