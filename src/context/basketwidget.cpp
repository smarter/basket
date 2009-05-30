#include "basketwidget.h"

#include "basketview.h"

#include <Plasma/Containment>
#include <Plasma/Corona>

BasketWidget::BasketWidget(QWidget *parent)
    : KVBox(parent),
      m_view(0)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    setSpacing(0);

    m_corona = new Plasma::Corona(this);
    connect( m_corona, SIGNAL(containmentAdded(Plasma::Containment*)),
            this, SLOT(createView(Plasma::Containment*)));
    //m_corona->addContainment("basket_containment_free");
    m_corona->addContainment("basket_containment_columns");
}

QSize BasketWidget::sizeHint() const
{
    return QSize( static_cast<QWidget*>( parent() )->size().width() / 3, 300 );
}

void BasketWidget::createView(Plasma::Containment *containment)
{
    disconnect( m_corona, SIGNAL(containmentAdded(Plasma::Containment*)),
            this, SLOT(createView(Plasma::Containment*)));

    m_view = new BasketView(containment, this);
    m_view->setFrameShape(QFrame::NoFrame);
}

void BasketWidget::addApplet(const QString &applet)
{
    if (!m_view) {
        return;
    }

    kDebug() << "adding applet" << applet;
    m_view->containment()->addApplet(applet);
}
