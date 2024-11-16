// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/components/dbliverycolorsearch.h"

#include "ui_dbliverycolorsearch.h"

#include "core/webdataservices.h"
#include "gui/guiapplication.h"
#include "misc/aviation/liverylist.h"

using namespace swift::misc;
using namespace swift::misc::aviation;

namespace swift::gui::components
{
    CDbLiveryColorSearch::CDbLiveryColorSearch(QWidget *parent) : QFrame(parent), ui(new Ui::CDbLiveryColorSearch)
    {
        ui->setupUi(this);
        this->setFocusProxy(ui->comp_FuselageSelector);
    }

    CDbLiveryColorSearch::~CDbLiveryColorSearch() {}

    CLivery CDbLiveryColorSearch::getLivery() const
    {
        const CRgbColor fuselage = ui->comp_FuselageSelector->getColor();
        const CRgbColor tail = ui->comp_TailSelector->getColor();
        const CLiveryList liveries(sGui->getWebDataServices()->getLiveries());
        return liveries.findClosestColorLiveryOrDefault(fuselage, tail);
    }

    void CDbLiveryColorSearch::presetColorLivery(const CLivery &livery)
    {
        if (livery.isColorLivery())
        {
            ui->comp_FuselageSelector->setColor(livery.getColorFuselage());
            ui->comp_TailSelector->setColor(livery.getColorTail());
        }
    }
} // namespace swift::gui::components
