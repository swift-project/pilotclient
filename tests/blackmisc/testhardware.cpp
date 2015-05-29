/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "testhardware.h"
#include "blackmisc/hardware/keyboardkey.h"

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
        key2 = key;
        key2.setModifier1(CKeyboardKey::ModifierCtrlAny);
        QVERIFY2(key != key2, "Modifiers differs, values shall be unequal");
        QVERIFY2(key.equalsWithRelaxedModifiers(key2), "Modifiers are relaxed easy, values shall be equal");
    }

} // namespace
