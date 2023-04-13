/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "dbusserveraddressselector.h"
#include "ui_dbusserveraddressselector.h"
#include "blackgui/guiapplication.h"
#include "blackmisc/network/networkutils.h"
#include "blackmisc/dbusserver.h"
#include "blackconfig/buildconfig.h"

#include <QIntValidator>

using namespace BlackConfig;
using namespace BlackMisc;
using namespace BlackMisc::Network;

namespace BlackGui::Components
{
    CDBusServerAddressSelector::CDBusServerAddressSelector(QWidget *parent) : QFrame(parent),
                                                                              ui(new Ui::CDBusServerAddressSelector)
    {
        Q_ASSERT(sGui);
        ui->setupUi(this);

        this->setSystemDBusVisible(false);

        ui->cb_DBusServerAddress->addItems(CNetworkUtils::getKnownLocalIpV4Addresses());
        ui->cb_DBusServerAddress->setCurrentIndex(0);
        this->set(sGui->getCmdDBusAddressValue());

        ui->le_DBusServerPort->setValidator(new QIntValidator(0, 65535, this));
        connect(ui->rb_DBusP2P, &QRadioButton::released, this, &CDBusServerAddressSelector::onRadioButtonReleased);
        connect(ui->rb_DBusSession, &QRadioButton::released, this, &CDBusServerAddressSelector::onRadioButtonReleased);
        connect(ui->rb_DBusSystem, &QRadioButton::released, this, &CDBusServerAddressSelector::onRadioButtonReleased);
        connect(ui->le_DBusServerPort, &QLineEdit::editingFinished, this, &CDBusServerAddressSelector::editingFinished);
        connect(ui->cb_DBusServerAddress, &QComboBox::currentTextChanged, this, &CDBusServerAddressSelector::editingFinished);
    }

    CDBusServerAddressSelector::~CDBusServerAddressSelector()
    {}

    QString CDBusServerAddressSelector::getP2PAddress() const
    {
        if (!this->isP2P()) { return {}; }
        return CDBusServer::p2pAddress(
            ui->cb_DBusServerAddress->currentText() + ":" +
            ui->le_DBusServerPort->text());
    }

    QString CDBusServerAddressSelector::getDBusAddress() const
    {
        if (ui->rb_DBusSession->isChecked()) { return CDBusServer::sessionBusAddress(); }
        if (ui->rb_DBusSystem->isChecked()) { return CDBusServer::systemBusAddress(); }
        return this->getP2PAddress();
    }

    QStringList CDBusServerAddressSelector::getDBusCmdLineArgs() const
    {
        return QStringList { "--dbus", this->getDBusAddress() };
    }

    bool CDBusServerAddressSelector::isP2P() const
    {
        return ui->rb_DBusP2P->isChecked();
    }

    void CDBusServerAddressSelector::setDefaultP2PAddress(const QString &dBus)
    {
        const QString dBusLc = dBus.toLower().trimmed();
        QString host, port;
        CDBusServer::dBusAddressToHostAndPort(dBusLc, host, port);
        if (!host.isEmpty())
        {
            if (ui->cb_DBusServerAddress->findText(host) < 0)
            {
                ui->cb_DBusServerAddress->addItem(host);
            }
            ui->cb_DBusServerAddress->setCurrentText(host);
            ui->le_DBusServerPort->setText(port);
        }
    }

    void CDBusServerAddressSelector::set(const QString &dBus)
    {
        const QString dBusLc = dBus.toLower().trimmed();
        if (dBusLc.isEmpty() || dBusLc.startsWith("session"))
        {
            ui->rb_DBusSession->setChecked(true);
        }
        else if (dBusLc.startsWith("sys"))
        {
            ui->rb_DBusSystem->setChecked(true);
        }
        else
        {
            ui->rb_DBusP2P->setChecked(true);
            QString host, port;
            CDBusServer::dBusAddressToHostAndPort(dBusLc, host, port);
            if (!host.isEmpty())
            {
                if (ui->cb_DBusServerAddress->findText(host) < 0)
                {
                    ui->cb_DBusServerAddress->addItem(host);
                }
                ui->cb_DBusServerAddress->setCurrentText(host);
                ui->le_DBusServerPort->setText(port);
            }
        }
        this->onRadioButtonReleased();
    }

    void CDBusServerAddressSelector::setSystemDBusVisible(bool visible)
    {
        const bool wasChecked = ui->rb_DBusSystem->isChecked();
        ui->rb_DBusSystem->setVisible(visible);
        if (!visible && wasChecked)
        {
            ui->rb_DBusSession->setChecked(true);
        }
    }

    void CDBusServerAddressSelector::setP2PDBusVisible(bool visible)
    {
        ui->rb_DBusP2P->setVisible(visible);
        ui->fr_DBusServerAddress->setVisible(visible);
    }

    void CDBusServerAddressSelector::setForXSwiftBus()
    {
        this->setP2PDBusVisible(true);
        this->setSystemDBusVisible(false);
    }

    void CDBusServerAddressSelector::onRadioButtonReleased()
    {
        const bool p2p = this->isP2P();
        ui->le_DBusServerPort->setEnabled(p2p);
        ui->cb_DBusServerAddress->setEnabled(p2p);
        emit this->editingFinished();
    }
} // ns
