// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_LOGCATEGORIES_H
#define SWIFT_MISC_LOGCATEGORIES_H

#include <QString>

#include "misc/swiftmiscexport.h"

namespace swift::misc
{
    /*!
     * Predefined special log categories.
     *
     * \see CLogCategory
     */
    class SWIFT_MISC_EXPORT CLogCategories
    {
    public:
        //! Uncategorized
        static const QString &uncategorized()
        {
            static const QString cat { "swift.uncategorized" };
            return cat;
        }

        //! Verification
        static const QString &verification()
        {
            static const QString cat { "swift.verification" };
            return cat;
        }

        //! Validation
        static const QString &validation()
        {
            static const QString cat { "swift.validation" };
            return cat;
        }

        //! Core/base services such as caching etc.
        static const QString &services()
        {
            static const QString cat { "swift.services" };
            return cat;
        }

        //! Audio related
        static const QString &audio()
        {
            static const QString cat { "swift.audio" };
            return cat;
        }

        //! DBus related
        static const QString &dbus()
        {
            static const QString cat { "swift.dbus" };
            return cat;
        }

        //! Contexts
        static const QString &context()
        {
            static const QString cat { "swift.context" };
            return cat;
        }

        //! Interpolator
        static const QString &interpolator()
        {
            static const QString cat { "swift.interpolator" };
            return cat;
        }

        //! Flight plan
        static const QString &flightPlan()
        {
            static const QString cat { "swift.flightplan" };
            return cat;
        }

        //! Context slots
        static const QString &contextSlot()
        {
            static const QString cat { "swift.context.slot" };
            return cat;
        }

        //! GUI components
        static const QString &guiComponent()
        {
            static const QString cat { "swift.gui.component" };
            return cat;
        }

        //! Generic downloads
        static const QString &download()
        {
            static const QString cat { "swift.download" };
            return cat;
        }

        //! Webservice
        static const QString &webservice()
        {
            static const QString cat { "swift.webservice" };
            return cat;
        }

        //! Mapping
        static const QString &mapping()
        {
            static const QString cat { "swift.mapping" };
            return cat;
        }

        //! Matching
        static const QString &matching()
        {
            static const QString cat { "swift.matching" };
            return cat;
        }

        //! Settings
        static const QString &settings()
        {
            static const QString cat { "swift.settings" };
            return cat;
        }

        //! Cache
        static const QString &cache()
        {
            static const QString cat { "swift.cache" };
            return cat;
        }

        //! Cmd.line parsing
        static const QString &cmdLine()
        {
            static const QString cat { "swift.cmdLine" };
            return cat;
        }

        //! Driver
        static const QString &driver()
        {
            static const QString cat { "swift.driver" };
            return cat;
        }

        //! Model loader
        static const QString &modelLoader()
        {
            static const QString cat { "swift.modelloader" };
            return cat;
        }

        //! Model cache
        static const QString &modelCache()
        {
            static const QString cat { "swift.modelcache" };
            return cat;
        }

        //! Model set cache
        static const QString &modelSetCache()
        {
            static const QString cat { "swift.modelsetcache" };
            return cat;
        }

        //! Model UI
        static const QString &modelGui()
        {
            static const QString cat { "swift.modelui" };
            return cat;
        }

        //! Plugin
        static const QString &plugin()
        {
            static const QString cat { "swift.plugin" };
            return cat;
        }

        //! Wizard
        static const QString &wizard()
        {
            static const QString cat { "swift.wizard" };
            return cat;
        }

        //! Background task
        static const QString &worker()
        {
            static const QString cat { "swift.worker" };
            return cat;
        }

        //! Data inconsistency
        static const QString &dataInconsistency()
        {
            static const QString cat { "swift.datainconsistency" };
            return cat;
        }

        //! JSON and JSON conversions
        static const QString &json()
        {
            static const QString cat { "swift.json" };
            return cat;
        }

        //! Startup of application
        static const QString &startup()
        {
            static const QString cat { "swift.startup" };
            return cat;
        }

        //! Webservice with swift DB
        static const QString &swiftDbWebservice()
        {
            static const QString cat { "swift.db.webservice" };
            return cat;
        }

        //! swift GUI
        static const QString &swiftPilotClient()
        {
            static const QString cat { "SwiftGuiStd" };
            return cat;
        }

        //! swift data tool (aka mapping tool)
        static const QString &swiftDataTool()
        {
            static const QString cat { "CSwiftData" };
            return cat;
        }

        //! swift core
        static const QString &swiftCore()
        {
            static const QString cat { "CSwiftCore" };
            return cat;
        }

        //! VATSIM specific
        static const QString &vatsimSpecific()
        {
            static const QString cat { "swift.vatsim" };
            return cat;
        }

        //! Network specific, but not necessarily one specific flight network
        static const QString &network()
        {
            static const QString cat { "swift.network" };
            return cat;
        }

        //! FSD specific
        static const QString &fsd()
        {
            static const QString cat { "swift.fsd" };
            return cat;
        }

        //! Aviation specific
        static const QString &aviation()
        {
            static const QString cat { "swift.aviation" };
            return cat;
        }

        //! All predefined special categories
        //! \note Human readable patterns are defined in CLogPattern::allHumanReadablePatterns
        static const QStringList &allSpecialCategories()
        {
            static const QStringList cats { uncategorized(),
                                            aviation(),
                                            audio(),
                                            cache(),
                                            cmdLine(),
                                            context(),
                                            contextSlot(),
                                            dbus(),
                                            dataInconsistency(),
                                            download(),
                                            driver(),
                                            flightPlan(),
                                            fsd(),
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
                                            worker() };
            return cats;
        }
    };
} // namespace swift::misc

#endif
