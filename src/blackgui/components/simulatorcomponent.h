/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_SIMULATORCOMPONENT_H
#define BLACKGUI_SIMULATORCOMPONENT_H

#include "blackgui/enablefordockwidgetinfoarea.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/simulation/simulatorinfo.h"
#include "blackmisc/logcategories.h"
#include "blackmisc/icons.h"

#include <QObject>
#include <QTimer>
#include <QScopedPointer>
#include <QString>
#include <QTabWidget>
#include <QtGlobal>

namespace Ui
{
    class CSimulatorComponent;
}
namespace BlackMisc
{
    class CIcon;
    class CStatusMessageList;
    namespace Simulation
    {
        class CSimulatedAircraft;
    }
}
namespace BlackGui::Components
{
    //! Simulator component
    class BLACKGUI_EXPORT CSimulatorComponent :
        public QTabWidget,
        public CEnableForDockWidgetInfoArea
    {
        Q_OBJECT

    public:
        //! Categories
        static const QStringList &getLogCategories();

        //! Constructor
        explicit CSimulatorComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CSimulatorComponent();

        //! Number of entries
        int rowCount() const;

        //! Clear
        void clear(bool addInternalsAfterwards = false);

        //! Update simulator
        void update();

    private:
        //! \copydoc ISimulator::simulatorStatusChanged
        void onSimulatorStatusChanged(int status);

        //! \copydoc ISimulator::addingRemoteModelFailed
        void onAddingRemoteModelFailed(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, bool disabled, bool failover, const BlackMisc::CStatusMessage &message);

        //! \copydoc ISimulator::onSimulatorMessages
        void onSimulatorMessages(const BlackMisc::CStatusMessageList &messages);

        //! Refresh the internals
        void refreshInternals();

        //! Update interval
        int getUpdateIntervalMs() const;

        //! Simple add or update name / value pair
        void addOrUpdateLiveDataByName(const QString &name, const QString &value, const BlackMisc::CIcon &icon);

        //! Simple add or update name / value pair
        void addOrUpdateLiveDataByName(const QString &name, const QString &value, BlackMisc::CIcons::IconIndex iconIndex);

        //! Remove name
        void removeLiveDataByName(const QString &name);

        QScopedPointer<Ui::CSimulatorComponent> ui;
        QTimer m_updateTimer;
        BlackMisc::Simulation::CSimulatorInfo m_simulator;
    };
} // ns

#endif // guard
