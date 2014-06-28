#include "settingssimulatorcomponent.h"
#include "ui_settingssimulatorcomponent.h"

#include "blackcore/context_settings.h"
#include "blacksim/simulatorinfolist.h"
#include "blacksim/setsimulator.h"
#include "blackmisc/settingutilities.h"

#include <QComboBox>

using namespace BlackMisc;
using namespace BlackMisc::Settings;
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
        if (this->getIContextSimulator())
        {
            QStringList plugins = this->getIContextSimulator()->getAvailableSimulatorPlugins().toStringList(true);
            CSimulatorInfo currentDriver = this->getIContextSimulator()->getSimulatorInfo();
            this->ui->cb_SimulatorDriver->addItems(plugins);
            this->setCurrentPlugin(currentDriver);

            // disable / enable driver specific GUI parts
            bool fsxDriver =
                this->getIContextSimulator()->getAvailableSimulatorPlugins().supportsSimulator(CSimulatorInfo::FSX());
            this->ui->comp_SettingsSimulatorFsx->setVisible(fsxDriver);

            // only with set GUI values
            bool connected = this->connect(this->ui->cb_SimulatorDriver, SIGNAL(currentIndexChanged(int)), this, SLOT(pluginHasChanged(int)));
            Q_ASSERT(connected);
            Q_UNUSED(connected);
        }
    }

    void CSettingsSimulatorComponent::pluginHasChanged(int index)
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

    void CSettingsSimulatorComponent::setCurrentPlugin(const CSimulatorInfo &driver)
    {
        if (driver.isUnspecified()) return;
        const QString searchFor = driver.getShortName();
        for (int i = 0; i < this->ui->cb_SimulatorDriver->count(); ++i)
        {
            const QString t = ui->cb_SimulatorDriver->itemText(i);
            if (t.indexOf(searchFor, 0, Qt::CaseInsensitive) >= 0)
            {
                if (i == this->ui->cb_SimulatorDriver->currentIndex()) return;
                this->ui->cb_SimulatorDriver->setCurrentIndex(i);
                break;
            }
        }
    }

} // namespace
