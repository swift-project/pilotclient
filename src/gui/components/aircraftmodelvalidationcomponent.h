// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_AIRCRAFTMODELVALIDATIONCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_AIRCRAFTMODELVALIDATIONCOMPONENT_H

#include <QFrame>
#include <QScopedPointer>

#include "gui/overlaymessagesframe.h"
#include "gui/swiftguiexport.h"
#include "misc/settingscache.h"
#include "misc/simulation/settings/modelmatchersettings.h"

namespace Ui
{
    class CAircraftModelValidationComponent;
}
namespace swift::misc
{
    class CStatusMessageList;
    namespace simulation
    {
        class CSimulatorInfo;
        class CAircraftModelList;
    } // namespace simulation
} // namespace swift::misc
namespace swift::gui::components
{
    /*!
     * Handle validation result
     */
    class SWIFT_GUI_EXPORT CAircraftModelValidationComponent : public COverlayMessagesFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAircraftModelValidationComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CAircraftModelValidationComponent() override;

        //! Validated a model set
        //! \remark coming from swift::misc::simulation::CBackgroundValidation
        void validatedModelSet(const swift::misc::simulation::CSimulatorInfo &simulator, const swift::misc::simulation::CAircraftModelList &valid, const swift::misc::simulation::CAircraftModelList &invalid, bool stopped, const swift::misc::CStatusMessageList &msgs);

    private:
        QScopedPointer<Ui::CAircraftModelValidationComponent> ui;
        swift::misc::CSetting<swift::misc::simulation::settings::TModelMatching> m_matchingSettings { this }; //!< settings
        qint64 m_lastResults = -1; //!< when received last results

        //! Invalid models
        void tempDisableModels(const swift::misc::simulation::CAircraftModelList &models);

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
        void saveInvalidModels(const swift::misc::simulation::CAircraftModelList &models) const;
    };
} // namespace swift::gui::components
#endif // guard
