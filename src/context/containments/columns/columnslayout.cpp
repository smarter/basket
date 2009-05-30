#include "columnslayout.h"

#include <Plasma/Applet>

ColumnsLayout::ColumnsLayout(QGraphicsLayoutItem *parent)
    : QGraphicsLinearLayout(parent)
{
    setOrientation(Qt::Vertical);
}

void ColumnsLayout::addItem(QGraphicsLayoutItem * item)
{
    kDebug() << "adding item";
    QGraphicsLinearLayout::addItem(item);
    addStretch();
}
