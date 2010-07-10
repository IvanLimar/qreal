#pragma once

#include "item.h"
#include "ellipse.h"

class PointPort : public Item
{
public:
	PointPort(qreal x, qreal y, Item *parent = 0);
	virtual QRectF boundingRect() const;
	virtual void drawItem(QPainter* painter);
	virtual void drawExtractionForItem(QPainter* painter);
private:
	qreal mRadius;
};
