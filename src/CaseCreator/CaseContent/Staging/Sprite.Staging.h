#ifndef SPRITE_STAGING_H
#define SPRITE_STAGING_H

#include "Vector2.h"
#include "Rectangle.h"
#include "XmlReader.h"

#include <QString>

namespace Staging
{

class Sprite
{
public:
    Sprite(XmlReader *pReader);

    QString SpriteSheetImageId;
    Vector2 SpriteDrawOffset;
    Vector2 OriginalSize;
    RectangleWH SpriteClipRect;
};

}

#endif // SPRITE_H
