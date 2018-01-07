/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "pilotform.h"
#include "ui_pilotform.h"
#include "blackgui/guiapplication.h"
#include "blackgui/uppercasevalidator.h"
#include "blackcore/webdataservices.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/network/user.h"
#include "blackconfig/buildconfig.h"
#include <QCompleter>
#include <QStyledItemDelegate>

using namespace BlackConfig;
using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackCore;

namespace BlackGui
{
    namespace Editors
    {
        CPilotForm::CPilotForm(QWidget *parent) :
            CForm(parent),
            ui(new Ui::CPilotForm)
        {
            ui->setupUi(this);
            ui->lblp_HomeAirport->setToolTips("ok", "wrong");
            ui->lblp_Id->setToolTips("ok", "wrong");
            ui->lblp_Password->setToolTips("ok", "wrong");
            ui->lblp_RealName->setToolTips("ok", "wrong");

            ui->le_HomeAirport->setValidator(new CUpperCaseValidator(this));
            this->initCompleters(CEntityFlags::AirportEntity);

            connect(ui->tb_UnhidePassword, &QToolButton::clicked, this, &CPilotForm::unhidePassword);
            connect(ui->le_Id, &QLineEdit::editingFinished, this, &CPilotForm::doValidation);
            connect(ui->le_HomeAirport, &QLineEdit::editingFinished, this, &CPilotForm::doValidation);
            connect(ui->le_Password, &QLineEdit::editingFinished, this, &CPilotForm::doValidation);
            connect(ui->le_RealName, &QLineEdit::editingFinished, this, &CPilotForm::doValidation);

            // web service data
            Q_ASSERT_X(sGui && sGui->hasWebDataServices(), Q_FUNC_INFO, "Need web data services");
            connect(sGui->getWebDataServices(), &CWebDataServices::dataRead, this, &CPilotForm::onWebServiceDataRead);
        }

        CPilotForm::~CPilotForm()
        { }

        void CPilotForm::setVatsimValidation(bool vatsim)
        {
            if (vatsim == m_vatsim) { return; }
            m_vatsim = vatsim;
            ui->le_Id->setValidator(vatsim ? m_vatsimIntValidator.data() : nullptr);
        }

        CUser CPilotForm::getUser() const
        {
            CUser user = CUser(ui->le_Id->text().trimmed(), CUser::beautifyRealName(ui->le_RealName->text()), "", ui->le_Password->text());
            user.setHomeBase(CAirportIcaoCode(ui->le_HomeAirport->text().trimmed()));
            return user;
        }

        bool CPilotForm::setUser(const CUser &user, bool ignoreEmptyUser)
        {
            if (ignoreEmptyUser && user.isNull()) { return false; }
            if (user.hasCredentials())
            {
                ui->le_Id->setText(user.getId());
                ui->le_Password->setText(user.getPassword());
                ui->le_RealName->setText(user.getRealName());
                if (user.hasHomeBase())
                {
                    ui->le_HomeAirport->setText(user.getHomeBase().asString());
                }
            }
            else if (CBuildConfig::isLocalDeveloperDebugBuild())
            {
                ui->le_Id->setText("1288459");
                ui->le_Password->setText("4769");
                ui->le_HomeAirport->setText("LOWI");
                ui->le_RealName->setText("Swift Project");
            }
            this->validate();
            return true;
        }

        void CPilotForm::clear()
        {
            ui->le_Id->clear();
            ui->le_Password->clear();
            ui->le_HomeAirport->clear();
            ui->le_RealName->clear();

            ui->lblp_Id->setTicked(false);
            ui->lblp_HomeAirport->setTicked(false);
            ui->lblp_Password->setTicked(false);
            ui->lblp_RealName->setTicked(false);
        }

        void CPilotForm::setReadOnly(bool readonly)
        {
            ui->le_HomeAirport->setReadOnly(readonly);
            ui->le_Id->setReadOnly(readonly);
            ui->le_Password->setReadOnly(readonly);
            ui->le_RealName->setReadOnly(readonly);
            ui->lblp_Id->setVisible(!readonly);
            ui->lblp_HomeAirport->setVisible(!readonly);
            ui->lblp_Password->setVisible(!readonly);
            ui->lblp_RealName->setVisible(!readonly);
        }

        CStatusMessageList CPilotForm::validate(bool nested) const
        {
            Q_UNUSED(nested);

            CStatusMessageList msgs;
            const CUser user(this->getUser());
            const bool validId = m_vatsim ? user.hasValidVatsimId() : user.hasId();
            ui->lblp_Id->setTicked(validId);
            if (!validId) { msgs.push_back(CStatusMessage(this).validationError("Invalid id")); }

            const bool validHomeAirport = user.hasValidHomeBase();
            ui->lblp_HomeAirport->setTicked(validHomeAirport);
            if (!validHomeAirport) { msgs.push_back(CStatusMessage(this).validationError("Wromg home airport")); }

            const bool validCredentials = user.hasCredentials();
            ui->lblp_Password->setTicked(validCredentials);
            if (!validCredentials) { msgs.push_back(CStatusMessage(this).validationError("Invalid credentials")); }

            const bool validRealName = user.hasRealName();
            ui->lblp_RealName->setTicked(validRealName);
            if (!validRealName) { msgs.push_back(CStatusMessage(this).validationError("Invalid real name")); }

            return msgs;
        }

        void CPilotForm::unhidePassword()
        {
            static const QLineEdit::EchoMode originalMode = ui->le_Password->echoMode();
            ui->le_Password->setEchoMode(QLineEdit::Normal);
            QTimer::singleShot(5000, this, [ = ]
            {
                ui->le_Password->setEchoMode(originalMode);
            });
        }

        void CPilotForm::initCompleters(CEntityFlags::Entity entity)
        {
            // completers where possible
            if (sGui && sGui->hasWebDataServices())
            {
                if (entity.testFlag(CEntityFlags::AirportEntity) && !ui->le_HomeAirport->completer())
                {
                    // one time init
                    const QStringList airports = sGui->getWebDataServices()->getAirports().allIcaoCodes(true);
                    if (!airports.isEmpty())
                    {
                        QCompleter *completer = new QCompleter(airports, this);
                        QStyledItemDelegate *itemDelegate = new QStyledItemDelegate(completer);
                        completer->popup()->setItemDelegate(itemDelegate);
                        ui->le_HomeAirport->setCompleter(completer);
                        completer->popup()->setObjectName("AirportCompleter");
                        completer->popup()->setMinimumWidth(175);
                    }
                }
            }
        }

        void CPilotForm::onWebServiceDataRead(CEntityFlags::Entity entity, CEntityFlags::ReadState state, int number)
        {
            if (state != CEntityFlags::ReadFinished) { return; }
            Q_UNUSED(number);
            this->initCompleters(entity);
        }

        void CPilotForm::doValidation()
        {
            this->validate();
        }
    } // ns
} // ns
