/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "pilotform.h"
#include "ui_pilotform.h"
#include "blackgui/components/airportsmallcompleter.h"
#include "blackgui/guiapplication.h"
#include "blackgui/uppercasevalidator.h"
#include "blackgui/guiutility.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/network/user.h"
#include "blackconfig/buildconfig.h"
#include <QCompleter>
#include <QStyledItemDelegate>

using namespace BlackConfig;
using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackGui::Components;

namespace BlackGui::Editors
{
    CPilotForm::CPilotForm(QWidget *parent) : CForm(parent),
                                              ui(new Ui::CPilotForm)
    {
        ui->setupUi(this);
        ui->lblp_HomeAirport->setToolTips("ok", "wrong");
        ui->lblp_Id->setToolTips("ok", "wrong");
        ui->lblp_Password->setToolTips("ok", "wrong");
        ui->lblp_RealName->setToolTips("ok", "wrong");

        connect(ui->pb_UnhidePassword, &QPushButton::clicked, this, &CPilotForm::unhidePassword);
        connect(ui->le_Id, &QLineEdit::editingFinished, this, &CPilotForm::doValidation);
        connect(ui->comp_HomeAirport, &CAirportSmallCompleter::editingFinished, this, &CPilotForm::doValidation);
        connect(ui->le_Password, &QLineEdit::editingFinished, this, &CPilotForm::doValidation);
        connect(ui->le_RealName, &QLineEdit::editingFinished, this, &CPilotForm::doValidation);
    }

    CPilotForm::~CPilotForm()
    {}

    void CPilotForm::setVatsimValidation(bool vatsim)
    {
        if (vatsim == m_vatsim) { return; }
        m_vatsim = vatsim;
        ui->le_Id->setValidator(vatsim ? m_vatsimIntValidator.data() : nullptr);
    }

    CUser CPilotForm::getUser() const
    {
        CUser user = CUser(ui->le_Id->text().trimmed(), CUser::beautifyRealName(ui->le_RealName->text()), "", ui->le_Password->text());
        user.setHomeBase(ui->comp_HomeAirport->getAirportIcaoCode());
        return user;
    }

    bool CPilotForm::setUser(const CUser &user, bool ignoreEmptyUser)
    {
        if (ignoreEmptyUser && user.isNull()) { return false; }
        if (user.isValid())
        {
            ui->le_Id->setText(user.getId());
            ui->le_Password->setText(user.getPassword());
            ui->le_RealName->setText(user.getRealName());
            if (user.hasHomeBase())
            {
                ui->comp_HomeAirport->setAirportIcaoCode(user.getHomeBase());
            }
        }
        else if (CBuildConfig::isLocalDeveloperDebugBuild())
        {
            ui->le_Id->setText("1288459");
            ui->le_Password->setText("4769");
            ui->comp_HomeAirport->setAirportIcaoCode("LOWI");
            ui->le_RealName->setText("Swift Project");
        }
        this->validate();
        return true;
    }

    void CPilotForm::clear()
    {
        ui->le_Id->clear();
        ui->le_Password->clear();
        ui->comp_HomeAirport->clear();
        ui->le_RealName->clear();

        ui->lblp_Id->setTicked(false);
        ui->lblp_HomeAirport->setTicked(false);
        ui->lblp_Password->setTicked(false);
        ui->lblp_RealName->setTicked(false);
    }

    void CPilotForm::setReadOnly(bool readonly)
    {
        ui->comp_HomeAirport->setReadOnly(readonly);
        ui->le_Id->setReadOnly(readonly);
        ui->le_Password->setReadOnly(readonly);
        ui->le_RealName->setReadOnly(readonly);
        ui->lblp_Id->setVisible(!readonly);
        ui->lblp_HomeAirport->setVisible(!readonly);
        ui->lblp_Password->setVisible(!readonly);
        ui->lblp_RealName->setVisible(!readonly);
        this->forceStyleSheetUpdate();
    }

    CStatusMessageList CPilotForm::validate(bool nested) const
    {
        Q_UNUSED(nested);

        CStatusMessageList msgs;
        const CUser user(this->getUser());
        const bool validId = m_vatsim ? user.hasValidVatsimId() : user.hasId();
        ui->lblp_Id->setTicked(validId);
        if (!validId) { msgs.push_back(CStatusMessage(this).validationError(u"Invalid id")); }

        // check against user and if the data entry contains a valid code (or is empty)
        const bool validHomeAirport = user.hasValidOrEmptyHomeBase() && (ui->comp_HomeAirport->isEmpty() || ui->comp_HomeAirport->getAirportIcaoCode().hasValidIcaoCode(false));
        ui->lblp_HomeAirport->setTicked(validHomeAirport);
        if (!validHomeAirport) { msgs.push_back(CStatusMessage(this).validationError(u"Wromg home airport")); }

        const bool validCredentials = user.hasCredentials();
        ui->lblp_Password->setTicked(validCredentials);
        if (!validCredentials) { msgs.push_back(CStatusMessage(this).validationError(u"Invalid credentials")); }

        const bool validRealName = user.hasRealName();
        ui->lblp_RealName->setTicked(validRealName);
        if (!validRealName) { msgs.push_back(CStatusMessage(this).validationError(u"Invalid real name")); }

        return msgs;
    }

    void CPilotForm::unhidePassword()
    {
        CGuiUtility::tempUnhidePassword(ui->le_Password);
    }

    void CPilotForm::doValidation()
    {
        // ui->comp_HomeAirport->clearIfInvalidCode(false);
        this->validate();
    }
} // ns
