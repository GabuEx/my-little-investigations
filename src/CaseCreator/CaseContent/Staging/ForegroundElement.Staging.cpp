#include "ForegroundElement.Staging.h"

#include "Animation.Staging.h"
#include "Condition.Staging.h"
#include "Encounter.Staging.h"
#include "HitBox.Staging.h"

#include "CaseContentLoadingStager.h"

Staging::ForegroundElementAnimation::ForegroundElementAnimation(XmlReader *pReader)
{
    pReader->StartElement("ForegroundElementAnimation");

    AnimationId = pReader->ReadTextElement("AnimationId");

    pReader->StartElement("Position");
    Position = Vector2(pReader);
    pReader->EndElement();

    pReader->EndElement();
}

void Staging::ForegroundElementAnimation::AddSpritePaths(QMap<QString, QString> &spriteIdToSavePathMap, QString basePath)
{
    CaseContentLoadingStager::GetCurrent()->AnimationIdToAnimationMap[AnimationId]->AddSpritePaths(spriteIdToSavePathMap, basePath);
}

Staging::ForegroundElement::ForegroundElement(XmlReader *pReader)
{
    pReader->StartElement("ForegroundElement");

    LoadFromXmlCore(pReader);

    pReader->EndElement();
}

Staging::ForegroundElement::~ForegroundElement()
{
    for (ForegroundElementAnimation *pAnimation : ForegroundElementAnimationList)
    {
        delete pAnimation;
    }

    ForegroundElementAnimationList.clear();

    delete pHitBox;
    pHitBox = NULL;
    delete pEncounter;
    pEncounter = NULL;
    delete pCondition;
    pCondition = NULL;
}

void Staging::ForegroundElement::AddSpritePaths(QMap<QString, QString> &spriteIdToSavePathMap, QString basePath)
{
    spriteIdToSavePathMap[SpriteId] = basePath + ".png";

    for (int i = 0; i < ForegroundElementAnimationList.size(); i++)
    {
        char fileName[256];
        sprintf(fileName, "Animation%d", (i + 1));

        ForegroundElementAnimationList[i]->AddSpritePaths(spriteIdToSavePathMap, basePath + fileName);
    }
}

void Staging::ForegroundElement::AddDialogPaths(QMap<QString, QString> &dialogIdToSavePathMap)
{
    pEncounter->AddDialogPaths(dialogIdToSavePathMap);
}

void Staging::ForegroundElement::LoadFromXmlCore(XmlReader *pReader)
{
    pEncounter = NULL;
    pHitBox = NULL;
    pCondition = NULL;

    if (pReader->ElementExists("Name"))
    {
        Name = pReader->ReadTextElement("Name");
    }

    if (pReader->ElementExists("SpriteId"))
    {
        SpriteId = pReader->ReadTextElement("SpriteId");
    }

    pReader->StartElement("Position");
    Position = Vector2(pReader);
    pReader->EndElement();

    AnchorPosition = pReader->ReadIntElement("AnchorPosition");

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

    if (pReader->ElementExists("HitBox"))
    {
        pReader->StartElement("HitBox");
        pHitBox = new HitBox(pReader);
        pReader->EndElement();
    }

    pReader->StartElement("ClickPolygon");
    ClickPolygon = GeometricPolygon(pReader);
    pReader->EndElement();

    if (pReader->ElementExists("Encounter"))
    {
        pReader->StartElement("Encounter");
        pEncounter = new Encounter(pReader);
        pReader->EndElement();
    }

    if (pReader->ElementExists("ZoomedViewId"))
    {
        ZoomedViewId = pReader->ReadTextElement("ZoomedViewId");
    }

    pReader->StartElement("Animations");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        ForegroundElementAnimationList.push_back(new ForegroundElementAnimation(pReader));
    }

    pReader->EndElement();

    if (pReader->ElementExists("Condition"))
    {
        pReader->StartElement("Condition");
        pCondition = new Condition(pReader);
        pReader->EndElement();
    }
}

Staging::HiddenForegroundElement::HiddenForegroundElement(XmlReader *pReader)
{
    pReader->StartElement("HiddenForegroundElement");

    LoadFromXmlCore(pReader);
    RequiredPartnerId = pReader->ReadTextElement("RequiredPartnerId");
    IsRequired = pReader->ReadBooleanElement("IsRequired");

    pReader->EndElement();
}
