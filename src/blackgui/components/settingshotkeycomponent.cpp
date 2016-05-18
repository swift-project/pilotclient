/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/contextapplication.h"
#include "blackgui/components/hotkeydialog.h"
#include "blackgui/components/settingshotkeycomponent.h"
#include "blackgui/guiapplication.h"
#include "blackmisc/identifier.h"
#include "blackmisc/identifierlist.h"
#include "blackmisc/input/hotkeycombination.h"
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

namespace BlackGui
{
    namespace Components
    {
        CSettingsHotkeyComponent::CSettingsHotkeyComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CSettingsHotkeyComponent)
        {
            ui->setupUi(this);
            ui->tv_hotkeys->setModel(&m_model);

            connect(ui->pb_addHotkey, &QPushButton::clicked, this, &CSettingsHotkeyComponent::ps_addEntry);
            connect(ui->pb_editHotkey, &QPushButton::clicked, this, &CSettingsHotkeyComponent::ps_editEntry);
            connect(ui->pb_removeHotkey, &QPushButton::clicked, this, &CSettingsHotkeyComponent::ps_removeEntry);

            ui->tv_hotkeys->selectRow(0);
        }

        CSettingsHotkeyComponent::~CSettingsHotkeyComponent()
        {
        }

        void CSettingsHotkeyComponent::ps_addEntry()
        {
            BlackMisc::CIdentifierList registeredApps;
            if (sGui->getIContextApplication()) registeredApps = sGui->getIContextApplication()->getRegisteredApplications();
            // add local application
            registeredApps.push_back(CIdentifier());
            auto selectedActionHotkey = CHotkeyDialog::getActionHotkey(CActionHotkey(), registeredApps, this);
            if (selectedActionHotkey.isValid() && checkAndConfirmConflicts(selectedActionHotkey))
            {
                addHotkeytoSettings(selectedActionHotkey);
                int position = m_model.rowCount();
                m_model.insertRows(position, 1, QModelIndex());
                QModelIndex index = m_model.index(position, 0, QModelIndex());
                m_model.setData(index, QVariant::fromValue(selectedActionHotkey), CActionHotkeyListModel::ActionHotkeyRole);
            }
        }

        void CSettingsHotkeyComponent::ps_editEntry()
        {
            auto index = ui->tv_hotkeys->selectionModel()->currentIndex();
            if (!index.isValid()) return;

            const auto model = ui->tv_hotkeys->model();
            const QModelIndex indexHotkey = model->index(index.row(), 0, QModelIndex());
            Q_ASSERT(indexHotkey.data(CActionHotkeyListModel::ActionHotkeyRole).canConvert<CActionHotkey>());
            CActionHotkey actionHotkey = indexHotkey.data(CActionHotkeyListModel::ActionHotkeyRole).value<CActionHotkey>();
            BlackMisc::CIdentifierList registeredApps;
            if (sGui->getIContextApplication()) registeredApps = sGui->getIContextApplication()->getRegisteredApplications();
            // add local application
            registeredApps.push_back(CIdentifier());
            auto selectedActionHotkey = CHotkeyDialog::getActionHotkey(actionHotkey, registeredApps, this);
            if (selectedActionHotkey.isValid() && checkAndConfirmConflicts(selectedActionHotkey, { actionHotkey }))
            {
                updateHotkeyInSettings(actionHotkey, selectedActionHotkey);
                m_model.setData(indexHotkey, QVariant::fromValue(selectedActionHotkey), CActionHotkeyListModel::ActionHotkeyRole);
            }
        }

        void CSettingsHotkeyComponent::ps_removeEntry()
        {
            QModelIndexList indexes = ui->tv_hotkeys->selectionModel()->selectedRows();
            for (const auto &index : indexes)
            {
                CActionHotkey actionHotkey = index.data(CActionHotkeyListModel::ActionHotkeyRole).value<CActionHotkey>();
                removeHotkeyFromSettings(actionHotkey);
                m_model.removeRows(index.row(), 1, QModelIndex());
            }
        }

        void CSettingsHotkeyComponent::addHotkeytoSettings(const CActionHotkey &actionHotkey)
        {
            CActionHotkeyList actionHotkeyList(m_actionHotkeys.get());
            actionHotkeyList.push_back(actionHotkey);
            m_actionHotkeys.set(actionHotkeyList);
        }

        void CSettingsHotkeyComponent::updateHotkeyInSettings(const CActionHotkey &oldValue, const CActionHotkey &newValue)
        {
            CActionHotkeyList actionHotkeyList(m_actionHotkeys.get());
            actionHotkeyList.replace(oldValue, newValue);
            m_actionHotkeys.set(actionHotkeyList);
        }

        void CSettingsHotkeyComponent::removeHotkeyFromSettings(const CActionHotkey &actionHotkey)
        {
            CActionHotkeyList actionHotkeyList(m_actionHotkeys.get());
            actionHotkeyList.remove(actionHotkey);
            m_actionHotkeys.set(actionHotkeyList);
        }

        bool CSettingsHotkeyComponent::checkAndConfirmConflicts(const CActionHotkey &actionHotkey, const CActionHotkeyList &ignore)
        {
            auto configuredHotkeys = m_actionHotkeys.get();
            CActionHotkeyList conflicts = configuredHotkeys.findSupersetsOf(actionHotkey);
            conflicts.push_back(configuredHotkeys.findSubsetsOf(actionHotkey));
            conflicts.removeIfIn(ignore);

            if (!conflicts.isEmpty())
            {
                QString message = QString("The selected combination conflicts with the following %1 combinations(s):\n\n").arg(conflicts.size());
                for (const auto &conflict : conflicts)
                {
                    message += conflict.getCombination().toQString();
                    message += "\n";
                }
                message += "\n Do you want to use it anway?";
                auto reply = QMessageBox::warning(this, "SettingsHotkeyComponent",
                                                  message,
                                                  QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
                if (reply == QMessageBox::No) { return false; }
            }
            return true;
        }

        void CSettingsHotkeyComponent::ps_hotkeySlot(bool keyDown)
        {
            if (keyDown) QMessageBox::information(this, "Test", "Push-To-Talk");
        }
    } // ns
} // ns
