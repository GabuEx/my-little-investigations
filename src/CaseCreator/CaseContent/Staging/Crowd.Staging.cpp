#include "Crowd.Staging.h"

#include "Encounter.Staging.h"
#include "HitBox.Staging.h"

Staging::Crowd::Crowd(XmlReader *pReader)
{
    pReader->StartElement("Crowd");

    Name = pReader->ReadTextElement("Name");
    AnchorPosition = pReader->ReadIntElement("AnchorPosition");

    pReader->StartElement("Position");
    Position = Vector2(pReader);
    pReader->EndElement();

    pReader->StartElement("HitBox");
    pHitBox = new HitBox(pReader);
    pReader->EndElement();

    pReader->StartElement("ClickPolygon");
    ClickPolygon = GeometricPolygon(pReader);
    pReader->EndElement();

    if (pReader->ElementExists("InteractionLocation"))
    {
        pReader->StartElement("InteractionLocation");
        InteractionLocation = Vector2(pReader);
        pReader->EndElement();
    }
    else
    {
        InteractionLocation = Vector2(-1, -1);
    }

    InteractFromAnywhere = pReader->ReadBooleanElement("InteractFromAnywhere");

    pReader->StartElement("Encounter");
    pEncounter = new Encounter(pReader);
    pReader->EndElement();

    VideoId = pReader->ReadTextElement("VideoId");

    pReader->EndElement();
}

Staging::Crowd::~Crowd()
{
    delete pEncounter;
    pEncounter = NULL;
    delete pHitBox;
    pHitBox = NULL;
}

void Staging::Crowd::AddDialogPaths(QMap<QString, QString> &dialogIdToSavePathMap)
{
    pEncounter->AddDialogPaths(dialogIdToSavePathMap);
}
