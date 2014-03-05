/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISCTEST_TESTHARDWARE_H
#define BLACKMISCTEST_TESTHARDWARE_H

#include <QtTest/QtTest>


namespace BlackMiscTest
{
    /*!
     * \brief Hardware classes basic tests
     */
    class CTestHardware : public QObject
    {
        Q_OBJECT
    public:
        /*!
         * \brief Standard test case constructor
         * \param parent
         */
        explicit CTestHardware(QObject *parent = nullptr);

    signals:

    private slots:

        //! \brief CKeyboardKey basic tests
        void keyboardKey();

    };
} // namespace BlackMiscTest

#endif // BLACKMISCTEST_TESTHARDWARE_H
