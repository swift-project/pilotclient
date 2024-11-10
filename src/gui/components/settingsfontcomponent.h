// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_SETTINGSFONTCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_SETTINGSFONTCOMPONENT_H

#include <QFrame>

namespace Ui
{
    class CSettingsFontComponent;
}
namespace swift::gui::components
{
    /*!
     * Font settings
     */
    class CSettingsFontComponent : public QFrame
    {
        Q_OBJECT

    public:
        //! How to update
        enum Mode
        {
            DirectUpdate, //!< directly updating a font qss file
            GenerateQssOnly //!< builds a qss style string
        };

        //! Constructor
        explicit CSettingsFontComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CSettingsFontComponent() override;

        //! Set mode
        void setMode(Mode m);

        //! Get the stylesheet
        const QString &getQss() const { return m_qss; }

        //! Set the current font
        void setFont(const QFont &font);

        //! Get font selection
        QFont getFont() const;

        //! Strings such as
        QStringList getFamilySizeStyle() const;

        //! With color selection
        void setWithColorSelection(bool withColor);

        //! Set the default color from style sheet
        void setStyleSheetDefaultColor();

    signals:
        //! To be used with dialogs
        void accept();

        //! To be used with dialogs
        void reject();

    private:
        QScopedPointer<Ui::CSettingsFontComponent> ui;
        QColor m_selectedColor;
        QColor m_cancelColor;
        QFont m_cancelFont;
        QString m_qss;
        bool m_noColorDefault = true; //!< as of T571 no color default
        Mode m_mode = DirectUpdate;

        void changeFont();
        void resetFont();
        void resetFontAndReject();
        void clearQssAndResetFont();
        void fontColorDialog();
        void noColor();
        void initValues();
        void initUiValues(const QFont &font, const QColor &color);
    };
} // ns

#endif // guard
