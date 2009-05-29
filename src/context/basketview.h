#ifndef BASKET_VIEW_H
#define BASKET_VIEW_H

#include "basket_export.h"

#include <Plasma/View>

namespace Plasma
{
    class Containment;
}

class QResizeEvent;

class BASKET_EXPORT BasketView : public Plasma::View
{
    Q_OBJECT

public:
     BasketView( Plasma::Containment *cont, QWidget* parent = 0 );

protected:
    void resizeEvent(QResizeEvent *event);

private:
    /**
    * Set all containments geometry in the scene with the same geometry as the BasketView widget
    */
    void updateContainmentsGeometry();
};

#endif // BASKET_VIEW_H
