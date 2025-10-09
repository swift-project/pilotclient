// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_SETTINGSMODELCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_SETTINGSMODELCOMPONENT_H

#include <QFrame>

#include "gui/settings/guisettings.h"
#include "misc/simulation/settings/modelsettings.h"

namespace Ui
{
    class CSettingsModelComponent;
}
namespace swift::core::db
{
    class CBackgroundDataUpdater;
}
namespace swift::gui::components
{
    /*!
     * Settings UI for model matching/mapping
     */
    class CSettingsModelComponent : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CSettingsModelComponent(QWidget *parent = nullptr);

        //! Destructor
        ~CSettingsModelComponent() override;

        //! Interval, -1 for disabled
        int getBackgroundUpdaterIntervallSecs() const;

        //! Updater (the updater this setting is for)
        void setBackgroundUpdater(const swift::core::db::CBackgroundDataUpdater *updater);

    private:
        QScopedPointer<Ui::CSettingsModelComponent> ui;
        swift::misc::CSetting<settings::TBackgroundConsolidation> m_consolidationSetting {
            this, &CSettingsModelComponent::cacheChanged
        }; //!< consolidation time
        swift::misc::CSetting<swift::misc::simulation::settings::TModel> m_modelSettings {
            this, &CSettingsModelComponent::cacheChanged
        }; //!< model setting
        const swift::core::db::CBackgroundDataUpdater *m_updater =
            nullptr; //!< externally (i.e. other component) provided existing updater

        //! Consolidation time entered
        void consolidationEntered();

        //! Allow excluded models changed
        void allowExcludedModelsChanged(bool allow);

        //! Cache has been changed
        void cacheChanged();
    };
} // namespace swift::gui::components

#endif // SWIFT_GUI_COMPONENTS_SETTINGSMODELCOMPONENT_H
