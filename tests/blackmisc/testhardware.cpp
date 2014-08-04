/*  Copyright (C) 2013 VATSIM Community / authors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "testhardware.h"
#include "blackmisc/hwkeyboardkey.h"

using namespace BlackMisc::Hardware;

namespace BlackMiscTest
{

    /*
     * Constructor
     */
    CTestHardware::CTestHardware(QObject *parent): QObject(parent)
    {
        // void
    }

    // CKeyboardKey tests
    void CTestHardware::keyboardKey()
    {
        // Test equal operator
        CKeyboardKey key = CKeyboardKey(Qt::Key_5, CKeyboardKey::ModifierAltLeft, CKeyboardKey::ModifierCtrlLeft);
        CKeyboardKey key2;
        key2.setKey(Qt::Key_5);
        key2.setModifier1(CKeyboardKey::ModifierAltLeft);
        key2.setModifier2(CKeyboardKey::ModifierCtrlLeft);
        QVERIFY2(key == key2, "CKeyboardKey::operator== failed!");

        key = CKeyboardKey();
        key.setKey(QString("A"));
        QVERIFY2(key.getKey() == Qt::Key_A, "CKeyboardKey::setKey(const QString &key) failed");

        key.setKey(QChar('B'));
        QVERIFY2(key.getKey() == Qt::Key_B, "CKeyboardKey::setKey(const QChar key) failed");

        key.setKey(Qt::Key_C);
        QVERIFY2(key.getKey() == Qt::Key_C, "CKeyboardKey::setKey(const QChar &key) failed");

        key.setKey('D');
        QVERIFY2(key.getKey() == Qt::Key_D, "CKeyboardKey::setKey(const int key) failed");

        key = CKeyboardKey();
        QVERIFY2(key.addModifier(CKeyboardKey::ModifierAltLeft), "CKeyboardKey::addModifier() returned wrong result");
        QVERIFY2(key.numberOfModifiers() == 1, "Expected number of modifiers to be equal to 1");
        QVERIFY2(key.getModifier1() == CKeyboardKey::ModifierAltLeft, "Expected modifier to be ModifierAltLeft");
        // Add same modifier again. This should not change the status
        QVERIFY2(!key.addModifier(CKeyboardKey::ModifierAltLeft), "CKeyboardKey::addModifier() returned wrong result");
        QVERIFY2(key.numberOfModifiers() == 1, "Expected number of modifiers to be equal to 1");
        QVERIFY2(key.getModifier1() == CKeyboardKey::ModifierAltLeft, "Expected modifier to be ModifierAltLeft");
        QVERIFY2(key.getModifier2() == CKeyboardKey::ModifierNone, "Expected modifier to be ModifierAltLeft");

        // relaxed checks
        key = CKeyboardKey(Qt::Key_1, CKeyboardKey::ModifierCtrlLeft, CKeyboardKey::ModifierNone);
        key2 = CKeyboardKey(Qt::Key_1, CKeyboardKey::ModifierCtrlLeft, CKeyboardKey::ModifierNone);
        key2 = key;
        key2.setModifier1(CKeyboardKey::ModifierCtrlAny);
        QVERIFY2(key != key2, "Modifiers differs, values shall be unequal");
        QVERIFY2(key.equalsWithRelaxedModifiers(key2), "Modifiers are relaxed easy, values shall be equal");
    }

} // namespace
