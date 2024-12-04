// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_SETTINGSFONTDIALOG_H
#define SWIFT_GUI_COMPONENTS_SETTINGSFONTDIALOG_H

#include <QDialog>
#include <QFont>
#include <QStringList>

namespace Ui
{
    class CSettingsFontDialog;
}
namespace swift::gui::components
{
    /*!
     * Dialog to edit fonts
     * \see swift::misc::components::CSettingsFontComponent
     */
    class CSettingsFontDialog : public QDialog
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CSettingsFontDialog(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CSettingsFontDialog() override;

        //! Get stylesheet
        const QString &getQss() const;

        //! Set the current font
        void setFont(const QFont &font);

        //! Get font
        QFont getFont() const;

        //! Family, size and style
        QStringList getFamilySizeStyle() const;

        //! With color selection
        void setWithColorSelection(bool withColor);

    private:
        QScopedPointer<Ui::CSettingsFontDialog> ui;
    };
} // namespace swift::gui::components
#endif // SWIFT_GUI_COMPONENTS_SETTINGSFONTDIALOG_H
