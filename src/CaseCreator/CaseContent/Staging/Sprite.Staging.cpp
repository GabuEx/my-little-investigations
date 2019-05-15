#include "Sprite.Staging.h"

Staging::Sprite::Sprite(XmlReader *pReader)
{
    pReader->StartElement("Sprite");

    if (pReader->ElementExists("SpriteSheetImageId"))
    {
        SpriteSheetImageId = pReader->ReadTextElement("SpriteSheetImageId");

        pReader->StartElement("SpriteClipRect");
        SpriteClipRect = RectangleWH(pReader);
        pReader->EndElement();

        if (pReader->ElementExists("Offset"))
        {
            pReader->StartElement("Offset");
            SpriteDrawOffset = Vector2(pReader);
            pReader->EndElement();

            pReader->StartElement("OriginalSize");
            OriginalSize = Vector2(pReader);
            pReader->EndElement();
        }
    }

    pReader->EndElement();
}
