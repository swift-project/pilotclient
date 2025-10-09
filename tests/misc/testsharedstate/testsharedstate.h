// SPDX-FileCopyrightText: Copyright (C) 2020 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \cond PRIVATE_TESTS

/*!
 * \file
 * \ingroup testmisc
 */

#include <QMetaType>

#include "misc/sharedstate/datalink.h"
#include "misc/sharedstate/listjournal.h"
#include "misc/sharedstate/listmutator.h"
#include "misc/sharedstate/listobserver.h"
#include "misc/sharedstate/scalarjournal.h"
#include "misc/sharedstate/scalarmutator.h"
#include "misc/sharedstate/scalarobserver.h"

namespace MiscTest
{
    //! \private List filter for testing
    class CTestFilter : public swift::misc::CValueObject<CTestFilter>
    {
    public:
        bool matches(const swift::misc::CVariant &value) const
        {
            return value.canConvert<unsigned>() && (value.to<unsigned>() & m_mask);
        }
        QString convertToQString(bool = false) const { return QString::number(m_mask); }
        CTestFilter(unsigned mask = ~0u) : m_mask(mask) {}

    private:
        unsigned m_mask = ~0u;
        SWIFT_METACLASS(CTestFilter, SWIFT_METAMEMBER(mask));
    };
} // namespace MiscTest

//! \cond
Q_DECLARE_METATYPE(MiscTest::CTestFilter);
//! \endcond

namespace MiscTest
{
    //! Scalar mutator subclass
    class CTestScalarMutator : public swift::misc::shared_state::CScalarMutator<int>
    {
        Q_OBJECT
        SWIFT_SHARED_STATE_CHANNEL("test_scalar_channel")
    public:
        //! Ctor
        CTestScalarMutator(QObject *parent) : CScalarMutator(parent) {}
    };

    //! Scalar journal subclass
    class CTestScalarJournal : public swift::misc::shared_state::CScalarJournal<int>
    {
        Q_OBJECT
        SWIFT_SHARED_STATE_CHANNEL("test_scalar_channel")
    public:
        //! Ctor
        CTestScalarJournal(QObject *parent) : CScalarJournal(parent) {}
    };

    //! Scalar observer subclass
    class CTestScalarObserver : public swift::misc::shared_state::CScalarObserver<int>
    {
        Q_OBJECT
        SWIFT_SHARED_STATE_CHANNEL("test_scalar_channel")
    public:
        //! Ctor
        CTestScalarObserver(QObject *parent) : CScalarObserver(parent) {}
        void onValueChanged(const int &) override {}
    };

    //! List mutator subclass
    class CTestListMutator : public swift::misc::shared_state::CListMutator<QList<int>>
    {
        Q_OBJECT
        SWIFT_SHARED_STATE_CHANNEL("test_list_channel")
    public:
        //! Ctor
        CTestListMutator(QObject *parent) : CListMutator(parent) {}
    };

    //! List journal subclass
    class CTestListJournal : public swift::misc::shared_state::CListJournal<QList<int>>
    {
        Q_OBJECT
        SWIFT_SHARED_STATE_CHANNEL("test_list_channel")
    public:
        //! Ctor
        CTestListJournal(QObject *parent) : CListJournal(parent) {}
    };

    //! List observer subclass
    class CTestListObserver : public swift::misc::shared_state::CListObserver<QList<int>, CTestFilter>
    {
        Q_OBJECT
        SWIFT_SHARED_STATE_CHANNEL("test_list_channel")
    public:
        //! Ctor
        CTestListObserver(QObject *parent) : CListObserver(parent) {}

        //! \name Interface implementation
        //! @{

        //! \copydoc swift::misc::shared_state::CListObserver::onElementAdded
        void onElementAdded(const int &) override {}

        //! \copydoc swift::misc::shared_state::CListObserver::onElementsReplaced
        void onElementsReplaced(const QList<int> &) override {}
        //! @}
    };
} // namespace MiscTest

//! \endcond
