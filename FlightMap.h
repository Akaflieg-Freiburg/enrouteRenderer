#pragma once

#include <QtQuick/QQuickPaintedItem>


class FlightMap : public QQuickPaintedItem
{
    Q_OBJECT

public:
    explicit FlightMap(QQuickItem *parent = nullptr);

    Q_PROPERTY(double rotation READ rotation WRITE setRotation NOTIFY rotationChanged)
    Q_PROPERTY(double zoom READ zoom WRITE setZoom NOTIFY zoomChanged)

    static constexpr double minZoom = 7.0;
    static constexpr double maxZoom = 14.0;

    double rotation() const
    {
        return m_rotation;
    }

    void setRotation(double newRotation);

    void setZoom(double newZoom);

    double zoom() const
    {
        return m_zoom;
    }



signals:
    void rotationChanged();
    void zoomChanged();

protected:
    void paint(QPainter *painter) override;

private:
    double m_zoom {10.0};
    double m_rotation {0.0};

    Q_DISABLE_COPY_MOVE(FlightMap)
};

