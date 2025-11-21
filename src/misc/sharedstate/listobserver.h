// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SHAREDSTATE_LISTOBSERVER_H
#define SWIFT_MISC_SHAREDSTATE_LISTOBSERVER_H

#include <QMutex>
#include <QObject>

#include "misc/sharedstate/activeobserver.h"
#include "misc/sharedstate/datalink.h"
#include "misc/swiftmiscexport.h"
#include "misc/variantlist.h"

namespace swift::misc::shared_state
{
    /*!
     * Non-template base class for CListObserver.
     * \ingroup shared_state
     */
    class SWIFT_MISC_EXPORT CGenericListObserver : public QObject
    {
        Q_OBJECT

    protected:
        //! Constructor.
        CGenericListObserver(QObject *parent) : QObject(parent) {}

        //! Subscribe using the given transport mechanism.
        virtual void initialize(IDataLink *);

        //! Set list filter as variant.
        void setFilter(const CVariant &filter);

        //! Get filtered list value as variant list.
        CVariantList allValues() const;

        //! Remove any old values that no longer match the filter.
        int cleanValues();

    private:
        void reconstruct();
        void handleEvent(const CVariant &param);
        virtual void onGenericElementAdded(const CVariant &value) = 0;
        virtual void onGenericElementsReplaced(const CVariantList &values) = 0;

        QSharedPointer<CActiveObserver> m_observer = CActiveObserver::create(this, &CGenericListObserver::handleEvent);
        CDataLinkConnectionWatcher *m_watcher = nullptr;
        mutable QMutex m_listMutex;
        CVariantList m_list;
    };

    /*!
     * Base class for an object that shares state with a corresponding CListMutator subclass object.
     * \tparam T Datatype encapsulating the state to be shared.
     * \tparam U Datatype describing a filter to apply to the list.
     * \ingroup shared_state
     */
    template <typename T, typename U = CAnyMatch>
    class CListObserver : public CGenericListObserver
    {
    protected:
        //! Constructor.
        CListObserver(QObject *parent) : CGenericListObserver(parent) {}

    public:
        //! Subscribe using the given transport mechanism.
        void initialize(IDataLink *dataLink) override
        {
            CGenericListObserver::initialize(dataLink);
            if (std::is_same_v<U, CAnyMatch>) { setFilter({}); }
        }

        //! Set filter to choose list elements.
        void setFilter(const U &filter) { CGenericListObserver::setFilter(CVariant::from(filter)); }

        //! Get list value containing all elements matching the filter.
        T allValues() const { return CVariant::from(CGenericListObserver::allValues()).template to<T>(); }

        //! Called when an element matching the filter is added to the list.
        virtual void onElementAdded(const typename T::value_type &value) = 0;

        //! Called when the whole list is updated wholesale.
        virtual void onElementsReplaced(const T &values) = 0;

    private:
        void onGenericElementAdded(const CVariant &value) final { onElementAdded(value.to<typename T::value_type>()); }
        void onGenericElementsReplaced(const CVariantList &values) final { onElementsReplaced(values.to<T>()); }
    };
} // namespace swift::misc::shared_state

#endif // SWIFT_MISC_SHAREDSTATE_LISTOBSERVER_H
