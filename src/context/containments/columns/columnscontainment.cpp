#include "columnscontainment.h"

#include "columnslayout.h"

ColumnsContainment::ColumnsContainment( QGraphicsItem* parent, const QString &serviceId, uint containmentId)
    : Plasma::Containment( parent, serviceId, containmentId )
{
    init();
}

ColumnsContainment::ColumnsContainment(QObject *parent, const QVariantList &args)
    : Plasma::Containment( parent, args )
{
    init();
}


ColumnsContainment::~ColumnsContainment()
{
    setLayout(0);
    delete m_layout;
}

void ColumnsContainment::init()
{
    setContainmentType(Plasma::Containment::CustomContainment);
    m_layout = new ColumnsLayout;
    setLayout(m_layout);
    setFormFactor(Plasma::Vertical);

    Containment::init();
}

// Apparently that's where you should init stuff, taken from plasma DefaultDesktop containment
void ColumnsContainment::constraintsEvent(Plasma::Constraints constraints)
{
    if (constraints & Plasma::StartupCompletedConstraint) {
        connect(this, SIGNAL(appletAdded(Plasma::Applet *, const QPointF &)),
                this, SLOT(onAppletAdded(Plasma::Applet *, const QPointF &)));

        connect(this, SIGNAL(appletRemoved(Plasma::Applet *)),
                this, SLOT(onAppletRemoved(Plasma::Applet *)));
    }
}

void ColumnsContainment::onAppletAdded(Plasma::Applet *applet, const QPointF &pos)
{
    /*if (pos != QPointF(-1,-1) || applet->geometry().topLeft() != QPointF(0,0))
        return;

    int axisY = 0;

    for (int i = 0; i < m_appletList.size(); ++i)
    {
        axisY += m_appletList.at(i)->effectiveSizeHint(Qt::PreferredSize, QSizeF(boundingRect().width(), -1)).height();
    }

    m_appletList << applet;

    kDebug() << "adding at" << axisY;
    applet->setPos(0, axisY);*/

    kDebug() << "I'm in ur containment, addin an applet";

    m_layout->addItem(applet);

    updateGeometry();

}


void ColumnsContainment::onAppletRemoved(Plasma::Applet *applet)
{
    /*// Returns incorrect height, probably due to the fact that the applet is no longer visible at that point
    int removedHeight = applet->effectiveSizeHint(Qt::PreferredSize, QSizeF(boundingRect().width(), -1)).height();

    for (int i = m_appletList.size() - 1; i > m_appletList.indexOf(applet); --i)
    {
        kDebug() << "moving" << i << "from" << m_appletList.at(i)->pos() << "of" << removedHeight;
        m_appletList.at(i)->moveBy(0, -removedHeight);
    }

    m_appletList.removeAll(applet);*/

    m_layout->removeItem(applet);
}

K_EXPORT_PLASMA_APPLET( basket_containment_columns, ColumnsContainment )
