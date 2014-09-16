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
#include <QTableView>
#include <QMenu>
#include <QPoint>

namespace BlackGui
{
    namespace Views
    {

        //! Non templated base class, allows Q_OBJECT and signals to be used
        class CViewBaseNonTemplate : public QTableView
        {
            Q_OBJECT

        public:
            //! Clear data
            virtual void clear() = 0;

        signals:
            //! Ask for new data
            void requestUpdate();

        protected:
            //! Constructor
            CViewBaseNonTemplate(QWidget *parent) : QTableView(parent)
            {
                this->setContextMenuPolicy(Qt::CustomContextMenu);
                connect(this, &QWidget::customContextMenuRequested, this, &CViewBaseNonTemplate::ps_customMenuRequested);
            }

            //! Method creating the menu
            //! \remarks override this method to contribute to the menu
            virtual void customMenu(QMenu &menu) const
            {
                menu.addAction(BlackMisc::CIcons::refresh16(),  "Update", this, SIGNAL(requestUpdate()));
                menu.addAction(BlackMisc::CIcons::delete16(), "Clear", this, SLOT(ps_clear()));
            }

        private slots:
            //! Custom menu was requested
            void ps_customMenuRequested(QPoint pos)
            {
                QMenu menu;
                this->customMenu(menu);
                if (menu.isEmpty()) { return; }

                QPoint globalPos = this->mapToGlobal(pos);
                menu.exec(globalPos);
            }

            //! Clear the model
            virtual void ps_clear() { this->clear(); }

        };

        //! Base class for views
        template <class ModelClass> class CViewBase : public CViewBaseNonTemplate
        {

        public:

            //! Model
            ModelClass *derivedModel() { return this->m_model; }

            //! Model
            const ModelClass *derivedModel() const { return this->m_model; }

            //! \copydoc CViewBaseNonTemplate::clear
            virtual void clear() override { Q_ASSERT(this->m_model); this->m_model->clear(); }

            //! Update whole container
            template<class ContainerType> int updateContainer(const ContainerType &container, bool resize = true)
            {
                Q_ASSERT(this->m_model);
                int c = this->m_model->update(container);
                if (!resize) return c;
                this->resizeColumnsToContents();
                this->resizeRowsToContents();
                return c;
            }

            //! Insert
            template<class ObjectType> void insert(const ObjectType &value, bool resize = true)
            {
                Q_ASSERT(this->m_model);
                this->m_model->insert(value);
                if (!resize) return;
                this->resizeColumnsToContents();
                this->resizeRowsToContents();
            }

            //! Value object at
            template<class ObjectType> const ObjectType &at(const QModelIndex &index) const
            {
                Q_ASSERT(this->m_model);
                return this->m_model->at(index);
            }

            //! Row count
            int rowCount() const
            {
                Q_ASSERT(this->m_model);
                return this->m_model->rowCount();
            }

            //! Column count
            int columnCount() const
            {
                Q_ASSERT(this->m_model);
                return this->m_model->columnCount();
            }

            //! Any data?
            bool isEmpty() const
            {
                Q_ASSERT(this->m_model);
                return this->m_model->rowCount() < 1;
            }

            //! Set own name and the model's name
            void setObjectName(const QString &name)
            {
                // then name here is mainly set for debugging purposes so each model can be identified
                Q_ASSERT(m_model);
                QTableView::setObjectName(name);
                QString modelName = QString(name).append(':').append(this->m_model->getTranslationContext());
                this->m_model->setObjectName(modelName);
            }

        protected:
            ModelClass *m_model = nullptr; //!< corresponding model

            //! Constructor
            CViewBase(QWidget *parent, ModelClass *model = nullptr) : CViewBaseNonTemplate(parent), m_model(model)
            {
                this->setSortingEnabled(true);
                if (model) { this->setModel(this->m_model); }
            }

            //! Destructor
            virtual ~CViewBase() {}

            //! Set the search indicator based on model
            void setSortIndicator()
            {
                if (this->m_model->hasValidSortColumn())
                {
                    this->horizontalHeader()->setSortIndicator(
                        this->m_model->getSortColumn(),
                        this->m_model->getSortOrder());
                }
            }

            //! Resize to content
            void resizeToContents()
            {
                this->resizeColumnsToContents();
                this->resizeRowsToContents();
            }

            //! Init
            void standardInit(ModelClass *model = nullptr)
            {
                Q_ASSERT(model || this->m_model);
                if (model) { this->m_model = model; }
                this->setModel(this->m_model); // via QTableView
                this->setSortIndicator();
                this->horizontalHeader()->setStretchLastSection(true);
            }
        };
    }
}
#endif // guard
