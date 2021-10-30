/***************************************************************************
 *   Copyright (C) 2019 by Stefan Kebekus                                  *
 *   stefan.kebekus@gmail.com                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
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

#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPainter>
#include <QSet>
#include <cmath>

#include "FlightMap.h"


FlightMap::FlightMap(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{

    QFile file(":data/Germany.geojson");
    file.open(QIODevice::ReadOnly);
    auto document = QJsonDocument::fromJson(file.readAll());
    file.close();

    foreach(auto value, document.object()["features"].toArray()) {
        auto object = value.toObject();

        // Check if the current object is a waypoint. If so, add it to the list of waypoints.
        GeoMaps::Waypoint wp(object);
        if (wp.isValid()) {
            m_waypoints.append(wp);
            continue;
        }

        // Check if the current object is an airspace. If so, add it to the list of airspaces.
        GeoMaps::Airspace as(object);
        if (as.isValid()) {
            m_airspaces.append(as);
            continue;
        }
    }

}


auto FlightMap::fromGeoCoordinate(const QGeoCoordinate& coordinate) const -> QPointF
{
    if (!coordinate.isValid()) {
        return {};
    }

    auto centerInWorldPixel = toWorldPixelCoordinate(m_center);
    auto coordinateInWorldPixel = toWorldPixelCoordinate(coordinate);

    return coordinateInWorldPixel-centerInWorldPixel+QPointF(width()/2.0, height()/2.0);
}


void FlightMap::paint(QPainter *painter)
{

    // Draw underlying white, slightly tranparent rectangle
    painter->fillRect(0, 0, static_cast<int>(width()), static_cast<int>(height()), QColor(0xe0, 0xe0, 0x00, 0xe0));

    foreach(auto waypoint, m_waypoints) {
        auto center = fromGeoCoordinate(waypoint.coordinate());
        painter->fillRect(center.x(), center.y(), 2, 2, Qt::black);
    }

}


void FlightMap::setBearing(double newBearing)
{
    if (!qIsFinite(newBearing)) {
        return;
    }
    if (newBearing < 0.0) {
        return;
    }
    if (newBearing > 360.0) {
        return;
    }
    if (newBearing == m_bearing) {
        return;
    }

    qWarning() << "new bearing" << newBearing;
    m_bearing = newBearing;
    emit bearingChanged();
    update();
}


void FlightMap::setCenter(const QGeoCoordinate& newCenter)
{
    if (!newCenter.isValid()) {
        return;
    }
    if (newCenter == m_center) {
        return;
    }

    qWarning() << "new center" << newCenter;
    m_center = newCenter;
    emit centerChanged();
    update();
}


void FlightMap::setZoom(double newZoom)
{
    if (!qIsFinite(newZoom)) {
        return;
    }
    if (newZoom < minZoom) {
        return;
    }
    if (newZoom > maxZoom) {
        return;
    }
    if (newZoom == m_zoom) {
        return;
    }

    qWarning() << "new zoom" << newZoom;
    m_zoom = newZoom;
    emit zoomChanged();
    update();
}
