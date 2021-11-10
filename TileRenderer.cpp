
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
    QString fileName("/home/kebekus/Austausch/aviation_maps/Europe/Germany.mbtiles");

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
    qWarning() << "Render Landcover";

    painter->setPen({});
    painter->setBrush(Qt::green);
    foreach(auto feature, layer.features()) {
//        qWarning() << "A1";
        auto path = feature->path({1/8.0, 1/8.0});
//        qWarning() << "A2" << path;
        painter->drawPath(path);
//      qWarning() << "A3";
    }
}


void renderTransportation(QPainter* painter, vector_tile::Tile_Layer& pbfLayer)
{
    qWarning() << "Render Transportation";

    for (int i = 0; i < pbfLayer.features().size(); i++) {
        auto feature = pbfLayer.features(i);
    }
}


QImage TileRenderer::renderTile(QByteArray PBFdata)
{

    QImage image(imageSize, imageSize, QImage::Format_RGB32);
    image.fill(Qt::white);


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

    if (layers.contains("landcover")) {
        renderLandcover(&paint, layers["landcover"]);
    }

    paint.end();

    return image;
}
