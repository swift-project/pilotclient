/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "configurationwizard.h"
#include "ui_configurationwizard.h"
#include "blackgui/guiapplication.h"
#include "blackgui/guiutility.h"
#include "blackmisc/math/mathutils.h"
#include "blackmisc/directoryutils.h"
#include <QPointer>

using namespace BlackMisc;
using namespace BlackMisc::Math;

namespace BlackGui::Components
{
    CConfigurationWizard::CConfigurationWizard(QWidget *parent) : QWizard(parent),
                                                                  ui(new Ui::CConfigurationWizard)
    {
        ui->setupUi(this);
        this->setWindowFlags(windowFlags() | Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);

        ui->wp_CopyModels->setConfigComponent(ui->comp_CopyModels);
        ui->wp_CopySettingsAndCaches->setConfigComponent(ui->comp_CopySettingsAndCachesComponent);
        ui->wp_Simulator->setConfigComponent(ui->comp_Simulator);
        ui->wp_SimulatorSpecific->setConfigComponent(ui->comp_InstallXSwiftBus, ui->comp_InstallFsxTerrainProbe);
        ui->wp_DataLoad->setConfigComponent(ui->comp_DataLoad);
        ui->wp_Hotkeys->setConfigComponent(ui->comp_Hotkeys);
        ui->wp_Legal->setConfigComponent(ui->comp_LegalInformation);
        ui->comp_Hotkeys->registerDummyPttEntry();
        this->setButtonText(CustomButton1, "skip");

        // no other versions, skip copy pages
        // disabled afetr discussion with RP as it is confusing
        // if (!CApplicationInfoList::hasOtherSwiftDataDirectories()) { this->setStartId(ConfigSimulator); }

        ui->tb_SimulatorSpecific->setCurrentWidget(ui->comp_InstallXSwiftBus);

        const QList<int> ids = this->pageIds();
        const auto mm = std::minmax_element(ids.begin(), ids.end());
        m_maxId = *mm.second;
        m_minId = *mm.first;

        connect(this, &QWizard::currentIdChanged, this, &CConfigurationWizard::wizardCurrentIdChanged);
        connect(this, &QWizard::customButtonClicked, this, &CConfigurationWizard::clickedCustomButton);
        connect(this, &QWizard::rejected, this, &CConfigurationWizard::ended);
        connect(this, &QWizard::accepted, this, &CConfigurationWizard::ended);

        Q_ASSERT_X(sGui, Q_FUNC_INFO, "missing sGui");
        const QPointer<CConfigurationWizard> myself(this);
        connect(this, &QWizard::helpRequested, sGui, [=] {
            if (!myself) { return; }
            if (!sGui || sGui->isShuttingDown()) { return; }
            sGui->showHelp(this);
        });

        this->setScreenGeometry();
        CGuiUtility::setWizardButtonWidths(this);
    }

    CConfigurationWizard::~CConfigurationWizard()
    {}

    bool CConfigurationWizard::lastStepSkipped() const
    {
        return m_skipped;
    }

    bool CConfigurationWizard::lastWizardStepSkipped(const QWizard *standardWizard)
    {
        const CConfigurationWizard *wizard = qobject_cast<const CConfigurationWizard *>(standardWizard);
        return wizard && wizard->lastStepSkipped();
    }

    bool CConfigurationWizard::event(QEvent *event)
    {
        if (CGuiApplication::triggerShowHelp(this, event)) { return true; }
        return QDialog::event(event);
    }

    void CConfigurationWizard::wizardCurrentIdChanged(int id)
    {
        const int previousId = m_previousId;
        const bool backward = id < previousId;
        const bool skipped = m_skipped;
        m_previousId = id; // update
        m_skipped = false; // reset
        Q_UNUSED(skipped);
        Q_UNUSED(backward);

        this->setParentOpacity(0.5);
        const QWizardPage *page = this->currentPage();
        Q_UNUSED(page);

        this->setOption(HaveCustomButton1, id != m_maxId);
    }

    void CConfigurationWizard::clickedCustomButton(int which)
    {
        if (which == static_cast<int>(CustomButton1))
        {
            m_skipped = true;
            this->next();
        }
        else
        {
            m_skipped = false;
        }
    }

    void CConfigurationWizard::ended()
    {
        this->setParentOpacity(1.0);
    }

    void CConfigurationWizard::setParentOpacity(qreal opacity)
    {
        QWidget *parent = this->parentWidget();
        if (!parent) { return; }
        if (CMathUtils::epsilonEqual(parent->windowOpacity(), opacity)) { return; }
        parent->setWindowOpacity(opacity);
    }

    void CConfigurationWizard::setScreenGeometry()
    {
        if (!sGui) { return; }
        const QRect g = CGuiApplication::currentScreenGeometry();

        // 1280/720 on 4k hires
        // 1920/1280 on non hires 1920 displays
        const int gw = g.width();
        const int gh = g.height();
        const int calcW = qRound(gw * 0.8);
        const int calcH = qRound(gh * 0.9); // normally critical as buttons are hidden

        // do not get too huge
        const int w = qMin(900, calcW);
        const int h = qMin(750, calcH);
        this->resize(w, h);
    }
} // ns
