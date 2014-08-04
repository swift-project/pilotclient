/* Copyright (C) 2013 VATSIM Community / contributors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "hotkey.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>

HotkeyDialog::HotkeyDialog(QWidget *parent) :
    QDialog(parent),
    m_keyboard(nullptr),
    m_pbSelect(nullptr),
    m_lblHotkey(nullptr),
    m_led(nullptr)
{
    m_keyboard = BlackInput::IKeyboard::getInstance();

    setupUi();
    setupConnections();

}

HotkeyDialog::~HotkeyDialog()
{
}

void HotkeyDialog::selectHotKey()
{
    m_lblHotkey->setText("Press any key...");
    m_keyboard->startCapture(false);
}

void HotkeyDialog::keySelectionChanged(BlackMisc::Hardware::CKeyboardKey key)
{
    m_lblHotkey->setText(key.toFormattedQString());
}

void HotkeyDialog::keySelectionFinished(BlackMisc::Hardware::CKeyboardKey key)
{
    m_lblHotkey->setText(key.toFormattedQString());
    m_key = key;
}

void HotkeyDialog::processKeyDown(const BlackMisc::Hardware::CKeyboardKey &key)
{
    if (key == m_key) setPressed(true);
}

void HotkeyDialog::processKeyUp(const BlackMisc::Hardware::CKeyboardKey &key)
{
    if (key == m_key) setPressed(false);
}

void HotkeyDialog::setPressed(bool isPressed)
{
    m_led->setChecked(isPressed);
    update();
}

void HotkeyDialog::setupUi()
{
    m_pbSelect = new QPushButton(this);
    m_pbSelect->setText("Select");
    QFont font;
    font.setPointSize(15);
    m_lblHotkey = new QLabel(this);
    m_lblHotkey->setFont(font);
    m_lblHotkey->setAlignment(Qt::AlignCenter);
    m_led = new CLed(this);

    // PTT group box
    QGroupBox *upperGroup = new QGroupBox;
    upperGroup->setTitle("PTT");
    QHBoxLayout *upperLine = new QHBoxLayout;
    upperLine->addWidget(m_led);
    upperGroup->setLayout(upperLine);

    // Setting group box
    QGroupBox *lowerGroup = new QGroupBox;
    lowerGroup->setTitle("Setting");
    QVBoxLayout *lowerLine = new QVBoxLayout;
    lowerLine->addWidget(m_lblHotkey);
    lowerLine->addWidget(m_pbSelect);
    lowerGroup->setLayout(lowerLine);

    // Main Layout
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(upperGroup);
    mainLayout->addWidget(lowerGroup);

    setLayout(mainLayout);
    setWindowTitle("Sample Hotkey");

}
void HotkeyDialog::setupConnections()
{
    connect(m_keyboard, &BlackInput::IKeyboard::keySelectionChanged, this, &HotkeyDialog::keySelectionChanged);
    connect(m_keyboard, &BlackInput::IKeyboard::keySelectionFinished, this, &HotkeyDialog::keySelectionFinished);
    connect(m_keyboard, &BlackInput::IKeyboard::keyDown, this, &HotkeyDialog::processKeyDown);
    connect(m_keyboard, &BlackInput::IKeyboard::keyUp, this, &HotkeyDialog::processKeyUp);
    connect(m_pbSelect, &QPushButton::clicked, this, &HotkeyDialog::selectHotKey);
}
