#include "controlpoints.h"

bool ControlPoints::isConvex(const QPolygonF& polygon)
{
    QPointF prev = polygon.last();

    QVector3D v1(polygon.first() - prev);
    QVector3D v0(prev - polygon.at(polygon.size() - 2));

    int zValue = QVector3D::crossProduct(v0, v1).z();
    for (int i = 0; i < polygon.size() - 1; ++i) {
        QVector3D v1(polygon.at(i + 1) - polygon.at(i));
        QVector3D v0(polygon.at(i) - prev);
        if (QVector3D::crossProduct(v0, v1).z() * zValue <= 0)
            return false;
        prev = polygon.at(i);
    }

    return true;
}

bool ControlPoints::isWithinBoundingRect(const QPolygonF& polygon, const QRectF &bounds)
{
    qreal left = bounds.left();
    qreal right = bounds.right();
    qreal top = bounds.top();
    qreal bottom = bounds.bottom();

    for (const QPointF& pt : polygon) {
        if (pt.x() < left) return false;
        else if (pt.x() > right) return false;

        if (pt.y() < top) return false;
        else if (pt.y() > bottom) return false;
    }
    return true;
}

ControlPoints::ControlPoints(QWidget *parent)
    : QObject(parent)
    , parent(parent)
    , pointSize(QSize(11, 11))
    , pointShape(CircleShape)
    , lockType(NoLock)
    , connectionType(LineConnection)
    , loopType(Loop)
    , indexVisible(false)
    , selectedControlPoint(-1)
    , selected(false)
    , movePolygon(false)
    , keyPressed(0)
    , eventFilterInstalled(false)
{
    setEventFilterEnabled(true);

    pointPen = QPen(QColor(0, 0, 0, 191), 1);
    pointBrush = QBrush(QColor(193, 193, 193, 129));
    connectionPen = QPen(QColor(0, 0, 0, 127), 1);
    fillBrush = QBrush(QColor(95, 95, 95, 127));

    pointPenSelected = QPen(QColor(0, 0, 0, 255), 1);
    pointBrushSelected = QBrush(QColor(255, 63, 63, 191));
    connectionPenSelected = QPen(QColor(0, 0, 0, 255), 2);

    connect(this, SIGNAL(pointsChanged(QPolygonF)), parent, SLOT(update()));
}

void ControlPoints::setPoints(const QPolygonF& newPoints)
{
    if (newPoints.size() < 3) {
        qDebug("%s: Insufficient number of points. It should be more than 2.", __FUNCTION__);
    }
    else if (!isConvex(newPoints)) {
        qDebug("%s: Invalid shape. Given points are not convex.", __FUNCTION__);
    }
    else {
        controlPoints.clear();

        for (int i = 0; i < newPoints.size(); ++i) {
            controlPoints << boundPoint(newPoints.at(i), boundingRect(), 0);
        }
    }
}

void ControlPoints::setPointShape(PointShape newShape)
{
    pointShape = newShape;
}

void ControlPoints::setLockType(LockType newType)
{
    lockType = newType;
}

void ControlPoints::setConnectionType(ConnectionType newType)
{
    connectionType = newType;
}

void ControlPoints::setLoopType(LoopType newType)
{
    loopType = newType;
}

void ControlPoints::setIndexVisible(bool visible)
{
    indexVisible = visible;
}

void ControlPoints::setPointPen(const QPen& pen)
{
    pointPen = pen;
}

void ControlPoints::setPointBrush(const QBrush& brush)
{
    pointBrush = brush;
}

void ControlPoints::setConnectionPen(const QPen& pen)
{
    connectionPen = pen;
}

void ControlPoints::setFillBrush(const QBrush& brush)
{
    fillBrush = brush;
}

void ControlPoints::setSelected(bool selected)
{
    this->selected = selected;
}

QRectF ControlPoints::getBoundingBox() const
{
    return controlPoints.boundingRect();
}

inline qreal determinant(const QPointF& p1, const QPointF& p2)
{
    return p1.x() * p2.y() - p1.y() * p2.x();
}

inline bool isIntersect(const QPointF& a, const QPointF& b, const QPointF& c, const QPointF& d)
{
    qreal det = determinant(b - a, c - d);
    qreal t = determinant(c - a, c - d) / det;
    qreal u = determinant(b - a, c - a) / det;
    if (t < 0 || u < 0 || t > 1 || u > 1)
        return false;
    return true;
}

bool ControlPoints::isOverlap(const QPolygonF& other) const
{
    const QPointF& a = controlPoints.first();
    const QPointF& b = controlPoints.last();
    if (isIntersect(a, b, other.first(), other.last()))
        return true;
    for (int j = 1; j < other.size(); ++j) {
        if (isIntersect(a, b, other.at(j - 1), other.at(j)))
            return true;
    }

    for (int i = 1; i < controlPoints.size(); ++i) {
        const QPointF& a = controlPoints.at(i - 1);
        const QPointF& b = controlPoints.at(i);
        if (isIntersect(a, b, other.first(), other.last()))
            return true;
        for (int j = 1; j < other.size(); ++j) {
            if (isIntersect(a, b, other.at(j - 1), other.at(j)))
                return true;
        }
    }
    return false;
}

bool ControlPoints::isPointInside(const QPointF& pos) const
{
    qreal angle = 0;
    for (int i = 1; i < controlPoints.size(); ++i) {
        QLineF l0(controlPoints[i - 1], pos);
        QLineF l(controlPoints[i], pos);
        angle += qMin(l.angleTo(l0), l0.angleTo(l));
    }
    QLineF l0(controlPoints.back(), pos);
    QLineF l(controlPoints.first(), pos);
    angle += qMin(l.angleTo(l0), l0.angleTo(l));

    return qAbs(angle - 360) < 1E-3;
}

bool ControlPoints::isControlPointSelected() const
{
    return (selectedControlPoint >= 0);
}

int ControlPoints::selectControlPoint(const QPointF& clickPos)
{
    for (int idx = 0; idx < controlPoints.size(); ++idx) {
        QPainterPath path;
        if (pointShape == CircleShape)
            path.addEllipse(pointBoundingRect(idx));
        else
            path.addRect(pointBoundingRect(idx));

        if (path.contains(clickPos)) {
            return idx;
        }
    }
    return -1;
}

void ControlPoints::setEventFilterEnabled(bool enabled)
{
    if (enabled) {
        if (!eventFilterInstalled) 
            parent->installEventFilter(this);
    }
    else {
        parent->removeEventFilter(this);
    }
    eventFilterInstalled = enabled;
}

bool ControlPoints::movePoint(int index, const QPointF& newPoint)
{
    controlPoints[index] = boundPoint(newPoint, boundingRect(), 0);
    return true;
}

bool ControlPoints::movePointDelta(int index, QPointF delta, int flag)
{
    QPolygonF newPolygon = tryToMovePointDelta(index, delta);
    if (ControlPoints::isConvex(newPolygon)) {
        controlPoints[index] = newPolygon.at(index);
        return true;
    }
    else {
        qDebug("%s: %s", __FUNCTION__, "Not convex!!");
        return false;
    }
}

bool ControlPoints::moveAllPointsDelta(QPointF delta)
{
    QPolygonF newPolygon = tryToMoveAllPointsDelta(delta);

    if (ControlPoints::isWithinBoundingRect(newPolygon, boundingRect())) {
        controlPoints.clear();
        controlPoints = newPolygon;
        return true;
    }
    return false;
}

QPolygonF ControlPoints::tryToMovePointDelta(int index, QPointF delta)
{
    QPointF newPoint = controlPoints[index] + delta;
    QPolygonF newPolygon = controlPoints;
    newPolygon[index] = boundPoint(newPoint, boundingRect(), 0);
    return newPolygon;
}

QPolygonF ControlPoints::tryToMoveAllPointsDelta(QPointF delta)
{
    QPolygonF newPolygon;

    for (const QPointF& pt : controlPoints)
        newPolygon << (pt + delta);

    return newPolygon;
}

QPointF ControlPoints::getCenter() const
{
    QPointF center(0, 0);
    for (QPointF pos : controlPoints) {
        center += pos;
    }

    return center / controlPoints.size();
}

QPolygonF ControlPoints::getPoints() const
{
    return controlPoints;
}

void ControlPoints::paintPoints(
#ifndef NDEBUG
    QString text
#endif
)
{
    QPainter painter(parent);
    painter.setRenderHint(QPainter::Antialiasing);

    paintFill(painter);
    paintConnectionLines(painter);
    paintControlPoints(painter);

#ifndef NDEBUG
    painter.drawText(getCenter(), text);
#endif
}

void ControlPoints::paintFill(QPainter &painter)
{
    if (loopType == Loop) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(fillBrush);
        painter.drawPolygon(controlPoints);
    }
}

void ControlPoints::paintConnectionLines(QPainter &painter)
{
    if (connectionPen.style() != Qt::NoPen && connectionType != NoConnection) {
        painter.setPen(selected ? connectionPenSelected : connectionPen);

        if (connectionType == CurveConnection) {
            QPainterPath path;
            path.moveTo(controlPoints.at(0));
            for (int i = 1; i < controlPoints.size(); ++i) {
                QPointF p1 = controlPoints.at(i - 1);
                QPointF p2 = controlPoints.at(i);
                qreal distance = p2.x() - p1.x();

                path.cubicTo(p1.x() + distance / 2, p1.y(),
                    p1.x() + distance / 2, p2.y(),
                    p2.x(), p2.y());
            }

            if (loopType == Loop) {
                QPointF p1 = controlPoints.back();
                QPointF p2 = controlPoints.first();
                qreal distance = p2.x() - p1.x();

                path.cubicTo(p1.x() + distance / 2, p1.y(),
                    p1.x() + distance / 2, p2.y(),
                    p2.x(), p2.y());
            }
            painter.drawPath(path);
        }
        else {
            if (loopType == Loop) {
                painter.setBrush(Qt::NoBrush);
                painter.drawPolygon(controlPoints);
            }
            else
                painter.drawPolyline(controlPoints);
        }
    }
}

void ControlPoints::paintControlPoints(QPainter &painter)
{
    if (selected) {
        painter.setPen(pointPenSelected);
        painter.setBrush(pointBrushSelected);
    }
    else {
        painter.setPen(pointPen);
        painter.setBrush(pointBrush);
    }

    QFont font = painter.font();
    QFontMetrics fm(font);

    for (int i = 0; i < controlPoints.size(); ++i) {
        QRectF bounds = pointBoundingRect(i);
        if (pointShape == CircleShape)
            painter.drawEllipse(bounds);
        else
            painter.drawRect(bounds);

        if (indexVisible) {
            QString text = QString::number(i);
            QRect rect = fm.tightBoundingRect(text);
            QPointF textPos = controlPoints[i] + QPointF(-rect.width() / 2, rect.height() / 2);
            painter.drawText(textPos, text);
        }
    }
}

inline QRectF ControlPoints::pointBoundingRect(int index) const
{
    QPointF p = controlPoints.at(index);
    qreal w = pointSize.width();
    qreal h = pointSize.height();
    qreal x = p.x() - w / 2;
    qreal y = p.y() - h / 2;
    return QRectF(x, y, w, h);
}

inline QRectF ControlPoints::boundingRect() const
{
    if (bounds.isEmpty()) return parent->rect();
    else return bounds;
}

inline QPointF ControlPoints::boundPoint(const QPointF &point, const QRectF &bounds, int lock)
{
    QPointF p = point;

    qreal left = bounds.left();
    qreal right = bounds.right();
    qreal top = bounds.top();
    qreal bottom = bounds.bottom();

    if (p.x() < left) p.setX(left);
    else if (p.x() > right) p.setX(right);

    if (p.y() < top) p.setY(top);
    else if (p.y() > bottom) p.setY(bottom);

    return p;
}

bool ControlPoints::eventFilter(QObject* object, QEvent* event)
{
    if (object == parent) {
        switch (event->type()) {
        case QEvent::MouseButtonPress:
            return mouseButtonPress(static_cast<QMouseEvent*>(event));
        case QEvent::MouseMove:
            mouseMove(static_cast<QMouseEvent*>(event));
            break;
        case QEvent::MouseButtonRelease:
            return mouseButtonRelease(static_cast<QMouseEvent*>(event));
        case QEvent::Resize:
            resize(static_cast<QResizeEvent*>(event));
            break;
        case QEvent::Paint:
            QWidget *that_widget = parent;
            parent = nullptr;
            QApplication::sendEvent(object, event);
            parent = that_widget;

            paintPoints();
            return true;
        }
    }

    return QObject::eventFilter(object, event);
}

bool ControlPoints::mouseButtonPress(QMouseEvent *event)
{
    QPointF clickPos = event->pos();
    oldPos = clickPos;
    if (event->button() == Qt::LeftButton) {
        int idx = selectControlPoint(clickPos);
        if (idx == -1) {
            movePolygon = isPointInside(clickPos);
        }
        else
            selectedControlPoint = idx;
        return false;
    }
}

void ControlPoints::mouseMove(QMouseEvent *event)
{
    QPointF delta = event->pos() - oldPos;
    if (isControlPointSelected()) {
        movePointDelta(selectedControlPoint, delta, keyPressed);
        emit pointsChanged(controlPoints);
    }
    else if (movePolygon) {
        moveAllPointsDelta(delta);
        emit pointsChanged(controlPoints);
    }
    oldPos = event->pos();
}

bool ControlPoints::mouseButtonRelease(QMouseEvent *event)
{
    if (isControlPointSelected() || movePolygon) {
        selectedControlPoint = -1;
        movePolygon = false;
        return true;
    }
    return false;
}

bool ControlPoints::keyPress(QKeyEvent *event)
{
    keyPressed |= event->key();
    qDebug("%s: %d", __FUNCTION__, keyPressed);
    return true;
}

bool ControlPoints::keyRelease(QKeyEvent *event)
{
    keyPressed ^= event->key();
    qDebug("%s: %d", __FUNCTION__, keyPressed);
    return true;
}

void ControlPoints::resize(QResizeEvent* event)
{
    if (event->oldSize().width() == 0 || event->oldSize().height() == 0)
        return;

    qreal stretch_x = event->size().width() / qreal(event->oldSize().width());
    qreal stretch_y = event->size().height() / qreal(event->oldSize().height());
    for (int i = 0; i < controlPoints.size(); ++i) {
        QPointF p = controlPoints[i];
        movePoint(i, QPointF(p.x() * stretch_x, p.y() * stretch_y));
    }
}
