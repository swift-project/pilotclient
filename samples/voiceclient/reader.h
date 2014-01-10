/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef READER_H
#define READER_H

#include <QThread>

class LineReader : public QThread
{
    Q_OBJECT
public:
    explicit LineReader() {}

protected:
    void run();

signals:
    void command(const QString& line);

};

#endif // READER_H
