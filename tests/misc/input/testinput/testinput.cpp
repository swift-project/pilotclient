// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \cond PRIVATE_TESTS
//! \file
//! \ingroup testmisc

#include "misc/mixin/mixincompare.h"
#include "misc/identifier.h"
#include "misc/input/actionhotkey.h"
#include "misc/input/actionhotkeylist.h"
#include "misc/input/hotkeycombination.h"
#include "misc/input/keyboardkey.h"
#include "misc/input/keycodes.h"
#include "misc/variant.h"
#include "test.h"

#include <QString>
#include <QTest>

using namespace swift::misc;
using namespace swift::misc::input;

namespace MiscTest
{
    //! Input classes basic tests
    class CTestInput : public QObject
    {
        Q_OBJECT

    private slots:
        //! CKeyboardKey basic tests
        void keyboardKey();

        //! CHotkeyCombination tests
        void hotkeyCombination();

        void hotKeyEqual();
        void hotKeyNonEqual();
        void hotkeyEqualFromVariant();
        void hotkeyFindSupersetOf();
        void hotkeyFindSubsetOf();
    };

    void CTestInput::keyboardKey()
    {
        CKeyboardKey key;
        QVERIFY2(key.getKey() == Key_Unknown, "Default constructed object should have unknown key");
        QVERIFY2(key.getKeyAsString() == QString(), "Default constructed object should return empty string");
        QVERIFY2(key.hasKey() == false, "Default constructed object should return false");

        key.setKey('C');
        QVERIFY2(key.getKey() == Key_C, "CKeyboardKey::setKey(char key) failed");

        CKeyboardKey key2(Key_5);
        QVERIFY2(key2.getKey() == Key_5, "Returned key is wrong");
        QVERIFY2(key2.getKeyAsString() == "5", "Key as string failed");
        QVERIFY2(key2.hasKey() == true, "Should return true for key != Key_Unknown");

        CKeyboardKey key3(Key_AltRight);
        QVERIFY2(key3.isModifier(), "RightAlt is a modifier");
    }

    void CTestInput::hotkeyCombination()
    {
        CHotkeyCombination comb;
        comb.addKeyboardKey(Key_5);
        comb.addKeyboardKey(Key_AltLeft);
        comb.addKeyboardKey(Key_ControlLeft);
        QVERIFY2(comb.size() == 3, "Size is incorrect");
        QVERIFY2(comb.toQString() == "CtrlLeft+AltLeft+5", "String representation failed");

        comb.replaceKey(Key_AltLeft, Key_AltRight);
        QVERIFY2(comb.size() == 3, "Size is incorrect");
        QVERIFY2(comb.toQString() == "CtrlLeft+AltRight+5", "String representation failed");

        comb.replaceKey(Key_AltRight, CKeyboardKey());
        QVERIFY2(comb.size() == 2, "Size is incorrect");
        QVERIFY2(comb.toQString() == "CtrlLeft+5", "String representation failed");

        comb.removeKeyboardKey(Key_5);
        QVERIFY2(comb.size() == 1, "Size is incorrect");
        QVERIFY2(comb.toQString() == "CtrlLeft", "String representation failed");
    }

    void CTestInput::hotKeyEqual()
    {
        CHotkeyCombination comb1;
        comb1.addKeyboardKey(Key_ControlLeft);
        comb1.addKeyboardKey(Key_D);
        CActionHotkey actionHotkey1(CIdentifier::anonymous(), comb1, "action");

        CHotkeyCombination comb2;
        comb2.addKeyboardKey(Key_ControlLeft);
        comb2.addKeyboardKey(Key_D);
        CActionHotkey actionHotkey2(CIdentifier::anonymous(), comb2, "action");
        QVERIFY2(actionHotkey1 == actionHotkey2, "Objects should equal");
    }

    void CTestInput::hotKeyNonEqual()
    {
        CHotkeyCombination comb;
        comb.addKeyboardKey(Key_ControlLeft);
        comb.addKeyboardKey(Key_D);
        CActionHotkey actionHotkey1(CIdentifier::anonymous(), comb, "action");

        CHotkeyCombination comb2;
        comb2.addKeyboardKey(Key_ControlLeft);
        comb2.addKeyboardKey(Key_C);
        CActionHotkey actionHotkey2(CIdentifier::anonymous(), comb2, "action");
        QVERIFY2(actionHotkey1 != actionHotkey2, "Objects should not be equal");

        CHotkeyCombination comb3;
        comb3.addKeyboardKey(Key_ShiftLeft);
        comb3.addKeyboardKey(Key_D);
        CActionHotkey actionHotkey3(CIdentifier::anonymous(), comb2, "action");
        QVERIFY2(actionHotkey1 != actionHotkey3, "Objects should not be equal");

        CHotkeyCombination comb4;
        comb4.addKeyboardKey(Key_ControlLeft);
        comb4.addKeyboardKey(Key_D);
        CActionHotkey actionHotkey4(CIdentifier::anonymous(), comb2, "other_action");
        QVERIFY2(actionHotkey1 != actionHotkey4, "Objects should not be equal");
    }

    void CTestInput::hotkeyEqualFromVariant()
    {
        CHotkeyCombination comb1;
        comb1.addKeyboardKey(Key_ControlLeft);
        comb1.addKeyboardKey(Key_C);
        CActionHotkey actionHotkey1(CIdentifier::anonymous(), comb1, "action");
        CActionHotkeyList list1;
        list1.push_back(actionHotkey1);

        CHotkeyCombination comb2;
        comb2.addKeyboardKey(Key_ControlLeft);
        comb2.addKeyboardKey(Key_D);
        CActionHotkey actionHotkey2(CIdentifier::anonymous(), comb2, "action");
        CActionHotkeyList list2;
        list2.push_back(actionHotkey2);
        QVERIFY2(CVariant::fromValue(list1) != CVariant::fromValue(list2), "List should not be equal");
    }

    void CTestInput::hotkeyFindSupersetOf()
    {
        CHotkeyCombination comb1;
        comb1.addKeyboardKey(Key_ControlLeft);
        comb1.addKeyboardKey(Key_D);
        CActionHotkey actionHotkey1(CIdentifier::anonymous(), comb1, "action");

        CHotkeyCombination comb2;
        comb2.addKeyboardKey(Key_ControlLeft);
        CActionHotkey actionHotkey2(CIdentifier::anonymous(), comb2, "action");

        CHotkeyCombination comb3;
        comb3.addKeyboardKey(Key_ShiftLeft);
        comb3.addKeyboardKey(Key_D);
        CActionHotkey actionHotkey3(CIdentifier::anonymous(), comb3, "action");

        CActionHotkeyList list1;
        list1.push_back(actionHotkey1);

        QVERIFY2(!list1.findSupersetsOf(actionHotkey2).isEmpty(), "CTRL+F is a superset of CTRL");
        QVERIFY2(list1.findSupersetsOf(actionHotkey3).isEmpty(), "Shift+F is not a superset of CTRL");
    }

    void CTestInput::hotkeyFindSubsetOf()
    {
        CHotkeyCombination comb1;
        comb1.addKeyboardKey(Key_ControlLeft);
        CActionHotkey actionHotkey1(CIdentifier::anonymous(), comb1, "action");

        CHotkeyCombination comb2;
        comb2.addKeyboardKey(Key_ControlLeft);
        comb2.addKeyboardKey(Key_D);
        CActionHotkey actionHotkey2(CIdentifier::anonymous(), comb2, "action");

        CHotkeyCombination comb3;
        comb3.addKeyboardKey(Key_ShiftLeft);
        comb3.addKeyboardKey(Key_D);
        CActionHotkey actionHotkey3(CIdentifier::anonymous(), comb3, "action");

        CActionHotkeyList list1;
        list1.push_back(actionHotkey1);

        QVERIFY2(!list1.findSubsetsOf(actionHotkey2).isEmpty(), "CTRL+F is a subset of CTRL");
        QVERIFY2(list1.findSubsetsOf(actionHotkey3).isEmpty(), "Shift+F is not a subset of CTRL");
    }
} // namespace

//! main
BLACKTEST_APPLESS_MAIN(MiscTest::CTestInput);

#include "testinput.moc"

//! \endcond
