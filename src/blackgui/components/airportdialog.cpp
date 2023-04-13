#include "airportdialog.h"
#include "ui_airportdialog.h"

using namespace BlackMisc::Aviation;

namespace BlackGui::Components
{
    CAirportDialog::CAirportDialog(QWidget *parent) : QDialog(parent),
                                                      ui(new Ui::CAirportDialog)
    {
        ui->setupUi(this);
        this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    }

    CAirportDialog::~CAirportDialog()
    {}

    void CAirportDialog::setAirport(const CAirport &airport)
    {
        ui->comp_AirportCompleter->setAirport(airport);
    }

    CAirport CAirportDialog::getAirport() const
    {
        return ui->comp_AirportCompleter->getAirport();
    }

    void CAirportDialog::clear()
    {
        ui->comp_AirportCompleter->clear();
    }
} // ns
