/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "ui_matchingform.h"
#include "matchingform.h"
#include "guiutility.h"

using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackCore;

namespace BlackGui
{
    namespace Editors
    {
        CMatchingForm::CMatchingForm(QWidget *parent) :
            CForm(parent),
            ui(new Ui::CMatchingForm)
        {
            ui->setupUi(this);
        }

        CMatchingForm::~CMatchingForm()
        { }

        void CMatchingForm::setReadOnly(bool readonly)
        {
            CGuiUtility::checkBoxReadOnly(ui->cb_ByModelString, readonly);
            CGuiUtility::checkBoxReadOnly(ui->cb_ByICAOdata, readonly);
            CGuiUtility::checkBoxReadOnly(ui->cb_ByFamily, readonly);
            CGuiUtility::checkBoxReadOnly(ui->cb_ByLivery, readonly);
            CGuiUtility::checkBoxReadOnly(ui->cb_ByCombinedCode, readonly);
        }

        CStatusMessageList CMatchingForm::validate(bool withNestedForms) const
        {
            Q_UNUSED(withNestedForms);
            return CStatusMessageList();
        }

        void CMatchingForm::setValue(const CAircraftMatcherSetup &setup)
        {
            const CAircraftMatcherSetup::MatchingMode mode = setup.getMatchingMode();
            ui->cb_ByModelString->setChecked(mode.testFlag(CAircraftMatcherSetup::ByModelString));
            ui->cb_ByCombinedCode->setChecked(mode.testFlag(CAircraftMatcherSetup::ByCombinedType));
            ui->cb_ByICAOdata->setChecked(mode.testFlag(CAircraftMatcherSetup::ByIcaoData));
            ui->cb_ByLivery->setChecked(mode.testFlag(CAircraftMatcherSetup::ByLivery));
            ui->cb_ByFamily->setChecked(mode.testFlag(CAircraftMatcherSetup::ByFamily));
        }

        CAircraftMatcherSetup CMatchingForm::value() const
        {
            CAircraftMatcherSetup setup;
            setup.setMatchingMode(matchingMode());
            return setup;
        }

        CAircraftMatcherSetup::MatchingMode CMatchingForm::matchingMode() const
        {
            return CAircraftMatcherSetup::matchingMode(
                       ui->cb_ByModelString->isChecked(), ui->cb_ByICAOdata->isChecked(),
                       ui->cb_ByFamily->isChecked(), ui->cb_ByLivery->isChecked(),
                       ui->cb_ByCombinedCode->isChecked()
                   );
        }
    } // ns
} // ns
