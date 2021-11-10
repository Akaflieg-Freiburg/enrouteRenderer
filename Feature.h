
#include <QVariant>
#include <QPainterPath>

#include "vector_tile.pb.h"

class Layer;

class Feature {
public:
    Feature() = default;
    Feature(const vector_tile::Tile_Feature data, Layer *layer);

    const QVariant *value(const QString &key) const;
    vector_tile::Tile_GeomType type() const {return _data.type();}
    QPainterPath path(const QSizeF &factor) const;

    friend bool operator<(const Feature &f1, const Feature &f2);

private:
    const vector_tile::Tile_Feature _data;
    const Layer *_layer;
};

