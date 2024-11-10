// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_INTERNALSCOMPONENT_H
#define SWIFT_GUI_INTERNALSCOMPONENT_H

#include "gui/swiftguiexport.h"
#include "misc/aviation/aircraftparts.h"

#include <QObject>
#include <QScopedPointer>
#include <QWidget>

class QShowEvent;

namespace Ui
{
    class CInternalsComponent;
}
namespace swift::gui::components
{
    //! Internals for debugging, statistics
    class SWIFT_GUI_EXPORT CInternalsComponent : public QWidget
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CInternalsComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CInternalsComponent() override;

    protected:
        //! \copydoc QWidget::showEvent
        virtual void showEvent(QShowEvent *event) override;

    private:
        //! Enable / disable debugging
        void enableDebug(int state);

        //! Send the text message
        void sendTextMessage();

        //! Send ATIS
        void sendAtis();

        //! Send a dummy status message
        void logStatusMessage();

        //! Show log files
        void showLogFiles();

        //! Display own parts
        void displayOwnParts();

        //! Log in simulator
        void displayLogInSimulator();

        //! Set the context flags
        void contextFlagsToGui();

        //! Simulate a crash
        void simulateCrash();

        //! Simulate ASSERT
        void simulateAssert();

        //! Checkbox toggled
        void onCrashDumpUploadToggled(bool checked);

        //! Show network statistics
        void networkStatistics();

        //! Enable/disable statistics
        void onNetworkStatisticsToggled(bool checked);

        QScopedPointer<Ui::CInternalsComponent> ui;
    };
} // namespace

#endif // guard
