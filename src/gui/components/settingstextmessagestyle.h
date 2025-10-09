// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_SETTINGSTEXTMESSAGESTYLE_H
#define SWIFT_GUI_COMPONENTS_SETTINGSTEXTMESSAGESTYLE_H

#include <QFrame>
#include <QScopedPointer>
#include <QStringList>

namespace Ui
{
    class CSettingsTextMessageStyle;
}
namespace swift::gui::components
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
        ~CSettingsTextMessageStyle() override;

        //! Fmily, size and style
        QStringList getFamilySizeStyle() const;

        //! Style
        const QString &getStyle() { return m_style; }

        //! Style
        void setStyle(const QString &style) { m_style = style; }

        //! @{
        //! Font size
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
        CTextEditDialog *m_textEditDialog = nullptr;
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
} // namespace swift::gui::components

#endif // SWIFT_GUI_COMPONENTS_SETTINGSTEXTMESSAGESTYLE_H
