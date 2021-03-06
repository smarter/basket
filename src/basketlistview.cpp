/***************************************************************************
 *   Copyright (C) 2003 by Sébastien Laoût                                 *
 *   slaout@linux62.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "basketlistview.h"
#include <qregexp.h>
//Added by qt3to4:
#include <QDragLeaveEvent>
#include <QPixmap>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QResizeEvent>
#include <QFocusEvent>
#include <kiconloader.h>
#include <klocale.h>
#include <kstringhandler.h>
#include <qpainter.h>
#include <qbitmap.h>
#include <qpixmapcache.h>
#include <qtooltip.h>
#include <kdebug.h>
#include "global.h"
#include "bnpview.h"
#include "basket.h"
#include "tools.h"
#include "settings.h"
#include "notedrag.h"
#include <QStandardItemModel>

/** class BasketListViewItem: */

BasketListViewItem::BasketListViewItem(QTreeWidget *parent, Basket *basket)
	: QTreeWidgetItem(parent), m_basket(basket)
	, m_isUnderDrag(false)
	, m_isAbbreviated(false)
{
}

BasketListViewItem::BasketListViewItem(QTreeWidgetItem *parent, Basket *basket)
	: QTreeWidgetItem(parent), m_basket(basket)
	, m_isUnderDrag(false)
	, m_isAbbreviated(false)
{
}

BasketListViewItem::BasketListViewItem(QTreeWidget *parent, QTreeWidgetItem *after, Basket *basket)
	: QTreeWidgetItem(parent, after), m_basket(basket)
	, m_isUnderDrag(false)
	, m_isAbbreviated(false)
{
}

BasketListViewItem::BasketListViewItem(QTreeWidgetItem *parent, QTreeWidgetItem *after, Basket *basket)
	: QTreeWidgetItem(parent, after), m_basket(basket)
	, m_isUnderDrag(false)
	, m_isAbbreviated(false)
{
}

BasketListViewItem::~BasketListViewItem()
{
}

QString BasketListViewItem::escapedName(const QString &string)
{
    // Underlining the Alt+Letter shortcut (and escape all other '&' characters), if any:
    QString basketName = string;
    basketName.replace('&', "&&"); // First escape all the amperstamp
    QString letter;
    QRegExp letterExp("^Alt\\+(?:Shift\\+)?(.)$");

    QString basketShortcut = m_basket->shortcut().primary().toString();
    if (letterExp.indexIn(basketShortcut) != -1) {
        int index;
        letter = letterExp.cap(1);
        if ((index = basketName.indexOf(letter)) != -1)
            basketName.insert(index, '&');
    }

    return basketName;
}

void BasketListViewItem::setup()
{
	setText(/*column=*/0, escapedName(m_basket->basketName()));

        QPixmap icon = KIconLoader::global()->loadIcon(
        m_basket->icon(), KIconLoader::NoGroup, 16, KIconLoader::DefaultState,
        QStringList(), 0L, /*canReturnNull=*/false
        );

	setIcon(/*column=*/0, icon);
/*
	QBrush brush;

	bool withIcon = m_stateCopy || (m_tagCopy && !m_tagCopy->isMultiState());
	State* state = (m_tagCopy ? m_tagCopy->stateCopies[0]->newState : m_stateCopy->newState);
	brush.setColor(isSelected() ? kapp->palette().color(QPalette::Highlight)  : (withIcon && state->backgroundColor().isValid() ? state->backgroundColor() : viewport->palette().color(viewwport->backgroundRole())));
	setBackground(brush);
	*/
}

BasketListViewItem* BasketListViewItem::lastChild()
{
	int count = childCount();
	if (count <= 0)
		return 0;
	return (BasketListViewItem*)(child(count-1));
}

QStringList BasketListViewItem::childNamesTree(int deep)
{
	QStringList result;

	// Compute indentation spaces:
	QString spaces;
	for (int j = 0; j < deep; ++j)
		spaces += "  ";

	// Append the name:
	result.append(spaces + basket()->basketName());

	// Append the children:
	for (int i=0;i<childCount();i++){
		QStringList children = ((BasketListViewItem *)child(i))->childNamesTree(deep + 1);
		for (QStringList::iterator it = children.begin(); it != children.end(); ++it)
			result.append(*it);
	}
	return result;
}

void BasketListViewItem::moveChildsBaskets()
{
	int insertAfterThis = 0;
	if (!parent())
		insertAfterThis = treeWidget()->indexOfTopLevelItem(this);
	for (int i=0;i<childCount();i++){
		// Re-insert the item with the good parent:
		if (parent())
			parent()->insertChild(insertAfterThis, child(i));
		else
			treeWidget()->insertTopLevelItem(insertAfterThis, child(i));
		// And move it at the good place:
		insertAfterThis++;
	}
}

void BasketListViewItem::ensureVisible()
{
	BasketListViewItem *item = this;
	while (item->parent()) {
		item = (BasketListViewItem*)(item->parent());
		item->setExpanded(true);
	}
}

bool BasketListViewItem::isShown()
{
	QTreeWidgetItem *item = parent();
	while (item) {
		if (!item->isExpanded())
			return false;
		item = item->parent();
	}
	return true;
}

bool BasketListViewItem::isCurrentBasket()
{
	return basket() == Global::bnpView->currentBasket();
}

bool BasketListViewItem::isUnderDrag(){
	return m_isUnderDrag;
}

// TODO: Move this function from item.cpp to class Tools:
extern void drawGradient( QPainter *p, const QColor &colorTop, const QColor & colorBottom,
						  int x, int y, int w, int h,
						  bool sunken, bool horz, bool flat  ); /*const*/

QPixmap BasketListViewItem::circledTextPixmap(const QString &text, int height, const QFont &font, const QColor &color)
{
	QString key = QString("BLI-%1.%2.%3.%4")
		.arg(text).arg(height).arg(font.toString()).arg(color.rgb());
	if (QPixmap* cached=QPixmapCache::find(key)) {
		return *cached;
	}

	// Compute the sizes of the image components:
	QRect textRect = QFontMetrics(font).boundingRect(0, 0, /*width=*/1, height, Qt::AlignLeft | Qt::AlignTop, text);
	int xMargin = height / 6;
	int width   = xMargin + textRect.width() + xMargin;

	// Create the gradient image:
	QPixmap gradient(3 * width, 3 * height); // We double the size to be able to smooth scale down it (== antialiased curves)
	QPainter gradientPainter(&gradient);
#if 1 // Enable the new look of the gradient:
	QColor topColor       = treeWidget()->palette().color(QPalette::Highlight).lighter(130); //120
	QColor topMidColor    = treeWidget()->palette().color(QPalette::Highlight).lighter(105); //105
	QColor bottomMidColor = treeWidget()->palette().color(QPalette::Highlight).darker(130);  //120
	QColor bottomColor    = treeWidget()->palette().color(QPalette::Highlight);
	drawGradient(&gradientPainter, topColor, topMidColor,
				  0, 0, gradient.width(), gradient.height() / 2, /*sunken=*/false, /*horz=*/true, /*flat=*/false);
	drawGradient(&gradientPainter, bottomMidColor, bottomColor,
				  0, gradient.height() / 2, gradient.width(), gradient.height() - gradient.height() / 2, /*sunken=*/false, /*horz=*/true, /*flat=*/false);
	gradientPainter.fillRect(0, 0, gradient.width(), 3, treeWidget()->palette().color(QPalette::Highlight));
#else
	drawGradient(&gradientPainter, palette().color(QPalette::Highlight), palette().color(QPalette::Highlight).darker(),
				  0, 0, gradient.width(), gradient.height(), /*sunken=*/false, /*horz=*/true, /*flat=*/false);
#endif
	gradientPainter.end();

	// Draw the curved rectangle:
	QBitmap curvedRectangle(3 * width, 3 * height);
	curvedRectangle.fill(Qt::color0);
	QPainter curvePainter(&curvedRectangle);
	curvePainter.setPen(Qt::color1);
	curvePainter.setBrush(Qt::color1);
	curvePainter.setClipRect(0, 0, 3*(height / 5), 3*(height)); // If the width is small, don't fill the right part of the pixmap
	curvePainter.drawEllipse(0, 3*(-height / 4), 3*(height), 3*(height * 3 / 2)); // Don't forget we double the sizes
	curvePainter.setClipRect(3*(width - height / 5), 0, 3*(height / 5), 3*(height));
	curvePainter.drawEllipse(3*(width - height), 3*(-height / 4), 3*(height), 3*(height * 3 / 2));
	curvePainter.setClipping(false);
	curvePainter.fillRect(3*(height / 6), 0, 3*(width - 2 * height / 6), 3*(height), curvePainter.brush());
	curvePainter.end();

	// Apply the curved rectangle as the mask of the gradient:
	gradient.setMask(curvedRectangle);
	QImage resultImage = gradient.toImage();

	//resultImage.setAlphaBuffer(true);
	resultImage.convertToFormat(QImage::Format_ARGB32);

	// Scale down the image smoothly to get anti-aliasing:
	QPixmap pmScaled = QPixmap::fromImage(resultImage.scaled(width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

	// Draw the text, and return the result:
	QPainter painter(&pmScaled);
	painter.setPen(color);
	painter.setFont(font);
	painter.drawText(0+1, 0, width, height, Qt::AlignHCenter | Qt::AlignVCenter, text);
	painter.end();

	QPixmapCache::insert(key, pmScaled);

	return pmScaled;
}

QPixmap BasketListViewItem::foundCountPixmap(bool isLoading, int countFound, bool childsAreLoading, int countChildsFound, const QFont &font, int height)
{
	if (isLoading)
		return QPixmap();

	QFont boldFont(font);
	boldFont.setBold(true);

	QString text;
	if (childsAreLoading) {
		if (countChildsFound > 0)
			text = i18n("%1+%2+", QString::number(countFound), QString::number(countChildsFound));
		else
			text = i18n("%1+", QString::number(countFound));
	} else {
		if (countChildsFound > 0)
			text = i18n("%1+%2", QString::number(countFound), QString::number(countChildsFound));
		else if (countFound > 0)
			text = QString::number(countFound);
		else
			return QPixmap();
	}

	return circledTextPixmap(text, height, boldFont, treeWidget()->palette().color(QPalette::HighlightedText));
}

bool BasketListViewItem::haveChildsLoading()
{
	for (int i=0;i<childCount();i++){
		BasketListViewItem *childItem = (BasketListViewItem*)child(i);
		if (!childItem->basket()->isLoaded() && !childItem->basket()->isLocked())
			return true;
		if (childItem->haveChildsLoading())
			return true;
	}
	return false;
}

bool BasketListViewItem::haveHiddenChildsLoading()
{
	if (isExpanded())
		return false;
	return haveChildsLoading();
}

bool BasketListViewItem::haveChildsLocked()
{
	for (int i=0; i<childCount(); i++){
		BasketListViewItem *childItem = (BasketListViewItem*)child(i);
		if (/*!*/childItem->basket()->isLocked())
			return true;
		if (childItem->haveChildsLocked())
			return true;
	}
	return false;
}

bool BasketListViewItem::haveHiddenChildsLocked()
{
	if (isExpanded())
		return false;
	return haveChildsLocked();
}

int BasketListViewItem::countChildsFound()
{
	int count = 0;
	for (int i=0;i<childCount();i++){
		BasketListViewItem *childItem = (BasketListViewItem*)child(i);
		count += childItem->basket()->countFounds();
		count += childItem->countChildsFound();
	}
	return count;
}

int BasketListViewItem::countHiddenChildsFound()
{
	if (isExpanded())
		return 0;
	return countChildsFound();
}

void BasketListViewItem::setUnderDrag(bool underDrag)
{
	m_isUnderDrag = underDrag;
}

bool BasketListViewItem::isAbbreviated()
{
	return m_isAbbreviated;
}

void BasketListViewItem::setAbbreviated(bool b){
	m_isAbbreviated = b;
}

/** class BasketTreeListView: */

BasketTreeListView::BasketTreeListView(QWidget *parent)
	: QTreeWidget(parent), m_autoOpenItem(0)
	, m_itemUnderDrag(0)
{
	connect( &m_autoOpenTimer, SIGNAL(timeout()), this, SLOT(autoOpen()) );
}

void BasketTreeListView::contextMenuEvent(QContextMenuEvent *e){
	emit contextMenuRequested(e->pos());
}

bool BasketTreeListView::event(QEvent *e)
{
	if (e->type() == QEvent::ToolTip) {
		QHelpEvent *he = static_cast<QHelpEvent *>(e);
		QTreeWidgetItem *item = itemAt(he->pos());
		BasketListViewItem* bitem = dynamic_cast<BasketListViewItem*>(item);
		if (bitem && bitem->isAbbreviated()) {
			QRect rect = visualItemRect(bitem);
			QToolTip::showText(rect.topLeft(), bitem->basket()->basketName(),
							   viewport(), rect);
		}
		return true;
	}
	return QTreeWidget::event(e);
}

void BasketTreeListView::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->provides("application/x-qabstractitemmodeldatalist")){
		QTreeWidgetItemIterator it(this); // TODO: Don't show expanders if it's not a basket drag...
		while (*it) {
			QTreeWidgetItem *item = *it;
			if (item->childCount() <= 0) {
				item->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
				item->setExpanded(true);
			}
			++it;
		}
		update();
	}

	QTreeWidget::dragEnterEvent(event);
}

void BasketTreeListView::removeExpands()
{
	QTreeWidgetItemIterator it(this);
	while (*it) {
		QTreeWidgetItem *item = *it;
		if (item->childCount() <= 0)
			item->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicatorWhenChildless);
		++it;
	}
}


void BasketTreeListView::dragLeaveEvent(QDragLeaveEvent *event)
{
	kDebug() << "BasketTreeListView::dragLeaveEvent";
	m_autoOpenItem = 0;
	m_autoOpenTimer.stop();
	setItemUnderDrag(0);
	removeExpands();
	QTreeWidget::dragLeaveEvent(event);
}

void BasketTreeListView::dropEvent(QDropEvent *event)
{
	kDebug() << "BasketTreeListView::dropEvent()";
	if (event->provides("application/x-qabstractitemmodeldatalist"))
	{
		QTreeWidget::dropEvent(event);
	}
	else {
		kDebug() << "Forwarding dropped data to the basket";
		QTreeWidgetItem *item = itemAt(event->pos());
		BasketListViewItem* bitem = dynamic_cast<BasketListViewItem*>(item);
		if (bitem) {
			bitem->basket()->blindDrop(event);
		}
		else {
			kDebug() << "Forwarding failed: no bitem found";
		}
        }

	m_autoOpenItem = 0;
	m_autoOpenTimer.stop();
	setItemUnderDrag(0);
	removeExpands();

	Global::bnpView->save(); // TODO: Don't save if it was not a basket drop...
}

void BasketTreeListView::dragMoveEvent(QDragMoveEvent *event)
{
	kDebug() << "BasketTreeListView::dragMoveEvent";
	if (event->provides("application/x-qabstractitemmodeldatalist"))
		QTreeWidget::dragMoveEvent(event);
	else {
		QTreeWidgetItem *item = itemAt(event->pos());
		BasketListViewItem* bitem = dynamic_cast<BasketListViewItem*>(item);
		if (m_autoOpenItem != item) {
			m_autoOpenItem = item;
			m_autoOpenTimer.setSingleShot(true);
			m_autoOpenTimer.start(1700);
		}
		if (item) {
			event->accept();
		}
		setItemUnderDrag(bitem);

		QTreeWidget::dragMoveEvent(event); // FIXME: ADDED
	}
}

void BasketTreeListView::setItemUnderDrag(BasketListViewItem* item)
{
	if (m_itemUnderDrag != item) {
		if (m_itemUnderDrag) {
			// Remove drag status from the old item
			m_itemUnderDrag->setUnderDrag(false);
		}

		m_itemUnderDrag = item;

		if (m_itemUnderDrag) {
			// add drag status to the new item
			m_itemUnderDrag->setUnderDrag(true);
		}
	}
}

void BasketTreeListView::autoOpen()
{
	BasketListViewItem *item = (BasketListViewItem*)m_autoOpenItem;
	if (item)
		Global::bnpView->setCurrentBasket(item->basket());
}

void BasketTreeListView::resizeEvent(QResizeEvent *event)
{
	QTreeWidget::resizeEvent(event);
}

/** We should NEVER get focus (because of QWidget::NoFocus focusPolicy())
 * but QTreeView can programatically give us the focus.
 * So we give it to the basket.
 */
void BasketTreeListView::focusInEvent(QFocusEvent*)
{
	Basket *basket = Global::bnpView->currentBasket();
	if (basket)
		basket->setFocus();
}

Qt::DropActions BasketTreeListView::supportedDropActions () const
{
	return Qt::MoveAction | Qt::IgnoreAction;
}
