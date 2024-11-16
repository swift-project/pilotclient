// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "matchingform.h"

#include <QFileDialog>
#include <QPushButton>

#include "ui_matchingform.h"

#include "gui/guiutility.h"

using namespace swift::misc;
using namespace swift::misc::simulation;
using namespace swift::core;

namespace swift::gui::editors
{
    CMatchingForm::CMatchingForm(QWidget *parent) : CForm(parent),
                                                    ui(new Ui::CMatchingForm)
    {
        ui->setupUi(this);
        connect(ui->rb_Reduction, &QRadioButton::released, this, &CMatchingForm::onAlgorithmChanged, Qt::QueuedConnection);

        connect(ui->pb_ResetAlgorithm, &QPushButton::released, this, &CMatchingForm::resetByAlgorithm, Qt::QueuedConnection);
        connect(ui->pb_ResetAll, &QPushButton::released, this, &CMatchingForm::resetAll, Qt::QueuedConnection);
        connect(ui->pb_MsReverseLookup, &QPushButton::released, this, &CMatchingForm::fileDialog, Qt::QueuedConnection);
        connect(ui->pb_MsMatching, &QPushButton::released, this, &CMatchingForm::fileDialog, Qt::QueuedConnection);

        connect(ui->rb_ScoreAndReduction, &QRadioButton::released, this, &CMatchingForm::onAlgorithmChanged, Qt::QueuedConnection);
        connect(ui->rb_ScoreOnly, &QRadioButton::released, this, &CMatchingForm::onAlgorithmChanged, Qt::QueuedConnection);

        connect(ui->rb_AirlineGroupIfNoAirline, &QRadioButton::released, this, &CMatchingForm::onAirlineGroupChanged, Qt::QueuedConnection);
        connect(ui->rb_AirlineGroupAsAirline, &QRadioButton::released, this, &CMatchingForm::onAirlineGroupChanged, Qt::QueuedConnection);
        connect(ui->rb_AirlineGroupNo, &QRadioButton::released, this, &CMatchingForm::onAirlineGroupChanged, Qt::QueuedConnection);
    }

    CMatchingForm::~CMatchingForm()
    {}

    void CMatchingForm::setReadOnly(bool readonly)
    {
        CGuiUtility::checkBoxReadOnly(ui->cb_ByModelString, readonly);
        CGuiUtility::checkBoxReadOnly(ui->cb_ByFamily, readonly);
        CGuiUtility::checkBoxReadOnly(ui->cb_ByLivery, readonly);
        CGuiUtility::checkBoxReadOnly(ui->cb_ByCombinedCode, readonly);
        CGuiUtility::checkBoxReadOnly(ui->cb_ByVtol, readonly);
        CGuiUtility::checkBoxReadOnly(ui->cb_ByMilitary, readonly);
        CGuiUtility::checkBoxReadOnly(ui->cb_ByCivilian, readonly);
        CGuiUtility::checkBoxReadOnly(ui->cb_ScoreIgnoreZeros, readonly);
        CGuiUtility::checkBoxReadOnly(ui->cb_ScorePreferColorLiveries, readonly);
        CGuiUtility::checkBoxReadOnly(ui->cb_ModelSetRemoveFailed, readonly);
        CGuiUtility::checkBoxReadOnly(ui->cb_ModelFailedFailover, readonly);
        CGuiUtility::checkBoxReadOnly(ui->cb_ModelSetVerification, readonly);
        CGuiUtility::checkBoxReadOnly(ui->cb_ModelSetVerificationOnlyErrorWarning, readonly);
        CGuiUtility::checkBoxReadOnly(ui->cb_CategoryGlider, readonly);
        CGuiUtility::checkBoxReadOnly(ui->cb_CategoryMilitaryAircraft, readonly);
        CGuiUtility::checkBoxReadOnly(ui->cb_ReverseUseModelString, readonly);
        CGuiUtility::checkBoxReadOnly(ui->cb_ReverseUseSwiftLiveryIds, readonly);
        CGuiUtility::checkBoxReadOnly(ui->cb_MsReverseLookup, readonly);
        CGuiUtility::checkBoxReadOnly(ui->cb_MsMatching, readonly);

        const bool enabled = !readonly;
        ui->rb_Reduction->setEnabled(enabled);
        ui->rb_ScoreAndReduction->setEnabled(enabled);
        ui->rb_ScoreOnly->setEnabled(enabled);
        ui->rb_ByIcaoDataAircraft1st->setEnabled(enabled);
        ui->rb_ByIcaoDataAirline1st->setEnabled(enabled);
        ui->rb_AirlineGroupAsAirline->setEnabled(enabled);
        ui->rb_AirlineGroupNo->setEnabled(enabled);
        ui->rb_AirlineGroupIfNoAirline->setEnabled(enabled);

        ui->rb_PickFirst->setEnabled(enabled);
        ui->rb_PickByOrder->setEnabled(enabled);
        ui->rb_PickRandom->setEnabled(enabled);

        ui->le_MsReverseLookup->setEnabled(enabled);
        ui->le_MsMatching->setEnabled(enabled);
    }

    CStatusMessageList CMatchingForm::validate(bool withNestedForms) const
    {
        Q_UNUSED(withNestedForms)
        return CStatusMessageList();
    }

    void CMatchingForm::setValue(const CAircraftMatcherSetup &setup)
    {
        const CAircraftMatcherSetup::MatchingMode mode = setup.getMatchingMode();
        ui->cb_ByModelString->setChecked(mode.testFlag(CAircraftMatcherSetup::ByModelString));
        ui->cb_ByCombinedCode->setChecked(mode.testFlag(CAircraftMatcherSetup::ByCombinedType));

        ui->rb_ByIcaoDataAircraft1st->setChecked(mode.testFlag(CAircraftMatcherSetup::ByIcaoOrderAircraftFirst));
        ui->rb_ByIcaoDataAirline1st->setChecked(mode.testFlag(CAircraftMatcherSetup::ByIcaoOrderAirlineFirst));

        const bool nag = !mode.testFlag(CAircraftMatcherSetup::ByAirlineGroupSameAsAirline) && !mode.testFlag(CAircraftMatcherSetup::ByAirlineGroupIfNoAirline);
        if (nag)
        {
            ui->rb_AirlineGroupNo->setChecked(nag);
        }
        else
        {
            ui->rb_AirlineGroupAsAirline->setChecked(mode.testFlag(CAircraftMatcherSetup::ByAirlineGroupSameAsAirline));
            ui->rb_AirlineGroupIfNoAirline->setChecked(mode.testFlag(CAircraftMatcherSetup::CAircraftMatcherSetup::ByAirlineGroupIfNoAirline));
        }

        ui->cb_ByLivery->setChecked(mode.testFlag(CAircraftMatcherSetup::ByLivery));
        ui->cb_ByFamily->setChecked(mode.testFlag(CAircraftMatcherSetup::ByFamily));
        ui->cb_ByMilitary->setChecked(mode.testFlag(CAircraftMatcherSetup::ByMilitary));
        ui->cb_ByCivilian->setChecked(mode.testFlag(CAircraftMatcherSetup::ByCivilian));
        ui->cb_CategoryGlider->setChecked(mode.testFlag(CAircraftMatcherSetup::ByCategoryGlider));
        ui->cb_CategoryMilitaryAircraft->setChecked(mode.testFlag(CAircraftMatcherSetup::ByCategoryMilitary));
        ui->cb_CategorySmallAircraft->setChecked(mode.testFlag(CAircraftMatcherSetup::ByCategorySmallAircraft));
        ui->cb_ByVtol->setChecked(mode.testFlag(CAircraftMatcherSetup::ByVtol));
        ui->cb_ScoreIgnoreZeros->setChecked(mode.testFlag(CAircraftMatcherSetup::ScoreIgnoreZeros));
        ui->cb_ScorePreferColorLiveries->setChecked(mode.testFlag(CAircraftMatcherSetup::ScorePreferColorLiveries));
        ui->cb_ExclNoExcludedModels->setChecked(mode.testFlag(CAircraftMatcherSetup::ExcludeNoExcluded));
        ui->cb_ExclNoDbData->setChecked(mode.testFlag(CAircraftMatcherSetup::ExcludeNoDbData));
        ui->cb_ModelSetRemoveFailed->setChecked(mode.testFlag(CAircraftMatcherSetup::ModelSetRemoveFailedModel) || mode.testFlag(CAircraftMatcherSetup::ModelFailoverIfNoModelCanBeAdded));
        ui->cb_ModelFailedFailover->setChecked(mode.testFlag(CAircraftMatcherSetup::ModelFailoverIfNoModelCanBeAdded));
        ui->cb_ModelSetVerification->setChecked(mode.testFlag(CAircraftMatcherSetup::ModelVerificationAtStartup));
        ui->cb_ModelSetVerificationOnlyErrorWarning->setChecked(mode.testFlag(CAircraftMatcherSetup::ModelVerificationOnlyWarnError));
        ui->cb_ReverseUseModelString->setChecked(mode.testFlag(CAircraftMatcherSetup::ReverseLookupModelString));
        ui->cb_ReverseUseSwiftLiveryIds->setChecked(mode.testFlag(CAircraftMatcherSetup::ReverseLookupSwiftLiveryIds));

        this->setMatchingAlgorithm(setup);
        this->setPickStrategy(setup);

        ui->cb_MsReverseLookup->setChecked(setup.isMsReverseLookupEnabled());
        ui->cb_MsMatching->setChecked(setup.isMsMatchingStageEnabled());
        ui->le_MsReverseLookup->setText(setup.getMsReverseLookupFile());
        ui->le_MsMatching->setText(setup.getMsMatchingStageFile());
    }

    CAircraftMatcherSetup CMatchingForm::value() const
    {
        CAircraftMatcherSetup setup(algorithm(), matchingMode(), pickStrategy());
        setup.setAirlineGroupBehaviour(ui->rb_AirlineGroupIfNoAirline->isChecked(), ui->rb_AirlineGroupAsAirline->isChecked());
        setup.setMsReverseLookupFile(ui->le_MsReverseLookup->text());
        setup.setMsMatchingStageFile(ui->le_MsMatching->text());
        setup.setMsReverseLookupEnabled(ui->cb_MsReverseLookup->isChecked());
        setup.setMsMatchingStageEnabled(ui->cb_MsMatching->isChecked());
        return setup;
    }

    void CMatchingForm::clear()
    {
        this->resetAll();
    }

    void CMatchingForm::resetByAlgorithm()
    {
        const CAircraftMatcherSetup s(this->algorithm());
        this->setValue(s);
    }

    void CMatchingForm::resetAll()
    {
        const CAircraftMatcherSetup s;
        this->setValue(s);
    }

    void CMatchingForm::fileDialog()
    {
        const bool nw = (QObject::sender() == ui->pb_MsReverseLookup);
        QString fn = nw ? ui->le_MsReverseLookup->text() : ui->le_MsMatching->text();
        CDirectories swiftDirs = m_directories.get();

        fn = QFileDialog::getOpenFileName(nullptr, tr("Matching script"), fn.isEmpty() ? swiftDirs.getMatchingScriptDirectoryOrDefault() : fn, "Matching script (*.js)");
        const QFileInfo fi(fn);
        if (!fi.exists()) { return; }
        if (nw)
        {
            ui->le_MsReverseLookup->setText(fi.absoluteFilePath());
        }
        else
        {
            ui->le_MsMatching->setText(fi.absoluteFilePath());
        }

        swiftDirs.setMatchingScriptDirectory(fi.absolutePath());
        m_directories.setAndSave(swiftDirs);
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
            ui->cb_ReverseUseModelString->isChecked(), ui->cb_ReverseUseSwiftLiveryIds->isChecked(),
            ui->cb_ByModelString->isChecked(),
            ui->rb_ByIcaoDataAircraft1st->isChecked(), ui->rb_ByIcaoDataAirline1st->isChecked(),
            ui->cb_ByFamily->isChecked(), ui->cb_ByLivery->isChecked(),
            ui->cb_ByCombinedCode->isChecked(),
            ui->cb_ByMilitary->isChecked(),
            ui->cb_ByCivilian->isChecked(),
            ui->cb_ByVtol->isChecked(),
            ui->cb_CategoryGlider->isChecked(),
            ui->cb_CategoryMilitaryAircraft->isChecked(),
            ui->cb_CategorySmallAircraft->isChecked(),
            ui->cb_ScoreIgnoreZeros->isChecked(), ui->cb_ScorePreferColorLiveries->isChecked(),
            ui->cb_ExclNoDbData->isChecked(), ui->cb_ExclNoExcludedModels->isChecked(),
            ui->cb_ModelSetVerification->isChecked(), ui->cb_ModelSetVerificationOnlyErrorWarning->isChecked(),
            ui->cb_ModelSetRemoveFailed->isChecked(),
            ui->cb_ModelFailedFailover->isChecked());
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
            ui->rb_PickFirst->setChecked(true);
            break;
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

    void CMatchingForm::onAirlineGroupChanged()
    {
        const CAircraftMatcherSetup setup = this->value();
        this->setValue(setup);
    }
} // namespace swift::gui::editors
