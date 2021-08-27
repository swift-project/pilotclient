/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "applicationinfoview.h"
#include "blackmisc/fileutils.h"

#include <QMessageBox>
#include <QDir>

using namespace BlackMisc;
using namespace BlackGui::Models;
using namespace BlackGui::Menus;

namespace BlackGui
{
    namespace Views
    {
        CApplicationInfoView::CApplicationInfoView(QWidget *parent) : CViewBase(parent)
        {
            this->standardInit(new CApplicationInfoListModel(this));
            this->setCustomMenu(new CApplicationInfoMenu(this));
        }

        int CApplicationInfoView::otherSwiftVersionsFromDataDirectories(bool reinitOtherVersions)
        {
            const CApplicationInfoList others = CApplicationInfoList::fromOtherSwiftVersionsFromDataDirectories(reinitOtherVersions);
            m_acceptRowSelection = (!others.isEmpty());

            this->updateContainer(others);
            return others.size();
        }

        int CApplicationInfoView::otherSwiftVersionsFromDataDiretoriesAndResize(bool reinitOtherVersion)
        {
            const int r = this->otherSwiftVersionsFromDataDirectories(reinitOtherVersion);
            this->setPercentageColumnWidths();
            this->resizeRowsToContents();
            return r;
        }

        void CApplicationInfoView::deleteSelectedDataDirectories()
        {
            if (!this->hasSelection()) { return; }
            const QMessageBox::StandardButton reply = QMessageBox::question(this, "Delete?", "Delete selected data directories?", QMessageBox::Yes | QMessageBox::No);
            if (reply != QMessageBox::Yes) { return; }

            QStringList deletedDirectories;
            for (const CApplicationInfo &info : this->selectedObjects())
            {
                const QString d = CFileUtils::fixWindowsUncPath(info.getApplicationDataDirectory());
                QDir dir(d);
                if (!dir.exists()) { continue; }
                if (dir.removeRecursively())
                {
                    deletedDirectories << d;
                }
            }
            if (deletedDirectories.isEmpty()) { return; }
            this->otherSwiftVersionsFromDataDirectories(true);
        }

        void CApplicationInfoMenu::customMenu(CMenuActions &menuActions)
        {
            if (!this->view()) { return; }
            if (!this->view()->isEmpty())
            {
                m_menuActionDeleteDirectory = menuActions.addAction(m_menuActionDeleteDirectory, CIcons::delete16(), "Delete data directories", CMenuAction::pathNone(), this, { this->view(), &CApplicationInfoView::deleteSelectedDataDirectories });
            }
            this->nestedCustomMenu(menuActions);
        }

        CApplicationInfoView *CApplicationInfoMenu::view() const
        {
            return static_cast<CApplicationInfoView *>(this->parent());
        }
    } // namespace
} // namespace
