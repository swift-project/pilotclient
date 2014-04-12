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
    //! \brief Constructor
    explicit CIntroWindow(QWidget *parent = nullptr);

    //! Destructor
    ~CIntroWindow();

    //! Selected window mode
    GuiModes::WindowMode getWindowMode() const;

    //! Get core mode
    GuiModes::CoreMode getCoreMode() const;

    //! select DBus address/mode
    QString getDBusAddress() const;

private slots:
    //! Button has been clicked
    void buttonClicked() const;

private:
    QScopedPointer<Ui::CIntroWindow> ui;
};

#endif // guard
