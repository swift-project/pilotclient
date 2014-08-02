/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef SAMPLE_INTROWINDOW_H
#define SAMPLE_INTROWINDOW_H

#include "guimodeenums.h"
#include <QDialog>
#include <QScopedPointer>

namespace Ui { class CIntroWindow; }

//! Intro screen
class CIntroWindow : public QDialog
{
    Q_OBJECT

public:
    //! Constructor
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
