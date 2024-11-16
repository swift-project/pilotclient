// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "coordinatedialog.h"

#include <QPushButton>

#include "ui_coordinatedialog.h"

using namespace swift::misc;
using namespace swift::misc::geo;
using namespace swift::gui::editors;

namespace swift::gui::components
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
} // namespace swift::gui::components
