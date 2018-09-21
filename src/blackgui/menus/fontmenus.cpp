/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "fontmenus.h"
#include "blackgui/guiapplication.h"
#include "blackgui/shortcut.h"
#include "blackmisc/verify.h"
#include <QShortcut>

using namespace BlackMisc;
using namespace BlackGui::Components;

namespace BlackGui
{
    namespace Menus
    {
        const CLogCategoryList &CFontMenu::getLogCategories()
        {
            static const CLogCategoryList cats { CLogCategory::guiComponent() };
            return cats;
        }

        CFontMenu::CFontMenu(QWidget *widget, Qt::ShortcutContext shortcutContext) :
            IMenuDelegate(widget), m_widget(widget)
        {
            this->m_fontDialogAction.reset(new QAction(CIcons::font16(), "Font", this));
            QObject::connect(this->m_fontDialogAction.data(), &QAction::triggered, this, &CFontMenu::changeFontDialog);

            m_fontSizePlusShortcut = new QShortcut(CShortcut::keyFontPlus(), this->m_widget);
            m_fontSizePlusShortcut->setContext(shortcutContext);
            QObject::connect(this->m_fontSizePlusShortcut, &QShortcut::activated, this, &CFontMenu::fontSizePlus);

            m_fontSizeMinusShortcut = new QShortcut(CShortcut::keyFontMinus(), this->m_widget);
            m_fontSizeMinusShortcut->setContext(shortcutContext);
            QObject::connect(this->m_fontSizeMinusShortcut, &QShortcut::activated, this, &CFontMenu::fontSizeMinus);

            m_fontResetShortcut = new QShortcut(CShortcut::keyFontReset(), this->m_widget);
            m_fontResetShortcut->setContext(shortcutContext);
            QObject::connect(this->m_fontResetShortcut, &QShortcut::activated, this, &CFontMenu::fontReset);
        }

        void CFontMenu::customMenu(CMenuActions &menuActions)
        {
            menuActions.addAction(this->m_fontDialogAction.data(), CMenuAction::pathFont());
            this->nestedCustomMenu(menuActions);
        }

        QList<QAction *> CFontMenu::getActions() const
        {
            return QList<QAction *>({ m_fontDialogAction.data() });
        }

        QList<QShortcut *> CFontMenu::getShortcuts() const
        {
            return QList<QShortcut *>({ m_fontResetShortcut, m_fontSizeMinusShortcut, m_fontSizePlusShortcut });
        }

        void CFontMenu::changeFontDialog()
        {
            Q_ASSERT_X(m_widget, Q_FUNC_INFO, "No widget");
            if (!m_dialog)
            {
                m_dialog.reset(new CSettingsFontDialog(m_widget));
                m_dialog->setModal(true);
            }
            m_dialog->setCurrentFont(m_widget->font());
            const int r = m_dialog->exec();
            if (r == QDialog::Rejected) { return; }
            const QString qss(m_dialog->getQss());
            m_widget->setStyleSheet(qss);
        }

        void CFontMenu::fontSizePlus()
        {
            if (!m_widget) { return; }
            const int pt = m_widget->font().pointSize() + 1;
            if (pt > 24) { return; }

            m_widget->setStyleSheet(""); // avoid Qt crash
            m_widget->setStyleSheet(CStyleSheetUtility::asStylesheet(m_widget, pt));
        }

        void CFontMenu::fontSizeMinus()
        {
            if (!m_widget) { return; }
            const int pt = m_widget->font().pointSize() - 1;
            if (pt < 5) { return; }
            m_widget->setStyleSheet(""); // avoid Qt crash
            m_widget->setStyleSheet(CStyleSheetUtility::asStylesheet(m_widget, pt));
        }

        void CFontMenu::fontReset()
        {
            Q_ASSERT_X(m_widget, Q_FUNC_INFO, "No widget");
            m_widget->setStyleSheet("");
        }
    } // ns
} // ns
