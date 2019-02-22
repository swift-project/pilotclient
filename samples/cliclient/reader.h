/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file
//! \ingroup samplecliclient

#ifndef BLACKSAMPLE_CLICLIENT_LINEREADER_H
#define BLACKSAMPLE_CLICLIENT_LINEREADER_H

#include <QObject>
#include <QString>
#include <QThread>

namespace BlackSample
{

    /*!
     * \brief Simple console command reader
     *
     * This class reads the user input per line and emits each line
     * as a command signals.
     * This task is running in the background and does not block.
     */
    class LineReader : public QThread
    {
        Q_OBJECT

    public:
        LineReader() {}

    protected:
        //! Run reader in background
        virtual void run() override;

    signals:
        //! User command
        void command(const QString &line);
    };

} // ns

#endif // guard
