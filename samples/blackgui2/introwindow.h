/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
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
