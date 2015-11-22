/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef __BLACKSAMPLE_CLI_CLIENT_READER_H__
#define __BLACKSAMPLE_CLI_CLIENT_READER_H__

#include <QThread>
#include <QString>

class LineReader : public QThread
{
    Q_OBJECT

public:
    LineReader() {}

protected:
    void run();

signals:
    void command(const QString &line);
};

#endif //__BLACKSAMPLE_CLI_CLIENT_READER_H__
