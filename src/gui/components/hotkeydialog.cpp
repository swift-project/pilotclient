// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/components/hotkeydialog.h"
#include "gui/guiapplication.h"
#include "gui/stylesheetutility.h"
#include "core/context/contextapplication.h"
#include "core/inputmanager.h"
#include "misc/icons.h"
#include "misc/identifier.h"
#include "misc/input/hotkeycombination.h"
#include "misc/input/keyboardkeylist.h"
#include "misc/logmessage.h"
#include "misc/statusmessage.h"
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

using namespace swift::misc;
using namespace swift::misc::input;
using namespace swift::core;
using namespace swift::gui::models;

namespace swift::gui::components
{
    CHotkeyDialog::CHotkeyDialog(const CActionHotkey &actionHotkey, const CIdentifierList &identifiers, QWidget *parent) : QDialog(parent),
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

        CHotkeyCombination combination = actionHotkey.getCombination();
        if (!combination.isEmpty())
        {
            ui->pb_SelectedHotkey->setText(combination.toQString());
            ui->pb_SelectedHotkey->setToolTip(combination.asStringWithDeviceNames());
        }
        else
        {
            ui->pb_SelectedHotkey->setToolTip("Press to select an new combination...");
        }

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
        connect(ui->pb_SelectedHotkey, &QPushButton::clicked, this, &CHotkeyDialog::captureHotkey);
        connect(ui->pb_Accept, &QPushButton::clicked, this, &CHotkeyDialog::accept);
        connect(ui->pb_Cancel, &QPushButton::clicked, this, &CHotkeyDialog::reject);
        connect(ui->tv_Actions->selectionModel(), &QItemSelectionModel::selectionChanged, this, &CHotkeyDialog::changeSelectedAction);
        connect(ui->cb_Identifier, qOverload<int>(&QComboBox::currentIndexChanged), this, &CHotkeyDialog::changeApplicableMachine);

        if (sGui && sGui->getInputManager())
        {
            connect(sGui->getInputManager(), &swift::core::CInputManager::combinationSelectionChanged, this, &CHotkeyDialog::combinationSelectionChanged);
            connect(sGui->getInputManager(), &swift::core::CInputManager::combinationSelectionFinished, this, &CHotkeyDialog::combinationSelectionFinished);
        }

        initStyleSheet();
    }

    CHotkeyDialog::~CHotkeyDialog()
    {}

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
            { CStyleSheetUtility::fileNameFonts(),
              CStyleSheetUtility::fileNameStandardWidget() });
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
        // if (m_actionHotkey.getCombination().isEmpty()) return;
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

    void CHotkeyDialog::captureHotkey()
    {
        if (!sApp || sApp->isShuttingDown()) { return; }
        ui->pb_SelectedHotkey->setText("Press any key/button...");
        ui->pb_SelectedHotkey->setToolTip({});
        sApp->getInputManager()->startCapture();
    }

    void CHotkeyDialog::combinationSelectionChanged(const CHotkeyCombination &combination)
    {
        ui->pb_SelectedHotkey->setText(combination.toQString());
        ui->pb_SelectedHotkey->setToolTip(combination.asStringWithDeviceNames());
    }

    void CHotkeyDialog::combinationSelectionFinished(const CHotkeyCombination &combination)
    {
        m_actionHotkey.setCombination(combination);
        synchronize();
    }

    void CHotkeyDialog::changeSelectedAction(const QItemSelection &selected, const QItemSelection &deselected)
    {
        Q_UNUSED(deselected)
        if (selected.indexes().isEmpty()) { return; }
        const auto index = selected.indexes().first();
        m_actionHotkey.setAction(index.data(CActionModel::ActionRole).toString());
    }

    CKeySelectionBox *CHotkeyDialog::addSelectionBox(const CKeyboardKeyList &allSupportedKeys, const CKeyboardKey &keyboardKey)
    {
        int currentIndex = 0;
        const int width = qRound(1.5 * this->width());
        const bool select = !keyboardKey.isUnknown();

        CKeySelectionBox *ksb = new CKeySelectionBox(ui->qf_Advanced);
        ksb->addItem(noKeyButton(), QVariant::fromValue(CKeyboardKey())); // at front
        for (const CKeyboardKey &supportedKey : allSupportedKeys)
        {
            ksb->addItem(supportedKey.toQString(), QVariant::fromValue(supportedKey));
            if (select && supportedKey == keyboardKey)
            {
                currentIndex = ksb->count() - 1;
            }
        }

        ksb->setSelectedIndex(currentIndex);
        ksb->setPopupWidth(qMin(width, 600));
        ksb->addItem(noKeyButton(), QVariant::fromValue(CKeyboardKey())); // at back (easier to find it is there twice)

        ui->qf_Advanced->layout()->addWidget(ksb);
        const int position = ui->qf_Advanced->layout()->count() - 1;
        ksb->setProperty("position", position);
        connect(ksb, &CKeySelectionBox::keySelectionChanged, this, &CHotkeyDialog::advancedKeyChanged);

        return ksb;
    }

    CKeySelectionBox *CHotkeyDialog::addSelectionBox(const CJoystickButtonList &allAvailableButtons, const CJoystickButton &joystickButton)
    {
        int currentIndex = -1;
        const int width = qRound(1.5 * this->width());

        CKeySelectionBox *ksb = new CKeySelectionBox(ui->qf_Advanced);
        ksb->addItem(noKeyButton(), QVariant::fromValue(CJoystickButton())); // at front
        for (const CJoystickButton &availableButton : allAvailableButtons)
        {
            ksb->addItem(availableButton.toQString(), QVariant::fromValue(availableButton));
            if (availableButton == joystickButton)
            {
                currentIndex = ksb->count() - 1;
                ksb->setToolTip(joystickButton.getButtonAsStringWithDeviceName());
            }
        }

        ksb->setSelectedIndex(currentIndex);
        ksb->setPopupWidth(qMin(width, 600));
        ksb->addItem(noKeyButton(), QVariant::fromValue(CJoystickButton())); // at back (easier to find it is there twice)

        ui->qf_Advanced->layout()->addWidget(ksb);
        const int position = ui->qf_Advanced->layout()->count() - 1;
        ksb->setProperty("position", position);
        connect(ksb, &CKeySelectionBox::keySelectionChanged, this, &CHotkeyDialog::advancedKeyChanged);

        return ksb;
    }

    void CHotkeyDialog::changeApplicableMachine(int index)
    {
        Q_UNUSED(index)
        const QVariant userData = ui->cb_Identifier->currentData();
        Q_ASSERT(userData.canConvert<CIdentifier>());
        m_actionHotkey.setApplicableMachine(userData.value<CIdentifier>());
    }

    void CHotkeyDialog::accept()
    {
        if (m_actionHotkey.getApplicableMachine().getMachineName().isEmpty())
        {
            CLogMessage(this).validationWarning(u"Missing hotkey '%1'") << ui->gb_Machine->title();
            return;
        }

        if (m_actionHotkey.getCombination().isEmpty())
        {
            CLogMessage(this).validationWarning(u"Missing hotkey '%1'") << ui->gb_Hotkey->title();
            return;
        }

        if (m_actionHotkey.getAction().isEmpty())
        {
            CLogMessage(this).validationWarning(u"Missing hotkey '%1'") << ui->gb_Action->title();
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
        CHotkeyCombination combination = m_actionHotkey.getCombination();
        ui->pb_SelectedHotkey->setText(combination.toQString());
        ui->pb_SelectedHotkey->setToolTip(combination.asStringWithDeviceNames());
    }

    void CHotkeyDialog::synchronizeAdvancedSelection()
    {
        if (ui->qf_Advanced->isVisible()) { setupAdvancedFrame(); }
    }

    void CHotkeyDialog::setupAdvancedFrame()
    {
        if (!sGui || sGui->isShuttingDown()) { return; }
        this->clearAdvancedFrame();
        const CKeyboardKeyList allSupportedKeys = CKeyboardKeyList::allSupportedKeys();
        const CJoystickButtonList allAvailableButtons = sGui->getInputManager()->getAllAvailableJoystickButtons();

        const CKeyboardKeyList keyboardKeys = m_actionHotkey.getCombination().getKeyboardKeys();
        int c = 0;

        for (const CKeyboardKey &keyboardKey : keyboardKeys)
        {
            this->addSelectionBox(allSupportedKeys, keyboardKey);
            c++;
        }

        const CJoystickButtonList joystickButtons = m_actionHotkey.getCombination().getJoystickButtons();
        for (const CJoystickButton &joystickButton : joystickButtons)
        {
            this->addSelectionBox(allAvailableButtons, joystickButton);
            c++;
        }

        // add one box more so we can add keys/buttons
        if (c < 2)
        {
            this->addSelectionBox(allSupportedKeys);
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
        const CKeySelectionBox *ksb = qobject_cast<CKeySelectionBox *>(sender());
        Q_ASSERT(ksb);

        if (ksb->itemData(oldIndex).canConvert<CKeyboardKey>() && ksb->itemData(newIndex).canConvert<CKeyboardKey>())
        {
            CKeyboardKey oldKey = ksb->itemData(oldIndex).value<CKeyboardKey>();
            CKeyboardKey newKey = ksb->itemData(newIndex).value<CKeyboardKey>();

            CHotkeyCombination combination = m_actionHotkey.getCombination();
            if (newKey.isUnknown()) { combination.removeKeyboardKey(oldKey); }
            else { combination.replaceKey(oldKey, newKey); }
            m_actionHotkey.setCombination(combination);
        }

        if (ksb->itemData(oldIndex).canConvert<CJoystickButton>() && ksb->itemData(newIndex).canConvert<CJoystickButton>())
        {
            CJoystickButton oldButton = ksb->itemData(oldIndex).value<CJoystickButton>();
            CJoystickButton newButton = ksb->itemData(newIndex).value<CJoystickButton>();

            CHotkeyCombination combination = m_actionHotkey.getCombination();
            if (!newButton.isValid()) { combination.removeJoystickButton(oldButton); }
            else { combination.replaceButton(oldButton, newButton); }
            m_actionHotkey.setCombination(combination);
        }

        ui->pb_SelectedHotkey->setText(m_actionHotkey.getCombination().toQString());
        ui->pb_SelectedHotkey->setToolTip(m_actionHotkey.getCombination().asStringWithDeviceNames());
    }

    void CHotkeyDialog::selectAction()
    {
        if (m_actionHotkey.getAction().isEmpty()) { return; }
        const QStringList tokens = m_actionHotkey.getAction().split("/", Qt::SkipEmptyParts);
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

    const QString &CHotkeyDialog::noKeyButton()
    {
        static const QString k = "[none]";
        return k;
    }
} // ns
