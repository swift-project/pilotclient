#include "settingssimulatorcomponent.h"
#include "ui_settingssimulatorcomponent.h"

#include "blackcore/context_simulator.h"
#include "blackcore/context_network.h"
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
            ui(new Ui::CSettingsSimulatorComponent)
        {
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
            for (const auto &p : getAvailablePlugins(true))
            {
                ui->cb_Plugins->addItem(p.toQString(), QVariant::fromValue(p));
            }

            // connects
            connect(this->ui->cb_Plugins, static_cast<void (QComboBox::*)(int)> (&QComboBox::currentIndexChanged), this, &CSettingsSimulatorComponent::ps_pluginHasBeenSelectedInComboBox);
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

        void CSettingsSimulatorComponent::setCurrentPluginInComboBox(const CSimulatorPluginInfo &plugin)
        {
            if (plugin.isUnspecified())
            {
                ui->cb_Plugins->setCurrentIndex(0);
                return;
            }

            for (int i = 0; i < this->ui->cb_Plugins->count(); ++i)
            {
                QVariant data = this->ui->cb_Plugins->itemData(i);
                Q_ASSERT(data.canConvert<CSimulatorPluginInfo>());
                CSimulatorPluginInfo p = data.value<CSimulatorPluginInfo>();

                if (p.getIdentifier() == plugin.getIdentifier())
                {
                    if (i == this->ui->cb_Plugins->currentIndex()) { return; }
                    this->ui->cb_Plugins->setCurrentIndex(i);
                    break;
                }
            }
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

        CSimulatorPluginInfoList CSettingsSimulatorComponent::getAvailablePlugins(bool plusAuto) const
        {
            CSimulatorPluginInfoList l(getIContextSimulator()->getAvailableSimulatorPlugins());
            if (plusAuto) { l.push_front(CSimulatorPluginInfo::autoPlugin()); }
            return l;
        }

        void CSettingsSimulatorComponent::ps_pluginHasBeenSelectedInComboBox(int index)
        {
            Q_ASSERT(this->getIContextSimulator());
            if (!this->getIContextSimulator()) { return; }

            CSimulatorPluginInfoList simDrivers(getAvailablePlugins(true));
            if (simDrivers.isEmpty())
            {
                CLogMessage(this).error("No drivers available");
                return;
            }
            if (simDrivers.size() <= index)
            {
                CLogMessage(this).error("Wrong driver index");
                return;
            }

            // update
            CSimulatorPluginInfo selectedPlugin = simDrivers[index];
            this->getIContextSimulator()->startSimulatorPlugin(selectedPlugin);

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

            // I intentionally to not set the selected plugin combobox here
            // as this would cause undesired rountrips

            // other GUI values
            if (!info.isUnspecified())
            {
                m_pluginLoaded = true;
                this->ui->comp_SettingsSimulatorFsx->setVisible(hasFsxDriver);
                this->ui->lbl_PluginInfo->setText(info.getDescription());
            }
            else
            {
                m_pluginLoaded = false;
                this->ui->lbl_PluginInfo->setText("No plugin loaded");
            }
            this->setGuiValues();
        }
    }

} // namespace
