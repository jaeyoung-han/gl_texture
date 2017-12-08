/*
 * Copyright (C) 2017 Telesecurity Sciences, Inc.
 *
 */

#ifndef CONTROL_POINTS_H
#define CONTROL_POINTS_H

#include <QtGui>

inline qreal distancePointAndLine(const QLineF& line, const QPointF& point) {
    qreal num = (line.y2() - line.y1()) * point.x() - (line.x2() - line.x1()) * point.y() + line.x2() * line.y1() - line.y2() * line.x1();
    return qAbs(num) / line.length();
}

class ControlPoints : public QObject
{
    Q_OBJECT
public:
    enum PointShape {
        CircleShape,
        RectangleShape
    };

    enum LockType {
        NoLock,
        VerticalLock,
        HorizontalLock
    };

    enum ConnectionType {
        NoConnection,
        LineConnection,
        CurveConnection
    };

    enum LoopType {
        NoLoop,
        Loop
    };

    static bool isConvex(const QPolygonF& polygon);
    static bool isWithinBoundingRect(const QPolygonF& polygon, const QRectF &bounds);

    explicit ControlPoints(QWidget *parent);

    void setPointShape(PointShape newShape);
    void setLockType(LockType newType);
    void setConnectionType(ConnectionType newType);
    void setLoopType(LoopType newType);
    void setIndexVisible(bool visible);

    void setPointPen(const QPen& pen);
    void setPointBrush(const QBrush& brush);
    void setConnectionPen(const QPen& pen);
    void setFillBrush(const QBrush& brush);

    QPointF getCenter() const;
    QPolygonF getPoints() const;

    QRectF getBoundingBox() const;

    void setSelected(bool selected);

    bool isOverlap(const QPolygonF& other) const;
    bool isPointInside(const QPointF& pos) const;
    bool isControlPointSelected() const;
    int selectControlPoint(const QPointF& clickPos);

    void setEventFilterEnabled(bool enabled);

    QPolygonF tryToMovePointDelta(int index, QPointF delta);
    QPolygonF tryToMoveAllPointsDelta(QPointF delta);

    virtual void setPoints(const QPolygonF& newPoints);
    virtual bool movePoint(int index, const QPointF& newPoint);
    virtual bool movePointDelta(int index, QPointF delta, int flag = 0);
    virtual bool moveAllPointsDelta(QPointF delta);

    virtual void paintPoints(
#ifndef NDEBUG
        QString text = QString()
#endif
    );
    virtual bool eventFilter(QObject* object, QEvent* event);
    virtual void resize(QResizeEvent* event);

signals:
    void pointsChanged(QPolygonF);

protected:
    virtual void paintFill(QPainter &painter);
    virtual void paintConnectionLines(QPainter &painter);
    virtual void paintControlPoints(QPainter &painter);

    virtual bool mouseButtonPress(QMouseEvent *event);
    virtual void mouseMove(QMouseEvent *event);
    virtual bool mouseButtonRelease(QMouseEvent *event);

    virtual bool keyPress(QKeyEvent *event);
    virtual bool keyRelease(QKeyEvent *event);

    inline QRectF pointBoundingRect(int index) const;
    inline QRectF boundingRect() const;
    inline QPointF boundPoint(const QPointF &point, const QRectF &bounds, int lock);

protected:
    QWidget* parent;
    QPolygonF controlPoints;
    QSizeF pointSize;

    PointShape pointShape;
    LockType lockType;
    ConnectionType connectionType;
    LoopType loopType;
    bool indexVisible;

    int selectedControlPoint;
    bool selected;
    bool movePolygon;

    QPen pointPen;
    QBrush pointBrush;
    QPen connectionPen;

    QPen pointPenSelected;
    QBrush pointBrushSelected;
    QPen connectionPenSelected;

    QBrush fillBrush;
    QGradient* fillGradient;

    QRectF bounds;
    QPointF oldPos;
    int keyPressed;

    bool eventFilterInstalled;

private:
    ControlPoints(const ControlPoints& rhs) {};
    void operator=(const ControlPoints& rhs) {};
};

#endif // CONTROL_POINTS_H
