// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "configurationwizard.h"

#include <QPointer>

#include "ui_configurationwizard.h"

#include "gui/guiapplication.h"
#include "gui/guiutility.h"
#include "misc/directoryutils.h"
#include "misc/math/mathutils.h"

using namespace swift::misc;
using namespace swift::misc::math;

namespace swift::gui::components
{
    CConfigurationWizard::CConfigurationWizard(QWidget *parent) : QWizard(parent), ui(new Ui::CConfigurationWizard)
    {
        ui->setupUi(this);
        this->setWindowFlags(windowFlags() | Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint |
                             Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);

        ui->wp_Legal->setConfigComponent(ui->comp_LegalInformation);

        ui->wp_DataLoad->setConfigComponent(ui->comp_DataLoad);
        ui->wp_CopySettingsAndCaches->setConfigComponent(ui->comp_CopySettingsAndCachesComponent);
        ui->wp_SelectSimulator->setConfigComponent(ui->comp_SelectSimulator);
        ui->wp_Simulator->setConfigComponent(ui->comp_Simulator);
        ui->wp_CopyModels->setConfigComponent(ui->comp_CopyModels);

        ui->wp_SimulatorSpecific->setConfigComponent(ui->comp_InstallXSwiftBus, ui->comp_InstallFsxTerrainProbe);
        ui->wp_Hotkeys->setConfigComponent(ui->comp_Hotkeys);
        ui->wp_Finish->setConfigComponent(ui->comp_FinishWizard);

        ui->comp_Hotkeys->registerDummyPttEntry();

        this->setButtonText(CustomButton1, "skip");

        // no other versions, skip copy pages
        // disabled afetr discussion with RP as it is confusing
        // if (!CApplicationInfoList::hasOtherSwiftDataDirectories()) { this->setStartId(SelectSimulator); }

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
            sGui->showHelp("install");
        });

        this->setScreenGeometry();
        CGuiUtility::setWizardButtonWidths(this);
    }

    CConfigurationWizard::~CConfigurationWizard() = default;

    bool CConfigurationWizard::lastStepSkipped() const { return m_skipped; }

    bool CConfigurationWizard::lastWizardStepSkipped(const QWizard *standardWizard)
    {
        const auto *wizard = qobject_cast<const CConfigurationWizard *>(standardWizard);
        return wizard && wizard->lastStepSkipped();
    }

    void CConfigurationWizard::wizardCurrentIdChanged(int id)
    {
        // Remember previous ID
        const int previousId = m_previousId;

        // Update state: Save previous ID, determine forward
        m_previousId = id;
        m_forward = (id > previousId);

        // reset skipped flag
        m_skipped = false;

        // Load selected simulators and activated options
        const swift::misc::simulation::CSimulatorInfo selectedSims = ui->comp_SelectSimulator->getSelectedSimulators();

        bool showModelGenerationPage = false;
        bool showPttPage = false;
        QStringList enabledOptions = m_enabledConfigOptions.get(); // force reload
        if (!enabledOptions.isEmpty())
        {
            showModelGenerationPage = enabledOptions.takeFirst().contains("true");
            if (!enabledOptions.isEmpty()) { showPttPage = enabledOptions.first().contains("true"); }
        }

        // display semi-transparent
        this->setParentOpacity(0.5);

        // If no other data directories exist, skip certain pages.
        if (!CApplicationInfoList::hasOtherSwiftDataDirectories() && (id == 2 || id == 3))
        {
            if (!m_forward)
            {
                id = DataLoad;
                m_previousId = SelectSimulator;
            }
            else
            {
                id = SelectSimulator;
                m_previousId = DataLoad;
            }
        }

        // If we have just passed SelectSimulator, make sure a simulator is selected.
        if (previousId == SelectSimulator && id > SelectSimulator)
        {
            if (!selectedSims.isAnySimulator())
            {
                // return to selection, validation message and cancel
                id = SelectSimulator;
                m_previousId = SelectSimulator;
                this->setCurrentId(SelectSimulator);
                static const CStatusMessage msg = CStatusMessage().validationError(
                    u"Please select at least one simulator to continue the configuration.");
                CLogMessage::preformatted(msg);
            }
        }

        // Skip AutoModelSet if disabled
        if (id == AutoModelSet && !showModelGenerationPage) { ++id; }

        if (id == XSwiftBus && !selectedSims.isXPlane())
        {
            if (m_forward)
            {
                id = ConfigHotkeys;
                m_previousId = AutoModelSet;
            }
            else
            {
                id = AutoModelSet;
                m_previousId = ConfigHotkeys;
            }
        }

        // Skip ConfigHotkeys when PTT is disabled
        if (id == ConfigHotkeys && !showPttPage)
        {
            if (m_forward)
            {
                id = FinshWizard;
                m_previousId = XSwiftBus;
            }
            else
            {
                id = XSwiftBus;
                m_previousId = FinshWizard;
            }
        }

        // final adjustments
        // jump to a specific page
        this->setCurrentId(id);

        const QWizardPage *page = this->currentPage();
        Q_UNUSED(page);

        this->setOption(HaveCustomButton1, id != m_maxId);
    }

    void CConfigurationWizard::clickedCustomButton(int which)
    {

        if (which == static_cast<int>(CustomButton1)) // skip
        {
            m_skipped = true;
            this->next();
        }
        else { m_skipped = false; }
    }

    void CConfigurationWizard::ended() { this->setParentOpacity(1.0); }

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

} // namespace swift::gui::components
