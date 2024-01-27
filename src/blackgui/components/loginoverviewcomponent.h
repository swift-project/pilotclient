// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_LOGINOVERVIEWCOMPONENT_H
#define BLACKGUI_COMPONENTS_LOGINOVERVIEWCOMPONENT_H

#include "blackcore/vatsim/vatsimsettings.h"
#include "blackcore/data/networksetup.h"
#include "blackgui/settings/guisettings.h"
#include "blackgui/overlaymessagesframe.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/simulation/data/lastmodel.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/network/server.h"
#include "blackmisc/network/user.h"
#include "blackmisc/digestsignal.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/datacache.h"

#include <QFrame>
#include <QIcon>
#include <QTimer>
#include <QObject>
#include <QScopedPointer>
#include <QString>

namespace Ui
{
    class CLoginOverviewComponent;
}
namespace BlackMisc::Simulation
{
    class CAircraftModel;
    class CSimulatedAircraft;
}
namespace BlackGui::Components
{
    /*!
     * Login component to flight network
     */
    class BLACKGUI_EXPORT CLoginOverviewComponent : public COverlayMessagesFrame
    {
        Q_OBJECT

    public:
        //! Log categories
        static const QStringList &getLogCategories();

        //! Constructor
        explicit CLoginOverviewComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CLoginOverviewComponent() override;

        //! Login requested
        void toggleNetworkConnection();

        //! Show current values
        void showCurrentValues();

    signals:
        //! Login
        void loginOrLogoffSuccessful();

        //! Cancelled
        void closeOverview();

    private:
        // -------------- others -----------------

        //! Login cancelled
        void cancel();

        //! Has contexts?
        bool hasValidContexts() const;

        static constexpr int OverlayMessageMs = 5000;
        static constexpr int LogoffIntervalSeconds = 20; //!< time before logoff

        QScopedPointer<Ui::CLoginOverviewComponent> ui;
        BlackCore::Data::CNetworkSetup m_networkSetup; //!< servers last used
    };
} // namespace

#endif // guard
