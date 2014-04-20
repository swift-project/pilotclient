/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKGUI_VIEWBASE_H
#define BLACKGUI_VIEWBASE_H

#include <QTableView>

namespace BlackGui
{
    /*!
     * \brief List model
     */
    template <class ModelClass> class CViewBase : public QTableView
    {

    protected:

        //! Constructor
        CViewBase(QWidget *parent) : QTableView(parent)
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
    };
}
#endif // guard
