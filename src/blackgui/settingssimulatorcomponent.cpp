#include "settingssimulatorcomponent.h"
#include "ui_settingssimulatorcomponent.h"

#include "blackcore/context_settings.h"
#include "blacksim/simulatorinfolist.h"
#include "blacksim/setsimulator.h"
#include "blackmisc/settingutilities.h"

#include <QComboBox>

using namespace BlackMisc;
using namespace BlackMisc::Settings;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackSim;
using namespace BlackSim::Settings;
using namespace BlackCore;

namespace BlackGui
{
    CSettingsSimulatorComponent::CSettingsSimulatorComponent(QWidget *parent) :
        QFrame(parent), CRuntimeBasedComponent(nullptr, false),
        ui(new Ui::CSettingsSimulatorComponent)
    {
        ui->setupUi(this);
    }

    CSettingsSimulatorComponent::~CSettingsSimulatorComponent()
    {
        delete ui;
    }

    void CSettingsSimulatorComponent::runtimeHasBeenSet()
    {
        Q_ASSERT(this->getIContextSimulator());
        Q_ASSERT(this->getIContextSettings());
        if (this->getIContextSimulator())
        {
            QStringList plugins = this->getIContextSimulator()->getAvailableSimulatorPlugins().toStringList(true);
            CSimulatorInfo currentDriver = this->getIContextSimulator()->getSimulatorInfo();
            this->ui->cb_Plugins->addItems(plugins);
            this->setCurrentPlugin(currentDriver);

            // disable / enable driver specific GUI parts
            bool fsxDriver =
                this->getIContextSimulator()->getAvailableSimulatorPlugins().supportsSimulator(CSimulatorInfo::FSX());
            this->ui->comp_SettingsSimulatorFsx->setVisible(fsxDriver);

            // only with simulator context set GUI values
            bool connected = this->connect(this->ui->cb_Plugins, SIGNAL(currentIndexChanged(int)), this, SLOT(ps_pluginHasChanged(int)));
            Q_ASSERT(connected);
            Q_UNUSED(connected);
        }

        if (this->getIContextSettings())
        {
            connect(this->getIContextSettings(), &IContextSettings::changedSettings, this, &CSettingsSimulatorComponent::ps_settingsHaveChanged);
        }

        connect(this->ui->cb_TimeSync, &QCheckBox::released, this, &CSettingsSimulatorComponent::ps_guiValueHasChanged);
        connect(this->ui->le_TimeSyncOffset, &QLineEdit::returnPressed, this, &CSettingsSimulatorComponent::ps_guiValueHasChanged);
    }

    void CSettingsSimulatorComponent::setCurrentPlugin(const CSimulatorInfo &plugin)
    {
        if (plugin.isUnspecified()) return;
        const QString searchFor = plugin.getShortName();
        for (int i = 0; i < this->ui->cb_Plugins->count(); ++i)
        {
            const QString t = ui->cb_Plugins->itemText(i);
            if (t.indexOf(searchFor, 0, Qt::CaseInsensitive) >= 0)
            {
                if (i == this->ui->cb_Plugins->currentIndex()) return;
                this->ui->cb_Plugins->setCurrentIndex(i);
                break;
            }
        }
    }

    void CSettingsSimulatorComponent::ps_pluginHasChanged(int index)
    {
        Q_ASSERT(this->getIContextSimulator());
        Q_ASSERT(this->getIContextSettings());
        if (!this->getIContextSimulator() || !this->getIContextSettings()) return;

        CSimulatorInfoList simDrivers = this->getIContextSimulator()->getAvailableSimulatorPlugins();
        if (simDrivers.isEmpty())
        {
            this->sendStatusMessage(CStatusMessage::getErrorMessage("No drivers available", CStatusMessage::TypeSimulator));
            return;
        }
        if (simDrivers.size() <= index)
        {
            this->sendStatusMessage(CStatusMessage::getErrorMessage("Wrong driver index", CStatusMessage::TypeSimulator));
            return;
        }

        // update
        CSimulatorInfo currentDriver = simDrivers[index];
        const QString path = CSettingUtilities::appendPaths(IContextSettings::PathSimulatorSettings(), CSettingsSimulator::ValueSelectedDriver());
        this->sendStatusMessages(
            this->getIContextSettings()->value(path, CSettingUtilities::CmdUpdate(), currentDriver.toCVariant())
        );
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

    void CSettingsSimulatorComponent::ps_guiValueHasChanged()
    {
        Q_ASSERT(this->getIContextSettings());
        if (!this->getIContextSettings()) return;

        QObject *sender = QObject::sender();
        if (!sender) return;

        const QString ps = IContextSettings::PathSimulatorSettings();
        CStatusMessageList msgs;
        if (sender == this->ui->cb_TimeSync)
        {
            bool timeSync = this->ui->cb_TimeSync->isChecked();
            msgs = this->getIContextSettings()->value(CSettingUtilities::appendPaths(ps, CSettingsSimulator::ValueSyncTime()), CSettingUtilities::CmdUpdate(), QVariant(timeSync));
        }
        else if (sender == this->ui->le_TimeSyncOffset)
        {
            const QString os = this->ui->le_TimeSyncOffset->text();
            CTime ost(0, CTimeUnit::hrmin());
            if (!os.isEmpty())
            {
                ost.parseFromString(os);
            }
            if (ost.isNull())
            {
                msgs.push_back(CStatusMessage::getValidationError("Invalid offset time"));
            }
            else
            {
                msgs = this->getIContextSettings()->value(CSettingUtilities::appendPaths(ps, CSettingsSimulator::ValueSyncTimeOffset()), CSettingUtilities::CmdUpdate(), ost.toQVariant());
            }
        }
        if (!msgs.isEmpty())
        {
            this->getIContextApplication()->sendStatusMessages(msgs);
        }
    }

} // namespace
