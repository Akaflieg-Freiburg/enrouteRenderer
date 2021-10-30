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
#include <QPainter>
#include <cmath>

#include "FlightMap.h"


FlightMap::FlightMap(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{

}


void FlightMap::paint(QPainter *painter)
{

    // Draw underlying white, slightly tranparent rectangle
    painter->fillRect(0, 0, static_cast<int>(width()), static_cast<int>(height()), QColor(0xe0, 0xe0, 0x00, 0xe0));

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
}
