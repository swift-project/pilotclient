// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "aircraftpartsform.h"

#include <QJsonDocument>
#include <QJsonParseError>
#include <QPushButton>

#include "ui_aircraftpartsform.h"

#include "gui/guiutility.h"
#include "misc/aviation/aircraftenginelist.h"
#include "misc/aviation/aircraftlights.h"
#include "misc/logmessage.h"
#include "misc/statusmessage.h"

using namespace swift::misc;
using namespace swift::misc::aviation;

namespace swift::gui::editors
{
    CAircraftPartsForm::CAircraftPartsForm(QWidget *parent) : CForm(parent), ui(new Ui::CAircraftPartsForm)
    {
        ui->setupUi(this);
        connect(ui->pb_AircraftPartsLightsOn, &QPushButton::pressed, this, &CAircraftPartsForm::setAllLights);
        connect(ui->pb_AircraftPartsLightsOff, &QPushButton::pressed, this, &CAircraftPartsForm::setAllLights);
        connect(ui->pb_AircraftPartsEnginesOn, &QPushButton::pressed, this, &CAircraftPartsForm::setAllEngines);
        connect(ui->pb_AircraftPartsEnginesOff, &QPushButton::pressed, this, &CAircraftPartsForm::setAllEngines);
        connect(ui->pb_AircraftPartsUiToJson, &QPushButton::pressed, this, &CAircraftPartsForm::guiToJson);
        connect(ui->pb_Set, &QPushButton::pressed, this,
                &CAircraftPartsForm::changeAircraftParts); // force change signal
    }

    CAircraftPartsForm::~CAircraftPartsForm() {}

    void CAircraftPartsForm::setReadOnly(bool readonly)
    {
        CGuiUtility::childrenSetEnabled<QPushButton>(this, !readonly);
        CGuiUtility::checkBoxesReadOnly(this, readonly);
        this->forceStyleSheetUpdate();
    }

    void CAircraftPartsForm::setSelectOnly() { this->setReadOnly(true); }

    CStatusMessageList CAircraftPartsForm::validate(bool nested) const
    {
        Q_UNUSED(nested);
        CStatusMessageList msgs;
        return msgs;
    }

    CAircraftParts CAircraftPartsForm::getAircraftPartsFromGui() const { return this->guiToAircraftParts(); }

    CAircraftParts CAircraftPartsForm::getAircraftPartsFromJson() const
    {
        const QString jsonParts = ui->te_AircraftPartsJson->toPlainText().trimmed();
        CAircraftParts parts;
        if (jsonParts.isEmpty())
        {
            CLogMessage(this).validationError(u"No JSON content");
            return parts;
        }
        QJsonParseError jsonError;
        QJsonDocument jsonDoc(QJsonDocument::fromJson(jsonParts.toUtf8(), &jsonError));
        if (jsonError.error != QJsonParseError::NoError)
        {
            CLogMessage(this).validationError(u"Parse error: %1") << jsonError.errorString();
            return parts;
        }

        try
        {
            parts.convertFromJson(jsonDoc.object());
        }
        catch (const CJsonException &ex)
        {
            CLogMessage(this).preformatted(CStatusMessage::fromJsonException(ex, this, "Parse error"));
            return parts;
        }
        return parts;
    }

    void CAircraftPartsForm::setAircraftParts(const CAircraftParts &parts)
    {
        this->partsToGui(parts);
        ui->te_AircraftPartsJson->setText(parts.toJsonString());
    }

    void CAircraftPartsForm::showSetButton(bool visible) { ui->pb_Set->setVisible(visible); }

    void CAircraftPartsForm::showJson(bool visible) { ui->te_AircraftPartsJson->setVisible(visible); }

    void CAircraftPartsForm::setAllLights()
    {
        const bool on = (QObject::sender() == ui->pb_AircraftPartsLightsOn);
        ui->cb_AircraftPartsLightsStrobe->setChecked(on);
        ui->cb_AircraftPartsLightsLanding->setChecked(on);
        ui->cb_AircraftPartsLightsTaxi->setChecked(on);
        ui->cb_AircraftPartsLightsBeacon->setChecked(on);
        ui->cb_AircraftPartsLightsNav->setChecked(on);
        ui->cb_AircraftPartsLightsLogo->setChecked(on);
    }

    void CAircraftPartsForm::setAllEngines()
    {
        const bool on = (QObject::sender() == ui->pb_AircraftPartsEnginesOn);
        ui->cb_AircraftPartsEngine1->setChecked(on);
        ui->cb_AircraftPartsEngine2->setChecked(on);
        ui->cb_AircraftPartsEngine3->setChecked(on);
        ui->cb_AircraftPartsEngine4->setChecked(on);
        ui->cb_AircraftPartsEngine5->setChecked(on);
        ui->cb_AircraftPartsEngine6->setChecked(on);
    }

    aviation::CAircraftParts CAircraftPartsForm::guiToAircraftParts() const
    {
        const CAircraftLights lights(
            ui->cb_AircraftPartsLightsStrobe->isChecked(), ui->cb_AircraftPartsLightsLanding->isChecked(),
            ui->cb_AircraftPartsLightsTaxi->isChecked(), ui->cb_AircraftPartsLightsBeacon->isChecked(),
            ui->cb_AircraftPartsLightsNav->isChecked(), ui->cb_AircraftPartsLightsLogo->isChecked());
        const CAircraftEngineList engines(
            { ui->cb_AircraftPartsEngine1->isChecked(), ui->cb_AircraftPartsEngine2->isChecked(),
              ui->cb_AircraftPartsEngine3->isChecked(), ui->cb_AircraftPartsEngine4->isChecked(),
              ui->cb_AircraftPartsEngine5->isChecked(), ui->cb_AircraftPartsEngine6->isChecked() });
        const CAircraftParts parts(
            lights, ui->cb_AircraftPartsGearDown->isChecked(), ui->sb_AircraftPartsFlapsPercentage->value(),
            ui->cb_AircraftPartsSpoilers->isChecked(), engines, ui->cb_AircraftPartsIsOnGround->isChecked());
        return parts;
    }

    void CAircraftPartsForm::partsToGui(const CAircraftParts &parts)
    {
        ui->cb_AircraftPartsGearDown->setChecked(parts.isGearDown());
        ui->cb_AircraftPartsIsOnGround->setChecked(parts.isOnGround());
        ui->cb_AircraftPartsSpoilers->setChecked(parts.isSpoilersOut());
        ui->sb_AircraftPartsFlapsPercentage->setValue(parts.getFlapsPercent());

        const CAircraftLights lights = parts.getLights();
        ui->cb_AircraftPartsLightsBeacon->setChecked(lights.isBeaconOn());
        ui->cb_AircraftPartsLightsLanding->setChecked(lights.isLandingOn());
        ui->cb_AircraftPartsLightsLogo->setChecked(lights.isLogoOn());
        ui->cb_AircraftPartsLightsNav->setChecked(lights.isNavOn());
        ui->cb_AircraftPartsLightsStrobe->setChecked(lights.isStrobeOn());
        ui->cb_AircraftPartsLightsTaxi->setChecked(lights.isTaxiOn());

        const CAircraftEngineList engines = parts.getEngines();
        ui->cb_AircraftPartsEngine1->setChecked(engines.isEngineOn(1));
        ui->cb_AircraftPartsEngine2->setChecked(engines.isEngineOn(2));
        ui->cb_AircraftPartsEngine3->setChecked(engines.isEngineOn(3));
        ui->cb_AircraftPartsEngine4->setChecked(engines.isEngineOn(4));
        ui->cb_AircraftPartsEngine5->setChecked(engines.isEngineOn(5));
        ui->cb_AircraftPartsEngine6->setChecked(engines.isEngineOn(6));
    }

    void CAircraftPartsForm::guiToJson()
    {
        const QJsonDocument json(guiToAircraftParts().toJson());
        const QString j(json.toJson(QJsonDocument::Indented));
        ui->te_AircraftPartsJson->setText(j);
    }
} // namespace swift::gui::editors
