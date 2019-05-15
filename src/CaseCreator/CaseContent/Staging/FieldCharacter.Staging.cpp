#include "FieldCharacter.Staging.h"

#include "Condition.Staging.h"
#include "HitBox.Staging.h"

#include "CaseContentLoadingStager.h"

Staging::FieldCharacter::FieldCharacter(XmlReader *pReader)
{
    for (int i = 0; i < FieldCharacterDirection_Count; i++)
    {
        CharacterStandingAnimationIds[i] = "";
        CharacterWalkingAnimationIds[i] = "";
        CharacterRunningAnimationIds[i] = "";
    }

    pReader->StartElement("FieldCharacter");
    Id = pReader->ReadTextElement("Id");
    Name = pReader->ReadTextElement("Name");

    pReader->StartElement("CharacterStandingAnimationIds");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        FieldCharacterDirection direction = StringToFieldCharacterDirection(pReader->ReadTextElement("Direction"));
        CharacterStandingAnimationIds[(int)direction] = pReader->ReadTextElement("Id");
    }

    pReader->EndElement();

    pReader->StartElement("CharacterWalkingAnimationIds");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        FieldCharacterDirection direction = StringToFieldCharacterDirection(pReader->ReadTextElement("Direction"));
        CharacterWalkingAnimationIds[(int)direction] = pReader->ReadTextElement("Id");
    }

    pReader->EndElement();

    pReader->StartElement("CharacterRunningAnimationIds");
    pReader->StartList("Entry");

    while (pReader->MoveToNextListItem())
    {
        FieldCharacterDirection direction = StringToFieldCharacterDirection(pReader->ReadTextElement("Direction"));
        CharacterRunningAnimationIds[(int)direction] = pReader->ReadTextElement("Id");
    }

    pReader->EndElement();

    pReader->StartElement("Position");
    Position = Vector2(pReader);
    pReader->EndElement();

    pHitBox = new HitBox(pReader);
    Direction = StringToCharacterDirection(pReader->ReadTextElement("Direction"));
    SpriteDirection = StringToFieldCharacterDirection(pReader->ReadTextElement("SpriteDirection"));
    AnchorPosition = pReader->ReadIntElement("AnchorPosition");

    pReader->StartElement("ClickRect");
    ClickRect = RectangleWH(pReader);
    pReader->EndElement();

    if (pReader->ElementExists("ClickEncounterId"))
    {
        ClickEncounterId = pReader->ReadTextElement("ClickEncounterId");
    }

    if (pReader->ElementExists("ClickCutsceneId"))
    {
        ClickCutsceneId = pReader->ReadTextElement("ClickCutsceneId");
    }

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

    if (pReader->ElementExists("Condition"))
    {
        pReader->StartElement("Condition");
        pCondition = new Condition(pReader);
        pReader->EndElement();
    }
    else
    {
        pCondition = NULL;
    }

    pReader->EndElement();
}

Staging::FieldCharacter::~FieldCharacter()
{
    delete pHitBox;
    pHitBox = NULL;
    delete pCondition;
    pCondition = NULL;
}

void Staging::FieldCharacter::AddSpritePaths(QMap<QString, QString> &spriteIdToSavePathMap)
{
    QString baseDir = "Characters/" + Id + "/FieldSprites/";

    for (int i = 0; i < FieldCharacterDirection_Count; i++)
    {
        CaseContentLoadingStager::GetCurrent()->AnimationIdToAnimationMap[CharacterStandingAnimationIds[i]]->AddSpritePaths(spriteIdToSavePathMap, baseDir + "Standing" + FieldCharacterDirectionToString((FieldCharacterDirection) i));
        CaseContentLoadingStager::GetCurrent()->AnimationIdToAnimationMap[CharacterWalkingAnimationIds[i]]->AddSpritePaths(spriteIdToSavePathMap, baseDir + "Walking" + FieldCharacterDirectionToString((FieldCharacterDirection) i));
        CaseContentLoadingStager::GetCurrent()->AnimationIdToAnimationMap[CharacterRunningAnimationIds[i]]->AddSpritePaths(spriteIdToSavePathMap, baseDir + "Running" + FieldCharacterDirectionToString((FieldCharacterDirection) i));
    }
}
