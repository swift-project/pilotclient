/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

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
#include "blackmisc/simulation/simulatorplugininfo.h"
#include "blackmisc/simulation/simulatorplugininfolist.h"
#include "blackmisc/iterator.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/time.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/verify.h"
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

using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;
using namespace BlackCore;
using namespace BlackCore::Context;

namespace BlackGui
{
    namespace Components
    {
        CSettingsSimulatorComponent::CSettingsSimulatorComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CSettingsSimulatorComponent),
            m_plugins(new CPluginManagerSimulator(this))
        {
            Q_ASSERT_X(sGui, Q_FUNC_INFO, "Missing sGui");
            Q_ASSERT_X(sGui->getIContextSimulator(), Q_FUNC_INFO, "Missing context");

            m_plugins->collectPlugins();
            ui->setupUi(this);
            CLedWidget::LedShape shape = CLedWidget::Circle;
            ui->led_RestrictedRendering->setValues(CLedWidget::Yellow, CLedWidget::Black, shape, "Limited", "Unlimited", 14);
            ui->led_RenderingEnabled->setValues(CLedWidget::Yellow, CLedWidget::Black, shape, "Rendering enabled", "No aircraft will be rendered", 14);

            ui->le_MaxAircraft->setValidator(new QIntValidator(ui->le_MaxAircraft));
            ui->le_MaxDistance->setValidator(new QIntValidator(ui->le_MaxDistance));

            // connects
            connect(sGui->getIContextSimulator(), &IContextSimulator::simulatorPluginChanged, this, &CSettingsSimulatorComponent::simulatorPluginChanged);
            connect(ui->pluginSelector_EnabledSimulators, &CPluginSelector::pluginStateChanged, this, &CSettingsSimulatorComponent::pluginStateChanged);
            connect(ui->pluginSelector_EnabledSimulators, &CPluginSelector::pluginDetailsRequested, this, &CSettingsSimulatorComponent::showPluginDetails);
            connect(ui->pluginSelector_EnabledSimulators, &CPluginSelector::pluginConfigRequested, this, &CSettingsSimulatorComponent::showPluginConfig);
            connect(ui->pb_ApplyMaxAircraft, &QCheckBox::pressed, this, &CSettingsSimulatorComponent::onApplyMaxRenderedAircraft);
            connect(ui->pb_ApplyTimeSync, &QCheckBox::pressed, this, &CSettingsSimulatorComponent::onApplyTimeSync);
            connect(ui->pb_ApplyMaxDistance, &QCheckBox::pressed, this, &CSettingsSimulatorComponent::onApplyMaxRenderedDistance);
            connect(ui->pb_ClearRestrictedRendering, &QCheckBox::pressed, this, &CSettingsSimulatorComponent::clearRestricedRendering);
            connect(ui->pb_DisableRendering, &QCheckBox::pressed, this, &CSettingsSimulatorComponent::onApplyDisableRendering);
            connect(ui->le_MaxAircraft, &QLineEdit::editingFinished, this, &CSettingsSimulatorComponent::onApplyMaxRenderedAircraft);
            connect(ui->le_MaxDistance, &QLineEdit::editingFinished, this, &CSettingsSimulatorComponent::onApplyMaxRenderedDistance);
            connect(ui->le_MaxAircraft, &QLineEdit::returnPressed, this, &CSettingsSimulatorComponent::onApplyMaxRenderedAircraft);
            connect(ui->le_MaxDistance, &QLineEdit::returnPressed, this, &CSettingsSimulatorComponent::onApplyMaxRenderedDistance);

            // list all available simulators
            for (const auto &p : getAvailablePlugins())
            {
                const QString config = m_plugins->getPluginConfigId(p.getIdentifier());
                ui->pluginSelector_EnabledSimulators->addPlugin(p.getIdentifier(), p.getName(), !config.isEmpty(), false);
            }

            // config
            reloadPluginConfig();

            // init
            simulatorPluginChanged(sGui->getIContextSimulator()->getSimulatorPluginInfo());
        }

        CSettingsSimulatorComponent::~CSettingsSimulatorComponent()
        { }

        void CSettingsSimulatorComponent::setGuiValues()
        {
            const CInterpolationAndRenderingSetup setup = sGui->getIContextSimulator()->getInterpolationAndRenderingSetup();

            // time sync
            ui->cb_TimeSync->setEnabled(m_pluginLoaded);
            ui->le_TimeSyncOffset->setEnabled(m_pluginLoaded);
            ui->pb_ApplyTimeSync->setEnabled(m_pluginLoaded);

            // led
            ui->led_RestrictedRendering->setOn(m_pluginLoaded ? setup.isRenderingRestricted() : false);
            ui->lbl_RestrictionText->setText(m_pluginLoaded ? setup.getRenderRestrictionText() : "");

            ui->le_MaxDistance->setEnabled(m_pluginLoaded);
            ui->le_MaxAircraft->setEnabled(m_pluginLoaded);
            ui->pb_ApplyMaxAircraft->setEnabled(m_pluginLoaded);
            ui->pb_ApplyMaxDistance->setEnabled(m_pluginLoaded);
            ui->pb_ClearRestrictedRendering->setEnabled((m_pluginLoaded));
            ui->pb_DisableRendering->setEnabled(m_pluginLoaded);

            if (m_pluginLoaded)
            {
                const bool timeSynced = sGui->getIContextSimulator()->isTimeSynchronized();
                ui->cb_TimeSync->setChecked(timeSynced);
                const CTime timeOffset = sGui->getIContextSimulator()->getTimeSynchronizationOffset();
                ui->le_TimeSyncOffset->setText(timeOffset.formattedHrsMin());

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

        CSimulatorPluginInfoList CSettingsSimulatorComponent::getAvailablePlugins() const
        {
            return sGui->getIContextSimulator()->getAvailableSimulatorPlugins();
        }

        void CSettingsSimulatorComponent::pluginStateChanged(const QString &identifier, bool enabled)
        {
            Q_ASSERT(sGui && sGui->getIContextSimulator());

            const CSimulatorPluginInfoList simDrivers(getAvailablePlugins());
            const CSimulatorPluginInfo selected = simDrivers.findByIdentifier(identifier);
            if (selected.isUnspecified())
            {
                CLogMessage(this).error("Simulator plugin does not exist: '%1'") << identifier;
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
            // get initial aircraft to render
            CInterpolationAndRenderingSetup setup = sGui->getIContextSimulator()->getInterpolationAndRenderingSetup();
            const int noRequested = ui->le_MaxAircraft->text().isEmpty() ? setup.InfiniteAircraft() : ui->le_MaxAircraft->text().toInt();
            const int oldValue = setup.getMaxRenderedAircraft();
            if (oldValue == noRequested) { return; }

            // set value
            setup.setMaxRenderedAircraft(noRequested);
            sGui->getIContextSimulator()->setInterpolationAndRenderingSetup(setup);

            // re-read real value
            setup = sGui->getIContextSimulator()->getInterpolationAndRenderingSetup();
            const int noRendered = setup.getMaxRenderedAircraft();
            if (noRequested == noRendered)
            {
                CLogMessage(this).info("Max.rendered aircraft: %1") << noRendered;
            }
            else
            {
                CLogMessage(this).info("Max.rendered aircraft: %1, requested: %2") << noRendered << noRequested;
                ui->le_MaxAircraft->setText(QString::number(noRendered));
            }
            this->setGuiValues();
        }

        void CSettingsSimulatorComponent::onApplyMaxRenderedDistance()
        {
            // get initial aircraft to render
            CInterpolationAndRenderingSetup setup = sGui->getIContextSimulator()->getInterpolationAndRenderingSetup();
            CLength newDistance(0, nullptr);
            if (!ui->le_MaxDistance->text().isEmpty())
            {
                newDistance = CLength(ui->le_MaxDistance->text().toInt(), CLengthUnit::NM());
            }

            CLength currentDistance(setup.getMaxRenderedDistance());
            if (currentDistance == newDistance)
            {
                return;
            }
            else
            {
                CLogMessage(this).info("Max.distance requested: %1") << newDistance.valueRoundedWithUnit(2, true);
                setup.setMaxRenderedDistance(newDistance);
                sGui->getIContextSimulator()->setInterpolationAndRenderingSetup(setup);
                this->setGuiValues();
            }
        }

        void CSettingsSimulatorComponent::onApplyDisableRendering()
        {
            CInterpolationAndRenderingSetup setup = sGui->getIContextSimulator()->getInterpolationAndRenderingSetup();
            setup.disableRendering();
            sGui->getIContextSimulator()->setInterpolationAndRenderingSetup(setup);
            this->setGuiValues();
        }

        void CSettingsSimulatorComponent::onApplyTimeSync()
        {
            const bool timeSync = ui->cb_TimeSync->isChecked();
            const QString os = ui->le_TimeSyncOffset->text();
            CTime ost(0, CTimeUnit::hrmin());
            if (!os.isEmpty())
            {
                ost.parseFromString(os);
            }
            if (ost.isNull())
            {
                CLogMessage().validationWarning("Invalid offset time");
            }
            else
            {
                sGui->getIContextSimulator()->setTimeSynchronization(timeSync, ost);
            }
        }

        void CSettingsSimulatorComponent::clearRestricedRendering()
        {
            CInterpolationAndRenderingSetup setup;
            setup.clearAllRenderingRestrictions();
            sGui->getIContextSimulator()->setInterpolationAndRenderingSetup(setup);
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
            const CSimulatorPluginInfoList simDrivers(getAvailablePlugins());
            const CSimulatorPluginInfo selected = simDrivers.findByIdentifier(identifier);

            QWidget *aw = qApp->activeWindow();
            Q_ASSERT_X(aw, Q_FUNC_INFO, "Missing active window");

            CPluginDetailsWindow *w = new CPluginDetailsWindow(aw);
            BLACK_VERIFY_X(w, Q_FUNC_INFO, "Missing window");
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
            BLACK_VERIFY_X(config, Q_FUNC_INFO, "Missing config");
            if (!config) { return; }

            CPluginConfigWindow *window = config->createConfigWindow(qApp->activeWindow());
            BLACK_VERIFY_X(window, Q_FUNC_INFO, "Missing window");
            if (!window) { return; }

            window->setAttribute(Qt::WA_DeleteOnClose);
            window->show();
        }

        void CSettingsSimulatorComponent::reloadPluginConfig()
        {
            // list all available simulators
            const auto enabledSimulators = m_enabledSimulators.getThreadLocal();
            for (const auto &p : getAvailablePlugins())
            {
                ui->pluginSelector_EnabledSimulators->setEnabled(p.getIdentifier(), enabledSimulators.contains(p.getIdentifier()));
            }
        }
    }

} // namespace
