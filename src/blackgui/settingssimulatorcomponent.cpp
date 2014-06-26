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
        bool connected = this->connect(this->ui->cb_SimulatorDriver, SIGNAL(currentIndexChanged(int)), this, SLOT(driverHasChanged(int)));
        Q_ASSERT(connected);
        Q_UNUSED(connected);
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
            QStringList drivers = this->getIContextSimulator()->getAvailableSimulatorPlugins().toStringList(true);
            this->ui->cb_SimulatorDriver->addItems(drivers);

            bool fsxDriver =
                this->getIContextSimulator()->getAvailableSimulatorPlugins().supportsSimulator(CSimulatorInfo::FSX());
            this->ui->comp_SettingsSimulatorFsx->setVisible(fsxDriver);
        }
    }

    void CSettingsSimulatorComponent::driverHasChanged(int index)
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

} // namespace
