/* Copyright (C) 2014
 * swift Project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_LOGCATEGORY_H
#define BLACKMISC_LOGCATEGORY_H

#include "blackmisc/metaclass.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/blackmiscexport.h"

#include <QList>
#include <QMetaType>
#include <QString>

namespace BlackMisc
{
    /*!
     * A log category is an arbitrary string tag which can be attached to log messages.
     *
     * A log handler can filter messages based on their categories.
     */
    class BLACKMISC_EXPORT CLogCategory : public CValueObject<CLogCategory>
    {
    public:
        //! \name Predefined special categories (public static methods)
        //! @{

        //! Uncategorized
        static const CLogCategory &uncategorized()
        {
            static const CLogCategory cat { "swift.uncategorized" };
            return cat;
        }

        //! Verification
        static const CLogCategory &verification()
        {
            static const CLogCategory cat { "swift.verification" };
            return cat;
        }

        //! Validation
        static const CLogCategory &validation()
        {
            static const CLogCategory cat { "swift.validation" };
            return cat;
        }

        //! Core/base services such as caching etc.
        static const CLogCategory &services()
        {
            static const CLogCategory cat { "swift.services" };
            return cat;
        }

        //! Audio related
        static const CLogCategory &audio()
        {
            static const CLogCategory cat { "swift.audio" };
            return cat;
        }

        //! Contexts
        static const CLogCategory &context()
        {
            static const CLogCategory cat { "swift.context" };
            return cat;
        }

        //! Interpolator
        static const CLogCategory &interpolator()
        {
            static const CLogCategory cat { "swift.interpolator" };
            return cat;
        }

        //! Flight plan
        static const CLogCategory &flightPlan()
        {
            static const CLogCategory cat { "swift.flightplan" };
            return cat;
        }

        //! Context slots
        static const CLogCategory &contextSlot()
        {
            static const CLogCategory cat { "swift.context.slot" };
            return cat;
        }

        //! GUI components
        static const CLogCategory &guiComponent()
        {
            static const CLogCategory cat { "swift.gui.component" };
            return cat;
        }

        //! Generic downloads
        static const CLogCategory &download()
        {
            static const CLogCategory cat { "swift.download" };
            return cat;
        }

        //! Webservice
        static const CLogCategory &webservice()
        {
            static const CLogCategory cat { "swift.webservice" };
            return cat;
        }

        //! Mapping
        static const CLogCategory &mapping()
        {
            static const CLogCategory cat { "swift.mapping" };
            return cat;
        }

        //! Matching
        static const CLogCategory &matching()
        {
            static const CLogCategory cat { "swift.matching" };
            return cat;
        }

        //! Settings
        static const CLogCategory &settings()
        {
            static const CLogCategory cat { "swift.settings" };
            return cat;
        }

        //! Cache
        static const CLogCategory &cache()
        {
            static const CLogCategory cat { "swift.cache" };
            return cat;
        }

        //! Cmd.line parsing
        static const CLogCategory &cmdLine()
        {
            static const CLogCategory cat { "swift.cmdLine" };
            return cat;
        }

        //! Driver
        static const CLogCategory &driver()
        {
            static const CLogCategory cat { "swift.driver" };
            return cat;
        }

        //! Model loader
        static const CLogCategory &modelLoader()
        {
            static const CLogCategory cat { "swift.modelloader" };
            return cat;
        }

        //! Model cache
        static const CLogCategory &modelCache()
        {
            static const CLogCategory cat { "swift.modelcache" };
            return cat;
        }

        //! Model set cache
        static const CLogCategory &modelSetCache()
        {
            static const CLogCategory cat { "swift.modelsetcache" };
            return cat;
        }

        //! Model UI
        static const CLogCategory &modelGui()
        {
            static const CLogCategory cat { "swift.modelui" };
            return cat;
        }

        //! Plugin
        static const CLogCategory &plugin()
        {
            static const CLogCategory cat { "swift.plugin" };
            return cat;
        }

        //! Wizard
        static const CLogCategory &wizard()
        {
            static const CLogCategory cat { "swift.wizard" };
            return cat;
        }

        //! Background task
        static const CLogCategory &worker()
        {
            static const CLogCategory cat { "swift.worker" };
            return cat;
        }

        //! Data inconsistency
        static const CLogCategory &dataInconsistency()
        {
            static const CLogCategory cat { "swift.datainconsistency" };
            return cat;
        }

        //! JSON and JSON conversions
        static const CLogCategory &json()
        {
            static const CLogCategory cat { "swift.json" };
            return cat;
        }

        //! Startup of application
        static const CLogCategory &startup()
        {
            static const CLogCategory cat { "swift.startup" };
            return cat;
        }

        //! Webservice with swift DB
        static const CLogCategory &swiftDbWebservice()
        {
            static const CLogCategory cat { "swift.db.webservice" };
            return cat;
        }

        //! swift GUI
        static const CLogCategory &swiftPilotClient()
        {
            static const CLogCategory cat { "SwiftGuiStd" };
            return cat;
        }

        //! swift data tool (aka mapping tool)
        static const CLogCategory &swiftDataTool()
        {
            static const CLogCategory cat { "CSwiftData" };
            return cat;
        }

        //! swift core
        static const CLogCategory &swiftCore()
        {
            static const CLogCategory cat { "CSwiftCore" };
            return cat;
        }

        //! VATSIM specific
        static const CLogCategory &vatsimSpecific()
        {
            static const CLogCategory cat { "swift.vatsim" };
            return cat;
        }

        //! Network specific, but not necessarily one specific flight network
        static const CLogCategory &network()
        {
            static const CLogCategory cat { "swift.network" };
            return cat;
        }

        //! Aviation specific
        static const CLogCategory &aviation()
        {
            static const CLogCategory cat { "swift.aviation" };
            return cat;
        }

        //! All predefined special categories
        //! \note Human readable patterns are defined in CLogPattern::allHumanReadablePatterns
        static const QList<CLogCategory> &allSpecialCategories()
        {
            static const QList<CLogCategory> cats
            {
                uncategorized(),
                aviation(),
                audio(),
                cache(),
                cmdLine(),
                context(),
                contextSlot(),
                dataInconsistency(),
                download(),
                driver(),
                flightPlan(),
                guiComponent(),
                interpolator(),
                json(),
                mapping(),
                matching(),
                modelLoader(),
                modelCache(),
                modelSetCache(),
                modelGui(),
                network(),
                plugin(),
                swiftDbWebservice(),
                swiftCore(),
                swiftDataTool(),
                swiftPilotClient(),
                services(),
                settings(),
                startup(),
                validation(),
                vatsimSpecific(),
                verification(),
                webservice(),
                wizard(),
                worker()
            };
            return cats;
        }

        //! @}

        //! Constructor.
        CLogCategory() = default;

        //! Constructor.
        CLogCategory(const QString &categoryString) : m_string(categoryString) {}

        //! Constructor.
        CLogCategory(const char *categoryString) : m_string(categoryString) {}

        //! Returns true if the category string starts with the given prefix.
        bool startsWith(const QString &prefix) const { return m_string.startsWith(prefix); }

        //! Returns true if the category string ends with the given suffix.
        bool endsWith(const QString &suffix) const { return m_string.endsWith(suffix); }

        //! Returns true if the category string contains the given substring.
        bool contains(const QString &substring) const { return m_string.contains(substring); }

        //! \copydoc BlackMisc::Mixin::String::toQString()
        QString convertToQString(bool i18n = false) const;

    private:
        QString m_string;

        BLACK_METACLASS(
            CLogCategory,
            BLACK_METAMEMBER(string)
        );
    };
} // ns

Q_DECLARE_METATYPE(BlackMisc::CLogCategory)

#endif
