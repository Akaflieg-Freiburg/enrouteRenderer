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

    Q_INVOKABLE QPointF toScreenCoordinate(const QGeoCoordinate& coordinate)
    {
        auto delta = webMercatorProjection(coordinate)-webMercatorProjection(m_center);
        return exp2(m_zoom+8.0)*delta+QPointF(width()/2.0, height()/2.0);
    }

    Q_INVOKABLE QGeoCoordinate fromScreenCoordinate(const QPointF& screenCoordinate)
    {
        auto webMercatorCoords = exp2(-m_zoom-8.0)*(screenCoordinate-QPointF(width()/2.0, height()/2.0))+webMercatorProjection(m_center);
        return inverseWebMercatorProjection(webMercatorCoords);
    }

    void setBearing(double newBearing);

    void setCenter(const QGeoCoordinate& newCenter);

    Q_INVOKABLE void setCenter(const QGeoCoordinate& coordinate, QPointF screenCoordinate);

    void setZoom(double newZoom);

    Q_INVOKABLE void setZoom(double newZoom, QPointF screenCoordinate);

    double zoom() const
    {
        return m_zoom;
    }

    /*! \brief WebMercator projection
     *
     *  @param coordinate geographic coordinate
     *
     *  @returns webMercator projection of the coordinate, where x and y are in the range [0.0 … 1.0]
     */
    Q_INVOKABLE static QPointF webMercatorProjection(const QGeoCoordinate& coordinate)
    {
        auto x = (coordinate.longitude()+180.0)/360.0;
        auto y = qBound(0.0, (M_PI - log(tan(qDegreesToRadians(coordinate.latitude()/2.0 + 45.0))))/(2.0*M_PI), 1.0);
        return QPointF(x,y);
    }

    Q_INVOKABLE static QGeoCoordinate inverseWebMercatorProjection(const QPointF& point)
    {
        auto longitude = (360.0*point.x())-180.0;
        auto latitude = 2.0*(qRadiansToDegrees(atan(exp(-(2.0*M_PI)*point.y()+M_PI)))-45.0);
        return QGeoCoordinate(latitude, longitude);
    }

signals:
    void bearingChanged();
    void centerChanged();
    void zoomChanged();

protected:
    void paint(QPainter *painter) override;

private:
    QVector<GeoMaps::Airspace> m_airspaces;
    QVector<GeoMaps::Waypoint> m_waypoints;


    QGeoCoordinate m_center {48.00144, 7.76231};
    double m_bearing {0.0};
    double m_zoom {10.0};

    Q_DISABLE_COPY_MOVE(FlightMap)
};


