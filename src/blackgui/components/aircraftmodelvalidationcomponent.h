/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_AIRCRAFTMODELVALIDATIONCOMPONENT_H
#define BLACKGUI_COMPONENTS_AIRCRAFTMODELVALIDATIONCOMPONENT_H

#include "blackmisc/simulation/settings/modelmatchersettings.h"
#include "blackmisc/settingscache.h"
#include "blackgui/overlaymessagesframe.h"
#include "blackgui/blackguiexport.h"

#include <QFrame>
#include <QScopedPointer>

namespace Ui { class CAircraftModelValidationComponent; }
namespace BlackMisc
{
    class CStatusMessageList;
    namespace Simulation
    {
        class CSimulatorInfo;
        class CAircraftModelList;
    }
}
namespace BlackGui::Components
{
    /**
     * Handle validation result
     */
    class BLACKGUI_EXPORT CAircraftModelValidationComponent : public COverlayMessagesFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAircraftModelValidationComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CAircraftModelValidationComponent() override;

        //! Validated a model set
        //! \remark coming from BlackMisc::Simulation::CBackgroundValidation
        void validatedModelSet(const BlackMisc::Simulation::CSimulatorInfo &simulator, const BlackMisc::Simulation::CAircraftModelList &valid, const BlackMisc::Simulation::CAircraftModelList &invalid, bool stopped, const BlackMisc::CStatusMessageList &msgs);

    private:
        QScopedPointer<Ui::CAircraftModelValidationComponent> ui;
        BlackMisc::CSetting<BlackMisc::Simulation::Settings::TModelMatching> m_matchingSettings { this }; //!< settings
        qint64 m_lastResults = -1; //!< when received last results

        //! Invalid models
        void tempDisableModels(const BlackMisc::Simulation::CAircraftModelList &models);

        //! Enable/disable startup check
        void onCheckAtStartupChanged(bool checked);

        //! Only show ieth warnings/errors
        void onOnlyErrorWarningChanged(bool checked);

        //! Trigger new validation
        void triggerValidation();

        //! Last results if any
        void requestLastResults();

        //! Button has been clicked
        void onTempDisabledButtonClicked();

        //! Remove from model set
        void onRemoveButtonClicked();

        //! Show help
        void showHelp();

        //! Save invalid models
        void saveInvalidModels(const BlackMisc::Simulation::CAircraftModelList &models) const;
    };
} // ns
#endif // guard
