// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef SWIFT_GUI_MENUS_FONTMENUS_H
#define SWIFT_GUI_MENUS_FONTMENUS_H

#include "gui/components/settingsfontdialog.h"
#include "gui/menus/menudelegate.h"
#include "gui/views/aircraftmodelview.h"
#include "gui/overlaymessagesframe.h"
#include "misc/simulation/aircraftmodelinterfaces.h"

#include <QObject>

class QAction;
class QShortcut;

namespace swift::gui::menus
{
    //! Standard font (size, style) menus.
    //! Can be used as nested menu or via getActions added manually
    class CFontMenu : public IMenuDelegate
    {
        Q_OBJECT

    public:
        //! Constructor
        CFontMenu(QWidget *widget, Qt::ShortcutContext shortcutContext = Qt::WidgetShortcut);

        //! Log.categories
        static const QStringList &getLogCategories();

        //! \copydoc IMenuDelegate::customMenu
        virtual void customMenu(CMenuActions &menuActions) override;

        //! Allow to use the actions directly
        QList<QAction *> getActions() const;

        //! The shortcus owned by QWidget
        QList<QShortcut *> getShortcuts() const;

    signals:
        //! @{
        //! Font size signals
        void fontSizePlus();
        void fontSizeMinus();
        //! @}

    private:
        void changeFontDialog();
        void onFontSizePlus();
        void onFontSizeMinus();
        void onFontReset();

        QWidget *m_widget = nullptr;
        QShortcut *m_fontSizePlusShortcut = nullptr; //! owned by widget
        QShortcut *m_fontSizeMinusShortcut = nullptr; //! owned by widget
        QShortcut *m_fontResetShortcut = nullptr; //! owned by widget
        QScopedPointer<QAction> m_fontDialogAction;
        QScopedPointer<swift::gui::components::CSettingsFontDialog> m_dialog;
    };
} // ns

#endif // guard
