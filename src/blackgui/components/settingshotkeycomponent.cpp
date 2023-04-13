/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/components/configurationwizard.h"
#include "blackgui/components/settingshotkeycomponent.h"
#include "blackgui/components/hotkeydialog.h"
#include "blackgui/guiapplication.h"
#include "blackcore/context/contextapplication.h"
#include "blackcore/context/contextaudio.h"
#include "blackcore/inputmanager.h"
#include "blackmisc/input/actionhotkeydefs.h"
#include "blackmisc/metadatautils.h"
#include "ui_settingshotkeycomponent.h"

#include <QAbstractItemModel>
#include <QFlags>
#include <QItemSelectionModel>
#include <QList>
#include <QMessageBox>
#include <QModelIndex>
#include <QModelIndexList>
#include <QPushButton>
#include <QString>
#include <QTableView>
#include <QVariant>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::Input;
using namespace BlackGui::Models;
using namespace BlackCore;
using namespace BlackCore::Context;

namespace BlackGui::Components
{
    CSettingsHotkeyComponent::CSettingsHotkeyComponent(QWidget *parent) : QFrame(parent),
                                                                          ui(new Ui::CSettingsHotkeyComponent)
    {
        Q_ASSERT_X(sGui, Q_FUNC_INFO, "Missing sGui");
        ui->setupUi(this);
        ui->tv_Hotkeys->setModel(&m_model);

        connect(ui->pb_AddHotkey, &QPushButton::clicked, this, &CSettingsHotkeyComponent::addEntry);
        connect(ui->pb_EditHotkey, &QPushButton::clicked, this, &CSettingsHotkeyComponent::editEntry);
        connect(ui->pb_RemoveHotkey, &QPushButton::clicked, this, &CSettingsHotkeyComponent::removeEntry);
        connect(ui->tb_ReloadHotkey, &QPushButton::clicked, this, &CSettingsHotkeyComponent::reloadHotkeysFromSettings);

        this->reloadHotkeysFromSettings();
        ui->tv_Hotkeys->selectRow(0);
    }

    CSettingsHotkeyComponent::~CSettingsHotkeyComponent()
    {}

    void CSettingsHotkeyComponent::saveSettings()
    {
        const CStatusMessage msg = m_actionHotkeys.save();
        CLogMessage(this).preformatted(msg);
    }

    void CSettingsHotkeyComponent::registerDummyPttEntry()
    {
        Q_ASSERT_X(sApp && sApp->getInputManager(), Q_FUNC_INFO, "Missing input manager");
        sApp->getInputManager()->registerAction(pttHotkeyAction(), pttHotkeyIcon());
    }

    void CSettingsHotkeyComponent::addEntry()
    {
        const CActionHotkey selectedActionHotkey = CHotkeyDialog::getActionHotkey(CActionHotkey(), getAllIdentifiers(), this);
        if (selectedActionHotkey.isValid() && checkAndConfirmConflicts(selectedActionHotkey))
        {
            addHotkeyToSettings(selectedActionHotkey);
            const int position = m_model.rowCount();
            m_model.insertRows(position, 1, QModelIndex());
            const QModelIndex index = m_model.index(position, 0, QModelIndex());
            m_model.setData(index, QVariant::fromValue(selectedActionHotkey), CActionHotkeyListModel::ActionHotkeyRole);

            // T784, further info about the "key"/button
            CLogMessage(this).debug(u"%1, added key: '%2'") << classNameShort(this) << selectedActionHotkey.toQString(true);
        }
        this->resizeView();
    }

    void CSettingsHotkeyComponent::editEntry()
    {
        const auto index = ui->tv_Hotkeys->selectionModel()->currentIndex();
        if (!index.isValid()) { return; }

        const auto model = ui->tv_Hotkeys->model();
        const QModelIndex indexHotkey = model->index(index.row(), 0, QModelIndex());
        Q_ASSERT_X(indexHotkey.data(CActionHotkeyListModel::ActionHotkeyRole).canConvert<CActionHotkey>(), Q_FUNC_INFO, "No action hotkey");
        const CActionHotkey actionHotkey = indexHotkey.data(CActionHotkeyListModel::ActionHotkeyRole).value<CActionHotkey>();
        const CActionHotkey selectedActionHotkey = CHotkeyDialog::getActionHotkey(actionHotkey, getAllIdentifiers(), this);
        if (selectedActionHotkey.isValid() && checkAndConfirmConflicts(selectedActionHotkey, { actionHotkey }))
        {
            updateHotkeyInSettings(actionHotkey, selectedActionHotkey);
            m_model.setData(indexHotkey, QVariant::fromValue(selectedActionHotkey), CActionHotkeyListModel::ActionHotkeyRole);

            // T784, further info about the "key"/button
            CLogMessage(this).debug(u"%1, edited key: '%2'") << classNameShort(this) << selectedActionHotkey.toQString(true);
        }
        this->resizeView();
    }

    void CSettingsHotkeyComponent::removeEntry()
    {
        const QModelIndexList indexes = ui->tv_Hotkeys->selectionModel()->selectedRows();
        for (const auto &index : indexes)
        {
            const CActionHotkey actionHotkey = index.data(CActionHotkeyListModel::ActionHotkeyRole).value<CActionHotkey>();
            removeHotkeyFromSettings(actionHotkey);
            m_model.removeRows(index.row(), 1, QModelIndex());
        }
        this->resizeView();
    }

    void CSettingsHotkeyComponent::addHotkeyToSettings(const CActionHotkey &actionHotkey)
    {
        CActionHotkeyList actionHotkeyList(m_actionHotkeys.getThreadLocal());
        actionHotkeyList.push_back(actionHotkey);
        m_actionHotkeys.set(actionHotkeyList);
    }

    void CSettingsHotkeyComponent::updateHotkeyInSettings(const CActionHotkey &oldValue, const CActionHotkey &newValue)
    {
        //! \todo KB 2020-06 since there are many users reporting issues with replacing hotkey we use remove/add
        this->removeHotkeyFromSettings(oldValue);
        this->addHotkeyToSettings(newValue);

        // CActionHotkeyList actionHotkeyList(m_actionHotkeys.getThreadLocal());
        // actionHotkeyList.replace(oldValue, newValue);
        // m_actionHotkeys.set(actionHotkeyList);
    }

    void CSettingsHotkeyComponent::removeHotkeyFromSettings(const CActionHotkey &actionHotkey)
    {
        CActionHotkeyList actionHotkeyList(m_actionHotkeys.getThreadLocal());
        actionHotkeyList.remove(actionHotkey);
        m_actionHotkeys.set(actionHotkeyList);
    }

    bool CSettingsHotkeyComponent::checkAndConfirmConflicts(const CActionHotkey &actionHotkey, const CActionHotkeyList &ignore)
    {
        // check the hotkeys of the same machine only
        // and avoid duplicates (replace or add)
        const CActionHotkeyList configuredHotkeysSameMachine = m_actionHotkeys.getThreadLocal().findBySameMachine(actionHotkey);
        CActionHotkeyList conflicts = configuredHotkeysSameMachine.findSupersetsOf(actionHotkey);
        conflicts.replaceOrAdd(configuredHotkeysSameMachine.findSubsetsOf(actionHotkey));
        conflicts.removeIfIn(ignore.findBySameMachine(actionHotkey));

        if (!conflicts.isEmpty())
        {
            QString message = QStringLiteral("The selected combination conflicts with the following %1 combination(s):\n\n").arg(conflicts.size());
            for (const CActionHotkey &conflict : conflicts)
            {
                message += conflict.toQString();
                message += "\n";
            }
            message += "\n Do you want to use it anway?";
            const auto reply = QMessageBox::warning(this, "SettingsHotkeyComponent", message, QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
            if (reply == QMessageBox::No) { return false; }
        }
        return true;
    }

    void CSettingsHotkeyComponent::reloadHotkeysFromSettings()
    {
        const CActionHotkeyList hotkeys = m_actionHotkeys.getThreadLocal();
        m_model.clear();

        // list of all defined hotkeys (not the dialog)
        for (const CActionHotkey &hotkey : hotkeys)
        {
            const int position = m_model.rowCount();
            m_model.insertRows(position, 1, QModelIndex());
            const QModelIndex index = m_model.index(position, 0, QModelIndex());
            m_model.setData(index, QVariant::fromValue(hotkey), CActionHotkeyListModel::ActionHotkeyRole);
        }
        this->resizeView();
    }

    CIdentifierList CSettingsHotkeyComponent::getAllIdentifiers() const
    {
        CIdentifierList identifiers;
        if (!sGui || !sGui->getIContextApplication()) { return identifiers; }
        if (sGui->getIContextApplication()) { identifiers = sGui->getIContextApplication()->getRegisteredApplications(); }

        // add local application
        identifiers.push_back(CIdentifier("local identifer for hotkeys"));
        return identifiers;
    }

    void CSettingsHotkeyComponent::resizeView()
    {
        ui->tv_Hotkeys->resizeRowsToContents();
    }

    void CSettingsHotkeyComponent::hotkeySlot(bool keyDown)
    {
        if (keyDown)
        {
            QMessageBox *msgBox = new QMessageBox(this);
            msgBox->setAttribute(Qt::WA_DeleteOnClose);
            msgBox->setStandardButtons(QMessageBox::Ok);
            msgBox->setWindowTitle("Test");
            msgBox->setText("Hotkey test");
            msgBox->setIcon(QMessageBox::Information);
            msgBox->setModal(false);
            msgBox->open();
        }
    }

    bool CConfigHotkeyWizardPage::validatePage()
    {
        Q_ASSERT_X(m_config, Q_FUNC_INFO, "Missing configuration");
        if (CConfigurationWizard::lastWizardStepSkipped(this->wizard())) { return true; }
        m_config->saveSettings();
        return true;
    }

    void CConfigHotkeyWizardPage::initializePage()
    {
        Q_ASSERT_X(m_config, Q_FUNC_INFO, "Missing configuration");
        m_config->reloadHotkeysFromSettings();
    }
} // ns
