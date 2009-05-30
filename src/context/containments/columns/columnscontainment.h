#ifndef COLUMNS_CONTAINMENT_H
#define COLUMNS_CONTAINMENT_H

#include "basket_export.h"

#include <QList>

#include <Plasma/Containment>

class ColumnsLayout;

namespace Plasma
{
    class Applet;
}

class BASKET_EXPORT ColumnsContainment : public Plasma::Containment
{
    Q_OBJECT
    public:
        ColumnsContainment( QGraphicsItem* parent = 0, const QString &  serviceId = QString(), uint containmentId = 0 );
        ColumnsContainment(QObject *parent, const QVariantList &args);
        ~ColumnsContainment();

        void init();

        void constraintsEvent(Plasma::Constraints constraints);


    public slots:
        void onAppletAdded(Plasma::Applet *applet, const QPointF &pos);
        void onAppletRemoved(Plasma::Applet *applet);

    private:
        QList<Applet *> m_appletList;
        ColumnsLayout *m_layout;
};

#endif // COLUMNS_CONTAINMENT_H
