#ifndef BLACKGUI_ATCSTATIONCOMPONENT_H
#define BLACKGUI_ATCSTATIONCOMPONENT_H

#include "blackgui/runtimebasedcomponent.h"
#include "blackgui/timerbasedcomponent.h"
#include "blackmisc/avatcstation.h"

#include <QTabWidget>
#include <QModelIndex>

namespace Ui { class CAtcStationComponent; }

namespace BlackGui
{
    /*!
     * \brief ATC stations component
     */
    class CAtcStationComponent : public QTabWidget, public CRuntimeBasedComponent
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAtcStationComponent(QWidget *parent = nullptr);

        //! Destructor
        ~CAtcStationComponent();

        //! Timer for updating
        CTimerBasedComponent *getTimerComponent() { return this->m_timerComponent; }

    public slots:
        //! Update users
        void update();

        //! \copydoc CTimerBasedComponent::setUpdateIntervalSeconds
        void setUpdateIntervalSeconds(int seconds) { Q_ASSERT(this->m_timerComponent); this->m_timerComponent->setUpdateIntervalSeconds(seconds); }

        //! \copydoc CTimerBasedComponent::setUpdateInterval
        void setUpdateInterval(int milliSeconds) { Q_ASSERT(this->m_timerComponent); this->m_timerComponent->setUpdateInterval(milliSeconds); }

        //! \copydoc CTimerBasedComponent::stopTimer
        void stopTimer() { Q_ASSERT(this->m_timerComponent); this->m_timerComponent->stopTimer(); }

        //! Get METAR for given ICAO airport code
        void getMetar(const QString &airportIcaoCode = "");

        //! \copydoc CAtcStationListModel::changedAtcStationConnectionStatus
        void changedAtcStationOnlineConnectionStatus(const BlackMisc::Aviation::CAtcStation &station, bool added);

    protected:
        //! \copydoc CRuntimeBasedComponent::runtimeHasBeenSet
        void runtimeHasBeenSet() override;

    private slots:

        //! Request new ATIS
        void requestAtis();

        //! Online ATC station selected
        void onlineAtcStationSelected(QModelIndex index);

        //! Tab changed
        void atcStationsTabChanged();

        //! Booked stations
        void reloadAtcStationsBooked();

        //! Booked stations changed
        void changedAtcStationsBooked();

        //! Online stations changed
        void changedAtcStationsOnline();

        //! Connection status has been changed
        void connectionStatusChanged(uint from, uint to, const QString &message);

    private:
        Ui::CAtcStationComponent *ui;
        CTimerBasedComponent *m_timerComponent;
        QDateTime m_timestampLastReadOnlineStations;
        QDateTime m_timestampOnlineStationsChanged;
        QDateTime m_timestampLastReadBookedStations;
    };
}
#endif // guard
