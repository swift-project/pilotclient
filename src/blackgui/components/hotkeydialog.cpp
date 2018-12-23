/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/components/hotkeydialog.h"
#include "blackgui/guiapplication.h"
#include "blackgui/stylesheetutility.h"
#include "blackcore/context/contextapplication.h"
#include "blackcore/inputmanager.h"
#include "blackmisc/icons.h"
#include "blackmisc/identifier.h"
#include "blackmisc/input/hotkeycombination.h"
#include "blackmisc/input/keyboardkeylist.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/statusmessage.h"
#include "ui_hotkeydialog.h"

#include <QFrame>
#include <QGroupBox>
#include <QItemSelection>
#include <QItemSelectionModel>
#include <QLayout>
#include <QLayoutItem>
#include <QList>
#include <QModelIndex>
#include <QModelIndexList>
#include <QPushButton>
#include <QString>
#include <QStringList>
#include <QTreeView>
#include <QVariant>
#include <QWidget>
#include <Qt>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::Input;
using namespace BlackCore;

namespace BlackGui
{
    namespace Components
    {
        CHotkeyDialog::CHotkeyDialog(const CActionHotkey &actionHotkey, const CIdentifierList &identifiers, QWidget *parent) :
            QDialog(parent),
            ui(new Ui::CHotkeyDialog),
            m_actionHotkey(actionHotkey),
            m_actionModel(this)
        {
            setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

            ui->setupUi(this);
            ui->qf_Advanced->hide();

            ui->pb_AdvancedMode->setIcon(CIcons::arrowMediumSouth16());
            ui->tv_Actions->setModel(&m_actionModel);
            selectAction();

            if (!actionHotkey.getCombination().isEmpty()) { ui->pb_SelectedHotkey->setText(actionHotkey.getCombination().toQString()); }

            // get all remote identifiers in case there is no key for a remote machine yet
            CIdentifierList registeredApplications;
            if (sGui && sGui->getIContextApplication())
            {
                registeredApplications = sGui->getIContextApplication()->getRegisteredApplications();
                const CIdentifier appIdentifier = sGui->getIContextApplication()->getApplicationIdentifier();
                registeredApplications.push_back(appIdentifier);
            }

            CIdentifierList machineIdentifiers(identifiers);
            machineIdentifiers.push_back(registeredApplications); // add the registered applications

            if (actionHotkey.isValid()) { machineIdentifiers.push_back(actionHotkey.getApplicableMachine()); }
            const CIdentifierList machineIdentifiersUnique = machineIdentifiers.getMachinesUnique();

            int index = -1;
            for (const CIdentifier &app : machineIdentifiersUnique)
            {
                ui->cb_Identifier->addItem(app.getMachineName(), QVariant::fromValue(app));
                if (m_actionHotkey.getApplicableMachine().hasSameMachineName(app)) { index = ui->cb_Identifier->count() - 1; }
            }

            if (index < 0 && ui->cb_Identifier->count() > 0)
            {
                // if nothing was found
                ui->cb_Identifier->setCurrentIndex(0);
            }
            else if (index != ui->cb_Identifier->currentIndex())
            {
                ui->cb_Identifier->setCurrentIndex(index);
            }

            connect(ui->pb_AdvancedMode, &QPushButton::clicked, this, &CHotkeyDialog::advancedModeChanged);
            connect(ui->pb_SelectedHotkey, &QPushButton::clicked, this, &CHotkeyDialog::selectHotkey);
            connect(ui->pb_Accept, &QPushButton::clicked, this, &CHotkeyDialog::accept);
            connect(ui->pb_Cancel, &QPushButton::clicked, this, &CHotkeyDialog::reject);
            connect(ui->tv_Actions->selectionModel(), &QItemSelectionModel::selectionChanged, this, &CHotkeyDialog::changeSelectedAction);
            connect(ui->cb_Identifier, qOverload<int>(&QComboBox::currentIndexChanged), this, &CHotkeyDialog::changeApplicableMachine);

            if (sGui)
            {
                connect(sGui->getInputManager(), &BlackCore::CInputManager::combinationSelectionChanged, this, &CHotkeyDialog::combinationSelectionChanged);
                connect(sGui->getInputManager(), &BlackCore::CInputManager::combinationSelectionFinished, this, &CHotkeyDialog::combinationSelectionFinished);
            }

            initStyleSheet();
        }

        CHotkeyDialog::~CHotkeyDialog()
        { }

        CKeySelectionBox::CKeySelectionBox(QWidget *parent) : CHorizontalComboBox(parent)
        {
            connect(this, qOverload<int>(&CKeySelectionBox::currentIndexChanged), this, &CKeySelectionBox::updateSelectedIndex);
        }

        void CKeySelectionBox::setSelectedIndex(int index)
        {
            m_oldIndex = index;
            setCurrentIndex(m_oldIndex);
        }

        void CKeySelectionBox::updateSelectedIndex(int index)
        {
            emit keySelectionChanged(m_oldIndex, index);
            m_oldIndex = index;
        }

        void CHotkeyDialog::initStyleSheet()
        {
            if (!sGui) { return; }
            const QString s = sGui->getStyleSheetUtility().styles(
            {
                CStyleSheetUtility::fileNameFonts(),
                CStyleSheetUtility::fileNameStandardWidget()
            }
            );
            this->setStyleSheet(s);
        }

        CActionHotkey CHotkeyDialog::getActionHotkey(const CActionHotkey &initial, const CIdentifierList &identifiers, QWidget *parent)
        {
            CHotkeyDialog editDialog(initial, identifiers, parent);
            editDialog.setWindowModality(Qt::WindowModal);
            if (editDialog.exec()) { return editDialog.getSelectedActionHotkey(); }
            return {};
        }

        void CHotkeyDialog::advancedModeChanged()
        {
            if (m_actionHotkey.getCombination().isEmpty()) return;
            if (!ui->qf_Advanced->isVisible())
            {
                setupAdvancedFrame();
                ui->qf_Advanced->show();
                ui->pb_AdvancedMode->setIcon(CIcons::arrowMediumNorth16());
            }
            else
            {
                ui->pb_AdvancedMode->setIcon(CIcons::arrowMediumSouth16());
                ui->qf_Advanced->hide();
                ui->gb_Hotkey->resize(0, 0);
            }
        }

        void CHotkeyDialog::selectHotkey()
        {
            ui->pb_SelectedHotkey->setText("Press any key/button...");
            sApp->getInputManager()->startCapture();
        }

        void CHotkeyDialog::combinationSelectionChanged(const CHotkeyCombination &combination)
        {
            ui->pb_SelectedHotkey->setText(combination.toQString());
        }

        void CHotkeyDialog::combinationSelectionFinished(const CHotkeyCombination &combination)
        {
            m_actionHotkey.setCombination(combination);
            synchronize();
        }

        void CHotkeyDialog::changeSelectedAction(const QItemSelection &selected, const QItemSelection &deselected)
        {
            Q_UNUSED(deselected);
            if (selected.indexes().isEmpty()) { return; }
            const auto index = selected.indexes().first();
            m_actionHotkey.setAction(index.data(Models::CActionModel::ActionRole).toString());
        }

        void CHotkeyDialog::changeApplicableMachine(int index)
        {
            Q_UNUSED(index);
            const QVariant userData = ui->cb_Identifier->currentData();
            Q_ASSERT(userData.canConvert<CIdentifier>());
            m_actionHotkey.setApplicableMachine(userData.value<CIdentifier>());
        }

        void CHotkeyDialog::accept()
        {
            if (m_actionHotkey.getApplicableMachine().getMachineName().isEmpty())
            {
                CLogMessage().validationWarning(u"Missing hotkey '%1'") << ui->gb_Machine->title();
                return;
            }

            if (m_actionHotkey.getCombination().isEmpty())
            {
                CLogMessage().validationWarning(u"Missing hotkey '%1'") << ui->gb_Hotkey->title();
                return;
            }

            if (m_actionHotkey.getAction().isEmpty())
            {
                CLogMessage().validationWarning(u"Missing hotkey '%1'") << ui->gb_Action->title();
                return;
            }

            QDialog::accept();
        }

        void CHotkeyDialog::synchronize()
        {
            synchronizeSimpleSelection();
            synchronizeAdvancedSelection();
        }

        void CHotkeyDialog::synchronizeSimpleSelection()
        {
            ui->pb_SelectedHotkey->setText(m_actionHotkey.getCombination().toQString());
        }

        void CHotkeyDialog::synchronizeAdvancedSelection()
        {
            if (ui->qf_Advanced->isVisible()) { setupAdvancedFrame(); }
        }

        void CHotkeyDialog::setupAdvancedFrame()
        {
            this->clearAdvancedFrame();
            const CKeyboardKeyList allSupportedKeys = CKeyboardKeyList::allSupportedKeys();

            const QStringList splitKeys = m_actionHotkey.getCombination().toQString().split('+', QString::SkipEmptyParts);
            for (const QString &splitKey : splitKeys)
            {
                if (splitKey == "+") continue;

                int currentIndex = -1;
                CKeySelectionBox *ksb = new CKeySelectionBox(ui->qf_Advanced);
                for (const CKeyboardKey &supportedKey : allSupportedKeys)
                {
                    QString supportedKeyAsString = supportedKey.toQString();
                    ksb->addItem(supportedKeyAsString, QVariant::fromValue(supportedKey));
                    if (supportedKeyAsString == splitKey)
                    {
                        currentIndex = ksb->count() - 1;
                    }
                }
                ksb->setSelectedIndex(currentIndex);
                ui->qf_Advanced->layout()->addWidget(ksb);
                const int position = ui->qf_Advanced->layout()->count() - 1;
                ksb->setProperty("position", position);
                connect(ksb, &CKeySelectionBox::keySelectionChanged, this, &CHotkeyDialog::advancedKeyChanged);

                const int width = qRound(1.5 * this->width());
                ksb->setPopupWidth(qMin(width, 600));
            }
        }

        void CHotkeyDialog::clearAdvancedFrame()
        {
            QLayout *layout = ui->qf_Advanced->layout();
            QLayoutItem *child;

            while ((child = layout->takeAt(0)) != nullptr)
            {
                if (child->widget()) child->widget()->deleteLater();
                delete child;
            }
        }

        void CHotkeyDialog::advancedKeyChanged(int oldIndex, int newIndex)
        {
            CKeySelectionBox *ksb = qobject_cast<CKeySelectionBox *>(sender());
            Q_ASSERT(ksb);
            CKeyboardKey oldKey = ksb->itemData(oldIndex).value<CKeyboardKey>();
            CKeyboardKey newKey = ksb->itemData(newIndex).value<CKeyboardKey>();

            CHotkeyCombination combination = m_actionHotkey.getCombination();
            combination.replaceKey(oldKey, newKey);
            m_actionHotkey.setCombination(combination);
            synchronize();
        }

        void CHotkeyDialog::selectAction()
        {
            if (m_actionHotkey.getAction().isEmpty()) { return; }
            const QStringList tokens = m_actionHotkey.getAction().split("/", QString::SkipEmptyParts);
            QModelIndex parentIndex = QModelIndex();

            for (const QString &token : tokens)
            {
                const QModelIndex startIndex = m_actionModel.index(0, 0, parentIndex);
                const QModelIndexList indexList = m_actionModel.match(startIndex, Qt::DisplayRole, QVariant::fromValue(token));
                if (indexList.isEmpty()) { return; }
                parentIndex = indexList.first();
                ui->tv_Actions->expand(parentIndex);
            }

            QItemSelectionModel *selectionModel = ui->tv_Actions->selectionModel();
            selectionModel->select(parentIndex, QItemSelectionModel::Select);
        }
    } // ns
} // ns
