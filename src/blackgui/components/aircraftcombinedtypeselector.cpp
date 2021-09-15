/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/components/aircraftcombinedtypeselector.h"
#include "blackgui/guiutility.h"
#include "blackgui/uppercasevalidator.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "ui_aircraftcombinedtypeselector.h"

#include <QComboBox>
#include <QLineEdit>
#include <QStringBuilder>
#include <QtGlobal>

using namespace BlackMisc::Aviation;

namespace BlackGui::Components
{
    CAircraftCombinedTypeSelector::CAircraftCombinedTypeSelector(QWidget *parent) :
        QFrame(parent),
        ui(new Ui::CAircraftCombinedTypeSelector)
    {
        ui->setupUi(this);
        connect(ui->le_CombinedType, &QLineEdit::editingFinished, this, &CAircraftCombinedTypeSelector::combinedTypeEntered);
        connect(ui->le_CombinedType, &QLineEdit::returnPressed,   this, &CAircraftCombinedTypeSelector::combinedTypeEntered);

        connect(ui->cb_EngineCount, &QComboBox::currentTextChanged, this, &CAircraftCombinedTypeSelector::changedComboBox);
        connect(ui->cb_EngineType,  &QComboBox::currentTextChanged, this, &CAircraftCombinedTypeSelector::changedComboBox);
        connect(ui->cb_Type,        &QComboBox::currentTextChanged, this, &CAircraftCombinedTypeSelector::changedComboBox);

        ui->le_CombinedType->setValidator(new CUpperCaseValidator(this));
    }

    CAircraftCombinedTypeSelector::~CAircraftCombinedTypeSelector()
    { }

    void CAircraftCombinedTypeSelector::setCombinedType(const QString &combinedCode)
    {
        QString engineCount, engineType, aircraftType;
        const QString cc(combinedCode.trimmed().toUpper().left(3));
        if (m_cc == cc) { return; }
        m_cc = cc;

        if (cc.length() > 0) { aircraftType = cc.at(0); }
        if (cc.length() > 1) { engineCount  = cc.mid(1, 1); }
        if (cc.length() > 2) { engineType   = cc.mid(2, 1); }

        if (this->getCombinedTypeFromComboBoxes() != cc)
        {
            CGuiUtility::setComboBoxValueByStartingString(ui->cb_EngineCount, engineCount,  "unspecified");
            CGuiUtility::setComboBoxValueByStartingString(ui->cb_EngineType,  engineType,   "unspecified");
            CGuiUtility::setComboBoxValueByStartingString(ui->cb_Type,        aircraftType, "unspecified");
        }

        if (ui->le_CombinedType->text() != cc) { ui->le_CombinedType->setText(cc); }
        emit this->changedCombinedType(cc);
    }

    void CAircraftCombinedTypeSelector::setCombinedType(const CAircraftIcaoCode &icao)
    {
        this->setCombinedType(icao.getCombinedType());
    }

    void CAircraftCombinedTypeSelector::clear()
    {
        this->setCombinedType("");
        ui->le_CombinedType->clear();
    }

    void CAircraftCombinedTypeSelector::setReadOnly(bool readOnly)
    {
        ui->le_CombinedType->setReadOnly(readOnly);
        ui->cb_EngineCount->setEnabled(!readOnly);
        ui->cb_EngineType->setEnabled(!readOnly);
        ui->cb_Type->setEnabled(!readOnly);
    }

    QString CAircraftCombinedTypeSelector::getCombinedType() const
    {
        QString ct(ui->le_CombinedType->text().trimmed().toUpper());
        if (ct.isEmpty() || ct == QStringView(u"---")) { return {}; }
        if (CAircraftIcaoCode::isValidCombinedType(ct)) { return ct; }

        QString ct2(getCombinedTypeFromComboBoxes());
        return ct2;
    }

    void CAircraftCombinedTypeSelector::combinedTypeEntered()
    {
        const QString cc(ui->le_CombinedType->text().trimmed().toUpper() % u"---");
        this->setCombinedType(cc.left(3));
    }

    void CAircraftCombinedTypeSelector::changedComboBox(const QString &text)
    {
        Q_UNUSED(text);
        const QString ct(this->getCombinedTypeFromComboBoxes());
        if (ui->le_CombinedType->text() == ct) { return; }
        ui->le_CombinedType->setText(ct);
        emit this->changedCombinedType(ct);
    }

    QString CAircraftCombinedTypeSelector::getCombinedTypeFromComboBoxes() const
    {
        // try to get from combox boxes instead
        QString ec = ui->cb_EngineCount->currentText().left(1);
        QString t = ui->cb_Type->currentText().left(1);
        QString et = ui->cb_EngineType->currentText().left(1);

        QString ct2(QString(t + ec + et).toUpper());
        return ct2.replace('U', '-');
    }
} // ns
