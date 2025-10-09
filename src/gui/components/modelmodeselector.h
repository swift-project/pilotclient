// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_MODELMODESELECTOR_H
#define SWIFT_GUI_COMPONENTS_MODELMODESELECTOR_H

#include <QFrame>
#include <QObject>
#include <QScopedPointer>

#include "gui/swiftguiexport.h"
#include "misc/simulation/aircraftmodel.h"

class QWidget;

namespace Ui
{
    class CModelModeSelector;
}

namespace swift::gui::components
{
    /*!
     * Select the mode
     */
    class SWIFT_GUI_EXPORT CModelModeSelector : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CModelModeSelector(QWidget *parent = nullptr);

        //! Destructor
        ~CModelModeSelector() override;

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
} // namespace swift::gui::components

#endif // SWIFT_GUI_COMPONENTS_MODELMODESELECTOR_H
