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
#include <QImageReader>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
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



void FlightMap::paint(QPainter *painter)
{

    // Draw airspaces
    foreach(auto airspace, m_airspaces) {
        auto geoPolygon = airspace.polygon();
        QPolygonF pixelPolygon;
        foreach(auto coordinate, geoPolygon.perimeter()) {
            pixelPolygon.append( toScreenCoordinate(coordinate) );
        }

        if ((airspace.CAT() == "A") ||
            (airspace.CAT() == "B") ||
            (airspace.CAT() == "C") ||
            (airspace.CAT() == "D")) {
            QPainterPath clipPath;
            clipPath.addPolygon(pixelPolygon);
            painter->setClipPath(clipPath);

            painter->setPen({Qt::blue, 2});
            painter->setBrush({});
            painter->drawPolygon(pixelPolygon);

            painter->setClipping(true);
            QColor color(Qt::blue);
            color.setAlphaF(0.2);
            painter->setPen({color, 14});
            painter->setBrush({});
            painter->drawPolygon(pixelPolygon);
            painter->setClipping(false);

            continue;
        }
        if (airspace.CAT() == "CTR") {
            painter->setPen( QPen(Qt::blue, 2, Qt::DashLine));
            QColor colorR(Qt::red);
            colorR.setAlphaF(0.2);
            painter->setBrush(colorR);
            painter->drawPolygon(pixelPolygon);

            QPainterPath clipPath;
            clipPath.addPolygon(pixelPolygon);
            painter->setClipPath(clipPath);
            painter->setClipping(true);
            QColor color(Qt::blue);
            color.setAlphaF(0.2);
            painter->setPen({color, 14});
            painter->setBrush({});
            painter->drawPolygon(pixelPolygon);
            painter->setClipping(false);

            continue;
        }
        if (airspace.CAT() == "FIS") {
            painter->setPen({Qt::black, 2});
            painter->setBrush({});
            painter->drawPolygon(pixelPolygon);
            continue;
        }
        if (airspace.CAT() == "GLD") {
            QColor color(Qt::yellow);
            color.setAlphaF(0.1);
            painter->setPen({Qt::yellow, 2});
            painter->setBrush( color );
            painter->drawPolygon(pixelPolygon);
            continue;
        }
        if (airspace.CAT() == "NRA") {
            QPainterPath clipPath;
            clipPath.addPolygon(pixelPolygon);
            painter->setClipPath(clipPath);

            painter->setPen({Qt::green, 2});
            painter->setBrush({});
            painter->drawPolygon(pixelPolygon);

            painter->setClipping(true);
            QColor color(Qt::green);
            color.setAlphaF(0.2);
            painter->setPen({color, 14});
            painter->setBrush({});
            painter->drawPolygon(pixelPolygon);
            painter->setClipping(false);

            continue;
        }
        if ((airspace.CAT() == "DNG")  ||
            (airspace.CAT() == "P") ||
            (airspace.CAT() == "R")) {
            QPainterPath clipPath;
            clipPath.addPolygon(pixelPolygon);
            painter->setClipPath(clipPath);

            painter->setPen(QPen(Qt::red, 2, Qt::DashLine));
            painter->setBrush({});
            painter->drawPolygon(pixelPolygon);

            painter->setClipping(true);
            QColor color(Qt::red);
            color.setAlphaF(0.2);
            painter->setPen({color, 14});
            painter->setBrush({});
            painter->drawPolygon(pixelPolygon);
            painter->setClipping(false);

            continue;
        }
        if (airspace.CAT() == "RMZ") {
            QColor color(Qt::blue);
            color.setAlphaF(0.2);
            painter->setPen( QPen(Qt::blue, 2, Qt::DashLine) );
            painter->setBrush( color );
            painter->drawPolygon(pixelPolygon);
            continue;
        }
        if (airspace.CAT() == "PJE") {
            painter->setPen( QPen(Qt::red, 2, Qt::DashLine) );
            painter->setBrush( {} );
            painter->drawPolygon(pixelPolygon);
            continue;
        }
        if (airspace.CAT() == "TMZ") {
            painter->setPen( QPen(Qt::black, 2, Qt::DashDotDotLine) );
            painter->setBrush( {} );
            painter->drawPolygon(pixelPolygon);
            continue;
        }

        qWarning() << "Not implemented:" << airspace.CAT();
    }

    // Draw waypoints
    blockedRectagles.clear();
    foreach(auto waypoint, m_waypoints) {
        QImage image(":"+waypoint.icon());
        auto center = toScreenCoordinate(waypoint.coordinate());


        QRectF thisRect(center.x()-image.width()/2.0,
                        center.y()-image.height()/2.0,
                        image.width(),
                        image.height());
        bool intersects = false;
        foreach(auto rect, blockedRectagles) {
            if (rect.intersects(thisRect)) {
                intersects = true;
                break;
            }
        }
        if (intersects) {
            continue;
        }

        blockedRectagles.append(thisRect);

        painter->save();
        painter->translate(center);
        painter->rotate(waypoint.iconOrientation());
        painter->drawImage(qRound(-image.width()/2.0), qRound(-image.height()/2.0), image);
        painter->restore();
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

    m_center = newCenter;
    emit centerChanged();
    update();
}


void FlightMap::setCenter(const QGeoCoordinate& coordinate, QPointF screenPosition)
{
    auto inter = exp2(m_zoom+8.0)*webMercatorProjection(coordinate)-screenPosition+QPointF(width()/2.0, height()/2.0);
    setCenter( inverseWebMercatorProjection(exp2(-m_zoom-8.0)*inter) );
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

    m_zoom = newZoom;
    emit zoomChanged();
    update();
}


void FlightMap::setZoom(double newZoom, QPointF screenPosition)
{
    auto coord = fromScreenCoordinate(screenPosition);
    setZoom(newZoom);
    setCenter(coord, screenPosition);
}
