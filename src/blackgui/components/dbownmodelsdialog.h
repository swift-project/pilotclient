// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_CDBOWNMODELSDIALOG_H
#define BLACKGUI_COMPONENTS_CDBOWNMODELSDIALOG_H

#include "blackgui/blackguiexport.h"
#include "misc/simulation/simulatorinfo.h"
#include <QDialog>
#include <QScopedPointer>

namespace Ui
{
    class CDbOwnModelsDialog;
}
namespace BlackGui::Components
{
    class CDbOwnModelsComponent;

    //! Own models dialog
    class BLACKGUI_EXPORT CDbOwnModelsDialog : public QDialog
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CDbOwnModelsDialog(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CDbOwnModelsDialog() override;

        //! \copydoc CDbOwnModelsComponent::setSimulator
        void setSimulator(const swift::misc::simulation::CSimulatorInfo &simulator);

        //! \copydoc CDbOwnModelsComponent::requestModelsInBackground
        bool requestModelsInBackground(const swift::misc::simulation::CSimulatorInfo &simulator, bool onlyIfNotEmpty);

        //! \copydoc CDbOwnModelsComponent::getOwnModelsCount
        int getOwnModelsCount() const;

        //! Direct access to component
        //! \remark allows to use the powerful component class
        const CDbOwnModelsComponent *modelsComponent() const;

    signals:
        //! \copydoc CDbOwnModelsComponent::successfullyLoadedModels
        void successfullyLoadedModels(const swift::misc::simulation::CSimulatorInfo &simulator, int count);

    private:
        QScopedPointer<Ui::CDbOwnModelsDialog> ui;
    };
} // ns
#endif // guard
