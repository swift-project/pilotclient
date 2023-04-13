/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_CDBOWNMODELSDIALOG_H
#define BLACKGUI_COMPONENTS_CDBOWNMODELSDIALOG_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/simulation/simulatorinfo.h"
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
        void setSimulator(const BlackMisc::Simulation::CSimulatorInfo &simulator);

        //! \copydoc CDbOwnModelsComponent::requestModelsInBackground
        bool requestModelsInBackground(const BlackMisc::Simulation::CSimulatorInfo &simulator, bool onlyIfNotEmpty);

        //! \copydoc CDbOwnModelsComponent::getOwnModelsCount
        int getOwnModelsCount() const;

        //! Direct access to component
        //! \remark allows to use the powerful component class
        const CDbOwnModelsComponent *modelsComponent() const;

    signals:
        //! \copydoc CDbOwnModelsComponent::successfullyLoadedModels
        void successfullyLoadedModels(const BlackMisc::Simulation::CSimulatorInfo &simulator, int count);

    private:
        QScopedPointer<Ui::CDbOwnModelsDialog> ui;
    };
} // ns
#endif // guard
