/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKGUI_VIEWBASE_H
#define BLACKGUI_VIEWBASE_H

//! \file

#include <QTableView>

namespace BlackGui
{
    namespace Views
    {
        //! List model
        template <class ModelClass> class CViewBase : public QTableView
        {

        protected:

            //! Constructor
            CViewBase(QWidget *parent, ModelClass *model = nullptr) : QTableView(parent), m_model(model)
            {
                this->setSortingEnabled(true);
                this->horizontalHeader()->setStretchLastSection(true);
            }

            //! Destructor
            virtual ~CViewBase() {}

            ModelClass *m_model; //!< corresponding model

        public:

            //! Model
            ModelClass *derivedModel() { return this->m_model; }

            //! Model
            const ModelClass *derivedModel() const { return this->m_model; }

            //! Clear
            void clear() { Q_ASSERT(this->m_model); this->m_model->clear(); }

            //! Update
            template<class ContainerType> int update(const ContainerType &container, bool resize = true)
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

            //! Any data?
            bool isEmpty() const
            {
                Q_ASSERT(this->m_model);
                return this->m_model->rowCount() < 1;
            }
        };
    }
}
#endif // guard
