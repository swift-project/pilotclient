// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "atcbuttoncomponent.h"

#include <math.h>

#include <QGridLayout>
#include <QPushButton>

#include "ui_atcbuttoncomponent.h"

#include "gui/guiapplication.h"
#include "gui/guiutility.h"
#include "misc/aviation/atcstationlist.h"

using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::network;
using namespace swift::core;
using namespace swift::core::context;

namespace swift::gui::components
{
    CAtcButtonComponent::CAtcButtonComponent(QWidget *parent) : QFrame(parent),
                                                                ui(new Ui::CAtcButtonComponent)
    {
        ui->setupUi(this);
        if (sGui && sGui->getIContextNetwork())
        {
            connect(sGui->getIContextNetwork(), &IContextNetwork::changedAtcStationsOnlineDigest, this, &CAtcButtonComponent::onChangedAtcStations);
            connect(sGui->getIContextNetwork(), &IContextNetwork::connectionStatusChanged, this, &CAtcButtonComponent::onConnectionStatusChanged);
        }

        this->setVisible(false); // will be changed when ATC stations are reported
    }

    CAtcButtonComponent::~CAtcButtonComponent()
    {}

    void CAtcButtonComponent::updateStations()
    {
        this->setVisible(false);
        this->setMinimumHeight(0);

        if (!sGui || !sGui->getIContextNetwork()) { return; }
        if (m_maxNumber < 1) { return; }
        const int max = qMin(m_maxNumber, m_rows * m_cols);
        CAtcStationList stations = sGui->getIContextNetwork()->getClosestAtcStationsOnline(max);
        if (stations.isEmpty()) { return; }

        CGuiUtility::deleteLayout(this->layout(), true);
        QGridLayout *layout = new QGridLayout(this);

        layout->setObjectName("gl_CAtcButtonComponent");
        layout->setSpacing(4);
        layout->setContentsMargins(0, 0, 0, 0);

        int row = 0;
        int col = 0;
        int added = 0;
        for (const CAtcStation &station : std::as_const(stations))
        {
            if (m_ignoreNonAtc)
            {
                // strict check, only "real ATC stations", no supervisors etc
                if (!station.getCallsign().hasAtcSuffix()) { continue; }
            }

            QPushButton *button = new QPushButton(this);
            button->setText(station.getCallsignAsString());
            if (m_withIcons) { button->setIcon(CIcon(station.toIcon()).toQIcon()); }
            QObject::connect(button, &QPushButton::released, this, &CAtcButtonComponent::onButtonClicked);
            const CVariant atcv = CVariant::fromValue(station);
            layout->addWidget(button, row, col++);
            button->show();
            button->setProperty("atc", atcv.getQVariant());
            added++;

            if (col >= m_cols)
            {
                if (row == m_rows) { break; }
                row++;
                col = 0;
            }
        }

        const double a = added;
        const double c = m_cols;
        const int rows = qRound(ceil(a / c)); // row can be too high
        if (added > 0)
        {
            this->setVisible(true);
            this->setMinimumHeight(rows * 25);
        }
    }

    void CAtcButtonComponent::setRowsColumns(int rows, int cols, bool setMaxElements)
    {
        m_rows = rows;
        m_cols = cols;
        if (setMaxElements) { m_maxNumber = rows * cols; }
    }

    void CAtcButtonComponent::onChangedAtcStations()
    {
        if (!m_backgroundUpdates) { return; }
        if (!this->isVisible()) { return; }
        this->updateStations();
    }

    void CAtcButtonComponent::onConnectionStatusChanged(const CConnectionStatus &from, const CConnectionStatus &to)
    {
        Q_UNUSED(from);
        if (to.isDisconnected())
        {
            this->setVisible(false);
        }
    }

    void CAtcButtonComponent::onButtonClicked()
    {
        QPushButton *button = qobject_cast<QPushButton *>(QObject::sender());
        if (!button) { return; }
        const CVariant v(button->property("atc"));
        if (!v.isValid() || !v.canConvert<CAtcStation>()) { return; }
        const CAtcStation station = v.value<CAtcStation>();
        emit this->requestAtcStation(station);
    }
} // namespace swift::gui::components
