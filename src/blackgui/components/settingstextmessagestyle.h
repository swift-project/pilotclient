/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_SETTINGSTEXTMESSAGESTYLE_H
#define BLACKGUI_COMPONENTS_SETTINGSTEXTMESSAGESTYLE_H

#include <QFrame>
#include <QStringList>
#include <QScopedPointer>

namespace Ui { class CSettingsTextMessageStyle; }
namespace BlackGui
{
    namespace Components
    {
        class CSettingsFontDialog;
        class CTextEditDialog;

        //! Text message style
        class CSettingsTextMessageStyle : public QFrame
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CSettingsTextMessageStyle(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CSettingsTextMessageStyle() override;

            //! Fmily, size and style
            QStringList getFamilySizeStyle() const;

            //! Style
            const QString &getStyle() { return m_style; }

            //! Style
            void setStyle(const QString &style) { m_style = style; }

            //! Font size
            //! @{
            void fontSizeMinus();
            void fontSizePlus();
            //! @}

            //! Reset style
            void resetStyle() { m_style.clear(); }

        signals:
            //! Font or style changed from within the component
            void changed();

        private:
            QScopedPointer<Ui::CSettingsTextMessageStyle> ui;
            CSettingsFontDialog *m_fontSettingsDialog = nullptr;
            CTextEditDialog     *m_textEditDialog     = nullptr;
            QString m_style;

            //! Change font
            void changeFont();

            //! Change style
            void changeStyle();

            //! Update the font part
            bool setFontFamilySizeStyle(const QStringList &familySizeStlye);

            //! Replace the table style in style
            void replaceTableStyle(const QString &newTableStyle);

            //! Increase/decrease font size
            bool changeFontSize(bool increase);
        };
    } // ns
} // ns

#endif // guard
