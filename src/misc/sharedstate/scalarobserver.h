// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SHAREDSTATE_SCALAROBSERVER_H
#define SWIFT_MISC_SHAREDSTATE_SCALAROBSERVER_H

#include <QMutex>
#include <QObject>

#include "misc/sharedstate/activeobserver.h"
#include "misc/swiftmiscexport.h"
#include "misc/variant.h"

namespace swift::misc::shared_state
{
    class IDataLink;

    /*!
     * Non-template base class for CScalarObserver.
     * \ingroup shared_state
     */
    class SWIFT_MISC_EXPORT CGenericScalarObserver : public QObject
    {
        Q_OBJECT

    public:
        //! Subscribe using the given transport mechanism.
        void initialize(IDataLink *);

    protected:
        //! Constructor.
        CGenericScalarObserver(QObject *parent) : QObject(parent) {}

        //! Get scalar value as variant.
        CVariant value() const;

    private:
        void reconstruct();
        void handleEvent(const CVariant &param);
        virtual void onGenericValueChanged(const CVariant &value) = 0;

        QSharedPointer<CActiveObserver> m_observer =
            CActiveObserver::create(this, &CGenericScalarObserver::handleEvent);
        mutable QMutex m_valueMutex;
        CVariant m_value;
    };

    /*!
     * Base class for an object that shares state with a corresponding CScalarMutator subclass object.
     * \tparam T Datatype encapsulating the state to be shared.
     * \ingroup shared_state
     */
    template <typename T>
    class CScalarObserver : public CGenericScalarObserver
    {
    protected:
        //! Constructor.
        CScalarObserver(QObject *parent) : CGenericScalarObserver(parent) {}

    public:
        //! Get scalar value.
        T value() const { return CGenericScalarObserver::value().template to<T>(); }

    private:
        //! Called when the scalar value changes.
        virtual void onValueChanged(const T &value) = 0;

        void onGenericValueChanged(const CVariant &value) final { onValueChanged(value.to<T>()); }
    };
} // namespace swift::misc::shared_state

#endif // SWIFT_MISC_SHAREDSTATE_SCALAROBSERVER_H
