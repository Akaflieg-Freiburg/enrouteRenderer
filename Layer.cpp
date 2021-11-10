
#include "Feature.h"
#include "Layer.h"

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

Layer::Layer()
{

}

Layer::Layer(const vector_tile::Tile_Layer *data)
    : _data(data)
{
    _keys.reserve(data->keys_size());
    for (int i = 0; i < data->keys_size(); i++)
        _keys.insert(QString::fromStdString(data->keys(i)), i);
    _values.reserve(data->values_size());
    for (int i = 0; i < data->values_size(); i++)
        _values.append(value(data->values(i)));

    _features.reserve(data->features_size());
    for (int i = 0; i < data->features_size(); i++)
        _features.append( new Feature(data->features(i), this) );
#warning
//    std::sort(_features.begin(), _features.end());
}
