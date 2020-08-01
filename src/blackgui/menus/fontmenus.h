/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#ifndef BLACKGUI_MENUS_FONTMENUS_H
#define BLACKGUI_MENUS_FONTMENUS_H

#include "blackgui/components/settingsfontdialog.h"
#include "blackgui/menus/menudelegate.h"
#include "blackgui/views/aircraftmodelview.h"
#include "blackgui/overlaymessagesframe.h"
#include "blackmisc/simulation/aircraftmodelinterfaces.h"

#include <QObject>

class QAction;
class QShortcut;

namespace BlackGui
{
    namespace Menus
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
            static const BlackMisc::CLogCategoryList &getLogCategories();

            //! \copydoc IMenuDelegate::customMenu
            virtual void customMenu(CMenuActions &menuActions) override;

            //! Allow to use the actions directly
            QList<QAction *> getActions() const;

            //! The shortcus owned by QWidget
            QList<QShortcut *> getShortcuts() const;

        signals:
            //! Font size signals
            //! @{
            void fontSizePlus();
            void fontSizeMinus();
            //! @}

        private:
            void changeFontDialog();
            void onFontSizePlus();
            void onFontSizeMinus();
            void onFontReset();

            QWidget *m_widget = nullptr;
            QShortcut *m_fontSizePlusShortcut = nullptr;  //! owned by widget
            QShortcut *m_fontSizeMinusShortcut = nullptr; //! owned by widget
            QShortcut *m_fontResetShortcut = nullptr;     //! owned by widget
            QScopedPointer<QAction> m_fontDialogAction;
            QScopedPointer<BlackGui::Components::CSettingsFontDialog> m_dialog;
        };
    } // ns
} // ns

#endif // guard
