#pragma once

#include "geomaps/Airspace.h"
#include "geomaps/Waypoint.h"

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

    QPointF fromGeoCoordinate(const QGeoCoordinate& coordinate) const;

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
    QPointF toWorldPixelCoordinate(const QGeoCoordinate& coordinate) const {
        return QPointF(
            exp2(m_zoom)*256.0*(coordinate.longitude()+180.0)/360.0,
            exp2(m_zoom)*256.0*(M_PI - log(tan(qDegreesToRadians(coordinate.latitude()/2.0 + 45.0))))/(2.0*M_PI)
            );
    }

    QVector<GeoMaps::Airspace> m_airspaces;
    QVector<GeoMaps::Waypoint> m_waypoints;


    QGeoCoordinate m_center {48.00144, 7.76231};
    double m_bearing {0.0};
    double m_zoom {10.0};

    Q_DISABLE_COPY_MOVE(FlightMap)
};


