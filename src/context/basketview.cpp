#include "basketview.h"

#include <Plasma/Containment>

#include <KDE/KDebug>

BasketView::BasketView( Plasma::Containment *cont, QWidget* parent )
    : Plasma::View( cont, parent )
{
    setScreen(-1);
    cont->setPos(0, 0);
    cont->updateConstraints();
}

void BasketView::resizeEvent( QResizeEvent* event )
{
    Q_UNUSED( event )

    if ( testAttribute( Qt::WA_PendingResizeEvent ) ) {
        return; // lets not do this more than necessary, shall we?
    }

   updateContainmentsGeometry();
}

void BasketView::updateContainmentsGeometry()
{
    kDebug() << "resizing containment to: " << rect();

    containment()->resize( rect().size() );
    containment()->setPos( rect().topLeft() );
}
