// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_MODELMODESELECTOR_H
#define BLACKGUI_COMPONENTS_MODELMODESELECTOR_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/simulation/aircraftmodel.h"

#include <QFrame>
#include <QObject>
#include <QScopedPointer>

class QWidget;

namespace Ui
{
    class CModelModeSelector;
}

namespace BlackGui::Components
{
    /*!
     * Select the mode
     */
    class BLACKGUI_EXPORT CModelModeSelector : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CModelModeSelector(QWidget *parent = nullptr);

        //! Destructor
        ~CModelModeSelector();

        //! Get mode
        BlackMisc::Simulation::CAircraftModel::ModelMode getMode() const;

        //! Set mode
        void setValue(BlackMisc::Simulation::CAircraftModel::ModelMode mode);

        //! Set mode
        void setValue(const BlackMisc::Simulation::CAircraftModel &model);

        //! Read only
        void setReadOnly(bool readOnly);

    signals:
        //! Value changed
        void changed();

    private:
        QScopedPointer<Ui::CModelModeSelector> ui;
    };
} // ns

#endif // guard
