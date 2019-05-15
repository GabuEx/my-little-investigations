#include "Character.h"

#include "CaseContent.h"
#include "XmlReader.h"
#include "XmlWriter.h"
#include "MLIException.h"

#include "Condition.h"

#include "Staging/CaseContentLoadingStager.h"
#include "Staging/DialogCharacter.Staging.h"
#include "Staging/Enums.Staging.h"

#include "CaseCreator/UIComponents/LocationTab/FieldCharacterManipulator.h"
#include "CaseCreator/UIComponents/CharacterTab/CharacterDialogSpriteManipulator.h"

Character::Character()
{
    pHitBox = NULL;

    for (int i = 0; i < Staging::FieldCharacterDirection_Count; i++)
    {
        standingAnimations[i] = new Animation();
        walkingAnimations[i] = new Animation();
        runningAnimations[i] = new Animation();
    }

    standingSprite.SetId("Standing");
    walkingSprite.SetId("Walking");
    runningSprite.SetId("Running");

    for (int i = 0; i < FieldCharacterDirectionCount; i++)
    {
        standingSprite.SetAnimationForDirection(static_cast<FieldCharacterDirection>(i), &standingAnimations[i]);
        walkingSprite.SetAnimationForDirection(static_cast<FieldCharacterDirection>(i), &walkingAnimations[i]);
        runningSprite.SetAnimationForDirection(static_cast<FieldCharacterDirection>(i), &runningAnimations[i]);
    }
}

Character::Character(QString id, QString name, QString defaultEmotionId)
    : Character()
{
    this->id = id;
    this->name = name;
    this->defaultEmotionId = defaultEmotionId;

    this->AddDialogEmotion(defaultEmotionId);
}

Character::Character(Staging::DialogCharacter *pStagingDialogCharacter, Staging::FieldCharacter *pStagingFieldCharacter)
    : Character()
{
    id = pStagingDialogCharacter->Id;

    if (id != pStagingFieldCharacter->Id)
    {
        throw new MLIException("Incompatible dialog and field characters used together.");
    }

    name = pStagingFieldCharacter->Name;
    defaultEmotionId = pStagingDialogCharacter->DefaultEmotionId;

    for (QMap<QString, QString>::iterator iter = pStagingDialogCharacter->CharacterEmotionBaseSpriteIds.begin();
         iter != pStagingDialogCharacter->CharacterEmotionBaseSpriteIds.end();
         iter++)
    {
        QString emotionId = iter.key();
        DialogEmotion *pEmotion = GetDialogEmotion(emotionId);

        pEmotion->SetBaseFilePath(CaseContentLoadingStager::GetCurrent()->GetSpriteFilePathFromId(iter.value()));

        if (pStagingDialogCharacter->CharacterEmotionEyeSpriteIds.contains(emotionId))
        {
            for (QString eyeSpriteId : pStagingDialogCharacter->CharacterEmotionEyeSpriteIds[emotionId])
            {
                pEmotion->AddEyeFrame(CaseContentLoadingStager::GetCurrent()->GetSpriteFilePathFromId(eyeSpriteId));
            }
        }

        if (pStagingDialogCharacter->CharacterEmotionMouthSpriteIds.contains(emotionId))
        {
            for (QString mouthSpriteId : pStagingDialogCharacter->CharacterEmotionMouthSpriteIds[emotionId])
            {
                pEmotion->AddMouthFrame(CaseContentLoadingStager::GetCurrent()->GetSpriteFilePathFromId(mouthSpriteId));
            }
        }
    }

    for (int i = 0; i < Staging::FieldCharacterDirection_Count; i++)
    {
        if (pStagingFieldCharacter->CharacterStandingAnimationIds[i].length() > 0)
        {
            standingAnimations[i] = new Animation(CaseContentLoadingStager::GetCurrent()->AnimationIdToAnimationMap[pStagingFieldCharacter->CharacterStandingAnimationIds[i]]);
        }
        else
        {
            standingAnimations[i] = NULL;
        }

        if (pStagingFieldCharacter->CharacterWalkingAnimationIds[i].length() > 0)
        {
            walkingAnimations[i] = new Animation(CaseContentLoadingStager::GetCurrent()->AnimationIdToAnimationMap[pStagingFieldCharacter->CharacterWalkingAnimationIds[i]]);
        }
        else
        {
            walkingAnimations[i] = NULL;
        }

        if (pStagingFieldCharacter->CharacterRunningAnimationIds[i].length() > 0)
        {
            runningAnimations[i] = new Animation(CaseContentLoadingStager::GetCurrent()->AnimationIdToAnimationMap[pStagingFieldCharacter->CharacterRunningAnimationIds[i]]);
        }
        else
        {
            runningAnimations[i] = NULL;
        }
    }

    pHitBox = new HitBox(pStagingFieldCharacter->pHitBox);
    anchorPosition = pStagingFieldCharacter->AnchorPosition;
    clickRect = pStagingFieldCharacter->ClickRect;
}

Character::~Character()
{
    for (int i = 0; i < FieldCharacterDirectionCount; i++)
    {
        if (standingAnimations[i] != NULL)
        {
            delete standingAnimations[i];
            standingAnimations[i] = NULL;
        }

        if (walkingAnimations[i] != NULL)
        {
            delete walkingAnimations[i];
            walkingAnimations[i] = NULL;
        }

        if (runningAnimations[i] != NULL)
        {
            delete runningAnimations[i];
            runningAnimations[i] = NULL;
        }
    }

    delete pHitBox;
    pHitBox = NULL;

    for (QString emotionId : emotionByIdMap.keys())
    {
        delete emotionByIdMap[emotionId];
    }

    emotionByIdMap.clear();
}

void Character::SaveElementsToXml(XmlWriter *pWriter)
{
    XmlStorableObject::SaveElementsToXml(pWriter);

    pWriter->StartElement("FieldAnimations");

    pWriter->StartElement("StandingAnimations");

    for (int i = 0; i < FieldCharacterDirectionCount; i++)
    {
        if (standingAnimations[i] != NULL)
        {
            pWriter->StartElement("StandingAnimation");

            pWriter->WriteTextElement("Direction", FieldCharacterDirectionToString((FieldCharacterDirection)i));
            pWriter->StartElement("Animation");
            standingAnimations[i]->SaveToXml(pWriter);
            pWriter->EndElement();

            pWriter->EndElement();
        }
    }

    pWriter->EndElement();

    pWriter->StartElement("WalkingAnimations");

    for (int i = 0; i < FieldCharacterDirectionCount; i++)
    {
        if (walkingAnimations[i] != NULL)
        {
            pWriter->StartElement("WalkingAnimation");

            pWriter->WriteTextElement("Direction", FieldCharacterDirectionToString((FieldCharacterDirection)i));
            pWriter->StartElement("Animation");
            walkingAnimations[i]->SaveToXml(pWriter);
            pWriter->EndElement();

            pWriter->EndElement();
        }
    }

    pWriter->EndElement();

    pWriter->StartElement("RunningAnimations");

    for (int i = 0; i < FieldCharacterDirectionCount; i++)
    {
        if (runningAnimations[i] != NULL)
        {
            pWriter->StartElement("RunningAnimation");

            pWriter->WriteTextElement("Direction", FieldCharacterDirectionToString((FieldCharacterDirection)i));
            pWriter->StartElement("Animation");
            runningAnimations[i]->SaveToXml(pWriter);
            pWriter->EndElement();

            pWriter->EndElement();
        }
    }

    pWriter->EndElement();

    pWriter->EndElement();
}

void Character::LoadElementsFromXml(XmlReader *pReader)
{
    XmlStorableObject::LoadElementsFromXml(pReader);

    emotionIdList.clear();

    for (QString emotionId : emotionByIdMap.keys())
    {
        emotionIdList.push_back(emotionId);
    }

    pReader->StartElement("FieldAnimations");

    pReader->StartElement("StandingAnimations");
    pReader->StartList("StandingAnimation");

    while (pReader->MoveToNextListItem())
    {
        FieldCharacterDirection direction = StringToFieldCharacterDirection(pReader->ReadTextElement("Direction"));

        pReader->StartElement("Animation");
        standingAnimations[direction] = new Animation(pReader);
        pReader->EndElement();
    }

    pReader->EndElement();

    pReader->StartElement("WalkingAnimations");
    pReader->StartList("WalkingAnimation");

    while (pReader->MoveToNextListItem())
    {
        FieldCharacterDirection direction = StringToFieldCharacterDirection(pReader->ReadTextElement("Direction"));

        pReader->StartElement("Animation");
        walkingAnimations[direction] = new Animation(pReader);
        pReader->EndElement();
    }

    pReader->EndElement();

    pReader->StartElement("RunningAnimations");
    pReader->StartList("RunningAnimation");

    while (pReader->MoveToNextListItem())
    {
        FieldCharacterDirection direction = StringToFieldCharacterDirection(pReader->ReadTextElement("Direction"));

        pReader->StartElement("Animation");
        runningAnimations[direction] = new Animation(pReader);
        pReader->EndElement();
    }

    pReader->EndElement();

    pReader->EndElement();
}

QString Character::DialogEmotion::GetBaseFilePath()
{
    return baseFilePath;
}

QString Character::DialogEmotion::GetEyeFilePath(int eyeIndex)
{
    return
        this->eyeFilePathList.count() > eyeIndex ?
            this->eyeFilePathList[eyeIndex] :
            "";
}

QStringList Character::DialogEmotion::GetEyeFilePaths()
{
    return eyeFilePathList;
}

QString Character::DialogEmotion::GetMouthFilePath(int mouthIndex)
{
    return
        this->mouthFilePathList.count() > mouthIndex ?
            this->mouthFilePathList[mouthIndex] :
            "";
}

QString Character::DialogEmotion::GetRandomMouthFilePath()
{
    if (this->mouthFilePathList.count() == 0)
    {
        return "";
    }

    int newMouthIndex = this->lastMouthIndex;

    while (this->lastMouthIndex == newMouthIndex)
    {
        newMouthIndex = qrand() % this->mouthFilePathList.count();
    }

    this->lastMouthIndex = newMouthIndex;
    return this->mouthFilePathList[this->lastMouthIndex];
}

QStringList Character::DialogEmotion::GetMouthFilePaths()
{
    return mouthFilePathList;
}

int Character::DialogEmotion::GetEyeDuration(int eyeIndex)
{
    return eyeIndex > 0 ? 75 : -1;
}

int Character::DialogEmotion::GetEyeFrameCount()
{
    return this->eyeFilePathList.count();
}

int Character::DialogEmotion::GetMouthFrameCount()
{
    return this->mouthFilePathList.count();
}

void Character::DialogEmotion::SetBaseFilePath(QString filePath)
{
    this->baseFilePath = filePath;
}

void Character::DialogEmotion::AddEyeFrame(QString filePath)
{
    this->eyeFilePathList.push_back(filePath);
}

void Character::DialogEmotion::SetEyeFrame(int index, QString filePath)
{
    this->eyeFilePathList[index] = filePath;
}

void Character::DialogEmotion::SetEyeFrames(QStringList filePaths)
{
    this->eyeFilePathList = filePaths;
}

void Character::DialogEmotion::InsertEyeFrame(int index, QString filePath)
{
    this->eyeFilePathList.insert(index, filePath);
}

void Character::DialogEmotion::RemoveEyeFrame(int index)
{
    this->eyeFilePathList.removeAt(index);
}

void Character::DialogEmotion::AddMouthFrame(QString filePath)
{
    this->mouthFilePathList.push_back(filePath);
}

void Character::DialogEmotion::SetMouthFrame(int index, QString filePath)
{
    this->mouthFilePathList[index] = filePath;
}

void Character::DialogEmotion::SetMouthFrames(QStringList filePaths)
{
    this->mouthFilePathList = filePaths;
}

void Character::DialogEmotion::InsertMouthFrame(int index, QString filePath)
{
    this->mouthFilePathList.insert(index, filePath);
}

void Character::DialogEmotion::RemoveMouthFrame(int index)
{
    this->mouthFilePathList.removeAt(index);
}

Character::DialogEmotion::DrawingView::DrawingView(Character::DialogEmotion *pEmotion, IDrawable *pParent)
    : IDrawable(pParent)
    , IDrawingView<Character::DialogEmotion>(pEmotion, pParent)
{
    pBodyBasePixmapItem = new QGraphicsPixmapItem();
    pEyeDrawable = new EyeDrawable(pEmotion, this);
    pMouthDrawable = new MouthDrawable(pEmotion, this);
}

Character::DialogEmotion::DrawingView::~DrawingView()
{
    delete pEyeDrawable;
    pEyeDrawable = NULL;

    delete pMouthDrawable;
    pMouthDrawable = NULL;
}

void Character::DialogEmotion::DrawingView::DrawCore(QGraphicsScene *pScene, QList<QGraphicsItem *> &addedItems)
{
    if (pObject->GetBaseFilePath().length() > 0)
    {
        pBodyBasePixmapItem->setPixmap(QPixmap(pObject->GetBaseFilePath()));

        pScene->addItem(pBodyBasePixmapItem);
        addedItems.append(pBodyBasePixmapItem);
    }
}

void Character::DialogEmotion::DrawingView::DrawChildren(QGraphicsScene *pScene)
{
    pEyeDrawable->Draw(pScene);
    pMouthDrawable->Draw(pScene);
}

void Character::DialogEmotion::DrawingView::UpdateChildren()
{
    pEyeDrawable->Update();
    pMouthDrawable->Update();
}

void Character::DialogEmotion::DrawingView::ResetChildren()
{
    pEyeDrawable->Reset();
    pMouthDrawable->Reset();
}

void Character::DialogEmotion::DrawingView::SetIsSpeaking(bool isSpeaking)
{
    pMouthDrawable->SetIsSpeaking(isSpeaking);
}

Character::DialogEmotion::DrawingView::EyeDrawable::EyeDrawable(Character::DialogEmotion *pEmotion, IDrawable *pParent)
    : IDrawable(pParent)
{
    this->pEmotion = pEmotion;
    currentEyeIndex = 0;
}

void Character::DialogEmotion::DrawingView::EyeDrawable::DrawCore(QGraphicsScene *pScene, QList<QGraphicsItem *> &addedItems)
{
    for (const QString &eyeFilePath : pEmotion->GetEyeFilePaths())
    {
        eyePixmapItems.append(new QGraphicsPixmapItem(QPixmap(eyeFilePath)));
    }

    for (QGraphicsPixmapItem *pEyePixmapItem : eyePixmapItems)
    {
        pScene->addItem(pEyePixmapItem);
        addedItems.append(pEyePixmapItem);
    }

    currentEyeIndex = 0;

    if (eyePixmapItems.size() > 0)
    {
        UpdateTimeout();
    }
}

void Character::DialogEmotion::DrawingView::EyeDrawable::UpdateCore()
{
    for (int i = 0; i < eyePixmapItems.count(); i++)
    {
        if (i != currentEyeIndex)
        {
            eyePixmapItems[i]->setOpacity(0.0);
        }
    }
}

void Character::DialogEmotion::DrawingView::EyeDrawable::UpdateOnTimeout()
{
    currentEyeIndex = (currentEyeIndex + 1) % pEmotion->GetEyeFrameCount();
    UpdateTimeout();
}

void Character::DialogEmotion::DrawingView::EyeDrawable::UpdateTimeout()
{
    msTimeoutDuration = pEmotion->GetEyeDuration(currentEyeIndex);

    // A value of -1 indicates that it should be randomized.
    if (msTimeoutDuration < 0)
    {
        msTimeoutDuration = (int)(qrand() % 2001) + 2000;
    }
}

Character::DialogEmotion::DrawingView::MouthDrawable::MouthDrawable(Character::DialogEmotion *pEmotion, IDrawable *pParent)
    : IDrawable(pParent)
{
    this->pEmotion = pEmotion;
    currentMouthIndex = -1;
    isSpeaking = false;
}

void Character::DialogEmotion::DrawingView::MouthDrawable::DrawCore(QGraphicsScene *pScene, QList<QGraphicsItem *> &addedItems)
{
    for (const QString &mouthFilePath : pEmotion->GetMouthFilePaths())
    {
        mouthPixmapItems.append(new QGraphicsPixmapItem(QPixmap(mouthFilePath)));
    }

    for (QGraphicsPixmapItem *pMouthPixmapItem : mouthPixmapItems)
    {
        pScene->addItem(pMouthPixmapItem);
        addedItems.append(pMouthPixmapItem);
    }

    currentMouthIndex = -1;

    if (mouthPixmapItems.size() > 0)
    {
        UpdateOnTimeout();
    }
}

void Character::DialogEmotion::DrawingView::MouthDrawable::UpdateCore()
{
    for (int i = 0; i < mouthPixmapItems.count(); i++)
    {
        if (i != currentMouthIndex)
        {
            mouthPixmapItems[i]->setOpacity(0.0);
        }
    }
}

void Character::DialogEmotion::DrawingView::MouthDrawable::UpdateOnTimeout()
{
    if (isSpeaking)
    {
        int newMouthIndex = currentMouthIndex;

        while (currentMouthIndex == newMouthIndex)
        {
            newMouthIndex = qrand() % pEmotion->GetMouthFrameCount();
        }

        currentMouthIndex = newMouthIndex;
        msTimeoutDuration = 100;
    }
    else
    {
        currentMouthIndex = 0;
    }
}

void Character::DialogEmotion::DrawingView::MouthDrawable::SetIsSpeaking(bool isSpeaking)
{
    if (this->isSpeaking != isSpeaking)
    {
        this->isSpeaking = isSpeaking;

        if (mouthPixmapItems.size() > 0)
        {
            if (this->isSpeaking)
            {
                msTimeoutDuration = 100;
            }
            else
            {
                msTimeoutDuration = 0;
            }

            UpdateOnTimeout();
            Update();
        }
    }
}

void Character::AddDialogEmotion(QString emotionId)
{
    // If we don't already have a dialog emotion for this ID,
    // we'll create one and add it to the map.
    if (!this->emotionIdList.contains(emotionId))
    {
        this->emotionIdList.push_back(emotionId);
    }
    else
    {
        throw new MLIException("The emotion ID list and the emotion map by ID should always be in sync.");
    }

    Character::DialogEmotion *pDialogEmotion = new Character::DialogEmotion(emotionId);
    this->emotionByIdMap.insert(emotionId, pDialogEmotion);
}

Character::DialogEmotion * Character::GetDialogEmotion(QString emotionId)
{
    if (this->emotionByIdMap.contains(emotionId))
    {
        return this->emotionByIdMap[emotionId];
    }
    else
    {
        AddDialogEmotion(emotionId);
        return this->emotionByIdMap[emotionId];
    }
}

void Character::SetDialogEmotion(QString emotionId, DialogEmotion *pDialogEmotion)
{
    // If we don't already have a dialog emotion for this ID,
    // we'll create one and add it to the map.
    if (!this->emotionIdList.contains(emotionId))
    {
        this->emotionIdList.push_back(emotionId);
    }

    pDialogEmotion->SetId(emotionId);
    this->emotionByIdMap[emotionId] = pDialogEmotion;
}

Animation * Character::GetStandingAnimationForDirection(FieldCharacterDirection direction)
{
    return this->standingAnimations[direction];
}

Animation * Character::GetWalkingAnimationForDirection(FieldCharacterDirection direction)
{
    return this->walkingAnimations[direction];
}

Animation * Character::GetRunningAnimationForDirection(FieldCharacterDirection direction)
{
    return this->runningAnimations[direction];
}

Character::FieldInstance::FieldInstance(Staging::FieldCharacter *pStagingFieldCharacter)
{
    pCharacter = CaseContent::GetInstance()->GetById<Character>(pStagingFieldCharacter->Id);

    position = pStagingFieldCharacter->Position;
    direction = (CharacterDirection)pStagingFieldCharacter->Direction;
    spriteDirection = (FieldCharacterDirection)pStagingFieldCharacter->SpriteDirection;
    clickEncounterId = pStagingFieldCharacter->ClickEncounterId;
    clickCutsceneId = pStagingFieldCharacter->ClickCutsceneId;
    interactionLocation = pStagingFieldCharacter->InteractionLocation;
    interactFromAnywhere = pStagingFieldCharacter->InteractFromAnywhere;
    pCondition = new Condition(pStagingFieldCharacter->pCondition);
}

Character::FieldInstance::~FieldInstance()
{
    delete pCondition;
    pCondition = NULL;
}

void Character::FieldInstance::WriteToCaseXml(XmlWriter *pWriter)
{
    pWriter->StartElement("FieldCharacter");
    pWriter->WriteTextElement("Id", pCharacter->id);
    pWriter->WriteTextElement("Name", pCharacter->name);

    pWriter->StartElement("CharacterStandingAnimationIds");

    for (int i = 0; i < FieldCharacterDirectionCount; i++)
    {
        FieldCharacterDirection direction = static_cast<FieldCharacterDirection>(i);

        if (pCharacter->standingAnimations[direction] != NULL)
        {
            QString directionString = FieldCharacterDirectionToString(direction);

            pWriter->StartElement("Entry");
            pWriter->WriteTextElement("Direction", directionString);
            pWriter->WriteTextElement("Id", QString(pCharacter->id) + QString("StandingAnimation") + directionString);
            pWriter->EndElement();
        }
    }

    pWriter->EndElement();

    pWriter->StartElement("CharacterWalkingAnimationIds");

    for (int i = 0; i < FieldCharacterDirectionCount; i++)
    {
        FieldCharacterDirection direction = static_cast<FieldCharacterDirection>(i);

        if (pCharacter->walkingAnimations[direction] != NULL)
        {
            QString directionString = FieldCharacterDirectionToString(direction);

            pWriter->StartElement("Entry");
            pWriter->WriteTextElement("Direction", directionString);
            pWriter->WriteTextElement("Id", QString(pCharacter->id) + QString("WalkingAnimation") + directionString);
            pWriter->EndElement();
        }
    }

    pWriter->EndElement();

    pWriter->StartElement("CharacterRunningAnimationIds");

    for (int i = 0; i < FieldCharacterDirectionCount; i++)
    {
        FieldCharacterDirection direction = static_cast<FieldCharacterDirection>(i);

        if (pCharacter->walkingAnimations[direction] != NULL)
        {
            QString directionString = FieldCharacterDirectionToString(direction);

            pWriter->StartElement("Entry");
            pWriter->WriteTextElement("Direction", directionString);
            pWriter->WriteTextElement("Id", QString(pCharacter->id) + QString("RunningAnimation") + directionString);
            pWriter->EndElement();
        }
    }

    pWriter->EndElement();

    pWriter->StartElement("Position");
    position.SaveToXml(pWriter);
    pWriter->EndElement();

    pWriter->StartElement("HitBox");
    // Placeholder
    pWriter->EndElement();

    pWriter->WriteTextElement("Direction", CharacterDirectionToString(direction));
    pWriter->WriteTextElement("SpriteDirection", FieldCharacterDirectionToString(spriteDirection));

    pWriter->StartElement("ClickRect");
    //clickRect.SaveToXml(pWriter); // Placeholder
    pWriter->EndElement();

    if (clickCutsceneId.length() > 0)
    {
        pWriter->WriteTextElement("ClickCutsceneId", clickCutsceneId);
    }

    if (clickEncounterId.length() > 0)
    {
        pWriter->WriteTextElement("ClickEncounterId", clickEncounterId);
    }

    pWriter->WriteBooleanElement("InteractFromAnywhere", interactFromAnywhere);

    pWriter->EndElement();
}

QString Character::FieldInstance::GetId()
{
    return pCharacter->GetId();
}

QString Character::FieldInstance::GetName()
{
    return pCharacter->GetName();
}

void Character::FieldInstance::SetName(QString name)
{
    pCharacter->SetName(name);
    emit PropertyChanged("Name");
}

QString Character::FieldInstance::GetDisplayName()
{
    return pCharacter->GetDisplayName();
}

void Character::FieldInstance::SetPosition(Vector2 position)
{
    this->position = position;
    emit PropertyChanged("Position");
}

Vector2 Character::FieldInstance::GetMidpoint() const
{
    return GetMidpointForPosition(position);
}

Vector2 Character::FieldInstance::GetMidpointForPosition(const Vector2 &position) const
{
    return
        Vector2(
            position.GetX() + pCharacter->GetClickRect().GetX() + pCharacter->GetClickRect().GetWidth() / 2,
            position.GetY() + pCharacter->GetClickRect().GetY() + pCharacter->GetClickRect().GetHeight() / 2); // TODO: Subtract the "extra height" once we have it.
}

void Character::FieldInstance::SaveElementsToXml(XmlWriter *pWriter)
{
    pWriter->WriteTextElement("CharacterId", pCharacter->GetId());
    XmlStorableObject::SaveElementsToXml(pWriter);
}

void Character::FieldInstance::LoadElementsFromXml(XmlReader *pReader)
{
    pCharacter = CaseContent::GetInstance()->GetById<Character>(pReader->ReadTextElement("CharacterId"));
    XmlStorableObject::LoadElementsFromXml(pReader);
}

ObjectManipulatorSlots *Character::FieldInstance::GetManipulationWidget()
{
    FieldCharacterManipulator *pManipulator = FieldCharacterManipulator::Create();
    pManipulator->Init(this);
    return pManipulator;
}

Character::FieldInstance::ManipulationView::ManipulationView(Character::FieldInstance *pCharacterFieldInstance, IDrawable *pParent, UndoCommandSink *pUndoCommandSink)
    : IDrawable(pParent)
    , IManipulationView<Character::FieldInstance>(pCharacterFieldInstance, pParent, pUndoCommandSink)
{
    pSpriteAnimationDrawingViews[FieldCharacterDirectionUp] = pObject->pCharacter->GetStandingAnimationForDirection(FieldCharacterDirectionUp)->GetDrawingView(this);
    pSpriteAnimationDrawingViews[FieldCharacterDirectionDiagonalUp] = pObject->pCharacter->GetStandingAnimationForDirection(FieldCharacterDirectionDiagonalUp)->GetDrawingView(this);
    pSpriteAnimationDrawingViews[FieldCharacterDirectionSide] = pObject->pCharacter->GetStandingAnimationForDirection(FieldCharacterDirectionSide)->GetDrawingView(this);
    pSpriteAnimationDrawingViews[FieldCharacterDirectionDiagonalDown] = pObject->pCharacter->GetStandingAnimationForDirection(FieldCharacterDirectionDiagonalDown)->GetDrawingView(this);
    pSpriteAnimationDrawingViews[FieldCharacterDirectionDown] = pObject->pCharacter->GetStandingAnimationForDirection(FieldCharacterDirectionDown)->GetDrawingView(this);

    positionIsOverridden = false;
    directionIsOverridden = false;
    spriteDirectionIsOverridden = false;

    pManipulationViewBox = new ManipulationViewBox(this);
    UpdateAnimationDrawingView();
}

Character::FieldInstance::ManipulationView::~ManipulationView()
{
    delete pSpriteAnimationDrawingViews[FieldCharacterDirectionUp];
    delete pSpriteAnimationDrawingViews[FieldCharacterDirectionDiagonalUp];
    delete pSpriteAnimationDrawingViews[FieldCharacterDirectionSide];
    delete pSpriteAnimationDrawingViews[FieldCharacterDirectionDiagonalDown];
    delete pSpriteAnimationDrawingViews[FieldCharacterDirectionDown];
    pSpriteAnimationDrawingViews[FieldCharacterDirectionUp] = NULL;
    pSpriteAnimationDrawingViews[FieldCharacterDirectionDiagonalUp] = NULL;
    pSpriteAnimationDrawingViews[FieldCharacterDirectionSide] = NULL;
    pSpriteAnimationDrawingViews[FieldCharacterDirectionDiagonalDown] = NULL;
    pSpriteAnimationDrawingViews[FieldCharacterDirectionDown] = NULL;

    delete pManipulationViewBox;
    pManipulationViewBox = NULL;
}

void Character::FieldInstance::ManipulationView::DrawCore(QGraphicsScene */*pScene*/, QList<QGraphicsItem *> &/*addedItems*/)
{
}

void Character::FieldInstance::ManipulationView::DrawChildren(QGraphicsScene *pScene)
{
    pSpriteAnimationDrawingViews[FieldCharacterDirectionUp]->Draw(pScene);
    pSpriteAnimationDrawingViews[FieldCharacterDirectionDiagonalUp]->Draw(pScene);
    pSpriteAnimationDrawingViews[FieldCharacterDirectionSide]->Draw(pScene);
    pSpriteAnimationDrawingViews[FieldCharacterDirectionDiagonalDown]->Draw(pScene);
    pSpriteAnimationDrawingViews[FieldCharacterDirectionDown]->Draw(pScene);

    pManipulationViewBox->SetBoxRect(GetBoundingRect());
    pManipulationViewBox->Draw(pScene);
}

void Character::FieldInstance::ManipulationView::UpdateChildren()
{
    pSpriteAnimationDrawingViews[FieldCharacterDirectionUp]->Update();
    pSpriteAnimationDrawingViews[FieldCharacterDirectionDiagonalUp]->Update();
    pSpriteAnimationDrawingViews[FieldCharacterDirectionSide]->Update();
    pSpriteAnimationDrawingViews[FieldCharacterDirectionDiagonalDown]->Update();
    pSpriteAnimationDrawingViews[FieldCharacterDirectionDown]->Update();

    pManipulationViewBox->Update();
}

void Character::FieldInstance::ManipulationView::ResetChildren()
{
    pSpriteAnimationDrawingViews[FieldCharacterDirectionUp]->Reset();
    pSpriteAnimationDrawingViews[FieldCharacterDirectionDiagonalUp]->Reset();
    pSpriteAnimationDrawingViews[FieldCharacterDirectionSide]->Reset();
    pSpriteAnimationDrawingViews[FieldCharacterDirectionDiagonalDown]->Reset();
    pSpriteAnimationDrawingViews[FieldCharacterDirectionDown]->Reset();

    pManipulationViewBox->Reset();
}

RectangleWH Character::FieldInstance::ManipulationView::GetBoundingRect()
{
    RectangleWH clickRect = pObject->pCharacter->GetClickRect();

    if (clickRect.GetWidth() > 0 && clickRect.GetHeight() > 0)
    {
        return clickRect;
    }
    else
    {
        return IDrawable::GetBoundingRect();
    }
}

qreal Character::FieldInstance::ManipulationView::GetZOrder()
{
    // Multiply by 100 in order to allow intraelement granularity -
    // e.g., enabling ForegroundElementAnimations to always be drawn on top of
    // their ForegroundElements without causing them to also be drawn on top of
    // other ForegroundElements.
    return 100 * (pObject->position.GetY() + pObject->pCharacter->GetAnchorPosition());
}

void Character::FieldInstance::ManipulationView::SetShouldDimIfDisabled(bool shouldDimIfDisabled)
{
    this->shouldDimIfDisabled = shouldDimIfDisabled;
    Update();
}

void Character::FieldInstance::ManipulationView::SetPositionOverride(Vector2 position)
{
    if (positionOverride != position)
    {
        positionIsOverridden = true;
        positionOverride = position;

        Update();

        emit PositionOverrideChanged(positionOverride);
    }
}

void Character::FieldInstance::ManipulationView::ClearPositionOverride()
{
    positionIsOverridden = false;

    Update();
}

void Character::FieldInstance::ManipulationView::SetDirectionOverride(CharacterDirection direction)
{
    directionIsOverridden = true;
    directionOverride = direction;

    Update();
}

void Character::FieldInstance::ManipulationView::ClearDirectionOverride()
{
    directionIsOverridden = false;

    Update();
}

void Character::FieldInstance::ManipulationView::SetSpriteDirectionOverride(FieldCharacterDirection direction)
{
    spriteDirectionIsOverridden = true;
    spriteDirectionOverride = direction;

    UpdateAnimationDrawingView();
    Redraw();
}

void Character::FieldInstance::ManipulationView::ClearSpriteDirectionOverride()
{
    spriteDirectionIsOverridden = false;

    UpdateAnimationDrawingView();
    Redraw();
}

void Character::FieldInstance::ManipulationView::OnGotFocus()
{
    SetIsSelected(true);
}

void Character::FieldInstance::ManipulationView::OnLostFocus()
{
    SetIsSelected(false);
}

void Character::FieldInstance::ManipulationView::OnMouseEnter()
{
    SetIsMouseOver(true);
}

void Character::FieldInstance::ManipulationView::OnMouseLeave()
{
    SetIsMouseOver(false);
}

void Character::FieldInstance::ManipulationView::OnMousePress()
{
    if (positionIsOverridden)
    {
        positionBeforeDrag = positionOverride;
    }
    else
    {
        positionBeforeDrag = this->pObject->position;
    }
}

void Character::FieldInstance::ManipulationView::OnMouseDrag(Vector2 delta)
{
    SetIsBeingDragged(true);

    if (positionIsOverridden)
    {
        SetPositionOverride(positionOverride + delta);
    }
    else
    {
        SetPosition(this->pObject->position + delta);
    }
}

void Character::FieldInstance::ManipulationView::OnMouseRelease()
{
    SetIsBeingDragged(false);

    if (positionIsOverridden && positionOverride != positionBeforeDrag)
    {
        AddUndoCommand(new MoveUndoCommand(this, positionBeforeDrag, positionOverride, true /* isPositionOverride */));
    }
    else if (!positionIsOverridden && this->pObject->position != positionBeforeDrag)
    {
        AddUndoCommand(new MoveUndoCommand(this, positionBeforeDrag, this->pObject->position));
    }
}

void Character::FieldInstance::ManipulationView::SetPosition(Vector2 position)
{
    this->pObject->SetPosition(position);
    Update();
}

void Character::FieldInstance::ManipulationView::SetIsMouseOver(bool isMouseOver)
{
    pManipulationViewBox->SetIsMouseOver(isMouseOver);
}

void Character::FieldInstance::ManipulationView::SetIsSelected(bool isSelected)
{
    pManipulationViewBox->SetIsSelected(isSelected);
}

void Character::FieldInstance::ManipulationView::SetIsBeingDragged(bool isBeingDragged)
{
    pManipulationViewBox->SetIsBeingDragged(isBeingDragged);
}

void Character::FieldInstance::ManipulationView::UpdateAnimationDrawingView()
{
    FieldCharacterDirection spriteDirection = spriteDirectionIsOverridden ? spriteDirectionOverride : pObject->GetSpriteDirection();

    pSpriteAnimationDrawingViews[FieldCharacterDirectionUp]->SetOpacity(spriteDirection == FieldCharacterDirectionUp ? 1.0 : 0.0);
    pSpriteAnimationDrawingViews[FieldCharacterDirectionDiagonalUp]->SetOpacity(spriteDirection == FieldCharacterDirectionDiagonalUp ? 1.0 : 0.0);
    pSpriteAnimationDrawingViews[FieldCharacterDirectionSide]->SetOpacity(spriteDirection == FieldCharacterDirectionSide ? 1.0 : 0.0);
    pSpriteAnimationDrawingViews[FieldCharacterDirectionDiagonalDown]->SetOpacity(spriteDirection == FieldCharacterDirectionDiagonalDown ? 1.0 : 0.0);
    pSpriteAnimationDrawingViews[FieldCharacterDirectionDown]->SetOpacity(spriteDirection == FieldCharacterDirectionDown ? 1.0 : 0.0);

    Update();
}

GET_MANIPULATABLE_OBJECT_FROM_DEFINITION(Character::FieldInstance)

Character::DialogDrawingView::DialogDrawingView(Character *pCharacter, IDrawable *pParent)
    : IDrawable(pParent)
    , IDrawingView<Character>(pCharacter, pParent)
{
    position = Vector2(0, 0);
    isFlipped = false;

    pEmotionDrawingView = NULL;
}

Character::DialogDrawingView::~DialogDrawingView()
{
    delete pEmotionDrawingView;
    pEmotionDrawingView = NULL;
}

void Character::DialogDrawingView::DrawChildren(QGraphicsScene *pScene)
{
    pEmotionDrawingView->Draw(pScene);
}

void Character::DialogDrawingView::UpdateChildren()
{
    pEmotionDrawingView->Update();
}

void Character::DialogDrawingView::ResetChildren()
{
    pEmotionDrawingView->Reset();
}

void Character::DialogDrawingView::SetEmotion(QString emotionId)
{
    if (currentEmotionId == emotionId)
    {
        return;
    }

    if (pEmotionDrawingView != NULL)
    {
        pEmotionDrawingView->Reset();

        delete pEmotionDrawingView;
        pEmotionDrawingView = NULL;
    }

    Character::DialogEmotion *pEmotion = pObject->GetDialogEmotion(emotionId);

    if (pEmotion != NULL)
    {
        pEmotionDrawingView = pEmotion->GetDrawingView(this);
        currentEmotionId = emotionId;
    }

    Redraw();
}

void Character::DialogDrawingView::SetIsSpeaking(bool isSpeaking)
{
    pEmotionDrawingView->SetIsSpeaking(isSpeaking);
}

CharacterSelector::CharacterSelector(bool allowNoCharacter, QWidget *parent)
    : QComboBox(parent)
{
    characterIds = CaseContent::GetInstance()->GetIds<Character>();
    this->allowNoCharacter = allowNoCharacter;

    if (allowNoCharacter)
    {
        addItem("(no character)");
    }

    for (QString characterId : characterIds)
    {
        QString displayName = CaseContent::GetInstance()->GetById<Character>(characterId)->GetDisplayName();
        characterDisplayNames.append(displayName);
        addItem(displayName);
    }

    QObject::connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(CurrentIndexChanged(int)));
}

QString CharacterSelector::GetSelectedCharacterId()
{
    if (currentIndex() == 0 && allowNoCharacter)
    {
        return "";
    }
    else
    {
        return characterIds[currentIndex() - (allowNoCharacter ? 1 : 0)];
    }
}

void CharacterSelector::SetToCharacterById(const QString &id)
{
    int indexOfCharacter = characterIds.indexOf(QRegExp(id, Qt::CaseInsensitive));

    if (indexOfCharacter >= 0)
    {
        setCurrentIndex(indexOfCharacter + (allowNoCharacter ? 1 : 0));
    }
    else if (allowNoCharacter)
    {
        setCurrentIndex(0);
    }
}

void CharacterSelector::CurrentIndexChanged(int /*currentIndex*/)
{
    emit CharacterSelected(GetSelectedCharacterId());
}

template <>
void CaseContent::Add<Character::DialogEmotion>(QString id, Character::DialogEmotion *pDialogEmotion, void *pParentObject)
{
    Character *pCharacter = reinterpret_cast<Character *>(pParentObject);

    if (pCharacter != NULL)
    {
        pCharacter->SetDialogEmotion(id, pDialogEmotion);
    }
}

template <>
Character::DialogEmotion * CaseContent::GetById<Character::DialogEmotion>(QString id, void *pParentObject)
{
    Character *pCharacter = reinterpret_cast<Character *>(pParentObject);

    if (pCharacter != NULL)
    {
        return pCharacter->GetDialogEmotion(id);
    }
    else
    {
        return NULL;
    }
}

template <>
QStringList CaseContent::GetIds<Character::DialogEmotion>(void *pParentObject)
{
    Character *pCharacter = reinterpret_cast<Character *>(pParentObject);

    if (pCharacter != NULL)
    {
        return pCharacter->GetEmotionIds();
    }
    else
    {
        return QStringList();
    }
}

template <>
QStringList CaseContent::GetDisplayNames<Character::DialogEmotion>(void *pParentObject)
{
    Character *pCharacter = reinterpret_cast<Character *>(pParentObject);

    if (pCharacter != NULL)
    {
        return pCharacter->GetEmotionIds();
    }
    else
    {
        return QStringList();
    }
}

template <>
void CaseContent::Add<Character::FieldSprite>(QString /*id*/, Character::FieldSprite * /*pDialogEmotion*/, void * /*pParentObject*/)
{
    throw new MLIException("There is always a set number of field sprites on a character - we should never be adding a new one.");
}

template <>
Character::FieldSprite * CaseContent::GetById<Character::FieldSprite>(QString id, void *pParentObject)
{
    Character *pCharacter = reinterpret_cast<Character *>(pParentObject);

    if (pCharacter != NULL)
    {
        if (id == pCharacter->GetStandingSprite()->GetId())
        {
            return pCharacter->GetStandingSprite();
        }
        else if (id == pCharacter->GetWalkingSprite()->GetId())
        {
            return pCharacter->GetWalkingSprite();
        }
        else if (id == pCharacter->GetRunningSprite()->GetId())
        {
            return pCharacter->GetRunningSprite();
        }
        else
        {
            throw new MLIException("Unknown field sprite ID.");
        }
    }
    else
    {
        return NULL;
    }
}

template <>
QStringList CaseContent::GetIds<Character::FieldSprite>(void *pParentObject)
{
    Character *pCharacter = reinterpret_cast<Character *>(pParentObject);

    if (pCharacter != NULL)
    {
        QStringList ids;

        ids.append(pCharacter->GetStandingSprite()->GetId());
        ids.append(pCharacter->GetWalkingSprite()->GetId());
        ids.append(pCharacter->GetRunningSprite()->GetId());

        return ids;
    }
    else
    {
        return QStringList();
    }
}

template <>
QStringList CaseContent::GetDisplayNames<Character::FieldSprite>(void *pParentObject)
{
    return GetIds<Character::FieldSprite>(pParentObject);
}
