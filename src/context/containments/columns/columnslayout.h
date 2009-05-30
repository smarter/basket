#ifndef BASKET_VERTICAL_LAYOUT_H
#define BASKET_VERTICAL_LAYOUT_H

#include "basket_export.h"

#include <QGraphicsLinearLayout>

class BASKET_EXPORT ColumnsLayout : public QGraphicsLinearLayout
{
public:
    explicit ColumnsLayout( QGraphicsLayoutItem *parent = 0 );

    void addItem(QGraphicsLayoutItem* item);
};

#endif
