#include "settingssimulatorcomponent.h"
#include "ui_settingssimulatorcomponent.h"

#include "blackcore/context_settings.h"
#include "blackcore/context_simulator.h"
#include "blackcore/context_network.h"
#include "blackmisc/simulation/simulatorinfolist.h"
#include "blackmisc/simulation/setsimulator.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/settingutilities.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/variant.h"

#include <QComboBox>

using namespace BlackMisc;
using namespace BlackMisc::Settings;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Simulation::Settings;
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
        }

        CSettingsSimulatorComponent::~CSettingsSimulatorComponent()
        { }

        void CSettingsSimulatorComponent::runtimeHasBeenSet()
        {
            Q_ASSERT(this->getIContextSimulator());
            Q_ASSERT(this->getIContextSettings());

            // set values
            this->setRestrictedValues();

            ui->cb_Plugins->addItem(tr("Auto"), CSimulatorPluginInfo().toQVariant());

            for (const auto &p: getIContextSimulator()->getAvailableSimulatorPlugins())
            {
                ui->cb_Plugins->addItem(p.toQString(), p.toQVariant());
            }

            this->setCurrentPlugin(getIContextSettings()->getSimulatorSettings().getSelectedPlugin());

            // disable / enable driver specific GUI parts
            bool hasFsxDriver =
                this->getIContextSimulator()->getAvailableSimulatorPlugins().supportsSimulator(QStringLiteral("fsx"));
            this->ui->comp_SettingsSimulatorFsx->setVisible(hasFsxDriver);

            // time sync
            bool timeSynced = this->getIContextSimulator()->isTimeSynchronized();
            this->ui->cb_TimeSync->setChecked(timeSynced);
            CTime timeOffset = this->getIContextSimulator()->getTimeSynchronizationOffset();
            this->ui->le_TimeSyncOffset->setText(timeOffset.formattedHrsMin());

            // max.aircraft
            this->ui->sb_MaxAircraft->setValue(getIContextSimulator()->getMaxRenderedAircraft());

            connect(this->ui->cb_Plugins, static_cast<void (QComboBox::*)(int)> (&QComboBox::currentIndexChanged), this, &CSettingsSimulatorComponent::ps_pluginHasChanged);
            connect(this->getIContextSettings(), &IContextSettings::changedSettings, this, &CSettingsSimulatorComponent::ps_settingsHaveChanged);
            connect(this->ui->pb_ApplyMaxAircraft, &QCheckBox::pressed, this, &CSettingsSimulatorComponent::ps_onApplyMaxRenderedAircraft);
            connect(this->ui->pb_ApplyTimeSync, &QCheckBox::pressed, this, &CSettingsSimulatorComponent::ps_onApplyTimeSync);
            connect(this->ui->pb_ApplyMaxDistance, &QCheckBox::pressed, this, &CSettingsSimulatorComponent::ps_onApplyMaxRenderedDistance);
            connect(this->ui->pb_ClearRestrictedRendering, &QCheckBox::pressed, this, &CSettingsSimulatorComponent::ps_clearRestricedRendering);
        }

        void CSettingsSimulatorComponent::setCurrentPlugin(const CSimulatorPluginInfo &plugin)
        {
            if (plugin.isUnspecified()) {
                ui->cb_Plugins->setCurrentIndex(0);
                return;
            }
            
            for (int i = 0; i < this->ui->cb_Plugins->count(); ++i)
            {
                QVariant data = this->ui->cb_Plugins->itemData(i);
                Q_ASSERT(data.canConvert<CSimulatorPluginInfo>());
                CSimulatorPluginInfo p = data.value<CSimulatorPluginInfo>();
                
                if (p.getName() == plugin.getName())
                {
                    if (i == this->ui->cb_Plugins->currentIndex())
                        return;
                    
                    this->ui->cb_Plugins->setCurrentIndex(i);
                    break;
                }
            }
        }

        void CSettingsSimulatorComponent::setRestrictedValues()
        {
            Q_ASSERT(getIContextSimulator());
            this->ui->led_RestrictedRendering->setOn(getIContextSimulator()->isRenderingRestricted());
            this->ui->lbl_RestrictionText->setText(getIContextSimulator()->getRenderRestrictionText());

            int distanceBoundaryNM = getIContextSimulator()->getRenderedDistanceBoundary().valueInteger(CLengthUnit::NM());
            this->ui->sb_MaxDistance->setMaximum(distanceBoundaryNM);
            this->ui->sb_MaxAircraft->setValue(getIContextSimulator()->getMaxRenderedAircraft());

            int distanceNM = getIContextSimulator()->getMaxRenderedDistance().valueInteger(CLengthUnit::NM());
            this->ui->sb_MaxDistance->setValue(distanceNM);
        }

        void CSettingsSimulatorComponent::ps_pluginHasChanged(int index)
        {
            Q_ASSERT(this->getIContextSimulator());
            Q_ASSERT(this->getIContextSettings());
            if (!this->getIContextSimulator() || !this->getIContextSettings()) return;

            CSimulatorPluginInfoList simDrivers = this->getIContextSimulator()->getAvailableSimulatorPlugins();
            simDrivers.insert(simDrivers.begin(), CSimulatorPluginInfo());
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
            CSimulatorPluginInfo currentDriver = simDrivers[index];
            const QString path = CSettingUtilities::appendPaths(IContextSettings::PathSimulatorSettings(), CSettingsSimulator::ValueSelectedDriver());
            this->getIContextSettings()->value(path, CSettingUtilities::CmdUpdate(), currentDriver.toCVariant());
        }

        void CSettingsSimulatorComponent::ps_settingsHaveChanged(uint settingsType)
        {
            Q_ASSERT(this->getIContextSettings());
            IContextSettings::SettingsType type = static_cast<IContextSettings::SettingsType>(settingsType);
            if (type != IContextSettings::SettingsSimulator || !this->getIContextSettings()) return;
            CSettingsSimulator simSettings = this->getIContextSettings()->getSimulatorSettings();

            this->setCurrentPlugin(simSettings.getSelectedPlugin());
            this->ui->le_TimeSyncOffset->setText(simSettings.getSyncTimeOffset().formattedHrsMin());
            this->ui->cb_TimeSync->setChecked(simSettings.isTimeSyncEnabled());
        }

        void CSettingsSimulatorComponent::ps_onApplyMaxRenderedAircraft()
        {
            Q_ASSERT(getIContextSimulator());

            // get initial aircraft to render
            int noRequested = this->ui->sb_MaxAircraft->value();
            this->getIContextSimulator()->setMaxRenderedAircraft(noRequested);

            // real value
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
        }

        void CSettingsSimulatorComponent::ps_onApplyMaxRenderedDistance()
        {
            Q_ASSERT(getIContextSimulator());

            // get initial aircraft to render
            int maxDistanceNM = this->ui->sb_MaxDistance->value();
            CLength distance(maxDistanceNM, CLengthUnit::NM());
            this->getIContextSimulator()->setMaxRenderedDistance(distance);

            CLogMessage(this).info("Max.distance requested: %1") << distance.valueRoundedWithUnit(2, true);
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

        void CSettingsSimulatorComponent::ps_onRenderingRestricted(bool restricted)
        {
            Q_UNUSED(restricted);
            setRestrictedValues();
        }

        void CSettingsSimulatorComponent::ps_clearRestricedRendering()
        {
            Q_ASSERT(getIContextSimulator());
            this->getIContextSimulator()->deleteAllRenderingRestrictions();
        }
    }

} // namespace
