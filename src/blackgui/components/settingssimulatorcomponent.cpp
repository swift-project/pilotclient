// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackcore/context/contextapplication.h"
#include "blackcore/context/contextsimulator.h"
#include "blackcore/pluginmanagersimulator.h"
#include "blackgui/components/settingssimulatorcomponent.h"
#include "blackgui/guiapplication.h"
#include "blackgui/led.h"
#include "blackgui/pluginconfig.h"
#include "blackgui/pluginconfigwindow.h"
#include "blackgui/plugindetailswindow.h"
#include "blackgui/pluginselector.h"
#include "misc/simulation/simulatorplugininfo.h"
#include "misc/simulation/simulatorplugininfolist.h"
#include "misc/pq/length.h"
#include "misc/pq/time.h"
#include "misc/pq/units.h"
#include "misc/iterator.h"
#include "misc/logmessage.h"
#include "misc/statusmessage.h"
#include "misc/verify.h"
#include "ui_settingssimulatorcomponent.h"

#include <QApplication>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QStringList>
#include <Qt>
#include <QtGlobal>
#include <algorithm>

using namespace swift::misc;
using namespace swift::misc::physical_quantities;
using namespace swift::misc::aviation;
using namespace swift::misc::simulation;
using namespace swift::misc::simulation::settings;
using namespace BlackCore;
using namespace BlackCore::Context;

namespace BlackGui::Components
{
    CSettingsSimulatorComponent::CSettingsSimulatorComponent(QWidget *parent) : QFrame(parent),
                                                                                ui(new Ui::CSettingsSimulatorComponent),
                                                                                m_plugins(new CPluginManagerSimulator(this))
    {
        Q_ASSERT_X(sGui, Q_FUNC_INFO, "Missing sGui");
        Q_ASSERT_X(sGui->getIContextSimulator(), Q_FUNC_INFO, "Missing context");

        m_plugins->collectPlugins();
        ui->setupUi(this);
        const CLedWidget::LedShape shape = CLedWidget::Circle;
        ui->led_RestrictedRendering->setValues(CLedWidget::Yellow, CLedWidget::Black, shape, "Limited", "Unlimited", 14);
        ui->led_RenderingEnabled->setValues(CLedWidget::Yellow, CLedWidget::Black, shape, "Rendering enabled", "No aircraft will be rendered", 14);

        ui->le_MaxAircraft->setValidator(new QIntValidator(ui->le_MaxAircraft));
        ui->le_MaxDistance->setValidator(new QIntValidator(ui->le_MaxDistance));

        // connects
        connect(ui->pluginSelector_EnabledSimulators, &CPluginSelector::pluginStateChanged, this, &CSettingsSimulatorComponent::pluginStateChanged);
        connect(ui->pluginSelector_EnabledSimulators, &CPluginSelector::pluginDetailsRequested, this, &CSettingsSimulatorComponent::showPluginDetails);
        connect(ui->pluginSelector_EnabledSimulators, &CPluginSelector::pluginConfigRequested, this, &CSettingsSimulatorComponent::showPluginConfig);

        connect(ui->pb_Reload, &QCheckBox::pressed, this, &CSettingsSimulatorComponent::onReload, Qt::QueuedConnection);
        connect(ui->pb_ApplyMaxAircraft, &QCheckBox::pressed, this, &CSettingsSimulatorComponent::onApplyMaxRenderedAircraft, Qt::QueuedConnection);
        connect(ui->pb_ApplyTimeSync, &QCheckBox::pressed, this, &CSettingsSimulatorComponent::onApplyTimeSync, Qt::QueuedConnection);
        connect(ui->pb_ApplyMaxDistance, &QCheckBox::pressed, this, &CSettingsSimulatorComponent::onApplyMaxRenderedDistance, Qt::QueuedConnection);
        connect(ui->pb_ApplyComSync, &QCheckBox::pressed, this, &CSettingsSimulatorComponent::onApplyComSync, Qt::QueuedConnection);
        connect(ui->pb_ApplyCGSource, &QCheckBox::pressed, this, &CSettingsSimulatorComponent::onApplyCGSource, Qt::QueuedConnection);
        connect(ui->pb_ApplyRecordOwnAircraftGnd, &QCheckBox::pressed, this, &CSettingsSimulatorComponent::onApplyRecordGnd, Qt::QueuedConnection);

        connect(ui->pb_ClearRestrictedRendering, &QCheckBox::pressed, this, &CSettingsSimulatorComponent::clearRestricedRendering);
        connect(ui->pb_DisableRendering, &QCheckBox::pressed, this, &CSettingsSimulatorComponent::onApplyDisableRendering);
        connect(ui->pb_Check, &QCheckBox::pressed, this, &CSettingsSimulatorComponent::checkSimulatorPlugins);
        connect(ui->le_MaxAircraft, &QLineEdit::editingFinished, this, &CSettingsSimulatorComponent::onApplyMaxRenderedAircraft);
        connect(ui->le_MaxDistance, &QLineEdit::editingFinished, this, &CSettingsSimulatorComponent::onApplyMaxRenderedDistance);
        connect(ui->le_MaxAircraft, &QLineEdit::returnPressed, this, &CSettingsSimulatorComponent::onApplyMaxRenderedAircraft);
        connect(ui->le_MaxDistance, &QLineEdit::returnPressed, this, &CSettingsSimulatorComponent::onApplyMaxRenderedDistance);

        // list all available simulators
        const CSimulatorPluginInfoList plugins = CSettingsSimulatorComponent::getAvailablePlugins();
        for (const auto &p : plugins)
        {
            const QString config = m_plugins->getPluginConfigId(p.getIdentifier());
            ui->pluginSelector_EnabledSimulators->addPlugin(p.getIdentifier(), p.getName(), !config.isEmpty(), false);
        }

        const int h = qRound(1.3 * plugins.size() * CGuiUtility::fontMetricsLazyDog43Chars().height());
        ui->pluginSelector_EnabledSimulators->setMinimumHeight(h);

        // config
        this->reloadPluginConfig(plugins);

        // init
        if (sGui && sGui->getIContextSimulator())
        {
            this->simulatorPluginChanged(sGui->getIContextSimulator()->getSimulatorPluginInfo());
            connect(sGui->getIContextSimulator(), &IContextSimulator::simulatorPluginChanged, this, &CSettingsSimulatorComponent::simulatorPluginChanged, Qt::QueuedConnection);
            connect(sGui->getIContextSimulator(), &IContextSimulator::simulatorSettingsChanged, this, &CSettingsSimulatorComponent::onReload, Qt::QueuedConnection);
        }
    }

    CSettingsSimulatorComponent::~CSettingsSimulatorComponent()
    {}

    void CSettingsSimulatorComponent::setGuiValues()
    {
        if (!sGui || !sGui->getIContextSimulator() || sGui->isShuttingDown()) { return; }
        const CInterpolationAndRenderingSetupGlobal setup = sGui->getIContextSimulator()->getInterpolationAndRenderingSetupGlobal();

        // time sync
        ui->cb_TimeSync->setEnabled(m_pluginLoaded);
        ui->le_TimeSyncOffset->setEnabled(m_pluginLoaded);
        ui->pb_ApplyTimeSync->setEnabled(m_pluginLoaded);

        // COM unit
        ui->pb_ApplyComSync->setEnabled(m_pluginLoaded);
        ui->cb_ComSync->setEnabled(m_pluginLoaded);

        // CG
        ui->comp_CGSourceSelector->setEnabled(m_pluginLoaded);

        // record GND
        ui->pb_ApplyRecordOwnAircraftGnd->setEnabled(m_pluginLoaded);
        ui->cb_RecordOwnGndPositions->setEnabled(m_pluginLoaded);
        ui->le_RecordOwnGndPositionsRadius->setEnabled(m_pluginLoaded);

        // led
        ui->led_RestrictedRendering->setOn(m_pluginLoaded ? setup.isRenderingRestricted() : false);
        ui->lbl_RestrictionText->setText(m_pluginLoaded ? setup.getRenderRestrictionText() : "");

        ui->le_MaxDistance->setEnabled(m_pluginLoaded);
        ui->le_MaxAircraft->setEnabled(m_pluginLoaded);
        ui->pb_ApplyMaxAircraft->setEnabled(m_pluginLoaded);
        ui->pb_ApplyMaxDistance->setEnabled(m_pluginLoaded);
        ui->pb_ClearRestrictedRendering->setEnabled((m_pluginLoaded));
        ui->pb_DisableRendering->setEnabled(m_pluginLoaded);
        ui->pb_Check->setEnabled(!m_pluginLoaded);
        ui->pb_ApplyCGSource->setEnabled(m_pluginLoaded);
        ui->pb_Reload->setEnabled(m_pluginLoaded);

        if (m_pluginLoaded)
        {
            const IContextSimulator *sim = sGui->getIContextSimulator();
            const bool timeSynced = sim->isTimeSynchronized();
            ui->cb_TimeSync->setChecked(timeSynced);
            const CTime timeOffset = sim->getTimeSynchronizationOffset();
            ui->le_TimeSyncOffset->setText(timeOffset.formattedHrsMin());

            // settings
            const CSimulatorSettings settings = sim->getSimulatorSettings();
            ui->cb_ComSync->setChecked(settings.isComIntegrated());

            // CG
            ui->comp_CGSourceSelector->setValue(settings);

            // record
            ui->le_RecordOwnGndPositionsRadius->setText(settings.getRecordedGndRadius().valueRoundedWithUnit(CLengthUnit::m(), 1, false, true));
            ui->cb_RecordOwnGndPositions->setChecked(settings.isRecordOwnAircraftGnd());

            // rendering
            const int maxAircraft = setup.getMaxRenderedAircraft();
            ui->le_MaxAircraft->setText(setup.isMaxAircraftRestricted() ? QString::number(maxAircraft) : "");

            const CLength maxDistance(setup.getMaxRenderedDistance());
            ui->le_MaxDistance->setText(setup.isMaxDistanceRestricted() ? QString::number(maxDistance.valueInteger(CLengthUnit::NM())) : "");
            ui->led_RenderingEnabled->setOn(setup.isRenderingEnabled());
        }
        else
        {
            ui->led_RenderingEnabled->setOn(false);
        }
    }

    CSimulatorPluginInfoList CSettingsSimulatorComponent::getAvailablePlugins()
    {
        if (!sGui || !sGui->getIContextSimulator()) { return {}; }
        return sGui->getIContextSimulator()->getAvailableSimulatorPlugins();
    }

    void CSettingsSimulatorComponent::pluginStateChanged(const QString &identifier, bool enabled)
    {
        Q_ASSERT(sGui && sGui->getIContextSimulator());

        const CSimulatorPluginInfoList simDrivers(getAvailablePlugins());
        const CSimulatorPluginInfo selected = simDrivers.findByIdentifier(identifier);
        if (selected.isUnspecified())
        {
            CLogMessage(this).error(u"Simulator plugin does not exist: '%1'") << identifier;
            return;
        }

        QStringList e = m_enabledSimulators.getThreadLocal(); // from setting
        if (enabled != e.contains(selected.getIdentifier()))
        {
            if (enabled)
            {
                e << selected.getIdentifier(); // add enabled plugin
            }
            else
            {
                e.removeAll(selected.getIdentifier());
            }
            const CStatusMessage msg = m_enabledSimulators.set(e); // change setting
            if (msg.isWarningOrAbove())
            {
                CLogMessage::preformatted(msg);
            }
        }

        // changing of GUI state will be done via received signal
    }

    void CSettingsSimulatorComponent::onApplyMaxRenderedAircraft()
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->getIContextSimulator()) { return; }

        // get initial aircraft to render
        CInterpolationAndRenderingSetupGlobal setup = sGui->getIContextSimulator()->getInterpolationAndRenderingSetupGlobal();
        const int noRequested = ui->le_MaxAircraft->text().isEmpty() ? setup.InfiniteAircraft() : ui->le_MaxAircraft->text().toInt();
        const int oldValue = setup.getMaxRenderedAircraft();
        if (oldValue == noRequested) { return; }

        // set value
        setup.setMaxRenderedAircraft(noRequested);
        sGui->getIContextSimulator()->setInterpolationAndRenderingSetupGlobal(setup);

        // re-read real value
        setup = sGui->getIContextSimulator()->getInterpolationAndRenderingSetupGlobal();
        const int noRendered = setup.getMaxRenderedAircraft();
        if (noRequested == noRendered)
        {
            CLogMessage(this).info(u"Max.rendered aircraft: %1") << noRendered;
        }
        else
        {
            CLogMessage(this).info(u"Max.rendered aircraft: %1, requested: %2") << noRendered << noRequested;
            ui->le_MaxAircraft->setText(QString::number(noRendered));
        }
        this->setGuiValues();
    }

    void CSettingsSimulatorComponent::onApplyMaxRenderedDistance()
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->getIContextSimulator()) { return; }

        // get initial aircraft to render
        CInterpolationAndRenderingSetupGlobal setup = sGui->getIContextSimulator()->getInterpolationAndRenderingSetupGlobal();
        CLength newDistance(0, nullptr);
        if (!ui->le_MaxDistance->text().isEmpty())
        {
            newDistance = CLength(ui->le_MaxDistance->text().toInt(), CLengthUnit::NM());
        }

        CLength currentDistance(setup.getMaxRenderedDistance());
        if (currentDistance == newDistance) { return; }

        CLogMessage(this).info(u"Max.distance requested: %1") << newDistance.valueRoundedWithUnit(2, true);
        setup.setMaxRenderedDistance(newDistance);
        sGui->getIContextSimulator()->setInterpolationAndRenderingSetupGlobal(setup);
        this->setGuiValues();
    }

    void CSettingsSimulatorComponent::onApplyDisableRendering()
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->getIContextSimulator()) { return; }

        CInterpolationAndRenderingSetupGlobal setup = sGui->getIContextSimulator()->getInterpolationAndRenderingSetupGlobal();
        setup.disableRendering();
        sGui->getIContextSimulator()->setInterpolationAndRenderingSetupGlobal(setup);
        this->setGuiValues();
    }

    void CSettingsSimulatorComponent::onApplyTimeSync()
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->getIContextSimulator()) { return; }

        const bool timeSync = ui->cb_TimeSync->isChecked();
        const QString os = ui->le_TimeSyncOffset->text();
        CTime ost(0, CTimeUnit::hrmin());
        if (!os.isEmpty())
        {
            ost.parseFromString(os);
        }
        if (ost.isNull())
        {
            CLogMessage().validationWarning(u"Invalid offset time");
        }
        else
        {
            sGui->getIContextSimulator()->setTimeSynchronization(timeSync, ost);
        }
    }

    CSimulatorSettings CSettingsSimulatorComponent::getSimulatorSettings(bool &ok)
    {
        ok = false;
        if (!sGui || sGui->isShuttingDown() || !sGui->getIContextSimulator()) { return {}; }
        const IContextSimulator *sim = sGui->getIContextSimulator();
        const CSimulatorInfo simulator = sim->getSimulatorPluginInfo().getSimulatorInfo();
        if (!simulator.isSingleSimulator()) { return {}; }
        ok = true;
        const CSimulatorSettings settings = sim->getSimulatorSettings();
        return settings;
    }

    void CSettingsSimulatorComponent::setSimulatorSettings(CSimulatorSettings &settings)
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->getIContextSimulator()) { return; }
        IContextSimulator *sim = sGui->getIContextSimulator();
        const CSimulatorInfo simulator = sim->getSimulatorPluginInfo().getSimulatorInfo();
        sim->setSimulatorSettings(settings, simulator);
    }

    void CSettingsSimulatorComponent::onApplyComSync()
    {
        bool ok = false;
        CSimulatorSettings settings = CSettingsSimulatorComponent::getSimulatorSettings(ok);
        if (!ok || !settings.setComIntegrated(ui->cb_ComSync->isChecked())) { return; }
        this->setSimulatorSettings(settings);
    }

    void CSettingsSimulatorComponent::onApplyCGSource()
    {
        bool ok = false;
        const CSimulatorSettings::CGSource source = ui->comp_CGSourceSelector->getValue();
        CSimulatorSettings settings = CSettingsSimulatorComponent::getSimulatorSettings(ok);
        if (!ok || !settings.setCGSource(source)) { return; }
        this->setSimulatorSettings(settings);
    }

    void CSettingsSimulatorComponent::onApplyRecordGnd()
    {
        bool ok = false;
        CSimulatorSettings settings = CSettingsSimulatorComponent::getSimulatorSettings(ok);
        if (!ok) { return; }

        // get value, automatically add default unit if unit is missing
        CLength radius = CLength::null();
        QString radiusString = ui->le_RecordOwnGndPositionsRadius->text().trimmed();
        if (!radiusString.isEmpty())
        {
            if (!CMeasurementUnit::endWithValidUnitSymbol<CLengthUnit>(radiusString)) { radiusString += "m"; }
            radius.parseFromString(radiusString);
        }
        ui->le_RecordOwnGndPositionsRadius->setText(radius.valueRoundedWithUnit(1));

        const bool c1 = settings.setRecordOwnAircraftGnd(ui->cb_RecordOwnGndPositions->isChecked());
        const bool c2 = settings.setRecordedGndRadius(radius);
        if (!c1 && !c2) { return; }
        this->setSimulatorSettings(settings);
    }

    void CSettingsSimulatorComponent::onReload()
    {
        this->setGuiValues();
    }

    void CSettingsSimulatorComponent::onEnabledSimulatorsChanged()
    {
        this->reloadPluginConfig(CSettingsSimulatorComponent::getAvailablePlugins());
    }

    void CSettingsSimulatorComponent::clearRestricedRendering()
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->getIContextSimulator()) { return; }

        CInterpolationAndRenderingSetupGlobal setup;
        setup.clearAllRenderingRestrictions();
        sGui->getIContextSimulator()->setInterpolationAndRenderingSetupGlobal(setup);
        this->setGuiValues();
    }

    void CSettingsSimulatorComponent::simulatorPluginChanged(const CSimulatorPluginInfo &info)
    {
        // I intentionally do not set the selected plugin combobox here
        // as this would cause undesired roundtrips

        // other GUI values
        if (!info.isUnspecified())
        {
            m_pluginLoaded = true;
            ui->lbl_PluginInfo->setText(info.getDescription());
        }
        else
        {
            m_pluginLoaded = false;
            ui->lbl_PluginInfo->setText("No plugin loaded");
        }
        this->setGuiValues();
    }

    void CSettingsSimulatorComponent::showPluginDetails(const QString &identifier)
    {
        const CSimulatorPluginInfoList simDrivers(this->getAvailablePlugins());
        const CSimulatorPluginInfo selected = simDrivers.findByIdentifier(identifier);

        QWidget *aw = qApp->activeWindow();
        Q_ASSERT_X(aw, Q_FUNC_INFO, "Missing active window");

        CPluginDetailsWindow *w = new CPluginDetailsWindow(aw);
        SWIFT_VERIFY_X(w, Q_FUNC_INFO, "Missing window");
        if (!w) { return; }

        w->setAttribute(Qt::WA_DeleteOnClose);
        w->setPluginIdentifier(selected.getIdentifier());
        w->setPluginName(selected.getName());
        w->setPluginDescription(selected.getDescription());

        w->show();
    }

    void CSettingsSimulatorComponent::showPluginConfig(const QString &identifier)
    {
        const CSimulatorPluginInfoList simDrivers(getAvailablePlugins());
        const CSimulatorPluginInfo selected = simDrivers.findByIdentifier(identifier);

        const QString configId = m_plugins->getPluginConfigId(selected.getIdentifier());
        IPluginConfig *config = m_plugins->getPluginById<IPluginConfig>(configId);
        SWIFT_VERIFY_X(config, Q_FUNC_INFO, "Missing config");
        if (!config) { return; }

        CPluginConfigWindow *window = config->createConfigWindow(qApp->activeWindow());
        SWIFT_VERIFY_X(window, Q_FUNC_INFO, "Missing window");
        if (!window) { return; }

        window->setAttribute(Qt::WA_DeleteOnClose);
        window->show();
    }

    void CSettingsSimulatorComponent::reloadPluginConfig(const CSimulatorPluginInfoList &plugins)
    {
        // list all available simulators
        const auto enabledSimulators = m_enabledSimulators.getThreadLocal();
        for (const auto &p : plugins)
        {
            ui->pluginSelector_EnabledSimulators->setEnabled(p.getIdentifier(), enabledSimulators.contains(p.getIdentifier()));
        }
    }

    void CSettingsSimulatorComponent::checkSimulatorPlugins()
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->getIContextSimulator()) { return; }
        if (sGui->getIContextSimulator()->isSimulatorAvailable()) { return; } // already available
        sGui->getIContextSimulator()->checkListeners();
    }
} // namespace
