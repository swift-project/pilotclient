#include "simulatorfsxconfigwindow.h"
#include "ui_simulatorfsxconfigwindow.h"


using namespace BlackGui;

namespace BlackSimPlugin
{
    namespace Fsx
    {
        CSimulatorFsxConfigWindow::CSimulatorFsxConfigWindow(QWidget *parent) :
            CPluginConfigWindow(parent),
            ui(new Ui::CSimulatorFsxConfigWindow)
        {
            ui->setupUi(this);
            connect(ui->bb_OkCancel, &QDialogButtonBox::rejected, this, &QWidget::close);
        }

        CSimulatorFsxConfigWindow::~CSimulatorFsxConfigWindow()
        {
            // void
        }
    }
}
