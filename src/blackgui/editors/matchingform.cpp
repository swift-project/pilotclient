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
#include <QPushButton>

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
            connect(ui->rb_Reduction, &QRadioButton::released, this, &CMatchingForm::onAlgorithmChanged, Qt::QueuedConnection);
            connect(ui->rb_ScoreAndReduction, &QRadioButton::released, this, &CMatchingForm::onAlgorithmChanged, Qt::QueuedConnection);
            connect(ui->rb_ScoreOnly, &QRadioButton::released, this, &CMatchingForm::onAlgorithmChanged, Qt::QueuedConnection);

            connect(ui->pb_Reset, &QPushButton::released, this, &CMatchingForm::clear, Qt::QueuedConnection);
        }

        CMatchingForm::~CMatchingForm()
        { }

        void CMatchingForm::setReadOnly(bool readonly)
        {
            CGuiUtility::checkBoxReadOnly(ui->cb_ByModelString, readonly);
            CGuiUtility::checkBoxReadOnly(ui->cb_ByFamily, readonly);
            CGuiUtility::checkBoxReadOnly(ui->cb_ByLivery, readonly);
            CGuiUtility::checkBoxReadOnly(ui->cb_ByCombinedCode, readonly);
            CGuiUtility::checkBoxReadOnly(ui->cb_ByVtol, readonly);
            CGuiUtility::checkBoxReadOnly(ui->cb_ByMilitary, readonly);
            CGuiUtility::checkBoxReadOnly(ui->cb_ScoreIgnoreZeros, readonly);
            CGuiUtility::checkBoxReadOnly(ui->cb_ScorePreferColorLiveries, readonly);

            const bool enabled = !readonly;
            ui->rb_Reduction->setEnabled(enabled);
            ui->rb_ScoreAndReduction->setEnabled(enabled);
            ui->rb_ScoreOnly->setEnabled(enabled);
            ui->rb_ByIcaoDataAircraft1st->setEnabled(enabled);
            ui->rb_ByIcaoDataAirline1st->setEnabled(enabled);
            ui->rb_PickFirst->setEnabled(enabled);
            ui->rb_PickByOrder->setEnabled(enabled);
            ui->rb_PickRandom->setEnabled(enabled);
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
            ui->rb_ByIcaoDataAircraft1st->setChecked(mode.testFlag(CAircraftMatcherSetup::ByIcaoOrderAircraftFirst));
            ui->rb_ByIcaoDataAirline1st->setChecked(mode.testFlag(CAircraftMatcherSetup::ByIcaoOrderAirlineFirst));
            ui->cb_ByLivery->setChecked(mode.testFlag(CAircraftMatcherSetup::ByLivery));
            ui->cb_ByFamily->setChecked(mode.testFlag(CAircraftMatcherSetup::ByFamily));
            ui->cb_ByMilitary->setChecked(mode.testFlag(CAircraftMatcherSetup::ByMilitary));
            ui->cb_ByVtol->setChecked(mode.testFlag(CAircraftMatcherSetup::ByVtol));
            ui->cb_ScoreIgnoreZeros->setChecked(mode.testFlag(CAircraftMatcherSetup::ScoreIgnoreZeros));
            ui->cb_ScorePreferColorLiveries->setChecked(mode.testFlag(CAircraftMatcherSetup::ScorePreferColorLiveries));

            this->setMatchingAlgorithm(setup);
            this->setPickStrategy(setup);
        }

        CAircraftMatcherSetup CMatchingForm::value() const
        {
            const CAircraftMatcherSetup setup(algorithm(), matchingMode(), pickStrategy());
            return setup;
        }

        void CMatchingForm::clear()
        {
            const CAircraftMatcherSetup s(algorithm());
            this->setValue(s);
        }

        CAircraftMatcherSetup::MatchingAlgorithm CMatchingForm::algorithm() const
        {
            if (ui->rb_Reduction->isChecked()) { return CAircraftMatcherSetup::MatchingStepwiseReduce; }
            if (ui->rb_ScoreOnly->isChecked()) { return CAircraftMatcherSetup::MatchingScoreBased; }
            return CAircraftMatcherSetup::MatchingStepwiseReducePlusScoreBased;
        }

        CAircraftMatcherSetup::MatchingMode CMatchingForm::matchingMode() const
        {
            return CAircraftMatcherSetup::matchingMode(
                       ui->cb_ByModelString->isChecked(),
                       ui->rb_ByIcaoDataAircraft1st->isChecked(), ui->rb_ByIcaoDataAirline1st->isChecked(),
                       ui->cb_ByFamily->isChecked(), ui->cb_ByLivery->isChecked(),
                       ui->cb_ByCombinedCode->isChecked(),
                       ui->cb_ByMilitary->isChecked(), ui->cb_ByVtol->isChecked(),
                       ui->cb_ScoreIgnoreZeros->isChecked(), ui->cb_ScorePreferColorLiveries->isChecked()
                   );
        }

        CAircraftMatcherSetup::PickSimilarStrategy CMatchingForm::pickStrategy() const
        {
            if (ui->rb_PickRandom->isChecked()) { return CAircraftMatcherSetup::PickRandom; }
            if (ui->rb_PickByOrder->isChecked()) { return CAircraftMatcherSetup::PickByOrder; }
            return CAircraftMatcherSetup::PickFirst;
        }

        void CMatchingForm::setPickStrategy(const CAircraftMatcherSetup &setup)
        {
            switch (setup.getPickStrategy())
            {
            case CAircraftMatcherSetup::PickByOrder: ui->rb_PickByOrder->setChecked(true); break;
            case CAircraftMatcherSetup::PickRandom: ui->rb_PickRandom->setChecked(true); break;
            case CAircraftMatcherSetup::PickFirst:
            default:
                ui->rb_PickFirst->setChecked(true); break;
            }
        }

        void CMatchingForm::setMatchingAlgorithm(const CAircraftMatcherSetup &setup)
        {
            switch (setup.getMatchingAlgorithm())
            {
            case CAircraftMatcherSetup::MatchingStepwiseReduce:
                ui->rb_Reduction->setChecked(true);
                ui->gb_Reduction->setEnabled(true);
                ui->gb_Scoring->setEnabled(false);
                break;
            case CAircraftMatcherSetup::MatchingScoreBased:
                ui->rb_ScoreOnly->setChecked(true);
                ui->gb_Reduction->setEnabled(false);
                ui->gb_Scoring->setEnabled(true);
                break;
            case CAircraftMatcherSetup::MatchingStepwiseReducePlusScoreBased:
            default:
                ui->rb_ScoreAndReduction->setChecked(true);
                ui->gb_Reduction->setEnabled(true);
                ui->gb_Scoring->setEnabled(true);
                break;
            }
        }

        void CMatchingForm::onAlgorithmChanged()
        {
            const CAircraftMatcherSetup setup = this->value();
            this->setValue(setup);
        }
    } // ns
} // ns
