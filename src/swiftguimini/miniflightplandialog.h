// SPDX-FileCopyrightText: Copyright (C) 2026 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFTGUIMINI_MINIFLIGHTPLANDIALOG_H
#define SWIFTGUIMINI_MINIFLIGHTPLANDIALOG_H

#include <QDialog>

#include "gui/settings/minisettings.h"
#include "misc/aviation/flightplan.h"
#include "misc/settingscache.h"

class QCloseEvent;

namespace swift::gui::components
{
    class CFlightPlanComponent;
}

//! Simple dialog that wraps CFlightPlanComponent (as in swiftguistandard).
class CMiniFlightPlanDialog : public QDialog
{
    Q_OBJECT

public:
    //! Constructor
    explicit CMiniFlightPlanDialog(QWidget *parent = nullptr);

    //! Destructor
    ~CMiniFlightPlanDialog() override;

    //! Validate and send the flight plan (if connected), delegates to CFlightPlanComponent::sendFlightPlan().
    void sendFlightPlan();

    //! Get the currently entered flight plan data (callsign, aircraft type etc.), without validation messages.
    //! \remark delegates to CFlightPlanComponent::getCurrentFlightPlanData().
    swift::misc::aviation::CFlightPlan getCurrentFlightPlanData() const;

    //! Whether a flight plan is currently considered available
    //! persisted Mini-GUI flight plan at construction time, or since then loaded/filled/sent in the dialog.
    //! \remark Needed because the initial flightPlanDataChanged() emission (triggered by restoring the
    //! last saved flight plan in the constructor) happens before CMiniGuiWindow has wired up its
    //! connections to this signal, and would otherwise be missed.
    bool hasFlightPlanLoaded() const { return m_hasFlightPlan; }

public slots:
    //! Prefills the flight plan dialog with the given data, see CFlightPlanComponent::fillWithFlightPlanData().
    //! \remark Used e.g. to forward a flight plan loaded via the quick-access SimBrief button in
    //! "Own Aircraft" (CMiniLoginWidget::simBriefFlightPlanLoaded()), so the "Flight Plan Management"
    //! dialog stays the single source of truth.
    void fillWithFlightPlanData(const swift::misc::aviation::CFlightPlan &flightPlan);

signals:
    //! A flight plan has been loaded, filled in or sent (SimBrief, file, manual input or network)
    void flightPlanDataChanged(bool available);

    //! Same trigger as flightPlanDataChanged(), but also carries the current flight plan content
    //! (e.g. callsign, aircraft type), so it can be compared against "Own Aircraft".
    void flightPlanContentChanged(const swift::misc::aviation::CFlightPlan &flightPlan);

protected:
    //! \copydoc QDialog::closeEvent
    //! Persists the currently entered flight plan values locally (Mini-GUI only), so they can be
    //! restored as the active flight plan the next time the Mini-GUI is started.
    void closeEvent(QCloseEvent *event) override;

private:
    //! Internal tracker for flightPlanDataChanged(), so hasFlightPlanLoaded() has a value even before
    //! CMiniGuiWindow's connections to this dialog exist (see constructor restoring the last flight plan).
    void onFlightPlanDataChanged(bool available);

    swift::gui::components::CFlightPlanComponent *m_flightPlanComponent = nullptr;
    swift::misc::CSetting<swift::gui::settings::TMiniLastFlightPlan> m_lastFlightPlanSetting { this };
    bool m_hasFlightPlan = false;
};

#endif // SWIFTGUIMINI_MINIFLIGHTPLANDIALOG_H