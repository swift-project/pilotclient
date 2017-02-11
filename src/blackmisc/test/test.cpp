/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/test/test.h"

namespace BlackMisc
{
    namespace Test
    {
        CTestBase::CTestBase(int argc, const char *const *argv)
        {
            for (int i = 0; i < argc; ++i)
            {
                m_arguments.append(argv[i]);
            }

            // Output to stdout
            m_arguments.append({ "-o", "-,txt" });
        }

        CTestBase::~CTestBase()
        {}

        int CTestBase::exec(QObject *test, const QString &name)
        {
            auto additionalArguments = QStringList { "-o", name + "_testresults.xml,xml" };
            return this->qExec(test, m_arguments + additionalArguments);
        }
    } // ns
} // ns
