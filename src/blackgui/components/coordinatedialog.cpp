/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "coordinatedialog.h"
#include "ui_coordinatedialog.h"
#include <QPushButton>

using namespace BlackMisc;
using namespace BlackMisc::Geo;
using namespace BlackGui::Editors;

namespace BlackGui::Components
{
    CCoordinateDialog::CCoordinateDialog(QWidget *parent) : QDialog(parent),
                                                            ui(new Ui::CCoordinateDialog)
    {
        ui->setupUi(this);
        ui->editor_Coordinate->showSetButton(false);
        this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
        this->noDefaultButtons();
        connect(this, &QDialog::accepted, this, &CCoordinateDialog::changedCoordinate);
    }

    CCoordinateDialog::~CCoordinateDialog()
    {}

    CCoordinateGeodetic CCoordinateDialog::getCoordinate() const
    {
        return ui->editor_Coordinate->getCoordinate();
    }

    void CCoordinateDialog::setCoordinate(const ICoordinateGeodetic &coordinate)
    {
        ui->editor_Coordinate->setCoordinate(coordinate);
    }

    void CCoordinateDialog::setReadOnly(bool readonly)
    {
        ui->editor_Coordinate->setReadOnly(readonly);
    }

    void CCoordinateDialog::setSelectOnly()
    {
        ui->editor_Coordinate->setSelectOnly();
    }

    void CCoordinateDialog::showElevation(bool show)
    {
        ui->editor_Coordinate->showElevation(show);
    }

    CStatusMessageList CCoordinateDialog::validate(bool nested) const
    {
        return ui->editor_Coordinate->validate(nested);
    }

    void CCoordinateDialog::noDefaultButtons()
    {
        QPushButton *okBtn = ui->bb_CoordinateDialog->button(QDialogButtonBox::Ok);
        okBtn->setAutoDefault(true);
        okBtn->setDefault(true);

        QPushButton *caBtn = ui->bb_CoordinateDialog->button(QDialogButtonBox::Cancel);
        caBtn->setAutoDefault(false);
        caBtn->setDefault(false);
    }
} // ns
