/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_VIEWBASE_H
#define BLACKGUI_VIEWBASE_H

#include "blackmisc/icons.h"
#include "blackgui/updateworker.h"
#include <QTableView>
#include <QHeaderView>
#include <QMenu>
#include <QPoint>
#include <QFont>

namespace BlackGui
{
    namespace Views
    {

        //! Non templated base class, allows Q_OBJECT and signals / slots to be used
        class CViewBaseNonTemplate : public QTableView
        {
            Q_OBJECT

        public:

            //! Resize mode
            //! \remarks Using own resizing (other than QHeaderView::ResizeMode)
            enum ResizeMode
            {
                ResizingAuto,  //!< always resizing, \sa m_resizeAutoNthTime
                ResizingOnce,  //!< only one time
                ResizingOff
            };

            //! When to use asynchronous updates
            static const int asyncThreshold = 50;

            //! Clear data
            virtual void clear() = 0;

            //! Resize mode
            ResizeMode getResizeMode() const { return m_resizeMode; }

            //! Set resize mode
            void setResizeMode(ResizeMode mode) { m_resizeMode = mode; }

            //! In ResizeAuto mode, how often to update. "1" updates every time, "2" every 2nd time, ..
            void setAutoResizeFrequency(int updateEveryNthTime) { this->m_resizeAutoNthTime = updateEveryNthTime; }

            //! Header (horizontal) font
            const QFont &getHorizontalHeaderFont() const { Q_ASSERT(this->horizontalHeader()); return this->horizontalHeader()->font(); }

            //! Horizontal font height
            int getHorizontalHeaderFontHeight() const;

        signals:
            //! Ask for new data
            void requestUpdate();

            //! Asynchronous update finished
            void asyncUpdateFinished();

        public slots:
            //! Resize to contents, strategy depends on container size
            virtual void resizeToContents();

            //! Full resizing to content, might be slow
            virtual void fullResizeToContents();

        protected:
            //! Constructor
            CViewBaseNonTemplate(QWidget *parent);

            //! Method creating the menu
            //! \remarks override this method to contribute to the menu
            virtual void customMenu(QMenu &menu) const;

            //! Perform resizing / non slot method for template
            virtual void performResizeToContents() = 0;

            //! Init as interactive, as this allows manually resizing
            void initRowsResizeModeToInteractive();

            //! Set fixed row height (vertical header with height)
            //! \sa initRowsResizeModeToFixed
            virtual void setRowsResizeModeToInteractive(int height);

            //! Helper method with template free signature
            //! \param variant contains the container
            //! \param sort
            //! \param resize
            virtual int performUpdateContainer(const QVariant &variant, bool sort, bool resize) = 0;

            //! Skip resizing because of size?
            virtual bool reachedResizeThreshold() const = 0;

            //! Resize or skip resize?
            virtual bool resize() const;

            //! Init default values
            virtual void standardInit();

            ResizeMode m_resizeMode   = ResizingAuto; //!< mode
            int        m_resizeCount  = 0;            //!< flag / counter,how many resize activities
            int m_skipResizeThreshold = 40;           //!< when to skip resize
            int m_resizeAutoNthTime   = 1;            //!< with ResizeAuto, resize every n-th time

        protected slots:
            //! Helper method with template free signature serving as callback from threaded worker
            int updateContainer(const QVariant &variant, bool sort, bool resize)
            {
                return this->performUpdateContainer(variant, sort, resize);
            }

        private slots:
            //! Custom menu was requested
            void ps_customMenuRequested(QPoint pos);

            //! Toggle the resize mode
            void ps_toggleResizeMode(bool checked);

            //! Clear the model
            virtual void ps_clear() { this->clear(); }
        };

        //! Base class for views
        template <class ModelClass, class ContainerType> class CViewBase : public CViewBaseNonTemplate
        {

        public:
            //! Destructor
            virtual ~CViewBase() {}

            //! Model
            ModelClass *derivedModel() { return this->m_model; }

            //! Model
            const ModelClass *derivedModel() const { return this->m_model; }

            //! \copydoc CViewBaseNonTemplate::clear
            virtual void clear() override { Q_ASSERT(this->m_model); this->m_model->clear(); }

            //! Update whole container
            int updateContainer(const ContainerType &container, bool sort = true, bool resize = true);

            //! Update whole container in background
            //! \returns Worker or nullptr if worker cannot be started
            IUpdateWorker *updateContainerAsync(const ContainerType &container, bool sort = true, bool resize = true);

            //! Based on size call sync / async update
            void updateContainerMaybeAsync(const ContainerType &container, bool sort = true, bool resize = true);

            //! Insert
            template<class ObjectType> void insert(const ObjectType &value, bool resize = true)
            {
                Q_ASSERT(this->m_model);
                this->m_model->insert(value);
                if (resize) { this->performResizeToContents(); }
            }

            //! Value object at
            template<class ObjectType> const ObjectType &at(const QModelIndex &index) const
            {
                Q_ASSERT(this->m_model);
                return this->m_model->at(index);
            }

            //! Row count
            int rowCount() const;

            //! Column count
            int columnCount() const;

            //! Any data?
            bool isEmpty() const;

            //! Set own name and the model's name
            virtual void setObjectName(const QString &name);

        protected:
            ModelClass *m_model = nullptr; //!< corresponding model

            //! Constructor
            CViewBase(QWidget *parent, ModelClass *model = nullptr) : CViewBaseNonTemplate(parent), m_model(model)
            {
                this->setSortingEnabled(true);
                if (model) { this->setModel(this->m_model); }
            }

            //! Set the search indicator based on model
            void setSortIndicator();

            //! \copydoc CViewBaseNonTemplate::standardInit
            void standardInit(ModelClass *model = nullptr);

            //! \copydoc CViewBaseNonTemplate::reachedResizeThreshold
            virtual bool reachedResizeThreshold() const override { return this->rowCount() > m_skipResizeThreshold; }

            //! \copydoc CViewBaseNonTemplate::performResizing
            virtual void performResizeToContents() override;

            //! \copydoc CViewBaseNonTemplate::performUpdateContainer
            virtual int performUpdateContainer(const QVariant &variant, bool sort, bool resize) override;

            // ---- worker -----------------------------------------------------------------------------------

            //! Worker class performing update and sorting in background
            class CViewUpdateWorker : public BlackGui::IUpdateWorker
            {

            public:
                //! Constructor
                CViewUpdateWorker(CViewBase *view, const ContainerType &container, bool sort, bool resize) :
                    BlackGui::IUpdateWorker(sort), m_view(view), m_container(container), m_resize(resize)
                {
                    Q_ASSERT(view);
                    this->m_sortColumn = view->derivedModel()->getSortColumn();
                    this->m_sortOrder = view->derivedModel()->getSortOrder();
                    this->setObjectName(view->objectName().append(":CViewUpdateWorker"));
                    connect(this, &CViewUpdateWorker::updateFinished, view, &CViewBase::asyncUpdateFinished, Qt::QueuedConnection);
                }

                //! Destructor
                virtual ~CViewUpdateWorker() {}

            protected:
                //! \copydoc CUpdateWorkerPrivate::update
                virtual void update() override
                {
                    Q_ASSERT(m_view);
                    Q_ASSERT(m_view->derivedModel());
                    if (m_view)
                    {
                        // KWB remove later
                        qDebug() << this->objectName() << "worker thread:" << QThread::currentThreadId();

                        // resize has to be in main thread
                        ModelClass *model = m_view->derivedModel();
                        if (m_sort)
                        {
                            // thread safe sort:
                            // 1) the container itself is copied when worker is created and hence thread safe
                            // 2) the sort order itself is not really thread safe,
                            //    but always represents the latest value from CListModelBase/QAbstractListModel::sort()
                            m_container = model->sortContainerByColumn(m_container, m_sortColumn, m_sortOrder);
                        }
                        // now update view itself thread safe, but time for sort was saved
                        QMetaObject::invokeMethod(m_view, "updateContainer", Qt::QueuedConnection,
                                                  Q_ARG(QVariant, m_container.toQVariant()), Q_ARG(bool, false), Q_ARG(bool, m_resize));
                    }
                }

                CViewBase    *m_view = nullptr; //!< view to be updated, actually const but invokeMethod does not allow const
                ContainerType m_container;      //!< container with data
                bool          m_resize;         //!< with resizing
            };

            // ---- worker -----------------------------------------------------------------------------------

        };
    } // namespace
} // namespace
#endif // guard
