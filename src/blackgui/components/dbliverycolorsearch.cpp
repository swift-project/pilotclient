/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "dbliverycolorsearch.h"
#include "blackgui/guiapplication.h"
#include "blackcore/webdataservices.h"
#include "blackmisc/aviation/liverylist.h"
#include "ui_dbliverycolorsearch.h"

using namespace BlackMisc;
using namespace BlackMisc::Aviation;

namespace BlackGui
{
    namespace Components
    {
        CDbLiveryColorSearch::CDbLiveryColorSearch(QWidget *parent) :
            QDialog(parent),
            ui(new Ui::CDbLiveryColorSearch)
        {
            ui->setupUi(this);
            connect(this, &CDbLiveryColorSearch::accepted, this, &CDbLiveryColorSearch::ps_onAccepted);
        }

        CDbLiveryColorSearch::~CDbLiveryColorSearch()
        { }

        BlackMisc::Aviation::CLivery CDbLiveryColorSearch::getLivery() const
        {
            return this->m_foundLivery;
        }

        void CDbLiveryColorSearch::ps_onAccepted()
        {
            if (!sGui || !sGui->hasWebDataServices())
            {
                this->m_foundLivery = CLivery();
                return;
            }

            const CRgbColor fuselage = ui->comp_FuselageSelector->getColor();
            const CRgbColor tail = ui->comp_TailSelector->getColor();
            const CLiveryList liveries(sGui->getWebDataServices()->getLiveries());
            this->m_foundLivery = liveries.findClosestColorLiveryOrDefault(fuselage, tail);
        }
    } // ns
} // ns
