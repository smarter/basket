#include "basketwidget.h"

#include <Plasma/Containment>
#include <Plasma/Corona>
#include <Plasma/View>

BasketWidget::BasketWidget(QWidget *parent)
    : KVBox(parent),
      m_view(0)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    setSpacing(0);
    setFrameShape(QFrame::NoFrame);
    setFrameShadow(QFrame::Sunken);

    m_corona = new Plasma::Corona(this);
    connect( m_corona, SIGNAL(containmentAdded(Plasma::Containment*)),
            this, SLOT(createView(Plasma::Containment*)));
    m_corona->addContainment("basket_containment_columns");
}

void BasketWidget::createView(Plasma::Containment *containment)
{
    disconnect( m_corona, SIGNAL(containmentAdded(Plasma::Containment*)),
            this, SLOT(createView(Plasma::Containment*)));

    m_view = new Plasma::View(containment, this);
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
