#include "countryfilterbar.h"
#include "ui_countryfilterbar.h"

using namespace BlackGui::Models;
using namespace BlackMisc;
using namespace BlackMisc::Network;

namespace BlackGui
{
    namespace Filters
    {
        CCountryFilterBar::CCountryFilterBar(QWidget *parent) :
            CFilterWidget(parent),
            ui(new Ui::CCountryFilterBar)
        {
            ui->setupUi(this);
            this->setButtonsAndCount(this->ui->filter_Buttons);
            connect(ui->le_IsoCode, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);
            connect(ui->le_Name, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);
        }

        CCountryFilterBar::~CCountryFilterBar()
        { }

        std::unique_ptr<BlackGui::Models::IModelFilter<CCountryList> > CCountryFilterBar::createModelFilter() const
        {
            return std::unique_ptr<CCountryFilter>(
                       new CCountryFilter(
                           ui->le_IsoCode->text(),
                           ui->le_Name->text()
                       ));
        }

        void CCountryFilterBar::setProvider(IWebDataServicesProvider *webDataReaderProvider)
        {
            Q_UNUSED(webDataReaderProvider);
        }

        void CCountryFilterBar::onRowCountChanged(int count, bool withFilter)
        {
            this->ui->filter_Buttons->onRowCountChanged(count, withFilter);
        }

        void CCountryFilterBar::clearForm()
        {
            ui->le_IsoCode->clear();
            ui->le_Name->clear();
        }
    }
}
