/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_EDITORS_FORM_H
#define BLACKGUI_EDITORS_FORM_H

#include <QFrame>

namespace BlackGui
{
    namespace Editors
    {
        /*!
         * Form base class
         */
        class CForm : public QFrame
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CForm(QWidget *parent = nullptr);

            //! Destructor
            ~CForm();

            //! Set editable
            virtual void setReadOnly(bool readOnly) = 0;

            //! Read only, but entity can be selected (normally used in mapping).
            //! Use setReadOnly to reset this very state
            virtual void setSelectOnly() = 0;

            //! Is read only?
            bool isReadOnly() const { return m_readOnly; }

        protected:
            bool m_readOnly = false; //!< read only
        };

    } // ns
} // ns

#endif // guard
