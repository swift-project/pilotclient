#include "keyboardkeyview.h"
#include <QHeaderView>

using namespace BlackMisc;

namespace BlackGui
{
    CKeyboardKeyView::CKeyboardKeyView(QWidget *parent) : CViewBase(parent)
    {
        this->m_model = new CKeyboardKeyListModel(this);
        this->setModel(this->m_model); // via QTableView
        this->m_model->setSortColumnByPropertyIndex(BlackMisc::Hardware::CKeyboardKey::IndexFunctionAsString);
        if (this->m_model->hasValidSortColumn())
            this->horizontalHeader()->setSortIndicator(
                this->m_model->getSortColumn(),
                this->m_model->getSortOrder());
        this->setItemDelegate(new BlackGui::CKeyboardKeyItemDelegate(this));
    }
}
