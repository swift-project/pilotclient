#include "blackcore/inputmanager.h"
#include "blackgui/components/hotkeydialog.h"
#include "blackgui/guiapplication.h"
#include "blackgui/stylesheetutility.h"
#include "blackmisc/icons.h"
#include "blackmisc/identifier.h"
#include "blackmisc/input/hotkeycombination.h"
#include "blackmisc/input/keyboardkey.h"
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

namespace BlackGui
{
    namespace Components
    {
        CKeySelectionBox::CKeySelectionBox(QWidget *parent) :
            QComboBox(parent)
        {
            connect(this, static_cast<void(CKeySelectionBox::*)(int)>(&CKeySelectionBox::currentIndexChanged), this, &CKeySelectionBox::ps_updateSelectedIndex);
        }

        void CKeySelectionBox::setSelectedIndex(int index)
        {
            m_oldIndex = index;
            setCurrentIndex(m_oldIndex);
        }

        void CKeySelectionBox::ps_updateSelectedIndex(int index)
        {
            emit keySelectionChanged(m_oldIndex, index);
            m_oldIndex = index;
        }

        CHotkeyDialog::CHotkeyDialog(const BlackMisc::Input::CActionHotkey &actionHotkey, QWidget *parent) :
            QDialog(parent),
            ui(new Ui::CHotkeyDialog),
            m_actionHotkey(actionHotkey),
            m_actionModel(this)
        {
            m_inputManager = BlackCore::CInputManager::instance();

            ui->setupUi(this);
            ui->advancedFrame->hide();

            ui->tv_actions->setModel(&m_actionModel);
            ui->pb_advancedMode->setIcon(BlackMisc::CIcons::arrowMediumSouth16());
            selectAction();

            if (!actionHotkey.getCombination().isEmpty()) { ui->pb_selectedHotkey->setText(actionHotkey.getCombination().toQString()); }

            connect(ui->pb_advancedMode, &QPushButton::clicked, this, &CHotkeyDialog::ps_advancedModeChanged);
            connect(ui->pb_selectedHotkey, &QPushButton::clicked, this, &CHotkeyDialog::ps_selectHotkey);
            connect(ui->pb_accept, &QPushButton::clicked, this, &CHotkeyDialog::ps_accept);
            connect(ui->pb_cancel, &QPushButton::clicked, this, &CHotkeyDialog::reject);
            connect(m_inputManager, &BlackCore::CInputManager::combinationSelectionChanged, this, &CHotkeyDialog::ps_combinationSelectionChanged);
            connect(m_inputManager, &BlackCore::CInputManager::combinationSelectionFinished, this, &CHotkeyDialog::ps_combinationSelectionFinished);
            connect(ui->tv_actions->selectionModel(), &QItemSelectionModel::selectionChanged, this, &CHotkeyDialog::ps_changeSelectedAction);

            initStyleSheet();
        }

        CHotkeyDialog::~CHotkeyDialog()
        {
        }

        void CHotkeyDialog::setRegisteredApplications(const BlackMisc::CIdentifierList &applications)
        {
            for (const auto &app : applications)
            {
                ui->cb_identifier->addItem(app.getMachineName(), QVariant::fromValue(app));
            }
        }

        void CHotkeyDialog::initStyleSheet()
        {
            const QString s = sGui->getStyleSheetUtility().styles(
            {
                CStyleSheetUtility::fileNameFonts(),
                CStyleSheetUtility::fileNameStandardWidget()
            }
            );
            setStyleSheet(s);
        }

        CActionHotkey CHotkeyDialog::getActionHotkey(const CActionHotkey &initial, const CIdentifierList &applications, QWidget *parent)
        {
            CHotkeyDialog editDialog(initial, parent);
            editDialog.setWindowModality(Qt::WindowModal);
            // add local application
            editDialog.setRegisteredApplications(applications);
            if (editDialog.exec()) { return editDialog.getSelectedActionHotkey(); }
            else { return {}; }

        }

        void CHotkeyDialog::ps_advancedModeChanged()
        {
            if (m_actionHotkey.getCombination().isEmpty()) return;

            if (!ui->advancedFrame->isVisible())
            {
                setupAdvancedFrame();
                ui->advancedFrame->show();
                ui->pb_advancedMode->setIcon(BlackMisc::CIcons::arrowMediumNorth16());
            }
            else
            {
                ui->pb_advancedMode->setIcon(BlackMisc::CIcons::arrowMediumSouth16());
                ui->advancedFrame->hide();
                ui->gb_hotkey->resize(0, 0);
            }
        }

        void CHotkeyDialog::ps_selectHotkey()
        {
            ui->pb_selectedHotkey->setText("Press any key/button...");
            m_inputManager->startCapture();
        }

        void CHotkeyDialog::ps_combinationSelectionChanged(const BlackMisc::Input::CHotkeyCombination &combination)
        {
            ui->pb_selectedHotkey->setText(combination.toFormattedQString());
        }

        void CHotkeyDialog::ps_combinationSelectionFinished(const BlackMisc::Input::CHotkeyCombination &combination)
        {
            m_actionHotkey.setCombination(combination);
            synchronize();
        }

        void CHotkeyDialog::ps_changeSelectedAction(const QItemSelection &selected, const QItemSelection &deselected)
        {
            Q_UNUSED(deselected);
            const auto index = selected.indexes().first();
            m_actionHotkey.setAction(index.data(Models::CActionModel::ActionRole).toString());
        }

        void CHotkeyDialog::ps_accept()
        {
            if (m_actionHotkey.getApplicableMachine().getMachineName().isEmpty())
            {
                CLogMessage().validationWarning("Missing %1") << ui->gb_machine->title();
                return;
            }

            if (m_actionHotkey.getCombination().isEmpty())
            {
                CLogMessage().validationWarning("Missing %1") << ui->gb_hotkey->title();
                return;
            }

            if (m_actionHotkey.getAction().isEmpty())
            {
                CLogMessage().validationWarning("Missing %1") << ui->gb_action->title();
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
            ui->pb_selectedHotkey->setText(m_actionHotkey.getCombination().toFormattedQString());
        }

        void CHotkeyDialog::synchronizeAdvancedSelection()
        {
            if (ui->advancedFrame->isVisible()) { setupAdvancedFrame(); }
        }

        void CHotkeyDialog::setupAdvancedFrame()
        {
            clearAdvancedFrame();
            auto allSupportedKeys = CKeyboardKeyList::allSupportedKeys();

            QStringList splittedKeys = m_actionHotkey.getCombination().toQString().split('+', QString::SkipEmptyParts);
            for (const auto &splittedKey : splittedKeys)
            {
                if (splittedKey == "+") continue;

                int currentIndex = -1;
                CKeySelectionBox *ksb = new CKeySelectionBox(ui->advancedFrame);
                for (const auto &supportedKey : allSupportedKeys)
                {
                    QString supportedKeyAsString = supportedKey.toQString();
                    ksb->addItem(supportedKeyAsString, QVariant::fromValue(supportedKey));
                    if (supportedKeyAsString == splittedKey)
                    {
                        currentIndex = ksb->count() - 1;
                    }
                }
                ksb->setSelectedIndex(currentIndex);
                ui->advancedFrame->layout()->addWidget(ksb);
                int position = ui->advancedFrame->layout()->count() - 1;
                ksb->setProperty("position", position);
                connect(ksb, &CKeySelectionBox::keySelectionChanged, this, &CHotkeyDialog::advancedKeyChanged);
            }
        }

        void CHotkeyDialog::clearAdvancedFrame()
        {
            QLayout *layout = ui->advancedFrame->layout();
            QLayoutItem *child;

            while ((child = layout->takeAt(0)) != 0)
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

            auto combination = m_actionHotkey.getCombination();
            combination.replaceKey(oldKey, newKey);
            m_actionHotkey.setCombination(combination);
            synchronize();
        }

        void CHotkeyDialog::selectAction()
        {
            if (m_actionHotkey.getAction().isEmpty()) return;

            const auto tokens = m_actionHotkey.getAction().split("/", QString::SkipEmptyParts);
            QModelIndex parentIndex = QModelIndex();

            for (const auto &token : tokens)
            {
                QModelIndex startIndex = m_actionModel.index(0, 0, parentIndex);
                auto indexList = m_actionModel.match(startIndex, Qt::DisplayRole, QVariant::fromValue(token));
                if (indexList.isEmpty()) return;
                parentIndex = indexList.first();
                ui->tv_actions->expand(parentIndex);
            }

            QItemSelectionModel *selectionModel = ui->tv_actions->selectionModel();
            selectionModel->select(parentIndex, QItemSelectionModel::Select);
        }
    } // ns
} // ns
