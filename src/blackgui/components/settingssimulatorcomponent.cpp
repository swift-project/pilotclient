#include "settingssimulatorcomponent.h"
#include "ui_settingssimulatorcomponent.h"

#include "blackcore/context_simulator.h"
#include "blackcore/context_network.h"
#include "blackgui/pluginconfig.h"
#include "blackgui/pluginconfigwindow.h"
#include "blackgui/plugindetailswindow.h"
#include "blackmisc/simulation/simulatorplugininfolist.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/variant.h"
#include <QComboBox>

using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;
using namespace BlackCore;

namespace BlackGui
{
    namespace Components
    {
        CSettingsSimulatorComponent::CSettingsSimulatorComponent(QWidget *parent) :
            QFrame(parent),
            CEnableForRuntime(nullptr, false),
            ui(new Ui::CSettingsSimulatorComponent),
            m_plugins(new CPluginManagerSimulator(this))
        {
            m_plugins->collectPlugins();
            ui->setupUi(this);
            CLedWidget::LedShape shape = CLedWidget::Circle;
            this->ui->led_RestrictedRendering->setValues(CLedWidget::Yellow, CLedWidget::Black, shape, "Limited", "Unlimited", 14);
            this->ui->led_RenderingEnabled->setValues(CLedWidget::Yellow, CLedWidget::Black, shape, "Rendering enabled", "No aircraft will be rendered", 14);
        }

        CSettingsSimulatorComponent::~CSettingsSimulatorComponent()
        { }

        void CSettingsSimulatorComponent::runtimeHasBeenSet()
        {
            Q_ASSERT_X(this->getIContextSimulator(), Q_FUNC_INFO, "missing simulator");

            // set values
            for (const auto &p : getAvailablePlugins())
            {
                QString config = m_plugins->getPluginConfigId(p.getIdentifier());
                ui->ps_EnabledSimulators->addPlugin(p.getIdentifier(), p.getName(), !config.isEmpty(), false);
            }

            // connects
            connect(this->getIContextSimulator(), &IContextSimulator::simulatorPluginChanged, this, &CSettingsSimulatorComponent::ps_simulatorPluginChanged);
            connect(this->ui->ps_EnabledSimulators, &CPluginSelector::pluginStateChanged, this, &CSettingsSimulatorComponent::ps_pluginStateChanged);
            connect(this->ui->ps_EnabledSimulators, &CPluginSelector::pluginDetailsRequested, this, &CSettingsSimulatorComponent::ps_showPluginDetails);
            connect(this->ui->ps_EnabledSimulators, &CPluginSelector::pluginConfigRequested, this, &CSettingsSimulatorComponent::ps_showPluginConfig);
            connect(this->ui->pb_ApplyMaxAircraft, &QCheckBox::pressed, this, &CSettingsSimulatorComponent::ps_onApplyMaxRenderedAircraft);
            connect(this->ui->pb_ApplyTimeSync, &QCheckBox::pressed, this, &CSettingsSimulatorComponent::ps_onApplyTimeSync);
            connect(this->ui->pb_ApplyMaxDistance, &QCheckBox::pressed, this, &CSettingsSimulatorComponent::ps_onApplyMaxRenderedDistance);
            connect(this->ui->pb_ClearRestrictedRendering, &QCheckBox::pressed, this, &CSettingsSimulatorComponent::ps_clearRestricedRendering);
            connect(this->ui->pb_DisableRendering, &QCheckBox::pressed, this, &CSettingsSimulatorComponent::ps_onApplyDisableRendering);
            connect(this->ui->sb_MaxAircraft, &QSpinBox::editingFinished, this, &CSettingsSimulatorComponent::ps_onApplyMaxRenderedAircraft);
            connect(this->ui->sb_MaxDistance, &QSpinBox::editingFinished, this, &CSettingsSimulatorComponent::ps_onApplyMaxRenderedDistance);

            // values
            this->ps_simulatorPluginChanged(getIContextSimulator()->getSimulatorPluginInfo());
        }

        void CSettingsSimulatorComponent::setGuiValues()
        {
            Q_ASSERT(getIContextSimulator());

            // time sync
            this->ui->cb_TimeSync->setEnabled(m_pluginLoaded);
            this->ui->le_TimeSyncOffset->setEnabled(m_pluginLoaded);
            this->ui->pb_ApplyTimeSync->setEnabled(m_pluginLoaded);

            // led
            this->ui->led_RestrictedRendering->setOn(m_pluginLoaded ? getIContextSimulator()->isRenderingRestricted() : false);
            this->ui->lbl_RestrictionText->setText(m_pluginLoaded ? getIContextSimulator()->getRenderRestrictionText() : "");

            this->ui->sb_MaxDistance->setEnabled(m_pluginLoaded);
            this->ui->sb_MaxAircraft->setEnabled(m_pluginLoaded);
            this->ui->pb_ApplyMaxAircraft->setEnabled(m_pluginLoaded);
            this->ui->pb_ApplyMaxDistance->setEnabled(m_pluginLoaded);
            this->ui->pb_ClearRestrictedRendering->setEnabled((m_pluginLoaded));
            this->ui->pb_DisableRendering->setEnabled(m_pluginLoaded);

            if (m_pluginLoaded)
            {
                bool timeSynced = this->getIContextSimulator()->isTimeSynchronized();
                this->ui->cb_TimeSync->setChecked(timeSynced);
                CTime timeOffset = this->getIContextSimulator()->getTimeSynchronizationOffset();
                this->ui->le_TimeSyncOffset->setText(timeOffset.formattedHrsMin());

                int maxAircraft = getIContextSimulator()->getMaxRenderedAircraft();
                this->ui->sb_MaxAircraft->setValue(maxAircraft);

                CLength distanceBoundary(getIContextSimulator()->getRenderedDistanceBoundary());
                int distanceBoundaryNM = distanceBoundary.valueInteger(CLengthUnit::NM());
                CLength maxDistance(getIContextSimulator()->getMaxRenderedDistance());
                int distanceNM = maxDistance.isNull() ? distanceBoundaryNM : maxDistance.valueInteger(CLengthUnit::NM());
                this->ui->sb_MaxDistance->setMaximum(distanceBoundaryNM);
                this->ui->sb_MaxDistance->setValue(distanceNM);
                this->ui->led_RenderingEnabled->setOn(getIContextSimulator()->isRenderingEnabled());
            }
            else
            {
                this->ui->led_RenderingEnabled->setOn(false);
            }
        }

        CSimulatorPluginInfoList CSettingsSimulatorComponent::getAvailablePlugins() const
        {
            return getIContextSimulator()->getAvailableSimulatorPlugins();
        }

        void CSettingsSimulatorComponent::ps_pluginStateChanged(const QString &identifier, bool enabled)
        {
            Q_ASSERT(getIContextSimulator());

            CSimulatorPluginInfoList simDrivers(getAvailablePlugins());
            auto selected = std::find_if(simDrivers.begin(), simDrivers.end(),
                                         [&identifier](const CSimulatorPluginInfo &info)
                {
                    return info.getIdentifier() == identifier;
                });

            if (selected->isUnspecified())
            {
                CLogMessage(this).error("Simulator plugin does not exist: %1") << identifier;
                return;
            }

            if (enabled)
            {
                getIContextSimulator()->startSimulatorPlugin(*selected);
                CLogMessage(this).info("Started listening for %1") << selected->getSimulator();
            }
            else
            {
                getIContextSimulator()->stopSimulatorPlugin(*selected);
                CLogMessage(this).info("Stopped listening for %1") << selected->getSimulator();
            }

            // changing of GUI state will be done via received signal
        }

        void CSettingsSimulatorComponent::ps_onApplyMaxRenderedAircraft()
        {
            Q_ASSERT(getIContextSimulator());

            // get initial aircraft to render
            int noRequested = this->ui->sb_MaxAircraft->value();
            int oldValue = this->getIContextSimulator()->getMaxRenderedAircraft();
            if (oldValue == noRequested) { return; }

            // set value
            this->getIContextSimulator()->setMaxRenderedAircraft(noRequested);

            // re-read real value
            int noRendered = this->getIContextSimulator()->getMaxRenderedAircraft();
            if (noRequested == noRendered)
            {
                CLogMessage(this).info("Max.rendered aircraft: %1") << noRendered;
            }
            else
            {
                CLogMessage(this).info("Max.rendered aircraft: %1, requested: %2") << noRendered << noRequested;
                this->ui->sb_MaxAircraft->setValue(noRendered);
            }
            this->setGuiValues();
        }

        void CSettingsSimulatorComponent::ps_onApplyMaxRenderedDistance()
        {
            Q_ASSERT_X(getIContextSimulator(), Q_FUNC_INFO, "missing context");

            // get initial aircraft to render
            int maxDistanceNM = this->ui->sb_MaxDistance->value();
            CLength currentDistance(this->getIContextSimulator()->getMaxRenderedDistance());
            if (maxDistanceNM == currentDistance.valueInteger(CLengthUnit::NM()))
            {
                return;
            }
            else
            {
                CLength distance(maxDistanceNM, CLengthUnit::NM());
                CLogMessage(this).info("Max.distance requested: %1") << distance.valueRoundedWithUnit(2, true);
                this->getIContextSimulator()->setMaxRenderedDistance(distance);
                this->setGuiValues();
            }
        }

        void CSettingsSimulatorComponent::ps_onApplyDisableRendering()
        {
            Q_ASSERT(getIContextSimulator());
            this->getIContextSimulator()->setMaxRenderedAircraft(0);
            this->setGuiValues();
        }

        void CSettingsSimulatorComponent::ps_onApplyTimeSync()
        {
            bool timeSync = this->ui->cb_TimeSync->isChecked();
            const QString os = this->ui->le_TimeSyncOffset->text();
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
                getIContextSimulator()->setTimeSynchronization(timeSync, ost);
            }
        }

        void CSettingsSimulatorComponent::ps_clearRestricedRendering()
        {
            Q_ASSERT(getIContextSimulator());
            this->getIContextSimulator()->deleteAllRenderingRestrictions();
            this->setGuiValues();
        }

        void CSettingsSimulatorComponent::ps_simulatorPluginChanged(const CSimulatorPluginInfo &info)
        {
            // disable / enable driver specific GUI parts
            bool hasFsxDriver = this->getIContextSimulator()->getAvailableSimulatorPlugins().supportsSimulator(QStringLiteral("fsx"));
            this->ui->comp_SettingsSimulatorFsx->setVisible(hasFsxDriver);

            // I intentionally to not set the selected plugin combobox here
            // as this would cause undesired rountrips

            // other GUI values
            if (!info.isUnspecified())
            {
                m_pluginLoaded = true;
                this->ui->lbl_PluginInfo->setText(info.getDescription());
            }
            else
            {
                m_pluginLoaded = false;
                this->ui->lbl_PluginInfo->setText("No plugin loaded");
            }
            this->setGuiValues();
        }

        void CSettingsSimulatorComponent::ps_showPluginDetails(const QString &identifier)
        {
            CSimulatorPluginInfoList simDrivers(getAvailablePlugins());
            auto selected = std::find_if(simDrivers.begin(), simDrivers.end(),
                                         [&identifier](const CSimulatorPluginInfo &info)
                {
                    return info.getIdentifier() == identifier;
                });

            QWidget* aw = qApp->activeWindow();

            CPluginDetailsWindow *w = new CPluginDetailsWindow(aw);
            w->setAttribute(Qt::WA_DeleteOnClose);
            w->setPluginIdentifier(selected->getIdentifier());
            w->setPluginName(selected->getName());
            w->setPluginDescription(selected->getDescription());

            w->show();
        }

        void CSettingsSimulatorComponent::ps_showPluginConfig(const QString &identifier)
        {
            CSimulatorPluginInfoList simDrivers(getAvailablePlugins());
            auto selected = std::find_if(simDrivers.begin(), simDrivers.end(),
                                         [&identifier](const CSimulatorPluginInfo &info)
                {
                    return info.getIdentifier() == identifier;
                });

            QString configId = m_plugins->getPluginConfigId(selected->getIdentifier());
            IPluginConfig *config = m_plugins->getPluginById<IPluginConfig>(configId);
            CPluginConfigWindow *window = config->createConfigWindow(qApp->activeWindow());
            window->setAttribute(Qt::WA_DeleteOnClose);
            window->show();
        }
    }

} // namespace
