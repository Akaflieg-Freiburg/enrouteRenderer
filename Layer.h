
#pragma once

#include <QVariant>
#include <QVector>

#include "vector_tile.pb.h"


class Feature;

class Layer {
public:
    Layer();
    Layer(const vector_tile::Tile_Layer *data);

    const QVector<Feature*> &features() const {return _features;}
    const QVector<QVariant> &values() const {return _values;}
    const QHash<QString, google::protobuf::uint32>& keys() const {return _keys;}
    const vector_tile::Tile_Layer *data() const {return _data;}

private:
    const vector_tile::Tile_Layer *_data {nullptr};
    QVector<Feature*> _features;
    QVector<QVariant> _values;
    QHash<QString, google::protobuf::uint32> _keys;
};

