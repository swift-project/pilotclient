// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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
