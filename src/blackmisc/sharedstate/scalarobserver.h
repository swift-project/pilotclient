/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SHAREDSTATE_SCALAROBSERVER_H
#define BLACKMISC_SHAREDSTATE_SCALAROBSERVER_H

#include "blackmisc/sharedstate/activeobserver.h"
#include "blackmisc/variant.h"
#include "blackmisc/blackmiscexport.h"
#include <QObject>
#include <QMutex>

namespace BlackMisc
{
    namespace SharedState
    {
        class IDataLink;

        /*!
         * Non-template base class for CScalarObserver.
         * \ingroup SharedState
         */
        class BLACKMISC_EXPORT CGenericScalarObserver : public QObject
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

            QSharedPointer<CActiveObserver> m_observer = CActiveObserver::create(this, &CGenericScalarObserver::handleEvent);
            mutable QMutex m_valueMutex;
            CVariant m_value;
        };

        /*!
         * Base class for an object that shares state with a corresponding CScalarMutator subclass object.
         * \tparam T Datatype encapsulating the state to be shared.
         * \ingroup SharedState
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

            virtual void onGenericValueChanged(const CVariant &value) override final { onValueChanged(value.to<T>()); }
        };
    }
}

#endif
