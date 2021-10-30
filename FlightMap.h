#pragma once

#include <QGeoCoordinate>
#include <QtQuick/QQuickPaintedItem>


class FlightMap : public QQuickPaintedItem
{
    Q_OBJECT

public:
    explicit FlightMap(QQuickItem *parent = nullptr);

    Q_PROPERTY(double bearing READ bearing WRITE setBearing NOTIFY bearingChanged)
    Q_PROPERTY(QGeoCoordinate center READ center WRITE setCenter NOTIFY centerChanged)
    Q_PROPERTY(double zoom READ zoom WRITE setZoom NOTIFY zoomChanged)

    static constexpr double minZoom = 7.0;
    static constexpr double maxZoom = 14.0;

    double bearing() const
    {
        return m_bearing;
    }

    QGeoCoordinate center() const
    {
        return m_center;
    }

    void setBearing(double newBearing);

    void setCenter(const QGeoCoordinate& newCenter);

    void setZoom(double newZoom);

    double zoom() const
    {
        return m_zoom;
    }

signals:
    void bearingChanged();
    void centerChanged();
    void zoomChanged();

protected:
    void paint(QPainter *painter) override;

private:
    QGeoCoordinate m_center {48.00144, 7.76231};
    double m_bearing {0.0};
    double m_zoom {10.0};

    Q_DISABLE_COPY_MOVE(FlightMap)
};


