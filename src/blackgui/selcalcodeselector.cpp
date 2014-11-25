/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "selcalcodeselector.h"
#include "ui_selcalcodeselector.h"
#include "blackmisc/avselcal.h"

using namespace BlackMisc::Aviation;

namespace BlackGui
{
    CSelcalCodeSelector::CSelcalCodeSelector(QWidget *parent) :
        QFrame(parent), ui(new Ui::CSelcalCodeSelector)
    {
        this->ui->setupUi(this);
        this->resetSelcalCodes(true);

        bool c;
        c = connect(this->ui->cb_SelcalPairs1, SIGNAL(currentIndexChanged(int)), this, SIGNAL(valueChanged()));
        Q_ASSERT(c);
        c = connect(this->ui->cb_SelcalPairs2, SIGNAL(currentIndexChanged(int)), this, SIGNAL(valueChanged()));
        Q_ASSERT(c);
    }

    CSelcalCodeSelector::~CSelcalCodeSelector()
    { }

    QString CSelcalCodeSelector::getSelcalCode() const
    {
        QString selcal = this->ui->cb_SelcalPairs1->currentText();
        selcal.append(this->ui->cb_SelcalPairs2->currentText());
        return selcal;
    }

    BlackMisc::Aviation::CSelcal CSelcalCodeSelector::getSelcal() const
    {
        CSelcal selcal(getSelcalCode());
        return selcal;
    }

    void CSelcalCodeSelector::resetSelcalCodes(bool allowEmptyValue)
    {
        this->ui->cb_SelcalPairs1->clear();
        if (allowEmptyValue) this->ui->cb_SelcalPairs1->addItem("  ");
        this->ui->cb_SelcalPairs1->addItems(BlackMisc::Aviation::CSelcal::codePairs());
        this->ui->cb_SelcalPairs2->clear();
        if (allowEmptyValue) this->ui->cb_SelcalPairs2->addItem("  ");
        this->ui->cb_SelcalPairs2->addItems(BlackMisc::Aviation::CSelcal::codePairs());
    }

    void CSelcalCodeSelector::setSelcalCode(const QString &selcal)
    {
        if (selcal.length() == 4 && this->getSelcalCode() == selcal) return; // avoid unintended signals
        QString s = selcal.isEmpty() ? "    " : selcal.toUpper().trimmed();
        Q_ASSERT(s.length() == 4);
        if (s.length() != 4) return;
        QString s1 = s.left(2);
        QString s2 = s.right(2);
        if (BlackMisc::Aviation::CSelcal::codePairs().contains(s1))
        {
            this->ui->cb_SelcalPairs1->setCurrentText(s1);
        }
        if (BlackMisc::Aviation::CSelcal::codePairs().contains(s2))
        {
            this->ui->cb_SelcalPairs2->setCurrentText(s2);
        }
    }

    void CSelcalCodeSelector::setSelcalCode(const BlackMisc::Aviation::CSelcal &selcal)
    {
        this->setSelcalCode(selcal.getCode());
    }

    bool CSelcalCodeSelector::hasValidCode() const
    {
        QString s = this->getSelcalCode();
        if (s.length() != 4) return false;
        return BlackMisc::Aviation::CSelcal::isValidCode(s);
    }

    void CSelcalCodeSelector::clear()
    {
        if (this->ui->cb_SelcalPairs1->count() < 1) { this->resetSelcalCodes(true); }
        this->ui->cb_SelcalPairs1->setCurrentIndex(0);
        this->ui->cb_SelcalPairs2->setCurrentIndex(0);
    }
}
