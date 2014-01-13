/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SAMPLE_INTROWINDOW_H
#define SAMPLE_INTROWINDOW_H

#include "guimodeenums.h"
#include <QDialog>


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
     * \param parent
     */
    explicit CIntroWindow(QWidget *parent = nullptr);

    /*!
     * Destructor
     */
    ~CIntroWindow();

    /*!
     * \brief Selected window mode
     * \return
     */
    GuiModes::WindowMode getWindowMode() const;

    /*!
     * \brief Get core mode
     * \return
     */
    GuiModes::CoreMode getCoreMode() const;

private slots:
    /*!
     * \brief Button has been clicked
     */
    void buttonClicked() const;

private:
    Ui::CIntroWindow *ui;
};

#endif // guard
