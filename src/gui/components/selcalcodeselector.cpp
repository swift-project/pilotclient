// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/components/selcalcodeselector.h"

#include <QComboBox>
#include <QStringList>
#include <QtGlobal>

#include "ui_selcalcodeselector.h"

#include "gui/ticklabel.h"
#include "misc/aviation/selcal.h"

using namespace swift::misc;
using namespace swift::misc::aviation;

namespace swift::gui::components
{
    CSelcalCodeSelector::CSelcalCodeSelector(QWidget *parent) : QFrame(parent), ui(new Ui::CSelcalCodeSelector)
    {
        ui->setupUi(this);
        this->resetSelcalCodes(true);
        this->setValidityHint();
        ui->lblp_ValidCodeIcon->setToolTips("valid SELCAL", "invalid SELCAL");

        // limit number of elements: https://forum.qt.io/topic/11315/limit-the-number-of-visible-items-on-qcombobox/6
        ui->cb_SelcalPairs1->setStyleSheet("combobox-popup: 0;");
        ui->cb_SelcalPairs2->setStyleSheet("combobox-popup: 0;");

        connect(ui->cb_SelcalPairs1, qOverload<int>(&QComboBox::currentIndexChanged), this,
                &CSelcalCodeSelector::selcalIndexChanged);
        connect(ui->cb_SelcalPairs2, qOverload<int>(&QComboBox::currentIndexChanged), this,
                &CSelcalCodeSelector::selcalIndexChanged);
    }

    CSelcalCodeSelector::~CSelcalCodeSelector() {}

    QString CSelcalCodeSelector::getSelcalCode() const
    {
        QString selcal = ui->cb_SelcalPairs1->currentText();
        selcal.append(ui->cb_SelcalPairs2->currentText());
        return selcal;
    }

    CSelcal CSelcalCodeSelector::getSelcal() const
    {
        CSelcal selcal(getSelcalCode());
        return selcal;
    }

    void CSelcalCodeSelector::resetSelcalCodes(bool allowEmptyValue)
    {
        ui->cb_SelcalPairs1->clear();
        if (allowEmptyValue) ui->cb_SelcalPairs1->addItem("  ");
        ui->cb_SelcalPairs1->addItems(swift::misc::aviation::CSelcal::codePairs());
        ui->cb_SelcalPairs2->clear();
        if (allowEmptyValue) ui->cb_SelcalPairs2->addItem("  ");
        ui->cb_SelcalPairs2->addItems(swift::misc::aviation::CSelcal::codePairs());
    }

    void CSelcalCodeSelector::setSelcalCode(const QString &selcal)
    {
        const QString s = selcal.isEmpty() ? "    " : selcal.toUpper().trimmed();
        if (s.length() != 4) { return; } // still incomplete code
        if (this->getSelcalCode() == s) { return; } // avoid unintended signals
        const QString s1 = s.left(2);
        const QString s2 = s.right(2);
        if (swift::misc::aviation::CSelcal::codePairs().contains(s1)) { ui->cb_SelcalPairs1->setCurrentText(s1); }
        if (swift::misc::aviation::CSelcal::codePairs().contains(s2)) { ui->cb_SelcalPairs2->setCurrentText(s2); }
    }

    void CSelcalCodeSelector::setSelcal(const swift::misc::aviation::CSelcal &selcal)
    {
        this->setSelcalCode(selcal.getCode());
    }

    bool CSelcalCodeSelector::hasValidCode() const
    {
        const QString s = this->getSelcalCode();
        if (s.length() != 4) return false;
        return swift::misc::aviation::CSelcal::isValidCode(s);
    }

    void CSelcalCodeSelector::clear()
    {
        if (ui->cb_SelcalPairs1->count() < 1) { this->resetSelcalCodes(true); }
        ui->cb_SelcalPairs1->setCurrentIndex(0);
        ui->cb_SelcalPairs2->setCurrentIndex(0);
    }

    int CSelcalCodeSelector::getComboBoxHeight() const { return ui->cb_SelcalPairs1->height(); }

    void CSelcalCodeSelector::setComboBoxMinimumHeight(int h)
    {
        ui->cb_SelcalPairs1->setMinimumHeight(h);
        ui->cb_SelcalPairs2->setMinimumHeight(h);
    }

    void CSelcalCodeSelector::selcalIndexChanged(int index)
    {
        Q_UNUSED(index);
        this->setValidityHint();
        emit valueChanged();
    }

    void CSelcalCodeSelector::setValidityHint() { ui->lblp_ValidCodeIcon->setTicked(this->hasValidCode()); }
} // namespace swift::gui::components
