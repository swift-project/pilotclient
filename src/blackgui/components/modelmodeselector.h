// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_MODELMODESELECTOR_H
#define BLACKGUI_COMPONENTS_MODELMODESELECTOR_H

#include "blackgui/blackguiexport.h"
#include "misc/simulation/aircraftmodel.h"

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
        swift::misc::simulation::CAircraftModel::ModelMode getMode() const;

        //! Set mode
        void setValue(swift::misc::simulation::CAircraftModel::ModelMode mode);

        //! Set mode
        void setValue(const swift::misc::simulation::CAircraftModel &model);

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
