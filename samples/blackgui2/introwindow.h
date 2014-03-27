/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SAMPLE_INTROWINDOW_H
#define SAMPLE_INTROWINDOW_H

#include "guimodeenums.h"
#include <QDialog>
#include <QScopedPointer>

namespace Ui
{
    class CIntroWindow;
}

class CIntroWindow : public QDialog
{
    Q_OBJECT

public:
    /*!
     * \brief Constructor
     * \param parent widget
     */
    explicit CIntroWindow(QWidget *parent = nullptr);

    /*!
     * Destructor
     */
    ~CIntroWindow();

    /*!
     * \brief Selected window mode
     * \see GuiModes::WindowMode
     */
    GuiModes::WindowMode getWindowMode() const;

    /*!
     * \brief Get core mode
     * \see GuiModes::CoreMode
     */
    GuiModes::CoreMode getCoreMode() const;

private slots:
    /*!
     * \brief Button has been clicked
     */
    void buttonClicked() const;

private:
    QScopedPointer<Ui::CIntroWindow> ui;
};

#endif // guard
