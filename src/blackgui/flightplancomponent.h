#ifndef BLACKGUI_FLIGHTPLANCOMPONENT_H
#define BLACKGUI_FLIGHTPLANCOMPONENT_H

#include "blackgui/runtimebasedcomponent.h"
#include "blackmisc/avaircraft.h"
#include "blackmisc/avflightplan.h"

#include <QTabWidget>

namespace Ui { class CFlightPlanComponent; }
namespace BlackGui
{
    //! Flight plan widget
    class CFlightPlanComponent : public QTabWidget, public CRuntimeBasedComponent
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CFlightPlanComponent(QWidget *parent = nullptr);

        //! Destructor
        ~CFlightPlanComponent();

    public slots:
        //! Prefill with aircraft data
        void prefillWithAircraftData(const BlackMisc::Aviation::CAircraft &aircraftData);

        //! Prefill with aircraft dara
        void fillWithFlightPlanData(const BlackMisc::Aviation::CFlightPlan &flightPlan);

        //! Get this flight plan
        BlackMisc::Aviation::CFlightPlan getFlightPlan() const;

    private:
        Ui::CFlightPlanComponent *ui;

        //! My flight plan
        BlackMisc::Aviation::CFlightPlan m_flightPlan;

        //! Validate, generates status messages
        BlackMisc::CStatusMessageList validateAndInitializeFlightPlan(BlackMisc::Aviation::CFlightPlan &fligtPlan);

        //! Default value for airport ICAO airports
        static const QString &defaultIcao() { static QString d("ICAO"); return d; }

        //! Default value for time
        static const QString &defaultTime() { static QString t("00:00"); return t; }

    private slots:
        //! Send flightplan
        void sendFlightPlan();

        //! Reset Flightplan
        void resetFlightPlan();

        //! Load Flightplan
        void loadFlightPlanFromNetwork();

        //! Validate Flightplan
        void validateFlightPlan();

        //! Remark
        void buildRemarkString();

        //! Copy over
        void copyRemarks();

        //! Show generator tab page
        void currentTabGenerator();

    };
}
#endif // guard
