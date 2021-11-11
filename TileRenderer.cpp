
#include "compression.h"
#include "Feature.h"
#include "Layer.h"
#include "TileRenderer.h"
#include "vector_tile.pb.h"

#include <QDebug>
#include <QtEndian>
#include <QFile>
#include <QImage>
#include <QPainter>
#include <QSqlDatabase>
#include <QSqlQuery>


#define GZIP_MAGIC      0x1F8B
#define GZIP_MAGIC_MASK 0xFFFF
#define PBF_MAGIC       0x1A00
#define PBF_MAGIC_MASK  0xFF00

static bool isMagic(quint16 magic, quint16 mask, quint16 value)
{
    return ((qFromBigEndian(value) & mask) == magic);
}

static bool isGZIPPBF(quint16 magic)
{
    return isMagic(GZIP_MAGIC, GZIP_MAGIC_MASK, magic);
}

static bool isPlainPBF(quint16 magic)
{
    return isMagic(PBF_MAGIC, PBF_MAGIC_MASK, magic);
}




TileRenderer::TileRenderer(QObject* parent)
    : QObject(parent)
{

}


QByteArray TileRenderer::getTileData(int zoom, int column, int row)
{
    //QString fileName("/home/kebekus/Austausch/ICAO+VFR+DE+21Q1.mbtiles");
    //QString fileName("/home/kebekus/Austausch/ed_256.mbtiles");
    //QString fileName("/home/kebekus/Austausch/aviation_maps/Europe/Germany.mbtiles");
    QString fileName("/mnt/storage/mbtiles/raw/osm-2017-07-03-v3.6.1-europe_germany.mbtiles");

    auto db = QSqlDatabase::database(fileName);
    if (!db.isValid()) {
        db = QSqlDatabase::addDatabase("QSQLITE", fileName);
        db.setDatabaseName(fileName);
        db.open();
        if (db.isOpenError()) {
            qWarning() << "Unable to open database";
            return {};
        }
    }

    auto rowFlipped = (1 << zoom)-1-row;
    QString queryString = QString("select zoom_level, tile_column, tile_row, tile_data from tiles where zoom_level=%1 and tile_column=%2 and tile_row=%3;").arg(zoom).arg(column).arg(rowFlipped);
    QSqlQuery query(db);
    query.exec(queryString);
    if (!query.first()) {
        qWarning() << queryString  << "Query has no result";
        return {};
    }
    return query.value(3).toByteArray();
}


QImage TileRenderer::render(int zoom, int column, int row)
{
    auto tileData = getTileData(zoom, column, row);
    if (tileData.size() < 2) {
        return {};
    }

    // Check if data is gzip compressed. Uncompress if it is
    quint16 magic = *((quint16*)tileData.constData());
    if (isGZIPPBF(magic)) {
        tileData = compression::gzUncompress(tileData);
        magic = *((quint16*)tileData.constData());
    }

    // Check if data is PBF.
    if (isPlainPBF(magic)) {
        return renderTile(tileData);
    }

    // If data is not PBF, we assume that it is a PNG image
    return QImage::fromData(tileData, "PNG");
}

static QVariant value(const vector_tile::Tile_Value &val)
{
    if (val.has_bool_value())
        return QVariant(val.bool_value());
    else if (val.has_int_value())
        return QVariant((qlonglong)val.int_value());
    else if (val.has_sint_value())
        return QVariant((qlonglong)val.sint_value());
    else if (val.has_uint_value())
        return QVariant((qulonglong)val.uint_value());
    else if (val.has_float_value())
        return QVariant(val.float_value());
    else if (val.has_double_value())
        return QVariant(val.double_value());
    else if (val.has_string_value())
        return QVariant(QString::fromStdString(val.string_value()));
    else
        return QVariant();
}


void renderLandcover(QPainter* painter, Layer& layer)
{
    painter->setPen({});
    foreach(auto feature, layer.features()) {
        auto classTag = feature->value("class", layer).toString();

        if (classTag == "grass") {
            painter->setBrush( QColor::fromHslF(82/360.0, .46, .72, .45) );
        } else if (classTag == "wood") {
            painter->setBrush( QColor::fromHslF(82/360.0, .46, .72) );
        } else {
//            qWarning() << "Layer landcover, unknown tag" << classTag << subclassTag;
            continue;
        }
        auto path = feature->path({1/8.0, 1/8.0});
        painter->drawPath(path);
    }
}


void renderPlaces(QPainter* painter, Layer& layer)
{

    foreach(auto feature, layer.features()) {

        auto classTag = feature->value("class", layer).toString();
        auto nameTag = feature->value("name", layer).toString();

        auto path = feature->path({1/8.0, 1/8.0});
        if ((classTag == "city") || (classTag == "town") || (classTag == "village")) {
            painter->drawText( path.pointAtPercent(0.5), nameTag);
        }

    }

}


void renderTransportation(QPainter* painter, Layer& layer)
{

    QPainterPath motorway;
    QPainterPath primary;
    QPainterPath secondary;
    QPainterPath railroad;

    foreach(auto feature, layer.features()) {
        auto classTag = feature->value("class", layer).toString();
        auto path = feature->path({1/8.0, 1/8.0});

        if ((classTag == "motorway") || (classTag == "trunk")){
            motorway.addPath(path);
        }
        if (classTag == "primary") {
            primary.addPath(path);
        }
        if (classTag == "secondary") {
            secondary.addPath(path);
        }
        if (classTag == "rail") {
            railroad.addPath(path);
        }
    }

    // Draw road background
    QPen pen;
    pen.setColor( Qt::red );
    pen.setWidth(8);
    painter->setPen( pen );
    painter->drawPath(motorway);
    painter->drawPath(primary);
    painter->drawPath(secondary);

    // Draw motorway foreground
    pen.setWidth(6);
    pen.setColor( pen.color().lighter() );
    painter->setPen( pen );
    painter->drawPath(motorway);

    // Draw primary
    pen.setColor( Qt::yellow );
    painter->setPen( pen );
    painter->drawPath(primary);

    // Draw secondary
    pen.setColor( Qt::white );
    painter->setPen( pen );
    painter->drawPath(secondary);

    // Draw railroads
    pen.setColor( Qt::black );
    pen.setWidth(2);
    painter->setPen( pen );
    painter->drawPath(railroad);

}


void renderWater(QPainter* painter, Layer& layer)
{
    QPen blue(QColor::fromHslF( 205/360.0, .56, .73));
    blue.setWidth(2);
    painter->setPen( blue );
    painter->setBrush( QColor::fromHslF( 205/360.0, .56, .73) );

    foreach(auto feature, layer.features()) {
        auto path = feature->path({1/8.0, 1/8.0});
        painter->drawPath(path);
    }
}


QImage TileRenderer::renderTile(QByteArray PBFdata)
{

    QImage image(imageSize, imageSize, QImage::Format_RGB32);
    image.fill( QColor::fromHslF(47/360.0, .26, .88) );


    // Parse protobuffer
    vector_tile::Tile data;
    if (!data.ParseFromArray(PBFdata.constData(), PBFdata.size())) {
        qCritical() << "Invalid PBF data";
        return {};
    }
    // Generate layers
    QHash<QString, Layer> layers;
    for(int i=0; i< data.layers_size(); i++) {
        auto PBFLayer = data.layers(i);
        auto name = QString::fromStdString(PBFLayer.name());
        layers[name] = Layer(&PBFLayer);
    }


    // Go through layers
    QPainter paint(&image);
    paint.setRenderHint(QPainter::Antialiasing);

    if (layers.contains("water")) {
        renderWater(&paint, layers["water"]);
    }
    if (layers.contains("landcover")) {
        renderLandcover(&paint, layers["landcover"]);
    }
    if (layers.contains("transportation")) {
        renderTransportation(&paint, layers["transportation"]);
    }
    if (layers.contains("place")) {
        renderPlaces(&paint, layers["place"]);
    }

    paint.end();

    return image;
}
