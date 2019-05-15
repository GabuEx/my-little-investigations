#include "FieldCutscene.h"

#include "CaseContent.h"
#include "Encounter.h"
#include "Location.h"
#include "Staging/CaseContentLoadingStager.h"

#include "SharedUtils.h"

// TODO: Replace "TwilightSparkle" here with something that actually retrieves the designated player character.
QString ReplacePlaceholderCharacterWithId(const QString &characterId)
{
    return (characterId == "PlayerCharacter" ? "TwilightSparkle" : (characterId == "PartnerCharacter" ? "Rarity" : characterId));
}

CharacterDirection FieldCutscene::State::GetCharacterDirection(const QString &characterId) const
{
    return this->characterIdToCharacterDirectionMap[ReplacePlaceholderCharacterWithId(characterId)];
}

FieldCharacterDirection FieldCutscene::State::GetFieldCharacterDirection(const QString &characterId) const
{
    return this->characterIdToFieldCharacterDirectionMap[ReplacePlaceholderCharacterWithId(characterId)];
}

Vector2 FieldCutscene::State::GetCharacterPosition(const QString &characterId) const
{
    return this->characterIdToPositionMap[ReplacePlaceholderCharacterWithId(characterId)];
}

Vector2 FieldCutscene::State::GetCharacterCameraCenterPosition(const QString &characterId) const
{
    return GetCharacter(characterId)->GetMidpointForPosition(GetCharacterPosition(ReplacePlaceholderCharacterWithId(characterId)));
}

Character::FieldInstance * FieldCutscene::State::GetCharacter(const QString &characterId) const
{
    return this->characterIdToCharacterMap[ReplacePlaceholderCharacterWithId(characterId)];
}

void FieldCutscene::State::SetCharacterDirection(const QString &characterId, CharacterDirection direction)
{
    this->characterIdToCharacterDirectionMap[ReplacePlaceholderCharacterWithId(characterId)] = direction;
}

void FieldCutscene::State::SetFieldCharacterDirection(const QString &characterId, FieldCharacterDirection direction)
{
    this->characterIdToFieldCharacterDirectionMap[ReplacePlaceholderCharacterWithId(characterId)] = direction;
}

void FieldCutscene::State::SetCharacterPosition(const QString &characterId, Vector2 position)
{
    this->characterIdToPositionMap[ReplacePlaceholderCharacterWithId(characterId)] = position;
}

void FieldCutscene::State::SetActiveCharacterId(const QString &activeCharacterId)
{
    this->activeCharacterId = ReplacePlaceholderCharacterWithId(activeCharacterId);
}

FieldCutscene::FieldCutscenePhase * FieldCutscene::FieldCutscenePhase::CreateFromXml(XmlReader *pReader)
{
    if (pReader->ElementExists(FieldCutsceneConversation::GetNameForXml()))
    {
        return new FieldCutsceneConversation(pReader);
    }
    else if (pReader->ElementExists(FieldCutsceneMovement::GetNameForXml()))
    {
        return new FieldCutsceneMovement(pReader);
    }
    else if (pReader->ElementExists(FieldCutsceneOrient::GetNameForXml()))
    {
        return new FieldCutsceneOrient(pReader);
    }
    else if (pReader->ElementExists(FieldCutsceneSetBackground::GetNameForXml()))
    {
        return new FieldCutsceneSetBackground(pReader);
    }
    else if (pReader->ElementExists(FieldCutsceneCenterCamera::GetNameForXml()))
    {
        return new FieldCutsceneCenterCamera(pReader);
    }
    else if (pReader->ElementExists(FieldCutscenePause::GetNameForXml()))
    {
        return new FieldCutscenePause(pReader);
    }
    else if (pReader->ElementExists(FieldCutscenePlayBgm::GetNameForXml()))
    {
        return new FieldCutscenePlayBgm(pReader);
    }
    else if (pReader->ElementExists(FieldCutsceneStopBgm::GetNameForXml()))
    {
        return new FieldCutsceneStopBgm(pReader);
    }
    else if (pReader->ElementExists(FieldCutscenePlayAmbiance::GetNameForXml()))
    {
        return new FieldCutscenePlayAmbiance(pReader);
    }
    else if (pReader->ElementExists(FieldCutsceneStopAmbiance::GetNameForXml()))
    {
        return new FieldCutsceneStopAmbiance(pReader);
    }
    else if (pReader->ElementExists(FieldCutsceneEndCase::GetNameForXml()))
    {
        return new FieldCutsceneEndCase(pReader);
    }
    else
    {
        throw new MLIException("Unknown field cutscene phase type.");
    }
}

QList<FieldCutscene::FieldCutscenePhase *> FieldCutscene::FieldCutscenePhase::CreateFromStaging(Staging::FieldCutscene::FieldCutscenePhase *pStagingFieldCutscenePhase)
{
    QList<FieldCutscene::FieldCutscenePhase *> phaseList;

    switch (pStagingFieldCutscenePhase->GetType())
    {
    case Staging::FieldCutscenePhaseType_Conversation:
        phaseList.append(new FieldCutsceneConversation(static_cast<Staging::FieldCutscene::FieldCutsceneConversation *>(pStagingFieldCutscenePhase)));
        break;

    case Staging::FieldCutscenePhaseType_ConcurrentMovements:
        {
            Staging::FieldCutscene::FieldCutsceneConcurrentMovements *pConcurrentMovements = static_cast<Staging::FieldCutscene::FieldCutsceneConcurrentMovements *>(pStagingFieldCutscenePhase);

            for (Staging::FieldCutscene::FieldCutscenePhase *pPhase : pConcurrentMovements->MovementList)
            {
                phaseList.append(FieldCutscene::FieldCutscenePhase::CreateFromStaging(pPhase));
            }
        }
        break;

    case Staging::FieldCutscenePhaseType_Movement:
        phaseList.append(new FieldCutsceneMovement(static_cast<Staging::FieldCutscene::FieldCutsceneMovement *>(pStagingFieldCutscenePhase)));
        break;

    case Staging::FieldCutscenePhaseType_Orient:
        phaseList.append(new FieldCutsceneOrient(static_cast<Staging::FieldCutscene::FieldCutsceneOrient *>(pStagingFieldCutscenePhase)));
        break;

    case Staging::FieldCutscenePhaseType_SetBackground:
        phaseList.append(new FieldCutsceneSetBackground(static_cast<Staging::FieldCutscene::FieldCutsceneSetBackground *>(pStagingFieldCutscenePhase)));
        break;

    case Staging::FieldCutscenePhaseType_CenterCamera:
        phaseList.append(new FieldCutsceneCenterCamera(static_cast<Staging::FieldCutscene::FieldCutsceneCenterCamera *>(pStagingFieldCutscenePhase)));
        break;

    case Staging::FieldCutscenePhaseType_Pause:
        phaseList.append(new FieldCutscenePause(static_cast<Staging::FieldCutscene::FieldCutscenePause *>(pStagingFieldCutscenePhase)));
        break;

    case Staging::FieldCutscenePhaseType_PlayBgm:
        phaseList.append(new FieldCutscenePlayBgm(static_cast<Staging::FieldCutscene::FieldCutscenePlayBgm *>(pStagingFieldCutscenePhase)));
        break;

    case Staging::FieldCutscenePhaseType_StopBgm:
        phaseList.append(new FieldCutsceneStopBgm(static_cast<Staging::FieldCutscene::FieldCutsceneStopBgm *>(pStagingFieldCutscenePhase)));
        break;

    case Staging::FieldCutscenePhaseType_PlayAmbiance:
        phaseList.append(new FieldCutscenePlayAmbiance(static_cast<Staging::FieldCutscene::FieldCutscenePlayAmbiance *>(pStagingFieldCutscenePhase)));
        break;

    case Staging::FieldCutscenePhaseType_StopAmbiance:
        phaseList.append(new FieldCutsceneStopAmbiance(static_cast<Staging::FieldCutscene::FieldCutsceneStopAmbiance *>(pStagingFieldCutscenePhase)));
        break;

    case Staging::FieldCutscenePhaseType_EndCase:
        phaseList.append(new FieldCutsceneEndCase(static_cast<Staging::FieldCutscene::FieldCutsceneEndCase *>(pStagingFieldCutscenePhase)));
        break;

    default:
        throw new MLIException("Unknown field cutscene phase type.");
    }

    return phaseList;
}

FieldCutscene::FieldCutscenePhase * FieldCutscene::FieldCutscenePhase::Clone()
{
    FieldCutscene::FieldCutscenePhase *pClonedPhase = NULL;

    switch(GetType())
    {
    case FieldCutscene::PhaseType::Conversation:
        pClonedPhase = new FieldCutscene::FieldCutsceneConversation();
        break;
    case FieldCutscene::PhaseType::Movement:
        pClonedPhase = new FieldCutscene::FieldCutsceneMovement();
        break;
    case FieldCutscene::PhaseType::Orient:
        pClonedPhase = new FieldCutscene::FieldCutsceneOrient();
        break;
    case FieldCutscene::PhaseType::SetBackground:
        pClonedPhase = new FieldCutscene::FieldCutsceneSetBackground();
        break;
    case FieldCutscene::PhaseType::CenterCamera:
        pClonedPhase = new FieldCutscene::FieldCutsceneCenterCamera();
        break;
    case FieldCutscene::PhaseType::Pause:
        pClonedPhase = new FieldCutscene::FieldCutscenePause();
        break;
    case FieldCutscene::PhaseType::PlayBgm:
        pClonedPhase = new FieldCutscene::FieldCutscenePlayBgm();
        break;
    case FieldCutscene::PhaseType::StopBgm:
        pClonedPhase = new FieldCutscene::FieldCutsceneStopBgm();
        break;
    case FieldCutscene::PhaseType::PlayAmbiance:
        pClonedPhase = new FieldCutscene::FieldCutscenePlayAmbiance();
        break;
    case FieldCutscene::PhaseType::StopAmbiance:
        pClonedPhase = new FieldCutscene::FieldCutsceneStopAmbiance();
        break;
    case FieldCutscene::PhaseType::EndCase:
        pClonedPhase = new FieldCutscene::FieldCutsceneEndCase();
        break;
    default:
        throw new MLIException("Unknown phase type");
    }

    CopyProperties(pClonedPhase, true /* isForEdit */);
    return pClonedPhase;
}

void FieldCutscene::FieldCutscenePhase::UpdateAndCacheCutsceneState(FieldCutscene::State &currentState)
{
    currentState.SetActiveElement(FieldCutscene::State::ActiveElement::None);
    currentState.SetActiveCharacterId("");

    currentState.AdvanceToNextPhase();
    UpdateCutsceneState(currentState);
    stateDuringPhase = currentState.Clone();
}

FieldCutscene::FieldCutsceneConversation::FieldCutsceneConversation(Staging::FieldCutscene::FieldCutsceneConversation *pStagingFieldCutsceneConversation)
    : FieldCutscene::FieldCutsceneConversation()
{
    millisecondDelayBeforeBegin = pStagingFieldCutsceneConversation->MillisecondDelayBeforeBegin;
    pEncounter = shared_ptr<Encounter>(new Encounter(pStagingFieldCutsceneConversation->pEncounter));
}

FieldCutscene::FieldCutsceneConversation::~FieldCutsceneConversation()
{
}

void FieldCutscene::FieldCutsceneConversation::SaveElementsToXml(XmlWriter *pWriter)
{
    XmlStorableObject::SaveElementsToXml(pWriter);

    pWriter->StartElement("Encounter");
    pEncounter->SaveToXml(pWriter);
    pWriter->EndElement();
}

void FieldCutscene::FieldCutsceneConversation::LoadElementsFromXml(XmlReader *pReader)
{
    XmlStorableObject::LoadElementsFromXml(pReader);

    pReader->StartElement("Encounter");
    pEncounter = shared_ptr<Encounter>(Encounter::CreateFromXml(pReader));
    pReader->EndElement();
}

EditorDialogContents<FieldCutscene::FieldCutscenePhase, FieldCutscene::State> * FieldCutscene::FieldCutsceneConversation::CreateEditorDialogContents(
        const FieldCutscene::State &stateBeforeObject,
        FieldCutscene::FieldCutscenePhase *pPhaseToEdit)
{
    return new FieldCutsceneConversationEditorDialogContents(stateBeforeObject, dynamic_cast<FieldCutscene::FieldCutsceneConversation *>(pPhaseToEdit));
}

void FieldCutscene::FieldCutsceneConversation::CopyPropertiesImpl(FieldCutscene::FieldCutscenePhase *pOther, bool /*isForEdit*/)
{
    FieldCutscene::FieldCutsceneConversation *pPhase = dynamic_cast<FieldCutscene::FieldCutsceneConversation *>(pOther);

    pPhase->millisecondDelayBeforeBegin = millisecondDelayBeforeBegin;
    pPhase->pEncounter = pEncounter;
}

QList<ListItemBase<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType> *> FieldCutscene::FieldCutsceneConversation::GetListItemsImpl(int indentLevel)
{
    QList<ListItemBase<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType> *> editorList;
    editorList.append(new FieldCutsceneConversationListItem(this, indentLevel));
    return editorList;
}

QString FieldCutsceneConversationListItem::GetDisplayString()
{
    QString displayString = "Display a conversation. (Double-click to edit it.)";
    return displayString;
}

FieldCutsceneConversationEditorDialogContents::FieldCutsceneConversationEditorDialogContents(
        const FieldCutscene::State &stateBeforeObject,
        FieldCutscene::FieldCutsceneConversation *pPhaseToEdit)
    : EditorDialogContents<FieldCutscene::FieldCutscenePhase, FieldCutscene::State>(stateBeforeObject, pPhaseToEdit)
{
    if (pPhaseToEdit != NULL)
    {
        pFieldCutsceneConversation = static_cast<FieldCutscene::FieldCutsceneConversation *>(pPhaseToEdit->Clone());
    }
    else
    {
        pFieldCutsceneConversation = new FieldCutscene::FieldCutsceneConversation();
    }

    pObject = pFieldCutsceneConversation;

    QHBoxLayout *pLayout = new QHBoxLayout();

    pLayout->addWidget(new QLabel("Editing! Yay!"));

    pLayout->addStretch(1);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(pLayout);
}

bool FieldCutsceneConversationEditorDialogContents::ValidateFields()
{
    return true;
}

void FieldCutscene::FieldCutsceneMovement::DirectionArrow::SetPositions(const Vector2 &startPosition, const Vector2 &endPosition)
{
    this->startPosition = startPosition;
    this->endPosition = endPosition;

    Update();
}

void FieldCutscene::FieldCutsceneMovement::DirectionArrow::DrawCore(QGraphicsScene *pScene, QList<QGraphicsItem *> &addedItems)
{
    QBrush blackBrush(qRgba(0, 0, 0, 255));
    QBrush whiteBrush(qRgba(255, 255, 255, 255));
    QPen blackPen(blackBrush, 4);
    QPen whitePen(whiteBrush, 2);

    pOriginOuterEllipse = new QGraphicsEllipseItem();
    pOriginOuterEllipse->setBrush(blackBrush);
    pPathOuterLine = new QGraphicsLineItem();
    pPathOuterLine->setPen(blackPen);
    pArrowTopOuterLine = new QGraphicsLineItem();
    pArrowTopOuterLine->setPen(blackPen);
    pArrowBottomOuterLine = new QGraphicsLineItem();
    pArrowBottomOuterLine->setPen(blackPen);

    pOuterArrowGroup = new QGraphicsItemGroup();
    pOuterArrowGroup->addToGroup(pOriginOuterEllipse);
    pOuterArrowGroup->addToGroup(pPathOuterLine);
    pOuterArrowGroup->addToGroup(pArrowTopOuterLine);
    pOuterArrowGroup->addToGroup(pArrowBottomOuterLine);

    pOriginInnerEllipse = new QGraphicsEllipseItem();
    pOriginInnerEllipse->setBrush(whiteBrush);
    pPathInnerLine = new QGraphicsLineItem();
    pPathInnerLine->setPen(whitePen);
    pArrowTopInnerLine = new QGraphicsLineItem();
    pArrowTopInnerLine->setPen(whitePen);
    pArrowBottomInnerLine = new QGraphicsLineItem();
    pArrowBottomInnerLine->setPen(whitePen);

    pInnerArrowGroup = new QGraphicsItemGroup();
    pInnerArrowGroup->addToGroup(pOriginInnerEllipse);
    pInnerArrowGroup->addToGroup(pPathInnerLine);
    pInnerArrowGroup->addToGroup(pArrowTopInnerLine);
    pInnerArrowGroup->addToGroup(pArrowBottomInnerLine);

    pArrowGroup = new QGraphicsItemGroup();
    pArrowGroup->addToGroup(pOuterArrowGroup);
    pArrowGroup->addToGroup(pInnerArrowGroup);

    pScene->addItem(pArrowGroup);
    addedItems.push_back(pArrowGroup);
}

void FieldCutscene::FieldCutsceneMovement::DirectionArrow::UpdateCore()
{
    QRect innerEllipseRect((startPosition - Vector2(4, 4)).ToQPoint(), (startPosition + Vector2(4, 4)).ToQPoint());
    QRect outerEllipseRect(innerEllipseRect.left() - 1, innerEllipseRect.top() - 1, innerEllipseRect.width() + 1, innerEllipseRect.height() + 1);
    QLine pathLine(startPosition.ToQPoint(), endPosition.ToQPoint());

    pOriginOuterEllipse->setRect(outerEllipseRect);
    pOriginInnerEllipse->setRect(innerEllipseRect);

    pPathOuterLine->setLine(pathLine);
    pPathInnerLine->setLine(pathLine);

    pOuterArrowGroup->setZValue(0);
    pInnerArrowGroup->setZValue(1);
    pArrowGroup->setZValue(99 * 1000000);
}

FieldCutscene::FieldCutsceneMovement::FieldCutsceneMovement(Staging::FieldCutscene::FieldCutsceneMovement *pStagingFieldCutsceneMovement)
{
    millisecondDelayBeforeBegin = pStagingFieldCutsceneMovement->MillisecondDelayBeforeBegin;
    characterId = pStagingFieldCutsceneMovement->CharacterId;
    targetPosition = pStagingFieldCutsceneMovement->TargetPosition;
    movementState = static_cast<FieldCharacterState>(pStagingFieldCutsceneMovement->MovementState);

    pGhostedCharacterDrawingView = NULL;
    pDirectionArrow = NULL;
}

FieldCutscene::FieldCutsceneMovement::~FieldCutsceneMovement()
{
    delete pGhostedCharacterDrawingView;
    pGhostedCharacterDrawingView = NULL;

    delete pDirectionArrow;
    pDirectionArrow = NULL;
}

void FieldCutscene::FieldCutsceneMovement::UpdateCharacterPosition(Vector2 newCharacterPosition)
{
    this->targetPosition = newCharacterPosition;

    /*pDirectionArrow->SetPositions(
        stateDuringPhase.GetPreviousState().GetCharacterCameraCenterPosition(characterId),
        stateDuringPhase.GetPreviousState().GetCharacter(characterId)->GetMidpointForPosition(targetPosition));*/
}

void FieldCutscene::FieldCutsceneMovement::InitializeForDrawable(IDrawable *pParentDrawable)
{
    delete pGhostedCharacterDrawingView;

    Character *pCharacter = CaseContent::GetInstance()->GetById<Character>(ReplacePlaceholderCharacterWithId(characterId));

    pGhostedCharacterDrawingView = pCharacter->GetStandingAnimationForDirection(stateDuringPhase.GetPreviousState().GetFieldCharacterDirection(characterId))->GetDrawingView(pParentDrawable);
    pGhostedCharacterDrawingView->SetPosition(stateDuringPhase.GetPreviousState().GetCharacterPosition(characterId));
    pGhostedCharacterDrawingView->SetFlipHorizontal(stateDuringPhase.GetPreviousState().GetCharacterDirection(characterId) == CharacterDirectionRight);
    pGhostedCharacterDrawingView->SetOpacity(0.333);

    stateDuringPhase.AddAdditionalDrawable(pGhostedCharacterDrawingView);

    /*pDirectionArrow = new DirectionArrow(
                pParentDrawable,
                stateDuringPhase.GetPreviousState().GetCharacterCameraCenterPosition(characterId),
                stateDuringPhase.GetPreviousState().GetCharacter(characterId)->GetMidpointForPosition(targetPosition));

    stateDuringPhase.AddAdditionalDrawable(pDirectionArrow);*/
}

EditorDialogContents<FieldCutscene::FieldCutscenePhase, FieldCutscene::State> * FieldCutscene::FieldCutsceneMovement::CreateEditorDialogContents(
        const FieldCutscene::State &stateBeforeObject,
        FieldCutscene::FieldCutscenePhase *pPhaseToEdit)
{
    return new FieldCutsceneMovementEditorDialogContents(stateBeforeObject, dynamic_cast<FieldCutscene::FieldCutsceneMovement *>(pPhaseToEdit));
}

void FieldCutscene::FieldCutsceneMovement::UpdateCutsceneState(FieldCutscene::State &currentState)
{
    Vector2 originPosition = currentState.GetPreviousState().GetCharacterPosition(characterId);

    if (targetPosition != originPosition)
    {
        CharacterDirection direction = CharacterDirectionNone;
        FieldCharacterDirection spriteDirection = FieldCharacterDirectionNone;

        GetCharacterDirectionFromDirectionVector(targetPosition - originPosition, &direction, &spriteDirection);

        currentState.SetCharacterDirection(characterId, direction);
        currentState.SetFieldCharacterDirection(characterId, spriteDirection);
        currentState.SetCharacterPosition(characterId, targetPosition);
    }

    stateDuringPhase.AddAdditionalDrawable(pGhostedCharacterDrawingView);

    currentState.SetActiveElement(FieldCutscene::State::ActiveElement::Character);
    currentState.SetActiveCharacterId(ReplacePlaceholderCharacterWithId(characterId));
}

void FieldCutscene::FieldCutsceneMovement::CopyPropertiesImpl(FieldCutscene::FieldCutscenePhase *pOther, bool /*isForEdit*/)
{
    FieldCutscene::FieldCutsceneMovement *pPhase = dynamic_cast<FieldCutscene::FieldCutsceneMovement *>(pOther);

    pPhase->millisecondDelayBeforeBegin = millisecondDelayBeforeBegin;
    pPhase->characterId = characterId;
    pPhase->targetPosition = targetPosition;
    pPhase->movementState = movementState;
}

QList<ListItemBase<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType> *> FieldCutscene::FieldCutsceneMovement::GetListItemsImpl(int indentLevel)
{
    QList<ListItemBase<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType> *> editorList;
    editorList.append(new FieldCutsceneMovementListItem(this, indentLevel));
    return editorList;
}

QString FieldCutsceneMovementListItem::GetDisplayString()
{
    QString characterDisplayName = CaseContent::GetInstance()->GetById<Character>(ReplacePlaceholderCharacterWithId(pPhase->characterId))->GetDisplayName();
    Vector2 startPosition = pPhase->stateDuringPhase.GetPreviousState().GetCharacterPosition(ReplacePlaceholderCharacterWithId(pPhase->characterId));

    QString displayString =
            "Move " + UnderlineString(characterDisplayName) +
            " from " + UnderlineString(VectorToString(startPosition)) +
            " to " + UnderlineString(VectorToString(pPhase->targetPosition)) + ".";
    return displayString;
}

FieldCutsceneMovementEditorDialogContents::FieldCutsceneMovementEditorDialogContents(
        const FieldCutscene::State &stateBeforeObject,
        FieldCutscene::FieldCutsceneMovement *pPhaseToEdit)
    : EditorDialogContents<FieldCutscene::FieldCutscenePhase, FieldCutscene::State>(stateBeforeObject, pPhaseToEdit)
{
    if (pPhaseToEdit != NULL)
    {
        pFieldCutsceneMovement = static_cast<FieldCutscene::FieldCutsceneMovement *>(pPhaseToEdit->Clone());
    }
    else
    {
        pFieldCutsceneMovement = new FieldCutscene::FieldCutsceneMovement();
    }

    pObject = pFieldCutsceneMovement;

    QHBoxLayout *pLayout = new QHBoxLayout();

    pLayout->addWidget(new QLabel("Editing! Yay!"));

    pLayout->addStretch(1);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(pLayout);
}

bool FieldCutsceneMovementEditorDialogContents::ValidateFields()
{
    return true;
}

FieldCutscene::FieldCutsceneOrient::FieldCutsceneOrient(Staging::FieldCutscene::FieldCutsceneOrient *pStagingFieldCutsceneOrient)
{
    millisecondDelayBeforeBegin = pStagingFieldCutsceneOrient->MillisecondDelayBeforeBegin;
    characterId = pStagingFieldCutsceneOrient->CharacterId;
    direction = static_cast<CharacterDirection>(pStagingFieldCutsceneOrient->Direction);
    spriteDirection = static_cast<FieldCharacterDirection>(pStagingFieldCutsceneOrient->SpriteDirection);
}

EditorDialogContents<FieldCutscene::FieldCutscenePhase, FieldCutscene::State> * FieldCutscene::FieldCutsceneOrient::CreateEditorDialogContents(
        const FieldCutscene::State &stateBeforeObject,
        FieldCutscene::FieldCutscenePhase *pPhaseToEdit)
{
    return new FieldCutsceneOrientEditorDialogContents(stateBeforeObject, dynamic_cast<FieldCutscene::FieldCutsceneOrient *>(pPhaseToEdit));
}

void FieldCutscene::FieldCutsceneOrient::CopyPropertiesImpl(FieldCutscene::FieldCutscenePhase *pOther, bool /*isForEdit*/)
{
    FieldCutscene::FieldCutsceneOrient *pPhase = dynamic_cast<FieldCutscene::FieldCutsceneOrient *>(pOther);

    pPhase->millisecondDelayBeforeBegin = millisecondDelayBeforeBegin;
    pPhase->characterId = characterId;
    pPhase->direction = direction;
    pPhase->spriteDirection = spriteDirection;
}

void FieldCutscene::FieldCutsceneOrient::UpdateCutsceneState(FieldCutscene::State &currentState)
{
    currentState.SetCharacterDirection(characterId, direction);
    currentState.SetFieldCharacterDirection(characterId, spriteDirection);
}

QList<ListItemBase<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType> *> FieldCutscene::FieldCutsceneOrient::GetListItemsImpl(int indentLevel)
{
    QList<ListItemBase<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType> *> editorList;
    editorList.append(new FieldCutsceneOrientListItem(this, indentLevel));
    return editorList;
}

QString FieldCutsceneOrientListItem::GetDisplayString()
{
    QString characterDisplayName = CaseContent::GetInstance()->GetById<Character>(ReplacePlaceholderCharacterWithId(pPhase->characterId))->GetDisplayName();
    QString directionString;

    switch (pPhase->spriteDirection)
    {
    case FieldCharacterDirectionUp:
        directionString = "up";
        break;
    case FieldCharacterDirectionDiagonalUp:
        if (pPhase->direction == CharacterDirectionLeft)
        {
            directionString = "up and to the left";
        }
        else
        {
            directionString = "up and to the right";
        }
        break;
    case FieldCharacterDirectionSide:
        if (pPhase->direction == CharacterDirectionLeft)
        {
            directionString = "left";
        }
        else
        {
            directionString = "right";
        }
        break;
    case FieldCharacterDirectionDiagonalDown:
        if (pPhase->direction == CharacterDirectionLeft)
        {
            directionString = "down and to the left";
        }
        else
        {
            directionString = "down and to the right";
        }
        break;
    case FieldCharacterDirectionDown:
        directionString = "down";
        break;
    default:
        throw new MLIException("Unknown field character direction.");
    }

    QString displayString = "Orient " + UnderlineString(characterDisplayName) + " to face " + UnderlineString(directionString) + ".";
    return displayString;
}

FieldCutsceneOrientEditorDialogContents::FieldCutsceneOrientEditorDialogContents(
        const FieldCutscene::State &stateBeforeObject,
        FieldCutscene::FieldCutsceneOrient *pPhaseToEdit)
    : EditorDialogContents<FieldCutscene::FieldCutscenePhase, FieldCutscene::State>(stateBeforeObject, pPhaseToEdit)
{
    if (pPhaseToEdit != NULL)
    {
        pFieldCutsceneOrient = static_cast<FieldCutscene::FieldCutsceneOrient *>(pPhaseToEdit->Clone());
    }
    else
    {
        pFieldCutsceneOrient = new FieldCutscene::FieldCutsceneOrient();
    }

    pObject = pFieldCutsceneOrient;

    QHBoxLayout *pLayout = new QHBoxLayout();

    pLayout->addWidget(new QLabel("Editing! Yay!"));

    pLayout->addStretch(1);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(pLayout);
}

bool FieldCutsceneOrientEditorDialogContents::ValidateFields()
{
    return true;
}

FieldCutscene::FieldCutsceneSetBackground::FieldCutsceneSetBackground(Staging::FieldCutscene::FieldCutsceneSetBackground *pStagingFieldCutsceneSetBackground)
{
    backgroundSpriteFilePath = CaseContentLoadingStager::GetCurrent()->GetSpriteFilePathFromId(pStagingFieldCutsceneSetBackground->BackgroundSpriteId);
    msFadeDuration = pStagingFieldCutsceneSetBackground->MsFadeDuration;
}

EditorDialogContents<FieldCutscene::FieldCutscenePhase, FieldCutscene::State> * FieldCutscene::FieldCutsceneSetBackground::CreateEditorDialogContents(
        const FieldCutscene::State &stateBeforeObject,
        FieldCutscene::FieldCutscenePhase *pPhaseToEdit)
{
    return new FieldCutsceneSetBackgroundEditorDialogContents(stateBeforeObject, dynamic_cast<FieldCutscene::FieldCutsceneSetBackground *>(pPhaseToEdit));
}

void FieldCutscene::FieldCutsceneSetBackground::CopyPropertiesImpl(FieldCutscene::FieldCutscenePhase *pOther, bool /*isForEdit*/)
{
    FieldCutscene::FieldCutsceneSetBackground *pPhase = dynamic_cast<FieldCutscene::FieldCutsceneSetBackground *>(pOther);

    pPhase->backgroundSpriteFilePath = backgroundSpriteFilePath;
    pPhase->msFadeDuration = msFadeDuration;
}

void FieldCutscene::FieldCutsceneSetBackground::UpdateCutsceneState(FieldCutscene::State &currentState)
{
    currentState.SetBackgroundSpritePath(backgroundSpriteFilePath);
}

QList<ListItemBase<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType> *> FieldCutscene::FieldCutsceneSetBackground::GetListItemsImpl(int indentLevel)
{
    QList<ListItemBase<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType> *> editorList;
    editorList.append(new FieldCutsceneSetBackgroundListItem(this, indentLevel));
    return editorList;
}

QString FieldCutsceneSetBackgroundListItem::GetDisplayString()
{
    QString displayString = "Set the background to " + CaseContent::GetInstance()->AbsolutePathToRelativePath(pPhase->backgroundSpriteFilePath);

    if (pPhase->msFadeDuration > 0)
    {
        displayString += " with a " + UnderlineString(QString() + QString(to_string(pPhase->msFadeDuration).c_str()) + " millisecond") + " fade-in time";
    }

    displayString += ".";
    return displayString;
}

FieldCutsceneSetBackgroundEditorDialogContents::FieldCutsceneSetBackgroundEditorDialogContents(
        const FieldCutscene::State &stateBeforeObject,
        FieldCutscene::FieldCutsceneSetBackground *pPhaseToEdit)
    : EditorDialogContents<FieldCutscene::FieldCutscenePhase, FieldCutscene::State>(stateBeforeObject, pPhaseToEdit)
{
    if (pPhaseToEdit != NULL)
    {
        pFieldCutsceneSetBackground = static_cast<FieldCutscene::FieldCutsceneSetBackground *>(pPhaseToEdit->Clone());
    }
    else
    {
        pFieldCutsceneSetBackground = new FieldCutscene::FieldCutsceneSetBackground();
    }

    pObject = pFieldCutsceneSetBackground;

    QHBoxLayout *pLayout = new QHBoxLayout();

    pLayout->addWidget(new QLabel("Editing! Yay!"));

    pLayout->addStretch(1);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(pLayout);
}

bool FieldCutsceneSetBackgroundEditorDialogContents::ValidateFields()
{
    return true;
}

FieldCutscene::FieldCutsceneCenterCamera::FieldCutsceneCenterCamera(Staging::FieldCutscene::FieldCutsceneCenterCamera *pStagingFieldCutsceneCenterCamera)
{
    millisecondDelayBeforeBegin = pStagingFieldCutsceneCenterCamera->MillisecondDelayBeforeBegin;
    characterId = pStagingFieldCutsceneCenterCamera->CharacterId;
    cameraCenterPosition = pStagingFieldCutsceneCenterCamera->CameraCenterPosition;
}

void FieldCutscene::FieldCutsceneCenterCamera::UpdateCameraPosition(Vector2 newCameraPosition)
{
    cameraCenterPosition = newCameraPosition;
}

EditorDialogContents<FieldCutscene::FieldCutscenePhase, FieldCutscene::State> * FieldCutscene::FieldCutsceneCenterCamera::CreateEditorDialogContents(
        const FieldCutscene::State &stateBeforeObject,
        FieldCutscene::FieldCutscenePhase *pPhaseToEdit)
{
    return new FieldCutsceneCenterCameraEditorDialogContents(stateBeforeObject, dynamic_cast<FieldCutscene::FieldCutsceneCenterCamera *>(pPhaseToEdit));
}

void FieldCutscene::FieldCutsceneCenterCamera::CopyPropertiesImpl(FieldCutscene::FieldCutscenePhase *pOther, bool /*isForEdit*/)
{
    FieldCutscene::FieldCutsceneCenterCamera *pPhase = dynamic_cast<FieldCutscene::FieldCutsceneCenterCamera *>(pOther);

    pPhase->millisecondDelayBeforeBegin = millisecondDelayBeforeBegin;
    pPhase->characterId = characterId;
    pPhase->cameraCenterPosition = cameraCenterPosition;
}

void FieldCutscene::FieldCutsceneCenterCamera::UpdateCutsceneState(FieldCutscene::State &currentState)
{
    if (characterId.length() > 0)
    {
        currentState.SetCameraPosition(currentState.GetCharacterCameraCenterPosition(ReplacePlaceholderCharacterWithId(characterId)));
    }
    else
    {
        currentState.SetCameraPosition(cameraCenterPosition);
    }

    currentState.SetActiveElement(FieldCutscene::State::ActiveElement::Camera);
}

QList<ListItemBase<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType> *> FieldCutscene::FieldCutsceneCenterCamera::GetListItemsImpl(int indentLevel)
{
    QList<ListItemBase<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType> *> editorList;
    editorList.append(new FieldCutsceneCenterCameraListItem(this, indentLevel));
    return editorList;
}

QString FieldCutsceneCenterCameraListItem::GetDisplayString()
{
    QString characterDisplayName;

    if (pPhase->characterId.length() > 0)
    {
        characterDisplayName = CaseContent::GetInstance()->GetById<Character>(ReplacePlaceholderCharacterWithId(pPhase->characterId))->GetDisplayName();
    }

    QString displayString = "Center the camera ";

    if (characterDisplayName.length() > 0)
    {
        displayString += UnderlineString("on " + characterDisplayName);
    }
    else
    {
        displayString += UnderlineString("at " + VectorToString(pPhase->cameraCenterPosition));
    }

    displayString += ".";
    return displayString;
}

FieldCutsceneCenterCameraEditorDialogContents::FieldCutsceneCenterCameraEditorDialogContents(
        const FieldCutscene::State &stateBeforeObject,
        FieldCutscene::FieldCutsceneCenterCamera *pPhaseToEdit)
    : EditorDialogContents<FieldCutscene::FieldCutscenePhase, FieldCutscene::State>(stateBeforeObject, pPhaseToEdit)
{
    if (pPhaseToEdit != NULL)
    {
        pFieldCutsceneCenterCamera = static_cast<FieldCutscene::FieldCutsceneCenterCamera *>(pPhaseToEdit->Clone());
    }
    else
    {
        pFieldCutsceneCenterCamera = new FieldCutscene::FieldCutsceneCenterCamera();
    }

    pObject = pFieldCutsceneCenterCamera;

    QHBoxLayout *pLayout = new QHBoxLayout();

    pLayout->addWidget(new QLabel("Editing! Yay!"));

    pLayout->addStretch(1);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(pLayout);
}

bool FieldCutsceneCenterCameraEditorDialogContents::ValidateFields()
{
    return true;
}

FieldCutscene::FieldCutscenePause::FieldCutscenePause(Staging::FieldCutscene::FieldCutscenePause *pStagingFieldCutscenePause)
{
    msPauseDuration = pStagingFieldCutscenePause->MsPauseDuration;
}

EditorDialogContents<FieldCutscene::FieldCutscenePhase, FieldCutscene::State> * FieldCutscene::FieldCutscenePause::CreateEditorDialogContents(
        const FieldCutscene::State &stateBeforeObject,
        FieldCutscene::FieldCutscenePhase *pPhaseToEdit)
{
    return new FieldCutscenePauseEditorDialogContents(stateBeforeObject, dynamic_cast<FieldCutscene::FieldCutscenePause *>(pPhaseToEdit));
}

void FieldCutscene::FieldCutscenePause::CopyPropertiesImpl(FieldCutscene::FieldCutscenePhase *pOther, bool /*isForEdit*/)
{
    FieldCutscene::FieldCutscenePause *pPhase = dynamic_cast<FieldCutscene::FieldCutscenePause *>(pOther);

    pPhase->msPauseDuration = msPauseDuration;
}

QList<ListItemBase<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType> *> FieldCutscene::FieldCutscenePause::GetListItemsImpl(int indentLevel)
{
    QList<ListItemBase<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType> *> editorList;
    editorList.append(new FieldCutscenePauseListItem(this, indentLevel));
    return editorList;
}

QString FieldCutscenePauseListItem::GetDisplayString()
{
    QString displayString = "Pause for " + UnderlineString(QString(to_string(pPhase->msPauseDuration).c_str()) + " milliseconds") + ".";
    return displayString;
}

FieldCutscenePauseEditorDialogContents::FieldCutscenePauseEditorDialogContents(
        const FieldCutscene::State &stateBeforeObject,
        FieldCutscene::FieldCutscenePause *pPhaseToEdit)
    : EditorDialogContents<FieldCutscene::FieldCutscenePhase, FieldCutscene::State>(stateBeforeObject, pPhaseToEdit)
{
    if (pPhaseToEdit != NULL)
    {
        pFieldCutscenePause = static_cast<FieldCutscene::FieldCutscenePause *>(pPhaseToEdit->Clone());
    }
    else
    {
        pFieldCutscenePause = new FieldCutscene::FieldCutscenePause();
    }

    pObject = pFieldCutscenePause;

    QHBoxLayout *pLayout = new QHBoxLayout();

    pLayout->addWidget(new QLabel("Editing! Yay!"));

    pLayout->addStretch(1);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(pLayout);
}

bool FieldCutscenePauseEditorDialogContents::ValidateFields()
{
    return true;
}

FieldCutscene::FieldCutscenePlayBgm::FieldCutscenePlayBgm(Staging::FieldCutscene::FieldCutscenePlayBgm *pFieldCutscenePlayBgm)
{
    bgmId = pFieldCutscenePlayBgm->BgmId;
}

EditorDialogContents<FieldCutscene::FieldCutscenePhase, FieldCutscene::State> * FieldCutscene::FieldCutscenePlayBgm::CreateEditorDialogContents(
        const FieldCutscene::State &stateBeforeObject,
        FieldCutscene::FieldCutscenePhase *pPhaseToEdit)
{
    return new FieldCutscenePlayBgmEditorDialogContents(stateBeforeObject, dynamic_cast<FieldCutscene::FieldCutscenePlayBgm *>(pPhaseToEdit));
}

void FieldCutscene::FieldCutscenePlayBgm::CopyPropertiesImpl(FieldCutscene::FieldCutscenePhase *pOther, bool /*isForEdit*/)
{
    FieldCutscene::FieldCutscenePlayBgm *pPhase = dynamic_cast<FieldCutscene::FieldCutscenePlayBgm *>(pOther);

    pPhase->bgmId = bgmId;
}

void FieldCutscene::FieldCutscenePlayBgm::UpdateCutsceneState(FieldCutscene::State &currentState)
{
    currentState.SetBgmId(bgmId);
}

QList<ListItemBase<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType> *> FieldCutscene::FieldCutscenePlayBgm::GetListItemsImpl(int indentLevel)
{
    QList<ListItemBase<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType> *> editorList;
    editorList.append(new FieldCutscenePlayBgmListItem(this, indentLevel));
    return editorList;
}

QString FieldCutscenePlayBgmListItem::GetDisplayString()
{
    QString displayString = "Play the background music " + UnderlineString(pPhase->bgmId) + ".";
    return displayString;
}

FieldCutscenePlayBgmEditorDialogContents::FieldCutscenePlayBgmEditorDialogContents(
        const FieldCutscene::State &stateBeforeObject,
        FieldCutscene::FieldCutscenePlayBgm *pPhaseToEdit)
    : EditorDialogContents<FieldCutscene::FieldCutscenePhase, FieldCutscene::State>(stateBeforeObject, pPhaseToEdit)
{
    if (pPhaseToEdit != NULL)
    {
        pFieldCutscenePlayBgm = static_cast<FieldCutscene::FieldCutscenePlayBgm *>(pPhaseToEdit->Clone());
    }
    else
    {
        pFieldCutscenePlayBgm = new FieldCutscene::FieldCutscenePlayBgm();
    }

    pObject = pFieldCutscenePlayBgm;

    QHBoxLayout *pLayout = new QHBoxLayout();

    pLayout->addWidget(new QLabel("Editing! Yay!"));

    pLayout->addStretch(1);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(pLayout);
}

bool FieldCutscenePlayBgmEditorDialogContents::ValidateFields()
{
    return true;
}

FieldCutscene::FieldCutsceneStopBgm::FieldCutsceneStopBgm(Staging::FieldCutscene::FieldCutsceneStopBgm *pFieldCutsceneStopBgm)
{
    isInstant = pFieldCutsceneStopBgm->IsInstant;
}

EditorDialogContents<FieldCutscene::FieldCutscenePhase, FieldCutscene::State> * FieldCutscene::FieldCutsceneStopBgm::CreateEditorDialogContents(
        const FieldCutscene::State &stateBeforeObject,
        FieldCutscene::FieldCutscenePhase *pPhaseToEdit)
{
    return new FieldCutsceneStopBgmEditorDialogContents(stateBeforeObject, dynamic_cast<FieldCutscene::FieldCutsceneStopBgm *>(pPhaseToEdit));
}

void FieldCutscene::FieldCutsceneStopBgm::CopyPropertiesImpl(FieldCutscene::FieldCutscenePhase *pOther, bool /*isForEdit*/)
{
    FieldCutscene::FieldCutsceneStopBgm *pPhase = dynamic_cast<FieldCutscene::FieldCutsceneStopBgm *>(pOther);

    pPhase->isInstant = isInstant;
}

void FieldCutscene::FieldCutsceneStopBgm::UpdateCutsceneState(FieldCutscene::State &currentState)
{
    currentState.SetBgmId("");
}

QList<ListItemBase<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType> *> FieldCutscene::FieldCutsceneStopBgm::GetListItemsImpl(int indentLevel)
{
    QList<ListItemBase<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType> *> editorList;
    editorList.append(new FieldCutsceneStopBgmListItem(this, indentLevel));
    return editorList;
}

QString FieldCutsceneStopBgmListItem::GetDisplayString()
{
    QString displayString = "Stop the currently playing background music ";

    if (pPhase->isInstant)
    {
        displayString += " instantly.";
    }
    else
    {
        displayString += " after a fade-out delay.";
    }

    return displayString;
}

FieldCutsceneStopBgmEditorDialogContents::FieldCutsceneStopBgmEditorDialogContents(
        const FieldCutscene::State &stateBeforeObject,
        FieldCutscene::FieldCutsceneStopBgm *pPhaseToEdit)
    : EditorDialogContents<FieldCutscene::FieldCutscenePhase, FieldCutscene::State>(stateBeforeObject, pPhaseToEdit)
{
    if (pPhaseToEdit != NULL)
    {
        pFieldCutsceneStopBgm = static_cast<FieldCutscene::FieldCutsceneStopBgm *>(pPhaseToEdit->Clone());
    }
    else
    {
        pFieldCutsceneStopBgm = new FieldCutscene::FieldCutsceneStopBgm();
    }

    pObject = pFieldCutsceneStopBgm;

    QHBoxLayout *pLayout = new QHBoxLayout();

    pLayout->addWidget(new QLabel("Editing! Yay!"));

    pLayout->addStretch(1);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(pLayout);
}

bool FieldCutsceneStopBgmEditorDialogContents::ValidateFields()
{
    return true;
}

FieldCutscene::FieldCutscenePlayAmbiance::FieldCutscenePlayAmbiance(Staging::FieldCutscene::FieldCutscenePlayAmbiance *pFieldCutscenePlayAmbiance)
{
    ambianceSfxId = pFieldCutscenePlayAmbiance->AmbianceSfxId;
}

EditorDialogContents<FieldCutscene::FieldCutscenePhase, FieldCutscene::State> * FieldCutscene::FieldCutscenePlayAmbiance::CreateEditorDialogContents(
        const FieldCutscene::State &stateBeforeObject,
        FieldCutscene::FieldCutscenePhase *pPhaseToEdit)
{
    return new FieldCutscenePlayAmbianceEditorDialogContents(stateBeforeObject, dynamic_cast<FieldCutscene::FieldCutscenePlayAmbiance *>(pPhaseToEdit));
}

void FieldCutscene::FieldCutscenePlayAmbiance::CopyPropertiesImpl(FieldCutscene::FieldCutscenePhase *pOther, bool /*isForEdit*/)
{
    FieldCutscene::FieldCutscenePlayAmbiance *pPhase = dynamic_cast<FieldCutscene::FieldCutscenePlayAmbiance *>(pOther);

    pPhase->ambianceSfxId = ambianceSfxId;
}

void FieldCutscene::FieldCutscenePlayAmbiance::UpdateCutsceneState(FieldCutscene::State &currentState)
{
    currentState.SetAmbianceSfxId(ambianceSfxId);
}

QList<ListItemBase<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType> *> FieldCutscene::FieldCutscenePlayAmbiance::GetListItemsImpl(int indentLevel)
{
    QList<ListItemBase<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType> *> editorList;
    editorList.append(new FieldCutscenePlayAmbianceListItem(this, indentLevel));
    return editorList;
}

QString FieldCutscenePlayAmbianceListItem::GetDisplayString()
{
    QString displayString = "Play the ambiance " + UnderlineString(pPhase->ambianceSfxId) + ".";
    return displayString;
}

FieldCutscenePlayAmbianceEditorDialogContents::FieldCutscenePlayAmbianceEditorDialogContents(
        const FieldCutscene::State &stateBeforeObject,
        FieldCutscene::FieldCutscenePlayAmbiance *pPhaseToEdit)
    : EditorDialogContents<FieldCutscene::FieldCutscenePhase, FieldCutscene::State>(stateBeforeObject, pPhaseToEdit)
{
    if (pPhaseToEdit != NULL)
    {
        pFieldCutscenePlayAmbiance = static_cast<FieldCutscene::FieldCutscenePlayAmbiance *>(pPhaseToEdit->Clone());
    }
    else
    {
        pFieldCutscenePlayAmbiance = new FieldCutscene::FieldCutscenePlayAmbiance();
    }

    pObject = pFieldCutscenePlayAmbiance;

    QHBoxLayout *pLayout = new QHBoxLayout();

    pLayout->addWidget(new QLabel("Editing! Yay!"));

    pLayout->addStretch(1);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(pLayout);
}

bool FieldCutscenePlayAmbianceEditorDialogContents::ValidateFields()
{
    return true;
}

FieldCutscene::FieldCutsceneStopAmbiance::FieldCutsceneStopAmbiance(Staging::FieldCutscene::FieldCutsceneStopAmbiance *pFieldCutsceneStopAmbiance)
{
    isInstant = pFieldCutsceneStopAmbiance->IsInstant;
}

EditorDialogContents<FieldCutscene::FieldCutscenePhase, FieldCutscene::State> * FieldCutscene::FieldCutsceneStopAmbiance::CreateEditorDialogContents(
        const FieldCutscene::State &stateBeforeObject,
        FieldCutscene::FieldCutscenePhase *pPhaseToEdit)
{
    return new FieldCutsceneStopAmbianceEditorDialogContents(stateBeforeObject, dynamic_cast<FieldCutscene::FieldCutsceneStopAmbiance *>(pPhaseToEdit));
}

void FieldCutscene::FieldCutsceneStopAmbiance::CopyPropertiesImpl(FieldCutscene::FieldCutscenePhase *pOther, bool /*isForEdit*/)
{
    FieldCutscene::FieldCutsceneStopAmbiance *pPhase = dynamic_cast<FieldCutscene::FieldCutsceneStopAmbiance *>(pOther);

    pPhase->isInstant = isInstant;
}

void FieldCutscene::FieldCutsceneStopAmbiance::UpdateCutsceneState(FieldCutscene::State &currentState)
{
    currentState.SetAmbianceSfxId("");
}

QList<ListItemBase<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType> *> FieldCutscene::FieldCutsceneStopAmbiance::GetListItemsImpl(int indentLevel)
{
    QList<ListItemBase<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType> *> editorList;
    editorList.append(new FieldCutsceneStopAmbianceListItem(this, indentLevel));
    return editorList;
}

QString FieldCutsceneStopAmbianceListItem::GetDisplayString()
{
    QString displayString = "Stop the currently playing ambiance ";

    if (pPhase->isInstant)
    {
        displayString += " instantly.";
    }
    else
    {
        displayString += " after a fade-out delay.";
    }

    return displayString;
}

FieldCutsceneStopAmbianceEditorDialogContents::FieldCutsceneStopAmbianceEditorDialogContents(
        const FieldCutscene::State &stateBeforeObject,
        FieldCutscene::FieldCutsceneStopAmbiance *pPhaseToEdit)
    : EditorDialogContents<FieldCutscene::FieldCutscenePhase, FieldCutscene::State>(stateBeforeObject, pPhaseToEdit)
{
    if (pPhaseToEdit != NULL)
    {
        pFieldCutsceneStopAmbiance = static_cast<FieldCutscene::FieldCutsceneStopAmbiance *>(pPhaseToEdit->Clone());
    }
    else
    {
        pFieldCutsceneStopAmbiance = new FieldCutscene::FieldCutsceneStopAmbiance();
    }

    pObject = pFieldCutsceneStopAmbiance;

    QHBoxLayout *pLayout = new QHBoxLayout();

    pLayout->addWidget(new QLabel("Editing! Yay!"));

    pLayout->addStretch(1);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(pLayout);
}

bool FieldCutsceneStopAmbianceEditorDialogContents::ValidateFields()
{
    return true;
}

FieldCutscene::FieldCutsceneEndCase::FieldCutsceneEndCase(Staging::FieldCutscene::FieldCutsceneEndCase *pFieldCutsceneEndCase)
{
    completesCase = pFieldCutsceneEndCase->CompletesCase;
}

EditorDialogContents<FieldCutscene::FieldCutscenePhase, FieldCutscene::State> * FieldCutscene::FieldCutsceneEndCase::CreateEditorDialogContents(
        const FieldCutscene::State &stateBeforeObject,
        FieldCutscene::FieldCutscenePhase *pPhaseToEdit)
{
    return new FieldCutsceneEndCaseEditorDialogContents(stateBeforeObject, dynamic_cast<FieldCutscene::FieldCutsceneEndCase *>(pPhaseToEdit));
}

void FieldCutscene::FieldCutsceneEndCase::CopyPropertiesImpl(FieldCutscene::FieldCutscenePhase *pOther, bool /*isForEdit*/)
{
    FieldCutscene::FieldCutsceneEndCase *pPhase = dynamic_cast<FieldCutscene::FieldCutsceneEndCase *>(pOther);

    pPhase->completesCase = completesCase;
}

QList<ListItemBase<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType> *> FieldCutscene::FieldCutsceneEndCase::GetListItemsImpl(int indentLevel)
{
    QList<ListItemBase<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType> *> editorList;
    editorList.append(new FieldCutsceneEndCaseListItem(this, indentLevel));
    return editorList;
}

QString FieldCutsceneEndCaseListItem::GetDisplayString()
{
    QString displayString = "End the current case, ";

    if (pPhase->completesCase)
    {
        displayString += "and mark it as completed.";
    }
    else
    {
        displayString += "but don't mark it as completed.";
    }

    return displayString;
}

FieldCutsceneEndCaseEditorDialogContents::FieldCutsceneEndCaseEditorDialogContents(
        const FieldCutscene::State &stateBeforeObject,
        FieldCutscene::FieldCutsceneEndCase *pPhaseToEdit)
    : EditorDialogContents<FieldCutscene::FieldCutscenePhase, FieldCutscene::State>(stateBeforeObject, pPhaseToEdit)
{
    if (pPhaseToEdit != NULL)
    {
        pFieldCutsceneEndCase = static_cast<FieldCutscene::FieldCutsceneEndCase *>(pPhaseToEdit->Clone());
    }
    else
    {
        pFieldCutsceneEndCase = new FieldCutscene::FieldCutsceneEndCase();
    }

    pObject = pFieldCutsceneEndCase;

    QHBoxLayout *pLayout = new QHBoxLayout();

    pLayout->addWidget(new QLabel("Editing! Yay!"));

    pLayout->addStretch(1);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(pLayout);
}

bool FieldCutsceneEndCaseEditorDialogContents::ValidateFields()
{
    return true;
}

FieldCutscene::FieldCutscene(const QString &id)
    : FieldCutscene()
{
    this->id = id;
}

FieldCutscene::FieldCutscene(Staging::FieldCutscene *pStagingFieldCutscene)
    : FieldCutscene()
{
    id = pStagingFieldCutscene->Id;

    for (Staging::FieldCutscene::FieldCutscenePhase *pPhase : pStagingFieldCutscene->PhaseList)
    {
        phaseList.append(FieldCutscene::FieldCutscenePhase::CreateFromStaging(pPhase));
    }

    for (QString characterId : pStagingFieldCutscene->IdToCharacterMap.keys())
    {
        idToCharacterMap.insert(characterId, new Character::FieldInstance(pStagingFieldCutscene->IdToCharacterMap[characterId]));
    }

    for (Staging::FieldCharacter *pCharacter : pStagingFieldCutscene->CharacterToOriginalCharacterDirectionMap.keys())
    {
        characterIdToOriginalCharacterDirectionMap.insert(pCharacter->Id, static_cast<CharacterDirection>(pStagingFieldCutscene->CharacterToOriginalCharacterDirectionMap[pCharacter]));
    }

    for (Staging::FieldCharacter *pCharacter : pStagingFieldCutscene->CharacterToOriginalFieldCharacterDirectionMap.keys())
    {
        characterIdToOriginalFieldCharacterDirectionMap.insert(pCharacter->Id, static_cast<FieldCharacterDirection>(pStagingFieldCutscene->CharacterToOriginalFieldCharacterDirectionMap[pCharacter]));
    }

    for (Staging::FieldCharacter *pCharacter : pStagingFieldCutscene->CharacterToOriginalPositionMap.keys())
    {
        characterIdToOriginalPositionMap.insert(pCharacter->Id, pStagingFieldCutscene->CharacterToOriginalPositionMap[pCharacter]);
    }

    initialCameraPosition = pStagingFieldCutscene->InitialCameraPosition;
    isEnabled = pStagingFieldCutscene->IsEnabled;
    preventsInitialBgm = pStagingFieldCutscene->PreventsInitialBgm;
    initialBgmReplacement = pStagingFieldCutscene->InitialBgmReplacement;
    preventsInitialAmbiance = pStagingFieldCutscene->PreventsInitialAmbiance;
    initialAmbianceReplacement = pStagingFieldCutscene->InitialAmbianceReplacement;

    if (idToCharacterMap.contains("PlayerCharacter"))
    {
        idToCharacterMap[ReplacePlaceholderCharacterWithId("PlayerCharacter")] = idToCharacterMap["PlayerCharacter"];
        idToCharacterMap.remove("PlayerCharacter");
    }

    if (idToCharacterMap.contains("PartnerCharacter"))
    {
        idToCharacterMap[ReplacePlaceholderCharacterWithId("PartnerCharacter")] = idToCharacterMap["PartnerCharacter"];
        idToCharacterMap.remove("PartnerCharacter");
    }
}

FieldCutscene::~FieldCutscene()
{
    pOwningLocation = NULL;
}

void FieldCutscene::LoadElementsFromXml(XmlReader *pReader)
{
    XmlStorableObject::LoadElementsFromXml(pReader);

    if (idToCharacterMap.contains("PlayerCharacter"))
    {
        idToCharacterMap[ReplacePlaceholderCharacterWithId("PlayerCharacter")] = idToCharacterMap["PlayerCharacter"];
        idToCharacterMap.remove("PlayerCharacter");
    }

    if (idToCharacterMap.contains("PartnerCharacter"))
    {
        idToCharacterMap[ReplacePlaceholderCharacterWithId("PartnerCharacter")] = idToCharacterMap["PartnerCharacter"];
        idToCharacterMap.remove("PartnerCharacter");
    }
}

void FieldCutscene::SetIdFromIndex(int indexInScene)
{
    char id[256];
    sprintf(id, "FieldCutscene%d", indexInScene + 1);
    this->id = id;
}

void FieldCutscene::InitializeForDrawable(IDrawable *pParentDrawable)
{
    for (FieldCutscenePhase *pPhase : phaseList)
    {
        pPhase->InitializeForDrawable(pParentDrawable);
    }
}

void FieldCutscene::UpdateAndCacheCutsceneStates()
{
    // TODO: Detect what location goes here and use its BGM and ambiance if one exists
    // and the initial BGM wasn't overridden.
    State state = State(
                characterIdToOriginalCharacterDirectionMap,
                characterIdToOriginalFieldCharacterDirectionMap,
                characterIdToOriginalPositionMap,
                initialCameraPosition,
                "",
                preventsInitialBgm ? initialBgmReplacement : "",
                preventsInitialAmbiance ? initialAmbianceReplacement : "",
                idToCharacterMap);

    for (FieldCutscenePhase *pPhase : phaseList)
    {
        pPhase->UpdateAndCacheCutsceneState(state);
    }
}

void FieldCutscene::ReplacePhase(FieldCutscenePhase *pNewPhase, FieldCutscenePhase *pOldPhase)
{
    for (int i = 0; i < phaseList.length(); i++)
    {
        if (pOldPhase == phaseList[i])
        {
             phaseList[i] = pNewPhase;
             delete pOldPhase;
             break;
        }
    }
}

ObjectListWidget<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType> * FieldCutscene::GetObjectListWidget()
{
    ObjectListWidget<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType> *pListWidget = new ObjectListWidget<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType>();

    pListWidget->BeginListItemChanges();

    QList<ListItemBase<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType> *> listItems;

    listItems.append(new TopSeparatorListItem<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType>(0, &phaseList, 0, true /* shouldInsert */));

    if (phaseList.length() > 0)
    {
        int currentPosition = 0;
        int itemCount = phaseList.length();

        for (FieldCutscene::FieldCutscenePhase *pPhase : phaseList)
        {
            for (ListItemBase<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType> *pChildListItem : pPhase->GetListItems(0))
            {
                listItems.append(pChildListItem);

                if (pChildListItem->GetIndentLevel() == 0)
                {
                    pChildListItem->SetContainingObjectList(&phaseList, currentPosition, false /* shouldInsert */);
                }
            }

            currentPosition++;

            if (currentPosition != itemCount)
            {
                 listItems.append(new MiddleSeparatorListItem<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType>(0, &phaseList, currentPosition, true /* shouldInsert */));
            }
        }

        listItems.append(new BottomSeparatorListItem<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType>(0, &phaseList, itemCount, true /* shouldInsert */));
    }

    pListWidget->AddListItems(listItems);
    pListWidget->NotifyListItemChangesComplete();
    pListWidget->UpdateListItemStatesBeforeObject();

    return pListWidget;
}

namespace FieldCutscenePrivate
{
    QMap<QString, FieldCutscene::PhaseType> PhaseTypeByComboBoxStringMap
    {
        { "Show a conversation...", FieldCutscene::PhaseType::Conversation },
        { "Move a character...", FieldCutscene::PhaseType::Movement },
        { "Orient a character...", FieldCutscene::PhaseType::Orient },
        { "Set the background...", FieldCutscene::PhaseType::SetBackground },
        { "Move the camera...", FieldCutscene::PhaseType::CenterCamera },
        { "Pause for a period of time...", FieldCutscene::PhaseType::Pause },
        { "Play background music...", FieldCutscene::PhaseType::PlayBgm },
        { "Stop background music...", FieldCutscene::PhaseType::StopBgm },
        { "Play ambiance...", FieldCutscene::PhaseType::PlayAmbiance },
        { "Stop ambiance...", FieldCutscene::PhaseType::StopAmbiance },
        { "End the current case...", FieldCutscene::PhaseType::EndCase },
    };

    typedef EditorDialogContents<FieldCutscene::FieldCutscenePhase, FieldCutscene::State> * (*PFNEDITORDIALOGCONTENTSCREATE)(const FieldCutscene::State &, FieldCutscene::FieldCutscenePhase *);

    QMap<FieldCutscene::PhaseType, PFNEDITORDIALOGCONTENTSCREATE> EditorDialogContentsCreationMethodsByTypeMap
    {
        { FieldCutscene::PhaseType::Conversation, FieldCutscene::FieldCutsceneConversation::CreateEditorDialogContents },
        { FieldCutscene::PhaseType::Movement, FieldCutscene::FieldCutsceneMovement::CreateEditorDialogContents },
        { FieldCutscene::PhaseType::Orient, FieldCutscene::FieldCutsceneOrient::CreateEditorDialogContents },
        { FieldCutscene::PhaseType::SetBackground, FieldCutscene::FieldCutsceneSetBackground::CreateEditorDialogContents },
        { FieldCutscene::PhaseType::CenterCamera, FieldCutscene::FieldCutsceneCenterCamera::CreateEditorDialogContents },
        { FieldCutscene::PhaseType::Pause, FieldCutscene::FieldCutscenePause::CreateEditorDialogContents },
        { FieldCutscene::PhaseType::PlayBgm, FieldCutscene::FieldCutscenePlayBgm::CreateEditorDialogContents },
        { FieldCutscene::PhaseType::StopBgm, FieldCutscene::FieldCutsceneStopBgm::CreateEditorDialogContents },
        { FieldCutscene::PhaseType::PlayAmbiance, FieldCutscene::FieldCutscenePlayAmbiance::CreateEditorDialogContents },
        { FieldCutscene::PhaseType::StopAmbiance, FieldCutscene::FieldCutsceneStopAmbiance::CreateEditorDialogContents },
        { FieldCutscene::PhaseType::EndCase, FieldCutscene::FieldCutsceneEndCase::CreateEditorDialogContents },
    };
}

template <>
QString GetObjectDisplayName<FieldCutscene::FieldCutscenePhase>()
{
    return "cutscene phase";
}

template <>
QMap<QString, FieldCutscene::PhaseType> GetTypeByComboBoxStringMap<FieldCutscene::FieldCutscenePhase, FieldCutscene::PhaseType>()
{
    return FieldCutscenePrivate::PhaseTypeByComboBoxStringMap;
}

template <>
QMap<FieldCutscene::PhaseType, FieldCutscenePrivate::PFNEDITORDIALOGCONTENTSCREATE> GetEditorDialogContentsCreationMethodsByTypeMap<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType>()
{
    return FieldCutscenePrivate::EditorDialogContentsCreationMethodsByTypeMap;
}

template <>
void CaseContent::Add<FieldCutscene>(QString, FieldCutscene *pFieldCutscene, void *pParentObject)
{
    Location *pLocation = reinterpret_cast<Location *>(pParentObject);

    if (pLocation != NULL)
    {
        pLocation->AddFieldCutscene(pFieldCutscene);
    }
}

template <>
FieldCutscene * CaseContent::GetById<FieldCutscene>(QString id, void *pParentObject)
{
    Location *pLocation = reinterpret_cast<Location *>(pParentObject);

    if (pLocation != NULL)
    {
        return pLocation->GetFieldCutsceneById(id);
    }
    else
    {
        return NULL;
    }
}

template <>
QStringList CaseContent::GetIds<FieldCutscene>(void *pParentObject)
{
    Location *pLocation = reinterpret_cast<Location *>(pParentObject);

    if (pLocation != NULL)
    {
        return pLocation->GetFieldCutsceneIds();
    }
    else
    {
        return QStringList();
    }
}

template <>
QStringList CaseContent::GetDisplayNames<FieldCutscene>(void *pParentObject)
{
    Location *pLocation = reinterpret_cast<Location *>(pParentObject);

    if (pLocation != NULL)
    {
        return pLocation->GetFieldCutsceneDisplayNames();
    }
    else
    {
        return QStringList();
    }
}
