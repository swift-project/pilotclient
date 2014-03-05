/* Copyright (C) 2013 VATSIM Community / contributors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKSAMPLE_HOTKEY_H
#define BLACKSAMPLE_HOTKEY_H

#include "led.h"
#include "blackcore/keyboard.h"
#include "blackmisc/hwkeyboardkey.h"

#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QScopedPointer>

class HotkeyDialog : public QDialog
{
    Q_OBJECT

public:
    /*!
     * \brief Constructor
     * \param parent
     */
    explicit HotkeyDialog(QWidget *parent = 0);

    //! Destructor
    ~HotkeyDialog();

public slots:
    //! \brief Slot to select a new hot key
    void selectHotKey();

private slots:
    /*!
     * \brief Slot when the selected key set has changed
     * \param keySet
     */
    void keySelectionChanged(BlackMisc::Hardware::CKeyboardKey key);

    /*!
     * \brief Slot when the key selection is finished
     * \param keySet
     */
    void keySelectionFinished(BlackMisc::Hardware::CKeyboardKey key);

private:
    /*!
     * \brief Set the key status
     * \param isPressed
     */
    void setPressed(bool isPressed);

private:
    void setupUi();
    void setupConnections();

    BlackCore::IKeyboard *m_keyboard;
    QPushButton *m_pbSelect;
    QLabel      *m_lblHotkey;
    CLed        *m_led;
};

#endif // BLACKSAMPLE_HOTKEY_H
