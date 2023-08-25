// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_SETTINGSMODELCOMPONENT_H
#define BLACKGUI_COMPONENTS_SETTINGSMODELCOMPONENT_H

#include "blackgui/settings/guisettings.h"
#include "blackmisc/simulation/settings/modelsettings.h"
#include <QFrame>

namespace Ui
{
    class CSettingsModelComponent;
}
namespace BlackCore::Db
{
    class CBackgroundDataUpdater;
}
namespace BlackGui::Components
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
        virtual ~CSettingsModelComponent();

        //! Interval, -1 for disabled
        int getBackgroundUpdaterIntervallSecs() const;

        //! Updater (the updater this setting is for)
        void setBackgroundUpdater(const BlackCore::Db::CBackgroundDataUpdater *updater);

    private:
        QScopedPointer<Ui::CSettingsModelComponent> ui;
        BlackMisc::CSetting<Settings::TBackgroundConsolidation> m_consolidationSetting { this, &CSettingsModelComponent::cacheChanged }; //!< consolidation time
        BlackMisc::CSetting<BlackMisc::Simulation::Settings::TModel> m_modelSettings { this, &CSettingsModelComponent::cacheChanged }; //!< model setting
        const BlackCore::Db::CBackgroundDataUpdater *m_updater = nullptr; //!< externally (i.e. other component) provided existing updater

        //! Consolidation time entered
        void consolidationEntered();

        //! Allow excluded models changed
        void allowExcludedModelsChanged(bool allow);

        //! Cache has been changed
        void cacheChanged();
    };
} // ns

#endif // guard
