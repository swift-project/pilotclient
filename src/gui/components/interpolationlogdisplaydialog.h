// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_INTERPOLATIONLOGDISPLAYDIALOG_H
#define SWIFT_GUI_COMPONENTS_INTERPOLATIONLOGDISPLAYDIALOG_H

#include <QDialog>
#include <QScopedPointer>

#include "gui/swiftguiexport.h"

namespace swift::core
{
    class ISimulator;
    class CAirspaceMonitor;
} // namespace swift::core
namespace Ui
{
    class CInterpolationLogDisplayDialog;
}
namespace swift::gui::components
{
    /*!
     * CInterpolationLogDisplay as dialog
     */
    class SWIFT_GUI_EXPORT CInterpolationLogDisplayDialog : public QDialog
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CInterpolationLogDisplayDialog(
            swift::core::ISimulator *simulator, swift::core::CAirspaceMonitor *airspaceMonitor,
            QWidget *parent = nullptr);

        //! Destructor
        virtual ~CInterpolationLogDisplayDialog() override;

        //! Set simulator
        void setSimulator(swift::core::ISimulator *simulator);

        //! Set airspace monitor
        void setAirspaceMonitor(swift::core::CAirspaceMonitor *airspaceMonitor);

    protected:
        //! \copydoc QObject::event
        virtual bool event(QEvent *event) override;

    private:
        QScopedPointer<Ui::CInterpolationLogDisplayDialog> ui;
    };
} // namespace swift::gui::components

#endif // guard
