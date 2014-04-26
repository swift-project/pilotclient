#ifndef BLACKGUI_SELCALCODESELECTOR_H
#define BLACKGUI_SELCALCODESELECTOR_H

#include <QFrame>

namespace Ui { class CSelcalCodeSelector; }
namespace BlackGui
{

    /*!
     * \brief SELCAL mode selector
     */
    class CSelcalCodeSelector : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CSelcalCodeSelector(QWidget *parent = nullptr);

        //! Destructor
        ~CSelcalCodeSelector();

        //! SELCAL code
        QString getSelcalCode() const;

        //! Reset the SELCAL code
        void resetSelcalCodes(bool allowEmptyValue = false);

        //! Set the SELCAL code
        void setSelcalCode(const QString &selcal);

        //! Valid code?
        bool hasValidCode() const;

    signals:
        //! Value has been changed
        void valueChanged();

    private:
        Ui::CSelcalCodeSelector *ui;
    };
}

#endif // guard
