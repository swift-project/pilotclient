// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "fontmenus.h"

#include <QShortcut>

#include "gui/guiapplication.h"
#include "gui/shortcut.h"

using namespace swift::misc;
using namespace swift::gui::components;

namespace swift::gui::menus
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

    QList<QAction *> CFontMenu::getActions() const { return QList<QAction *>({ m_fontDialogAction.data() }); }

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
} // namespace swift::gui::menus
