#ifndef BASKETWIDGET_H
#define BASKETWIDGET_H

#include "basket_export.h"

#include <KDE/KVBox>

namespace Plasma
{
    class Containment;
    class Corona;
    class View;
}

class BASKET_EXPORT BasketWidget : public KVBox
{
    Q_OBJECT
public:
    BasketWidget(QWidget *parent = 0);

    Plasma::Corona* corona() const { return m_corona; }

    QSize sizeHint() const;

public slots:
    void addApplet(const QString &applet);

private:
    Plasma::Corona *m_corona;
    Plasma::View *m_view;

private slots:
    void createView(Plasma::Containment *c);
};

#endif // BASKETWIDGET_H
