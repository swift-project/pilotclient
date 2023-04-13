/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "fontmenus.h"
#include "blackgui/guiapplication.h"
#include "blackgui/shortcut.h"
#include "blackmisc/verify.h"
#include <QShortcut>

using namespace BlackMisc;
using namespace BlackGui::Components;

namespace BlackGui::Menus
{
    const QStringList &CFontMenu::getLogCategories()
    {
        static const QStringList cats { CLogCategories::guiComponent() };
        return cats;
    }

    CFontMenu::CFontMenu(QWidget *widget, Qt::ShortcutContext shortcutContext) : IMenuDelegate(widget), m_widget(widget)
    {
        m_fontDialogAction.reset(new QAction(CIcons::font16(), "Font", this));
        QObject::connect(m_fontDialogAction.data(), &QAction::triggered, this, &CFontMenu::changeFontDialog);

        m_fontSizePlusShortcut = new QShortcut(CShortcut::keyFontPlus(), m_widget);
        m_fontSizePlusShortcut->setContext(shortcutContext);
        QObject::connect(m_fontSizePlusShortcut, &QShortcut::activated, this, &CFontMenu::onFontSizePlus);

        m_fontSizeMinusShortcut = new QShortcut(CShortcut::keyFontMinus(), m_widget);
        m_fontSizeMinusShortcut->setContext(shortcutContext);
        QObject::connect(m_fontSizeMinusShortcut, &QShortcut::activated, this, &CFontMenu::onFontSizeMinus);

        m_fontResetShortcut = new QShortcut(CShortcut::keyFontReset(), m_widget);
        m_fontResetShortcut->setContext(shortcutContext);
        QObject::connect(m_fontResetShortcut, &QShortcut::activated, this, &CFontMenu::onFontReset);
    }

    void CFontMenu::customMenu(CMenuActions &menuActions)
    {
        menuActions.addAction(m_fontDialogAction.data(), CMenuAction::pathFont());
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
        m_dialog->setFont(m_widget->font());
        const int r = m_dialog->exec();
        if (r == QDialog::Rejected) { return; }
        const QString qss(m_dialog->getQss());
        m_widget->setStyleSheet(qss);
    }

    void CFontMenu::onFontSizePlus()
    {
        if (!m_widget) { return; }
        const int pt = m_widget->font().pointSize() + 1;
        if (pt > 24) { return; }

        m_widget->setStyleSheet(""); // avoid Qt crash
        m_widget->setStyleSheet(CStyleSheetUtility::asStylesheet(m_widget, pt));

        emit this->fontSizePlus();
    }

    void CFontMenu::onFontSizeMinus()
    {
        if (!m_widget) { return; }
        const int pt = m_widget->font().pointSize() - 1;
        if (pt < 5) { return; }

        m_widget->setStyleSheet(""); // avoid Qt crash
        m_widget->setStyleSheet(CStyleSheetUtility::asStylesheet(m_widget, pt));

        emit this->fontSizeMinus();
    }

    void CFontMenu::onFontReset()
    {
        Q_ASSERT_X(m_widget, Q_FUNC_INFO, "No widget");
        m_widget->setStyleSheet("");
    }
} // ns
