#include "quadcontrolpolygon.h"

QuadControlPolygon* QuadControlPolygon::fromCenterPoint(QWidget* parent, const QPointF& center)
{
    qreal offset = 30;
    QuadControlPolygon* result = new QuadControlPolygon(parent);
    QPolygonF newPoints;
    newPoints << (center + QPointF(-offset, -offset));
    newPoints << (center + QPointF( offset, -offset));
    newPoints << (center + QPointF( offset,  offset));
    newPoints << (center + QPointF(-offset,  offset));

    result->setPoints(newPoints);
    result->horizontalLockMap[0] = 1;
    result->horizontalLockMap[1] = 0;
    result->horizontalLockMap[2] = 3;
    result->horizontalLockMap[3] = 2;

    result->verticalLockMap[0] = 3;
    result->verticalLockMap[3] = 0;
    result->verticalLockMap[1] = 2;
    result->verticalLockMap[2] = 1;

    result->setGradientRadius(offset);

    result->setEventFilterEnabled(false);
#ifndef NDEBUG
    result->setIndexVisible(true);
#endif

    return result;
}

QuadControlPolygon::QuadControlPolygon(QWidget* parent)
    : ControlPoints(parent)
{
    setPointShape(ControlPoints::CircleShape);
    setLockType(ControlPoints::HorizontalLock);
    setConnectionType(ControlPoints::LineConnection);
    setLoopType(ControlPoints::Loop);

    controlPoints.resize(4);

    gradientType = Solid;
    setCenterColor(QColor::fromRgba(0xff00ff00));
    setEdgeColor(QColor::fromRgba(0));
}

void QuadControlPolygon::setGradientType(GradientType type)
{
    gradientType = type;
}

QuadControlPolygon::GradientType QuadControlPolygon::getGradientType() const
{
    return gradientType;
}

void QuadControlPolygon::setCenterColor(QColor color)
{
    centerColor = color;

    QGradientStops stops;
    stops << QGradientStop(0.00, centerColor);
    stops << QGradientStop(1.00, edgeColor);

    setGradientStop(stops);
}

void QuadControlPolygon::setEdgeColor(QColor color)
{
    edgeColor = color;

    QGradientStops stops;
    stops << QGradientStop(0.00, centerColor);
    stops << QGradientStop(1.00, edgeColor);

    setGradientStop(stops);
}

QColor QuadControlPolygon::getCenterColor() const
{
    return centerColor;
}

QColor QuadControlPolygon::getEdgeColor() const
{
    return edgeColor;
}

void QuadControlPolygon::setPoints(const QPolygonF& newPoints)
{
    if (newPoints.size() != 4) {
        qDebug("%s: Invalid number of points. It should be 4.", __FUNCTION__);
    }
    ControlPoints::setPoints(newPoints);
}

bool QuadControlPolygon::isOverlap(const QuadControlPolygon& other) const
{
    for (const QPointF& pos : other.controlPoints) {
        if (isPointInside(pos)) return true;
    }
    return false;
}

void QuadControlPolygon::setGradientRadius(qreal newRadius)
{
    gradientRadius = newRadius;
    updateFillBrush();
}

void QuadControlPolygon::setGradientStop(const QGradientStops& stops)
{
    gradientStops = stops;
    updateFillBrush();
}

bool QuadControlPolygon::movePointDelta(int index, QPointF delta, int keyPressed)
{
    if (ControlPoints::movePointDelta(index, delta)) {
        delta.rx() = 0;
        ControlPoints::movePointDelta(horizontalLockMap[index], delta);

        QPointF difference = controlPoints[index] - controlPoints[verticalLockMap[index]];
        if (qAbs(difference.x()) < DIFFERENCE_THRESHOLD)
            controlPoints[index].rx() = controlPoints[verticalLockMap[index]].x();

        emit pointsChanged(controlPoints);
        return true;
    }
    return false;
}

void QuadControlPolygon::paintFill(QPainter &painter)
{
    updateFillBrush();
    ControlPoints::paintFill(painter);
}

void QuadControlPolygon::updateFillBrush()
{
    switch (gradientType) {
    default:
    case Solid:
        updateSolidFillBrush();
        break;
    case Radial:
        updateRadialFillBrush();
        break;
    case Linear:
        updateLinearFillBrush();
        break;
    }
}

void QuadControlPolygon::updateSolidFillBrush()
{
    QBrush brush(centerColor);
    setFillBrush(brush);
}

void QuadControlPolygon::updateLinearFillBrush()
{
    QPointF center = getCenter();

    qreal rr = 0;

    for (const QPointF& pt : controlPoints) {
        QLineF line(pt, center);
        rr = qMax(rr, qMax(qAbs(line.dx()), qAbs(line.dy())));
    }

    QGradient gradient = QLinearGradient(center, center - QPointF(-rr, 0));
    gradient.setSpread(QGradient::ReflectSpread);

    for (int i = 0; i < gradientStops.size(); ++i) {
        gradient.setColorAt(gradientStops.at(i).first, gradientStops.at(i).second);
    }

    setFillBrush(gradient);
}

void QuadControlPolygon::updateRadialFillBrush()
{
    QPointF center = getCenter();

    qreal rr = 0;
    QLineF line(controlPoints.at(0), controlPoints.at(1));
    rr = qMax(rr, distancePointAndLine(line, center));

    line.setP1(controlPoints.at(2));
    rr = qMax(rr, distancePointAndLine(line, center));

    line.setP2(controlPoints.at(3));
    rr = qMax(rr, distancePointAndLine(line, center));

    line.setP1(controlPoints.at(0));
    rr = qMax(rr, distancePointAndLine(line, center));

    QGradient gradient = QRadialGradient(center, rr, center);

    for (int i = 0; i < gradientStops.size(); ++i) {
        gradient.setColorAt(gradientStops.at(i).first, gradientStops.at(i).second);
    }

    setFillBrush(gradient);
}
