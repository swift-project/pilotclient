// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_SIMULATORCOMPONENT_H
#define SWIFT_GUI_SIMULATORCOMPONENT_H

#include <QObject>
#include <QScopedPointer>
#include <QString>
#include <QTabWidget>
#include <QTimer>
#include <QtGlobal>

#include "gui/enablefordockwidgetinfoarea.h"
#include "gui/swiftguiexport.h"
#include "misc/icons.h"
#include "misc/logcategories.h"
#include "misc/simulation/simulatorinfo.h"

namespace Ui
{
    class CSimulatorComponent;
}
namespace swift::misc
{
    class CIcon;
    class CStatusMessageList;
    namespace simulation
    {
        class CSimulatedAircraft;
    }
} // namespace swift::misc
namespace swift::gui::components
{
    //! Simulator component
    class SWIFT_GUI_EXPORT CSimulatorComponent : public QTabWidget, public CEnableForDockWidgetInfoArea
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
        void onAddingRemoteModelFailed(const swift::misc::simulation::CSimulatedAircraft &aircraft, bool disabled,
                                       bool failover, const swift::misc::CStatusMessage &message);

        //! \copydoc ISimulator::onSimulatorMessages
        void onSimulatorMessages(const swift::misc::CStatusMessageList &messages);

        //! Refresh the internals
        void refreshInternals();

        //! Update interval
        int getUpdateIntervalMs() const;

        //! Simple add or update name / value pair
        void addOrUpdateLiveDataByName(const QString &name, const QString &value, const swift::misc::CIcon &icon);

        //! Simple add or update name / value pair
        void addOrUpdateLiveDataByName(const QString &name, const QString &value,
                                       swift::misc::CIcons::IconIndex iconIndex);

        //! Remove name
        void removeLiveDataByName(const QString &name);

        QScopedPointer<Ui::CSimulatorComponent> ui;
        QTimer m_updateTimer;
        swift::misc::simulation::CSimulatorInfo m_simulator;
    };
} // namespace swift::gui::components

#endif // guard
