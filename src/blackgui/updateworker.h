/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_UPDATEWORKER_H
#define BLACKGUI_UPDATEWORKER_H

#include <QObject>
#include <QThread>
#include <QDebug>

namespace BlackGui
{
    //! Base class for workers. Runs itself in newly created thread when started.
    //! Provides access to related thread and cleans up itself when done.
    class IUpdateWorker : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor
        //! \remarks no parent, as objects with parents can not be moved to thread
        IUpdateWorker(bool sort) : QObject(), m_sort(sort) {}

        //! Destructor
        virtual ~IUpdateWorker() { terminateThread();}

    signals:
        //! Update is completed
        void updateFinished();

    public slots:

        //! Terminate myself when thread is done
        void terminate()
        {
            terminateThread();
            deleteLater();
        }

        //! Start thread, return nullptr if cannot be started
        //! \remarks If start fails (false), object needs to be terminated manually
        bool start()
        {
            qDebug() << "before init thread" << QThread::currentThreadId();
            if (!m_thread) { initializeThread(); }
            m_thread->start(); // starting, not yet doing anything

            // m_thread will start with invocation by event loop
            // invokeMethod "schedules" an update running in the newly created thread
            bool ok = QMetaObject::invokeMethod(this, "ps_runUpdate", Qt::QueuedConnection);
            if (ok) { return true; }

            // invocation failed, so I can clean up thread straight away
            this->terminateThread();
            return false;
        }

    protected:
        //! Do the update (perform work here in another thread)
        virtual void update() = 0;

        bool    m_sort   = true;      //!< perform sort?
        int     m_sortColumn = -1;    //!< which column to sort
        QThread *m_thread = nullptr;  //!< corresponding thread
        Qt::SortOrder m_sortOrder = Qt::AscendingOrder; //!< sort order

    private slots:
        //! Update, call virtual method so inheriting class needs no slots
        void ps_runUpdate()
        {
            this->update(); // call overridden method doing work
            emit this->updateFinished();
            this->terminate(); // clean up thread, delete worker (myself)
        }

    private:
        //! Terminate and clean up thread
        void terminateThread()
        {
            if (!m_thread)
            {
                QThread *t = m_thread;
                m_thread = nullptr;
                t->quit();
                t->deleteLater();
            }
        }

        //! Initialize thread
        void initializeThread()
        {
            if (m_thread) { return; }
            m_thread = new QThread();
            this->moveToThread(m_thread);
        }

    };
} // namespace

#endif // guard
