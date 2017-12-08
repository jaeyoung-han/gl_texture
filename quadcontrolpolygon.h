/*
 * Copyright (C) 2017 Telesecurity Sciences, Inc.
 *
 */

#ifndef QUAD_CONTROL_POLYGON_H
#define QUAD_CONTROL_POLYGON_H

#include "controlpoints.h"

class QuadControlPolygon : public ControlPoints
{
    const qreal DIFFERENCE_THRESHOLD = 2.5;

public:
    enum GradientType {
        Solid,
        Radial,
        Linear
    };

    static QuadControlPolygon* fromCenterPoint(QWidget* parent, const QPointF& center);

    QuadControlPolygon(QWidget* parent);

    bool isOverlap(const QuadControlPolygon& other) const;

    void setGradientType(GradientType type);
    GradientType getGradientType() const;

    void setGradientRadius(qreal radius);
    void setGradientStop(const QGradientStops& stops);

    void setCenterColor(QColor color);
    void setEdgeColor(QColor color);

    QColor getCenterColor() const;
    QColor getEdgeColor() const;

    virtual void setPoints(const QPolygonF& newPoints);
    virtual bool movePointDelta(int index, QPointF delta, int flag = 0);

protected:
    virtual void paintFill(QPainter &painter);

private:
    void updateFillBrush();

    void updateSolidFillBrush();
    void updateRadialFillBrush();
    void updateLinearFillBrush();

private:
    QMap<int, int> horizontalLockMap;
    QMap<int, int> verticalLockMap;

    GradientType gradientType;
    QGradientStops gradientStops;
    QColor centerColor;
    QColor edgeColor;
    qreal gradientRadius;
};

#endif // QUAD_CONTROL_POLYGON_H
