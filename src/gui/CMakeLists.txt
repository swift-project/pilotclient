# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

add_library(gui SHARED
        altitudeedit.cpp
        altitudeedit.h
        components/aboutdialog.cpp
        components/aboutdialog.h
        components/aboutdialog.ui
        components/abouthtmlcomponent.cpp
        components/abouthtmlcomponent.h
        components/abouthtmlcomponent.ui
        components/aircraftcombinedtypeselector.cpp
        components/aircraftcombinedtypeselector.h
        components/aircraftcombinedtypeselector.ui
        components/aircraftcomponent.cpp
        components/aircraftcomponent.h
        components/aircraftcomponent.ui
        components/aircraftmodelsetvalidationdialog.cpp
        components/aircraftmodelsetvalidationdialog.h
        components/aircraftmodelsetvalidationdialog.ui
        components/aircraftmodelstringcompleter.cpp
        components/aircraftmodelstringcompleter.h
        components/aircraftmodelstringcompleter.ui
        components/aircraftmodelvalidationcomponent.cpp
        components/aircraftmodelvalidationcomponent.h
        components/aircraftmodelvalidationcomponent.ui
        components/aircraftpartscomponent.cpp
        components/aircraftpartscomponent.h
        components/aircraftpartscomponent.ui
        components/aircraftpartshistory.cpp
        components/aircraftpartshistory.h
        components/aircraftpartshistory.ui
        components/airportcompleter.cpp
        components/airportcompleter.h
        components/airportcompleter.ui
        components/airportdialog.cpp
        components/airportdialog.h
        components/airportdialog.ui
        components/airportsmallcompleter.cpp
        components/airportsmallcompleter.h
        components/airportsmallcompleter.ui
        components/allmaininfoareacomponents.h
        components/altitudedialog.cpp
        components/altitudedialog.h
        components/altitudedialog.ui
        components/applicationclosedialog.cpp
        components/applicationclosedialog.h
        components/applicationclosedialog.ui
        components/atcbuttoncomponent.cpp
        components/atcbuttoncomponent.h
        components/atcbuttoncomponent.ui
        components/atcstationcomponent.cpp
        components/atcstationcomponent.h
        components/atcstationcomponent.ui
        components/audioadvanceddistributedcomponent.cpp
        components/audioadvanceddistributedcomponent.h
        components/audioadvanceddistributedcomponent.ui
        components/audioadvanceddistributeddialog.cpp
        components/audioadvanceddistributeddialog.h
        components/audioadvanceddistributeddialog.ui
        components/audiodevicevolumesetupcomponent.cpp
        components/audiodevicevolumesetupcomponent.h
        components/audiodevicevolumesetupcomponent.ui
        components/audionotificationcomponent.cpp
        components/audionotificationcomponent.h
        components/audionotificationcomponent.ui
        components/autopublishcomponent.cpp
        components/autopublishcomponent.h
        components/autopublishcomponent.ui
        components/autopublishdialog.cpp
        components/autopublishdialog.h
        components/autopublishdialog.ui
        components/callsigncompleter.cpp
        components/callsigncompleter.h
        components/callsigncompleter.ui
        components/cgsourceselector.cpp
        components/cgsourceselector.h
        components/cgsourceselector.ui
        components/cockpitcomaudiodialog.cpp
        components/cockpitcomaudiodialog.h
        components/cockpitcomaudiodialog.ui
        components/cockpitcomcomponent.cpp
        components/cockpitcomcomponent.h
        components/cockpitcomcomponent.ui
        components/cockpitcomponent.cpp
        components/cockpitcomponent.h
        components/cockpitcomponent.ui
        components/cockpitcomtransmissioncomponent.cpp
        components/cockpitcomtransmissioncomponent.h
        components/cockpitcomtransmissioncomponent.ui
        components/cockpitinfoareacomponent.cpp
        components/cockpitinfoareacomponent.h
        components/cockpitinfoareacomponent.ui
        components/cockpittranspondermodeledscomponent.cpp
        components/cockpittranspondermodeledscomponent.h
        components/colorselector.cpp
        components/colorselector.h
        components/colorselector.ui
        components/commandinput.cpp
        components/commandinput.h
        components/configsimulatorcomponent.cpp
        components/configsimulatorcomponent.h
        components/configsimulatorcomponent.ui
        components/configurationwizard.cpp
        components/configurationwizard.h
        components/configurationwizard.ui
        components/coordinatedialog.cpp
        components/coordinatedialog.h
        components/coordinatedialog.ui
        components/copymodelsfromotherswiftversionscomponent.cpp
        components/copymodelsfromotherswiftversionscomponent.h
        components/copymodelsfromotherswiftversionscomponent.ui
        components/copymodelsfromotherswiftversionsdialog.cpp
        components/copymodelsfromotherswiftversionsdialog.h
        components/copymodelsfromotherswiftversionsdialog.ui
        components/copysettingsandcachescomponent.cpp
        components/copysettingsandcachescomponent.h
        components/copysettingsandcachescomponent.ui
        components/coreinfoareacomponent.cpp
        components/coreinfoareacomponent.h
        components/coreinfoareacomponent.ui
        components/coresettingsdialog.cpp
        components/coresettingsdialog.h
        components/coresettingsdialog.ui
        components/corestatuscomponent.cpp
        components/corestatuscomponent.h
        components/corestatuscomponent.ui
        components/countryselector.cpp
        components/countryselector.h
        components/countryselector.ui
        components/datainfoareacomponent.cpp
        components/datainfoareacomponent.h
        components/datainfoareacomponent.ui
        components/datamaininfoareacomponent.cpp
        components/datamaininfoareacomponent.h
        components/datamaininfoareacomponent.ui
        components/datasettingscomponent.cpp
        components/datasettingscomponent.h
        components/datasettingscomponent.ui
        components/dbaircraftcategorycomponent.cpp
        components/dbaircraftcategorycomponent.h
        components/dbaircraftcategorycomponent.ui
        components/dbaircrafticaocomponent.cpp
        components/dbaircrafticaocomponent.h
        components/dbaircrafticaocomponent.ui
        components/dbaircrafticaoselectorcomponent.cpp
        components/dbaircrafticaoselectorcomponent.h
        components/dbaircrafticaoselectorcomponent.ui
        components/dbairlineicaocomponent.cpp
        components/dbairlineicaocomponent.h
        components/dbairlineicaocomponent.ui
        components/dbairlineicaoselectorbase.cpp
        components/dbairlineicaoselectorbase.h
        components/dbairlineicaoselectorcomponent.cpp
        components/dbairlineicaoselectorcomponent.h
        components/dbairlineicaoselectorcomponent.ui
        components/dbairlinenameselectorcomponent.cpp
        components/dbairlinenameselectorcomponent.h
        components/dbairlinenameselectorcomponent.ui
        components/dbautosimulatorstashingcomponent.cpp
        components/dbautosimulatorstashingcomponent.h
        components/dbautosimulatorstashingcomponent.ui
        components/dbautostashingcomponent.cpp
        components/dbautostashingcomponent.h
        components/dbautostashingcomponent.ui
        components/dbcountrycomponent.cpp
        components/dbcountrycomponent.h
        components/dbcountrycomponent.ui
        components/dbcountryselectorcomponent.cpp
        components/dbcountryselectorcomponent.h
        components/dbcountryselectorcomponent.ui
        components/dbdistributorcomponent.cpp
        components/dbdistributorcomponent.h
        components/dbdistributorcomponent.ui
        components/dbdistributorselectorcomponent.cpp
        components/dbdistributorselectorcomponent.h
        components/dbdistributorselectorcomponent.ui
        components/dbliverycolorsearch.cpp
        components/dbliverycolorsearch.h
        components/dbliverycolorsearch.ui
        components/dbliverycolorsearchdialog.cpp
        components/dbliverycolorsearchdialog.h
        components/dbliverycolorsearchdialog.ui
        components/dbliverycomponent.cpp
        components/dbliverycomponent.h
        components/dbliverycomponent.ui
        components/dbliveryselectorcomponent.cpp
        components/dbliveryselectorcomponent.h
        components/dbliveryselectorcomponent.ui
        components/dbloaddatadialog.cpp
        components/dbloaddatadialog.h
        components/dbloaddatadialog.ui
        components/dbloadoverviewcomponent.cpp
        components/dbloadoverviewcomponent.h
        components/dbloadoverviewcomponent.ui
        components/dblogincomponent.cpp
        components/dblogincomponent.h
        components/dblogincomponent.ui
        components/dbmappingcomponent.cpp
        components/dbmappingcomponent.h
        components/dbmappingcomponent.ui
        components/dbmappingcomponentaware.cpp
        components/dbmappingcomponentaware.h
        components/dbmodelcomponent.cpp
        components/dbmodelcomponent.h
        components/dbmodelcomponent.ui
        components/dbmodelkeycompleter.cpp
        components/dbmodelkeycompleter.h
        components/dbmodelmappingmodifydialog.cpp
        components/dbmodelmappingmodifydialog.h
        components/dbmodelmappingmodifydialog.ui
        components/dbmodelworkbenchcomponent.cpp
        components/dbmodelworkbenchcomponent.h
        components/dbmodelworkbenchcomponent.ui
        components/dbownmodelscomponent.cpp
        components/dbownmodelscomponent.h
        components/dbownmodelscomponent.ui
        components/dbownmodelsdialog.cpp
        components/dbownmodelsdialog.h
        components/dbownmodelsdialog.ui
        components/dbownmodelsetcomponent.cpp
        components/dbownmodelsetcomponent.h
        components/dbownmodelsetcomponent.ui
        components/dbownmodelsetdialog.cpp
        components/dbownmodelsetdialog.h
        components/dbownmodelsetdialog.ui
        components/dbownmodelsetformdialog.cpp
        components/dbownmodelsetformdialog.h
        components/dbownmodelsetformdialog.ui
        #components/dbquickmappingwizard.cpp
        #components/dbquickmappingwizard.h
        #components/dbquickmappingwizard.ui
        components/dbreducemodelduplicates.cpp
        components/dbreducemodelduplicates.h
        components/dbreducemodelduplicates.ui
        components/dbstashcomponent.cpp
        components/dbstashcomponent.h
        components/dbstashcomponent.ui
        components/dbusserveraddressselector.cpp
        components/dbusserveraddressselector.h
        components/dbusserveraddressselector.ui
        components/distributorpreferencescomponent.cpp
        components/distributorpreferencescomponent.h
        components/distributorpreferencescomponent.ui
        components/downloadcomponent.cpp
        components/downloadcomponent.h
        components/downloadcomponent.ui
        components/downloaddialog.cpp
        components/downloaddialog.h
        components/downloaddialog.ui
        components/firstmodelsetcomponent.cpp
        components/firstmodelsetcomponent.h
        components/firstmodelsetcomponent.ui
        components/firstmodelsetdialog.cpp
        components/firstmodelsetdialog.h
        components/firstmodelsetdialog.ui
        components/flightplancomponent.cpp
        components/flightplancomponent.h
        components/flightplancomponent.ui
        components/hotkeydialog.cpp
        components/hotkeydialog.h
        components/hotkeydialog.ui
        components/infobarstatuscomponent.cpp
        components/infobarstatuscomponent.h
        components/infobarstatuscomponent.ui
        components/infobarwebreadersstatuscomponent.cpp
        components/infobarwebreadersstatuscomponent.h
        components/infobarwebreadersstatuscomponent.ui
        components/infobarwebreadersstatussmallcomponent.cpp
        components/infobarwebreadersstatussmallcomponent.h
        components/infobarwebreadersstatussmallcomponent.ui
        components/initialdataloadcomponent.cpp
        components/initialdataloadcomponent.h
        components/initialdataloadcomponent.ui
        components/installfsxterrainprobecomponent.cpp
        components/installfsxterrainprobecomponent.h
        components/installfsxterrainprobecomponent.ui
        components/installsimulatorspecificwizardpage.h
        components/installxswiftbuscomponent.cpp
        components/installxswiftbuscomponent.h
        components/installxswiftbuscomponent.ui
        components/installxswiftbusdialog.cpp
        components/installxswiftbusdialog.h
        components/installxswiftbusdialog.ui
        components/internalscomponent.cpp
        components/internalscomponent.h
        components/internalscomponent.ui
        components/interpolationcomponent.cpp
        components/interpolationcomponent.h
        components/interpolationcomponent.ui
        components/interpolationlogdisplay.cpp
        components/interpolationlogdisplay.h
        components/interpolationlogdisplay.ui
        components/interpolationlogdisplaydialog.cpp
        components/interpolationlogdisplaydialog.h
        components/interpolationlogdisplaydialog.ui
        components/interpolationsetupcomponent.cpp
        components/interpolationsetupcomponent.h
        components/interpolationsetupcomponent.ui
        components/legalinfocomponent.cpp
        components/legalinfocomponent.h
        components/legalinfocomponent.ui
        components/logcomponent.cpp
        components/logcomponent.h
        components/logcomponent.ui
        components/logincomponent.cpp
        components/logincomponent.h
        components/logincomponent.ui
        components/maininfoareacomponent.cpp
        components/maininfoareacomponent.h
        components/maininfoareacomponent.ui
        components/mainkeypadareacomponent.cpp
        components/mainkeypadareacomponent.h
        components/mainkeypadareacomponent.ui
        components/mappingcomponent.cpp
        components/mappingcomponent.h
        components/mappingcomponent.ui
        components/marginsinput.cpp
        components/marginsinput.h
        components/marginsinput.ui
        components/matchingstatisticscomponent.cpp
        components/matchingstatisticscomponent.h
        components/matchingstatisticscomponent.ui
        components/modelbrowsercomponent.cpp
        components/modelbrowsercomponent.h
        components/modelbrowsercomponent.ui
        components/modelbrowserdialog.cpp
        components/modelbrowserdialog.h
        components/modelbrowserdialog.ui
        components/modelconverterxsetupcomponent.cpp
        components/modelconverterxsetupcomponent.h
        components/modelconverterxsetupcomponent.ui
        components/modelmatchercomponent.cpp
        components/modelmatchercomponent.h
        components/modelmatchercomponent.ui
        components/modelmatcherlogcomponent.cpp
        components/modelmatcherlogcomponent.h
        components/modelmatcherlogcomponent.ui
        components/modelmatcherlogenable.cpp
        components/modelmatcherlogenable.h
        components/modelmatcherlogenable.ui
        components/modelmodeselector.cpp
        components/modelmodeselector.h
        components/modelmodeselector.ui
        components/navigatordialog.cpp
        components/navigatordialog.h
        components/navigatordialog.ui
        components/networkaircraftvaluescomponent.cpp
        components/networkaircraftvaluescomponent.h
        components/networkaircraftvaluescomponent.ui
        components/networkdetailscomponent.cpp
        components/networkdetailscomponent.h
        components/networkdetailscomponent.ui
        components/otherswiftversionscomponent.cpp
        components/otherswiftversionscomponent.h
        components/otherswiftversionscomponent.ui
        components/otherswiftversionsdialog.cpp
        components/otherswiftversionsdialog.h
        components/otherswiftversionsdialog.ui
        components/ownaircraftcomponent.cpp
        components/ownaircraftcomponent.h
        components/ownaircraftcomponent.ui
        components/radarcomponent.cpp
        components/radarcomponent.h
        components/radarcomponent.ui
        components/rawfsdmessagescomponent.cpp
        components/rawfsdmessagescomponent.h
        components/rawfsdmessagescomponent.ui
        components/rawfsdmessagesdialog.cpp
        components/rawfsdmessagesdialog.h
        components/rawfsdmessagesdialog.ui
        components/registercomponent.cpp
        components/registercomponent.h
        components/registercomponent.ui
        components/registermetadatacomponents.cpp
        components/registermetadatacomponents.h
        components/remoteaircraftselector.cpp
        components/remoteaircraftselector.h
        components/remoteaircraftselector.ui
        components/scalescreenfactor.cpp
        components/scalescreenfactor.h
        components/scalescreenfactor.ui
        components/selcalcodeselector.cpp
        components/selcalcodeselector.h
        components/selcalcodeselector.ui
        components/serverlistselector.cpp
        components/serverlistselector.h
        components/settingsadvancedcomponent.cpp
        components/settingsadvancedcomponent.h
        components/settingsadvancedcomponent.ui
        components/settingscomponent.cpp
        components/settingscomponent.h
        components/settingscomponent.ui
        components/settingsfontcomponent.cpp
        components/settingsfontcomponent.h
        components/settingsfontcomponent.ui
        components/settingsfontdialog.cpp
        components/settingsfontdialog.h
        components/settingsfontdialog.ui
        components/settingsguicomponent.cpp
        components/settingsguicomponent.h
        components/settingsguicomponent.ui
        components/settingshotkeycomponent.cpp
        components/settingshotkeycomponent.h
        components/settingshotkeycomponent.ui
        components/settingsmatchingcomponent.cpp
        components/settingsmatchingcomponent.h
        components/settingsmatchingcomponent.ui
        components/settingsmatchingdialog.cpp
        components/settingsmatchingdialog.h
        components/settingsmatchingdialog.ui
        components/settingsmodelcomponent.cpp
        components/settingsmodelcomponent.h
        components/settingsmodelcomponent.ui
        components/settingsnetworkcomponent.cpp
        components/settingsnetworkcomponent.h
        components/settingsnetworkcomponent.ui
        components/settingsnetworkserverscomponent.cpp
        components/settingsnetworkserverscomponent.h
        components/settingsnetworkserverscomponent.ui
        components/settingssimulatorbasicscomponent.cpp
        components/settingssimulatorbasicscomponent.h
        components/settingssimulatorbasicscomponent.ui
        components/settingssimulatorcomponent.cpp
        components/settingssimulatorcomponent.h
        components/settingssimulatorcomponent.ui
        components/settingssimulatormessagescomponent.cpp
        components/settingssimulatormessagescomponent.h
        components/settingssimulatormessagescomponent.ui
        components/settingsswiftplugin.cpp
        components/settingsswiftplugin.h
        components/settingsswiftplugin.ui
        components/settingstextmessageinlinecomponent.cpp
        components/settingstextmessageinlinecomponent.h
        components/settingstextmessagestyle.cpp
        components/settingstextmessagestyle.h
        components/settingstextmessagestyle.ui
        components/settingsvatsimreaderscomponent.cpp
        components/settingsvatsimreaderscomponent.h
        components/settingsvatsimreaderscomponent.ui
        components/settingsviewupdatetimes.cpp
        components/settingsviewupdatetimes.h
        components/settingsviewupdatetimes.ui
        components/settingsxswiftbuscomponent.cpp
        components/settingsxswiftbuscomponent.h
        components/settingsxswiftbuscomponent.ui
        components/setuploadingdialog.cpp
        components/setuploadingdialog.h
        components/setuploadingdialog.ui
        components/simbriefdownloaddialog.cpp
        components/simbriefdownloaddialog.h
        components/simbriefdownloaddialog.ui
        components/simulatorcomponent.cpp
        components/simulatorcomponent.h
        components/simulatorcomponent.ui
        components/simulatorselector.cpp
        components/simulatorselector.h
        components/simulatorselector.ui
        components/statusmessageform.cpp
        components/statusmessageform.h
        components/statusmessageform.ui
        components/statusmessageformsmall.cpp
        components/statusmessageformsmall.h
        components/statusmessageformsmall.ui
        components/statusmessagesdetail.cpp
        components/statusmessagesdetail.h
        components/statusmessagesdetail.ui
        components/stringlistdialog.cpp
        components/stringlistdialog.h
        components/stringlistdialog.ui
        components/texteditdialog.cpp
        components/texteditdialog.h
        components/texteditdialog.ui
        components/textmessagecomponent.cpp
        components/textmessagecomponent.h
        components/textmessagecomponent.ui
        components/textmessagecomponenttab.h
        components/transpondercodespinbox.cpp
        components/transpondercodespinbox.h
        components/transpondermodecomponent.cpp
        components/transpondermodecomponent.h
        components/transpondermodecomponent.ui
        components/transpondermodeselector.cpp
        components/transpondermodeselector.h
        components/updateinfocomponent.cpp
        components/updateinfocomponent.h
        components/updateinfocomponent.ui
        components/updateinfodialog.cpp
        components/updateinfodialog.h
        components/updateinfodialog.ui
        components/usercomponent.cpp
        components/usercomponent.h
        components/usercomponent.ui
        dockwidget.cpp
        dockwidget.h
        dockwidgetinfoarea.cpp
        dockwidgetinfoarea.h
        dockwidgetinfobar.cpp
        dockwidgetinfobar.h
        dropbase.cpp
        dropbase.h
        dropsite.cpp
        dropsite.h
        ecosystemcombobox.cpp
        ecosystemcombobox.h
        editors/aircrafticaoform.cpp
        editors/aircrafticaoform.h
        editors/aircrafticaoform.ui
        editors/aircraftmodelform.cpp
        editors/aircraftmodelform.h
        editors/aircraftmodelform.ui
        editors/aircraftpartsform.cpp
        editors/aircraftpartsform.h
        editors/aircraftpartsform.ui
        editors/airlineicaoform.cpp
        editors/airlineicaoform.h
        editors/airlineicaoform.ui
        editors/cockpitcomform.cpp
        editors/cockpitcomform.h
        editors/cockpitcomform.ui
        editors/coordinateform.cpp
        editors/coordinateform.h
        editors/coordinateform.ui
        editors/distributorform.cpp
        editors/distributorform.h
        editors/distributorform.ui
        editors/form.cpp
        editors/form.h
        editors/fsdsetupform.cpp
        editors/fsdsetupform.h
        editors/fsdsetupform.ui
        editors/interpolationsetupform.cpp
        editors/interpolationsetupform.h
        editors/interpolationsetupform.ui
        editors/liveryform.cpp
        editors/liveryform.h
        editors/liveryform.ui
        editors/matchingform.cpp
        editors/matchingform.h
        editors/matchingform.ui
        editors/modelmappingform.cpp
        editors/modelmappingform.h
        editors/modelmappingform.ui
        editors/modelmappingmodifyform.cpp
        editors/modelmappingmodifyform.h
        editors/modelmappingmodifyform.ui
        editors/ownmodelsetform.cpp
        editors/ownmodelsetform.h
        editors/ownmodelsetform.ui
        editors/pbhsform.cpp
        editors/pbhsform.h
        editors/pbhsform.ui
        editors/pilotform.cpp
        editors/pilotform.h
        editors/pilotform.ui
        editors/relativeaircraftposition.cpp
        editors/relativeaircraftposition.h
        editors/relativeaircraftposition.ui
        editors/serverform.cpp
        editors/serverform.h
        editors/serverform.ui
        editors/situationform.cpp
        editors/situationform.h
        editors/situationform.ui
        editors/validationindicator.cpp
        editors/validationindicator.h
        editors/validationindicator.ui
        enablefordockwidgetinfoarea.cpp
        enablefordockwidgetinfoarea.h
        enableforframelesswindow.cpp
        enableforframelesswindow.h
        enableforviewbasedindicator.cpp
        enableforviewbasedindicator.h
        eventfilter.cpp
        eventfilter.h
        filters/aircrafticaofilterbar.cpp
        filters/aircrafticaofilterbar.h
        filters/aircrafticaofilterbar.ui
        filters/aircraftmodelfilterbar.cpp
        filters/aircraftmodelfilterbar.h
        filters/aircraftmodelfilterbar.ui
        filters/aircraftmodelfilterdialog.cpp
        filters/aircraftmodelfilterdialog.h
        filters/aircraftmodelfilterdialog.ui
        filters/airlineicaofilterbar.cpp
        filters/airlineicaofilterbar.h
        filters/airlineicaofilterbar.ui
        filters/countryfilterbar.cpp
        filters/countryfilterbar.h
        filters/countryfilterbar.ui
        filters/distributorfilterbar.cpp
        filters/distributorfilterbar.h
        filters/distributorfilterbar.ui
        filters/filterbarbuttons.cpp
        filters/filterbarbuttons.h
        filters/filterbarbuttons.ui
        filters/filterdialog.cpp
        filters/filterdialog.h
        filters/filterwidget.cpp
        filters/filterwidget.h
        filters/liveryfilterbar.cpp
        filters/liveryfilterbar.h
        filters/liveryfilterbar.ui
        filters/statusmessagefilterbar.cpp
        filters/statusmessagefilterbar.h
        filters/statusmessagefilterbar.ui
        filters/statusmessagefilterdialog.cpp
        filters/statusmessagefilterdialog.h
        filters/statusmessagefilterdialog.ui
        guiactionbind.cpp
        guiactionbind.h
        guiapplication.cpp
        guiapplication.h
        guiutility.cpp
        guiutility.h
        infoarea.cpp
        infoarea.h
        labelandicon.cpp
        labelandicon.h
        labelandicon.ui
        led.cpp
        led.h
        levelmeter.cpp
        levelmeter.h
        lineedithistory.cpp
        lineedithistory.h
        loadindicator.cpp
        loadindicator.h
        mainwindowaccess.cpp
        mainwindowaccess.h
        managedstatusbar.cpp
        managedstatusbar.h
        menus/aircraftmodelmenus.cpp
        menus/aircraftmodelmenus.h
        menus/fontmenus.cpp
        menus/fontmenus.h
        menus/menuaction.cpp
        menus/menuaction.h
        menus/menudelegate.h
        models/actionhotkeylistmodel.cpp
        models/actionhotkeylistmodel.h
        models/actionitem.cpp
        models/actionitem.h
        models/actionmodel.cpp
        models/actionmodel.h
        models/aircraftcategorylistmodel.cpp
        models/aircraftcategorylistmodel.h
        models/aircraftcategorytreemodel.cpp
        models/aircraftcategorytreemodel.h
        models/aircrafticaofilter.cpp
        models/aircrafticaofilter.h
        models/aircrafticaolistmodel.cpp
        models/aircrafticaolistmodel.h
        models/aircraftmodelfilter.cpp
        models/aircraftmodelfilter.h
        models/aircraftmodellistmodel.cpp
        models/aircraftmodellistmodel.h
        models/aircraftpartslistmodel.cpp
        models/aircraftpartslistmodel.h
        models/aircraftsituationchangelistmodel.cpp
        models/aircraftsituationchangelistmodel.h
        models/aircraftsituationlistmodel.cpp
        models/aircraftsituationlistmodel.h
        models/airlineicaofilter.cpp
        models/airlineicaofilter.h
        models/airlineicaolistmodel.cpp
        models/airlineicaolistmodel.h
        models/allmodelcontainers.h
        models/allmodels.h
        models/applicationinfolistmodel.cpp
        models/applicationinfolistmodel.h
        models/atcstationlistmodel.cpp
        models/atcstationlistmodel.h
        models/atcstationtreemodel.cpp
        models/atcstationtreemodel.h
        models/audiodeviceinfolistmodel.cpp
        models/audiodeviceinfolistmodel.h
        models/clientlistmodel.cpp
        models/clientlistmodel.h
        models/columnformatters.cpp
        models/columnformatters.h
        models/columns.cpp
        models/columns.h
        models/countryfilter.cpp
        models/countryfilter.h
        models/countrylistmodel.cpp
        models/countrylistmodel.h
        models/distributorfilter.cpp
        models/distributorfilter.h
        models/distributorlistmodel.cpp
        models/distributorlistmodel.h
        models/identifierlistmodel.cpp
        models/identifierlistmodel.h
        models/interpolationsetupmodel.cpp
        models/interpolationsetupmodel.h
        models/listmodelbase.cpp
        models/listmodelbase.h
        models/listmodelbaseaviation.cpp
        models/listmodelbasemisc.cpp
        models/listmodelbasenetwork.cpp
        models/listmodelbasenontemplate.cpp
        models/listmodelbasenontemplate.h
        models/listmodelbasesimulation.cpp
        models/listmodelbaseweather.cpp
        models/listmodelcallsignobjects.cpp
        models/listmodelcallsignobjects.h
        models/listmodeldbobjects.cpp
        models/listmodeldbobjects.h
        models/listmodeltimestampobjects.cpp
        models/listmodeltimestampobjects.h
        models/liveryfilter.cpp
        models/liveryfilter.h
        models/liverylistmodel.cpp
        models/liverylistmodel.h
        models/matchingstatisticsmodel.cpp
        models/matchingstatisticsmodel.h
        models/modelfilter.cpp
        models/modelfilter.h
        models/namevariantpairlistmodel.cpp
        models/namevariantpairlistmodel.h
        models/selectionmodel.h
        models/serverlistmodel.cpp
        models/serverlistmodel.h
        models/simulatedaircraftlistmodel.cpp
        models/simulatedaircraftlistmodel.h
        models/statusmessagefilter.cpp
        models/statusmessagefilter.h
        models/statusmessagelistmodel.cpp
        models/statusmessagelistmodel.h
        models/textmessagelistmodel.cpp
        models/textmessagelistmodel.h
        models/userlistmodel.cpp
        models/userlistmodel.h
        overlaymessages.cpp
        overlaymessages.h
        overlaymessages.ui
        overlaymessagesframe.cpp
        overlaymessagesframe.h
        pluginconfig.h
        pluginconfigwindow.cpp
        pluginconfigwindow.h
        plugindetailswindow.cpp
        plugindetailswindow.h
        plugindetailswindow.ui
        pluginselector.cpp
        pluginselector.h
        qml
        qml/AFVMap.qml
        qml/AtcRing.qml
        qml/Transceiver.qml
        qml/qml.qrc
        registermetadata.cpp
        registermetadata.h
        settings/dockwidgetsettings.cpp
        settings/dockwidgetsettings.h
        settings/guisettings.cpp
        settings/guisettings.h
        settings/navigatorsettings.cpp
        settings/navigatorsettings.h
        settings/textmessagesettings.cpp
        settings/textmessagesettings.h
        settings/updatenotification.h
        settings/viewupdatesettings.cpp
        settings/viewupdatesettings.h
        sharedstringlistcompleter.cpp
        sharedstringlistcompleter.h
        shortcut.cpp
        shortcut.h
        showhidebar.cpp
        showhidebar.h
        showhidebar.ui
        splashscreen.cpp
        splashscreen.h
        stylesheetutility.cpp
        stylesheetutility.h
        swiftguiexport.h
        systemtraywindow.cpp
        systemtraywindow.h
        textmessagetextedit.cpp
        textmessagetextedit.h
        ticklabel.cpp
        ticklabel.h
        uppercasevalidator.cpp
        uppercasevalidator.h
        views/aircraftcategorytreeview.cpp
        views/aircraftcategorytreeview.h
        views/aircraftcategoryview.cpp
        views/aircraftcategoryview.h
        views/aircrafticaoview.cpp
        views/aircrafticaoview.h
        views/aircraftmodelstatisticsdialog.cpp
        views/aircraftmodelstatisticsdialog.h
        views/aircraftmodelstatisticsdialog.ui
        views/aircraftmodelvalidationdialog.cpp
        views/aircraftmodelvalidationdialog.h
        views/aircraftmodelvalidationdialog.ui
        views/aircraftmodelview.cpp
        views/aircraftmodelview.h
        views/aircraftpartsview.cpp
        views/aircraftpartsview.h
        views/aircraftsituationchangeview.cpp
        views/aircraftsituationchangeview.h
        views/aircraftsituationview.cpp
        views/aircraftsituationview.h
        views/airlineicaoview.cpp
        views/airlineicaoview.h
        views/applicationinfoview.cpp
        views/applicationinfoview.h
        views/atcstationtreeview.cpp
        views/atcstationtreeview.h
        views/atcstationview.cpp
        views/atcstationview.h
        views/audiodeviceinfoview.cpp
        views/audiodeviceinfoview.h
        views/checkboxdelegate.cpp
        views/checkboxdelegate.h
        views/clientview.cpp
        views/clientview.h
        views/countryview.cpp
        views/countryview.h
        views/distributorview.cpp
        views/distributorview.h
        views/flightplandialog.cpp
        views/flightplandialog.h
        views/flightplandialog.ui
        views/identifierview.cpp
        views/identifierview.h
        views/interpolationsetupview.cpp
        views/interpolationsetupview.h
        views/liveryview.cpp
        views/liveryview.h
        views/matchingstatisticsview.cpp
        views/matchingstatisticsview.h
        views/namevariantpairview.cpp
        views/namevariantpairview.h
        views/radarview.cpp
        views/radarview.h
        views/serverview.cpp
        views/serverview.h
        views/simulatedaircraftview.cpp
        views/simulatedaircraftview.h
        views/statusmessageview.cpp
        views/statusmessageview.h
        views/textmessageview.cpp
        views/textmessageview.h
        views/userview.cpp
        views/userview.h
        views/viewbase.cpp
        views/viewbase.h
        views/viewbaseaviation.cpp
        views/viewbaseitemdelegate.cpp
        views/viewbaseitemdelegate.h
        views/viewbasemisc.cpp
        views/viewbasenetwork.cpp
        views/viewbasenontemplate.cpp
        views/viewbaseproxystyle.cpp
        views/viewbaseproxystyle.h
        views/viewbasesimulation.cpp
        views/viewcallsignobjects.cpp
        views/viewcallsignobjects.h
        views/viewdbobjects.cpp
        views/viewdbobjects.h
        views/viewtimestampobjects.cpp
        views/viewtimestampobjects.h
        )

# Copy resources
add_custom_command(TARGET gui
                    POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different share ${PROJECT_BINARY_DIR}/out/share
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(gui PUBLIC ${PROJECT_SOURCE_DIR}/src)

target_link_libraries(gui
        PUBLIC
        Qt::Core
        misc
        core
        Qt::Widgets
        Qt::Svg
        )

target_compile_definitions(gui PRIVATE BUILD_SWIFT_GUI_LIB)

target_precompile_headers(gui
        PRIVATE
            ${SWIFT_MISC_PCH}
            ${SWIFT_CORE_PCH}
            ${SWIFT_GUI_PCH}
)

install(TARGETS gui
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY share/qss DESTINATION ${CMAKE_INSTALL_PREFIX}/share/)
