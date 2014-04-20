#ifndef BLACKGUI_KEYBOARDKEYVIEW_H
#define BLACKGUI_KEYBOARDKEYVIEW_H

#include "viewbase.h"
#include "keyboardkeylistmodel.h"

namespace BlackGui
{
    /*!
     * \brief Keyboard key view
     */
    class CKeyboardKeyView : public CViewBase<CKeyboardKeyListModel>
    {

    public:

        //! Constructor
        explicit CKeyboardKeyView(QWidget *parent = nullptr);
    };
}
#endif // guard
