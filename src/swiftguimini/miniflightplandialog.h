// SPDX-FileCopyrightText: Copyright (C) 2026 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFTGUIMINI_MINIFLIGHTPLANDIALOG_H
#define SWIFTGUIMINI_MINIFLIGHTPLANDIALOG_H

#include <QDialog>

namespace swift::gui::components
{
    class CFlightPlanComponent;
}

//! Einfacher Dialog, der CFlightPlanComponent (wie in swiftguistandard) kapselt.
class CMiniFlightPlanDialog : public QDialog
{
    Q_OBJECT

public:
    //! Constructor
    explicit CMiniFlightPlanDialog(QWidget *parent = nullptr);

    //! Destructor
    ~CMiniFlightPlanDialog() override;

private:
    swift::gui::components::CFlightPlanComponent *m_flightPlanComponent = nullptr;
};

#endif // SWIFTGUIMINI_MINIFLIGHTPLANDIALOG_H