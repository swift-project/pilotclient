/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "dbliverycolorsearch.h"
#include "blackgui/guiapplication.h"
#include "blackcore/webdataservices.h"
#include "blackmisc/aviation/liverylist.h"
#include "ui_dbliverycolorsearch.h"

using namespace BlackMisc;
using namespace BlackMisc::Aviation;

namespace BlackGui::Components
{
    CDbLiveryColorSearch::CDbLiveryColorSearch(QWidget *parent) :
        QFrame(parent),
        ui(new Ui::CDbLiveryColorSearch)
    {
        ui->setupUi(this);
        this->setFocusProxy(ui->comp_FuselageSelector);
    }

    CDbLiveryColorSearch::~CDbLiveryColorSearch()
    { }

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
} // ns
