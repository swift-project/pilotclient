// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_INTERPOLATIONSETUPCOMPONENT_H
#define BLACKGUI_COMPONENTS_INTERPOLATIONSETUPCOMPONENT_H

#include "misc/simulation/interpolation/interpolationsetuplist.h"
#include "blackgui/overlaymessagesframe.h"
#include <QFrame>
#include <QScopedPointer>
#include <QModelIndex>

namespace Ui
{
    class CInterpolationSetupComponent;
}
namespace BlackGui::Components
{
    //! Setup component
    class CInterpolationSetupComponent : public COverlayMessagesFrame
    {
        Q_OBJECT

    public:
        //! Mode of setup
        enum Mode
        {
            SetupGlobal,
            SetupPerCallsign
        };

        //! Constructor
        explicit CInterpolationSetupComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CInterpolationSetupComponent() override;

        //! Setup mode
        Mode getSetupMode() const;

    signals:
        //! Request restrictions UI
        void requestRenderingRestrictionsWidget();

    private:
        QScopedPointer<Ui::CInterpolationSetupComponent> ui;

        //! Double clicked in view
        void onRowDoubleClicked(const QModelIndex &index);

        //! Mode has been changed
        void onModeChanged();

        //! View has been changed
        void onModelChanged();

        //! Requested reload
        void onReloadSetup();

        //! Reload
        void reloadSetup();

        //! Save a setup
        void saveSetup();

        //! Delete a setup
        void removeOrResetSetup();

        //! Values from global setup
        void setUiValuesFromGlobal();

        //! Values per callsign
        void displaySetupsPerCallsign();

        //! Check prerequisites before saving etc.
        bool checkPrerequisites(bool checkSim, bool showOverlay);

        //! Send to context
        bool setSetupsToContext(const swift::misc::simulation::CInterpolationSetupList &setups, bool force = false);

        //! Setup chaged
        void onSetupChanged();

        //! Objects have been deleted
        void onObjectsDeleted(const swift::misc::CVariant &deletedObjects);

        swift::misc::simulation::CInterpolationSetupList m_lastSetSetups; //!< last setups set to context
    };
} // ns

#endif // guard
