
#include "Feature.h"
#include "Layer.h"

#define MOVE_TO    1
#define LINE_TO    2
#define CLOSE_PATH 7


Feature::Feature(const vector_tile::Tile_Feature data, Layer *layer)
      : _data(data), _layer(layer)
{
    ;
}


QVariant Feature::value(const QString& key, const Layer& layer) const
{
    const QHash<QString, google::protobuf::uint32> &keys(layer.keys());
    QHash<QString, google::protobuf::uint32>::const_iterator it(keys.find(key));
    if (it == keys.constEnd())
        return 0;

    google::protobuf::uint32 index = *it;
    for (int i = 0; i < _data.tags_size(); i = i + 2)
        if (_data.tags(i) == index)
            return layer.values().at(_data.tags(i+1));

    return 0;
}


static inline qint32 zigzag32decode(quint32 value)
{
    return static_cast<qint32>((value >> 1u) ^ static_cast<quint32>(
      -static_cast<qint32>(value & 1u)));
}


static inline QPoint parameters(quint32 v1, quint32 v2)
{
    return QPoint(zigzag32decode(v1), zigzag32decode(v2));
}


QPainterPath Feature::path(const QSizeF &factor) const
{
    QPoint cursor;
    QPainterPath path;

    for (int i = 0; i < _data.geometry_size(); i++) {
        quint32 g = _data.geometry(i);
        unsigned cmdId = g & 0x7;
        unsigned cmdCount = g >> 3;

        if (cmdId == MOVE_TO) {
            for (unsigned j = 0; j < cmdCount; j++) {
                QPoint offset = parameters(_data.geometry(i+1),
                  _data.geometry(i+2));
                i += 2;
                cursor += offset;
                path.moveTo(QPointF(cursor.x() * factor.width(),
                  cursor.y() * factor.height()));
            }
        } else if (cmdId == LINE_TO) {
            for (unsigned j = 0; j < cmdCount; j++) {
                QPoint offset = parameters(_data.geometry(i+1),
                  _data.geometry(i+2));
                i += 2;
                cursor += offset;
                path.lineTo(QPointF(cursor.x() * factor.width(),
                  cursor.y() * factor.height()));
            }
        } else if (cmdId == CLOSE_PATH) {
            path.closeSubpath();
            path.moveTo(cursor);
        }
    }

    return path;
}
