#include "Location.h"

#include "CaseContent.h"
#include "XmlReader.h"
#include "XmlWriter.h"

#include <QtAlgorithms>

#include "Staging/CaseContentLoadingStager.h"
#include "Staging/ForegroundElement.Staging.h"
#include "Staging/Location.Staging.h"

Location::Location(Staging::Location *pStagingLocation)
    : Location()
{
    id = pStagingLocation->Id;
    backgroundFilePath = CaseContentLoadingStager::GetCurrent()->GetSpriteFilePathFromId(pStagingLocation->BackgroundSpriteId);

    pAreaHitBox = new HitBox(pStagingLocation->pAreaHitBox);

    int interactionElementCount = 0;
    int foregroundElementCount = 0;

    for (Staging::ForegroundElement *pForegroundElement : pStagingLocation->ForegroundElementList)
    {
        // In the case creator, we have two different types that are collapsed down into
        // "foreground elements" in the case file: interaction locations and genuine foreground elements.
        // Interaction locations are just a location with a position and a polygon, whereas
        // genuine foreground elements also have a visual associated with them.
        // We treat them differently in the case creator just to make things easier for the case creator.
        if (pForegroundElement->SpriteId.length() > 0 || pForegroundElement->ForegroundElementAnimationList.count() > 0)
        {
            AddForegroundElement(new ForegroundElement(pForegroundElement, foregroundElementCount++));
        }
        else
        {
            AddInteractionLocation(new InteractionLocation(pForegroundElement, interactionElementCount++));
        }
    }

    for (Staging::FieldCharacter *pFieldCharacter : pStagingLocation->CharacterList)
    {
        AddCharacterInstance(new Character::FieldInstance(pFieldCharacter));
    }

    for (QString zoomedViewId : pStagingLocation->ZoomedViewsByIdMap.keys())
    {
        AddZoomedView(new ZoomedView(pStagingLocation->ZoomedViewsByIdMap[zoomedViewId]));
    }
}

Location::~Location()
{
    delete pAreaHitBox;

    for (QMap<QString, InteractionLocation *>::iterator iter = idToInteractionLocationMap.begin(); iter != idToInteractionLocationMap.end(); iter++)
    {
        delete iter.value();
    }

    idToInteractionLocationMap.clear();

    for (QMap<QString, ForegroundElement *>::iterator iter = idToForegroundElementMap.begin(); iter != idToForegroundElementMap.end(); iter++)
    {
        delete iter.value();
    }

    idToForegroundElementMap.clear();

    for (QMap<QString, Character::FieldInstance *>::iterator iter = idToCharacterInstanceMap.begin(); iter != idToCharacterInstanceMap.end(); iter++)
    {
        delete iter.value();
    }

    idToCharacterInstanceMap.clear();

    for (QMap<QString, ZoomedView *>::iterator iter = idToZoomedViewMap.begin(); iter != idToZoomedViewMap.end(); iter++)
    {
        delete iter.value();
    }

    idToZoomedViewMap.clear();
}

void Location::LoadElementsFromXml(XmlReader *pReader)
{
    XmlStorableObject::LoadElementsFromXml(pReader);

    for (QString cutsceneId : idToFieldCutsceneMap.keys())
    {
        idToFieldCutsceneMap[cutsceneId]->SetOwningLocation(this);
    }
}

void Location::WriteToCaseXml(XmlWriter *pWriter)
{
    pWriter->StartElement("Location");
    pWriter->WriteTextElement("Id", this->id);
    pWriter->WriteTextElement("BackgroundSpriteId", this->backgroundFilePath);
    pWriter->WriteTextElement("Bgm", "placeholder");
    pWriter->WriteTextElement("AmbianceSfxId", "placeholder");
    this->pAreaHitBox->WriteToCaseXml(pWriter);

    pWriter->StartElement("CutsceneIdList");

    for (QMap<QString, FieldCutscene *>::iterator iter = idToFieldCutsceneMap.begin(); iter != idToFieldCutsceneMap.end(); iter++)
    {
        pWriter->StartElement("Entry");
        pWriter->WriteTextElement("CutsceneId", iter.key());
        pWriter->EndElement();
    }

    pWriter->EndElement();

    pWriter->StartElement("ForegroundElementList");

    for (QMap<QString, ForegroundElement *>::iterator iter = idToForegroundElementMap.begin(); iter != idToForegroundElementMap.end(); iter++)
    {
        pWriter->StartElement("Entry");
        iter.value()->WriteToCaseXml(pWriter);
        pWriter->EndElement();
    }

    pWriter->EndElement();

    pWriter->StartElement("HiddenForegroundElementList");

    // Placeholder - Nothing yet.

    pWriter->EndElement();

    pWriter->StartElement("ZoomedViewList");

    for (QMap<QString, ZoomedView *>::iterator iter = idToZoomedViewMap.begin(); iter != idToZoomedViewMap.end(); iter++)
    {
        pWriter->StartElement("Entry");
        iter.value()->WriteToCaseXml(pWriter);
        pWriter->EndElement();
    }

    pWriter->EndElement();

    pWriter->StartElement("CharacterList");

    for (QMap<QString, Character::FieldInstance *>::iterator iter = idToCharacterInstanceMap.begin(); iter != idToCharacterInstanceMap.end(); iter++)
    {
        pWriter->StartElement("Entry");
        iter.value()->WriteToCaseXml(pWriter);
        pWriter->EndElement();
    }

    pWriter->EndElement();

    pWriter->StartElement("CrowdList");

    // Placeholder

    pWriter->EndElement();

    pWriter->StartElement("TransitionList");

    // Placeholder

    pWriter->EndElement();

    pWriter->EndElement();
}

void Location::AddInteractionLocation(InteractionLocation *pInteractionLocation)
{
    if (pInteractionLocation->GetId().length() > 0)
    {
        idToInteractionLocationMap[pInteractionLocation->GetId()] = pInteractionLocation;
    }
    else
    {
        int index = idToInteractionLocationMap.keys().length();

        while (!idToInteractionLocationMap.values().contains(pInteractionLocation))
        {
            pInteractionLocation->SetIdFromIndex(index);

            if (!idToInteractionLocationMap.keys().contains(pInteractionLocation->GetId()))
            {
                idToInteractionLocationMap[pInteractionLocation->GetId()] = pInteractionLocation;
            }

            index++;
        }
    }
}

InteractionLocation * Location::GetInteractionLocationById(QString id)
{
    if (idToInteractionLocationMap.contains(id))
    {
        return idToInteractionLocationMap[id];
    }
    else
    {
        return NULL;
    }
}

QStringList Location::GetInteractionLocationIds()
{
    QStringList idList;

    for (QMap<QString, InteractionLocation *>::iterator iter = idToInteractionLocationMap.begin(); iter != idToInteractionLocationMap.end(); iter++)
    {
        idList.push_back(iter.value()->GetId());
    }

    return idList;
}

QStringList Location::GetInteractionLocationDisplayNames()
{
    QStringList displayNameList;

    for (QMap<QString, InteractionLocation *>::iterator iter = idToInteractionLocationMap.begin(); iter != idToInteractionLocationMap.end(); iter++)
    {
        displayNameList.push_back(iter.value()->GetDisplayName());
    }

    return displayNameList;
}

void Location::AddForegroundElement(ForegroundElement *pElement)
{
    if (pElement->GetId().length() > 0)
    {
        idToForegroundElementMap[pElement->GetId()] = pElement;
    }
    else
    {
        int index = idToForegroundElementMap.keys().length();

        while (!idToForegroundElementMap.values().contains(pElement))
        {
            pElement->SetIdFromIndex(index);

            if (!idToForegroundElementMap.keys().contains(pElement->GetId()))
            {
                idToForegroundElementMap[pElement->GetId()] = pElement;
            }

            index++;
        }
    }
}

ForegroundElement * Location::GetForegroundElementById(QString id)
{
    if (idToForegroundElementMap.contains(id))
    {
        return idToForegroundElementMap[id];
    }
    else
    {
        return NULL;
    }
}

QStringList Location::GetForegroundElementIds()
{
    QStringList idList;

    for (QMap<QString, ForegroundElement *>::iterator iter = idToForegroundElementMap.begin(); iter != idToForegroundElementMap.end(); iter++)
    {
        idList.push_back(iter.value()->GetId());
    }

    return idList;
}

QStringList Location::GetForegroundElementDisplayNames()
{
    QStringList displayNameList;

    for (QMap<QString, ForegroundElement *>::iterator iter = idToForegroundElementMap.begin(); iter != idToForegroundElementMap.end(); iter++)
    {
        displayNameList.push_back(iter.value()->GetDisplayName());
    }

    return displayNameList;
}

Character::FieldInstance * Location::GetCharacterInstanceById(QString id)
{
    return idToCharacterInstanceMap[id];
}

void Location::AddCharacterInstance(Character::FieldInstance *pElement)
{
    idToCharacterInstanceMap[pElement->GetId()] = pElement;
}

void Location::AddFieldCutscene(FieldCutscene *pElement)
{
    if (pElement->GetId().length() > 0)
    {
        idToFieldCutsceneMap[pElement->GetId()] = pElement;
    }
    else
    {
        int index = idToFieldCutsceneMap.keys().length();

        while (!idToFieldCutsceneMap.values().contains(pElement))
        {
            pElement->SetIdFromIndex(index);

            if (!idToFieldCutsceneMap.keys().contains(pElement->GetId()))
            {
                idToFieldCutsceneMap[pElement->GetId()] = pElement;
            }

            index++;
        }
    }

    pElement->SetOwningLocation(this);
}

FieldCutscene * Location::GetFieldCutsceneById(QString id)
{
    return idToFieldCutsceneMap[id];
}

QStringList Location::GetFieldCutsceneIds()
{
    QStringList idList;

    for (QMap<QString, FieldCutscene *>::iterator iter = idToFieldCutsceneMap.begin(); iter != idToFieldCutsceneMap.end(); iter++)
    {
        idList.push_back(iter.value()->GetId());
    }

    return idList;
}

QStringList Location::GetFieldCutsceneDisplayNames()
{
    QStringList displayNameList;

    for (QMap<QString, FieldCutscene *>::iterator iter = idToFieldCutsceneMap.begin(); iter != idToFieldCutsceneMap.end(); iter++)
    {
        displayNameList.push_back(iter.value()->GetDisplayName());
    }

    return displayNameList;
}

void Location::AddZoomedView(ZoomedView *pElement)
{
    if (pElement->GetId().length() == 0)
    {
        qFatal("We expect every zoomed view to have a non-empty ID.");
    }

    idToZoomedViewMap[pElement->GetId()] = pElement;
}

ZoomedView * Location::GetZoomedViewById(QString id)
{
    return idToZoomedViewMap[id];
}

QStringList Location::GetZoomedViewIds()
{
    QStringList idList;

    for (QMap<QString, ZoomedView *>::iterator iter = idToZoomedViewMap.begin(); iter != idToZoomedViewMap.end(); iter++)
    {
        idList.push_back(iter.value()->GetId());
    }

    return idList;
}

QStringList Location::GetZoomedViewDisplayNames()
{
    QStringList displayNameList;

    for (QMap<QString, ZoomedView *>::iterator iter = idToZoomedViewMap.begin(); iter != idToZoomedViewMap.end(); iter++)
    {
        displayNameList.push_back(iter.value()->GetDisplayName());
    }

    return displayNameList;
}

Location::OverviewView::OverviewView(Location *pLocation)
    : IDrawable(NULL)
    , IManipulationSurfaceView<Location>(pLocation, NULL)
{
    backgroundPixmap = QPixmap(pObject->backgroundFilePath, "png");

    for (QMap<QString, InteractionLocation *>::iterator iter = pObject->idToInteractionLocationMap.begin(); iter != pObject->idToInteractionLocationMap.end(); iter++)
    {
        IManipulatable *pInteractionLocationManipulatable = iter.value()->GetManipulationView(this, this);

        manipulatableList.push_back(pInteractionLocationManipulatable);
        interactionLocationManipulatableList.push_back(pInteractionLocationManipulatable);
    }

    for (QMap<QString, ForegroundElement *>::iterator iter = pObject->idToForegroundElementMap.begin(); iter != pObject->idToForegroundElementMap.end(); iter++)
    {
        IManipulatable *pForegroundElementManipulatable = iter.value()->GetManipulationView(this, this);

        manipulatableList.push_back(pForegroundElementManipulatable);
        foregroundElementManipulatableList.push_back(pForegroundElementManipulatable);
    }

    for (QMap<QString, Character::FieldInstance *>::iterator iter = pObject->idToCharacterInstanceMap.begin(); iter != pObject->idToCharacterInstanceMap.end(); iter++)
    {
        IManipulatable *pCharacterManipulatable = iter.value()->GetManipulationView(this, this);

        manipulatableList.push_back(pCharacterManipulatable);
        characterManipulatableList.push_back(pCharacterManipulatable);
    }

    for (HitBox::Polygon::ManipulationView *pManipulationView : pObject->pAreaHitBox->GetPolygonManipulationViews(this, this))
    {
        hitBoxManipulatableList.push_back(pManipulationView);
        manipulatableList.push_back(pManipulationView);
    }
}

Location::OverviewView::~OverviewView()
{
    backgroundPixmap = QPixmap();

    for (IManipulatable *pManipulatable : manipulatableList)
    {
        delete pManipulatable;
    }

    manipulatableList.clear();
}

void Location::OverviewView::DrawCore(QGraphicsScene *pScene, QList<QGraphicsItem *> &addedItems)
{
    IManipulationSurfaceView::DrawCore(pScene, addedItems);

    addedItems.push_back(pScene->addPixmap(backgroundPixmap));
}

void Location::OverviewView::DrawChildren(QGraphicsScene *pScene)
{
    IManipulationSurfaceView::DrawChildren(pScene);

    for (IManipulatable *pManipulatable : manipulatableList)
    {
        pManipulatable->Draw(pScene);
    }
}

void Location::OverviewView::UpdateChildren()
{
    IManipulationSurfaceView::UpdateChildren();

    for (IManipulatable *pManipulatable : manipulatableList)
    {
        pManipulatable->Update();
    }
}

void Location::OverviewView::ResetChildren()
{
    IManipulationSurfaceView::ResetChildren();

    for (IManipulatable *pManipulatable : manipulatableList)
    {
        pManipulatable->Reset();
    }
}

void Location::OverviewView::SetAreInteractionLocationsEnabled(bool areEnabled)
{
    for (IManipulatable *pManipulatable : interactionLocationManipulatableList)
    {
        pManipulatable->SetIsEnabled(areEnabled);
    }
}

void Location::OverviewView::SetAreForegroundElementsEnabled(bool areEnabled)
{
    for (IManipulatable *pManipulatable : foregroundElementManipulatableList)
    {
        pManipulatable->SetIsEnabled(areEnabled);
    }
}

void Location::OverviewView::SetAreCharactersEnabled(bool areEnabled)
{
    for (IManipulatable *pManipulatable : characterManipulatableList)
    {
        pManipulatable->SetIsEnabled(areEnabled);
    }
}

void Location::OverviewView::SetAreHitBoxesEnabled(bool areEnabled)
{
    for (IManipulatable *pManipulatable : hitBoxManipulatableList)
    {
        pManipulatable->SetIsEnabled(areEnabled);
    }
}

Location::CutsceneView::CameraManipulatable::CameraManipulatable(const Vector2 &backgroundSize, IDrawable *pParent, UndoCommandSink *pUndoCommandSink)
    : IDrawable(pParent)
    , IManipulatable(pParent, pUndoCommandSink)
{
    this->backgroundSize = backgroundSize;

    pOutsideCameraPathItem = NULL;
    pCameraCenterHorizontalOuterLineItem = NULL;
    pCameraCenterHorizontalInnerLineItem = NULL;
    pCameraCenterVerticalOuterLineItem = NULL;
    pCameraCenterVerticalInnerLineItem = NULL;
    pCameraCenterGraphicsItemGroup = NULL;
    pCameraCenterOpacityEffect = NULL;
}

void Location::CutsceneView::CameraManipulatable::SetWindowRect(RectangleWH windowRect)
{
    RectangleWH adjustedWindowRect =
        RectangleWH(
                min(max(windowRect.GetX(), 0.0), backgroundSize.GetX() - gameWindowSize.GetX()),
                min(max(windowRect.GetY(), 0.0), backgroundSize.GetY() - gameWindowSize.GetY()),
                gameWindowSize.GetX(),
                gameWindowSize.GetY());

    if (adjustedWindowRect != this->windowRect)
    {
        this->windowRect = adjustedWindowRect;
        Update();

        emit WindowRectChanged(adjustedWindowRect);
    }
}

void Location::CutsceneView::CameraManipulatable::DrawCore(QGraphicsScene * pScene, QList<QGraphicsItem *> & addedItems)
{
    QBrush blackBrush(qRgba(0, 0, 0, 255));
    QPen blackPen(blackBrush, 4);
    QPen whitePen(QBrush(qRgba(255, 255, 255, 255)), 2);

    pOutsideCameraPathItem = new QGraphicsPathItem();
    pOutsideCameraPathItem->setBrush(blackBrush);

    QGraphicsOpacityEffect *pOutsideCameraOpacityEffect = new QGraphicsOpacityEffect();
    pOutsideCameraOpacityEffect->setOpacity(0.5);

    pOutsideCameraPathItem->setGraphicsEffect(pOutsideCameraOpacityEffect);

    pScene->addItem(pOutsideCameraPathItem);
    addedItems.append(pOutsideCameraPathItem);

    pCameraCenterHorizontalOuterLineItem = new QGraphicsLineItem();
    pCameraCenterVerticalOuterLineItem = new QGraphicsLineItem();
    pCameraCenterHorizontalInnerLineItem = new QGraphicsLineItem();
    pCameraCenterVerticalInnerLineItem = new QGraphicsLineItem();

    pCameraCenterHorizontalOuterLineItem->setPen(blackPen);
    pCameraCenterVerticalOuterLineItem->setPen(blackPen);
    pCameraCenterHorizontalInnerLineItem->setPen(whitePen);
    pCameraCenterVerticalInnerLineItem->setPen(whitePen);

    pCameraCenterGraphicsItemGroup = new QGraphicsItemGroup();
    pCameraCenterGraphicsItemGroup->addToGroup(pCameraCenterHorizontalOuterLineItem);
    pCameraCenterGraphicsItemGroup->addToGroup(pCameraCenterVerticalOuterLineItem);
    pCameraCenterGraphicsItemGroup->addToGroup(pCameraCenterHorizontalInnerLineItem);
    pCameraCenterGraphicsItemGroup->addToGroup(pCameraCenterVerticalInnerLineItem);

    pCameraCenterOpacityEffect = new QGraphicsOpacityEffect();
    pCameraCenterOpacityEffect->setOpacity(1.0);

    pCameraCenterGraphicsItemGroup->setGraphicsEffect(pCameraCenterOpacityEffect);

    pScene->addItem(pCameraCenterGraphicsItemGroup);
    addedItems.append(pCameraCenterGraphicsItemGroup);
}

void Location::CutsceneView::CameraManipulatable::UpdateCore()
{
    if (pOutsideCameraPathItem != NULL)
    {
        QRectF gameWindowPosition = windowRect.ToQRectF();

        QPainterPath outsidePath;
        outsidePath.addRect(
                    0, 0,
                    backgroundSize.GetX(), gameWindowPosition.top());
        outsidePath.addRect(
                    0, gameWindowPosition.top(),
                    gameWindowPosition.left(), gameWindowPosition.height());
        outsidePath.addRect(
                    gameWindowPosition.right(), gameWindowPosition.top(),
                    backgroundSize.GetX() - gameWindowPosition.right(), gameWindowPosition.height());
        outsidePath.addRect(
                    0, gameWindowPosition.bottom(),
                    backgroundSize.GetX(), backgroundSize.GetY() - gameWindowPosition.bottom());

        pOutsideCameraPathItem->setPath(outsidePath);
        pOutsideCameraPathItem->setZValue(99 * 1000000);

        QLine horizontalLine(
                    gameWindowPosition.center().x() - 10, gameWindowPosition.center().y(),
                    gameWindowPosition.center().x() + 10, gameWindowPosition.center().y());
        QLine verticalLine(
                    gameWindowPosition.center().x(), gameWindowPosition.center().y() - 10,
                    gameWindowPosition.center().x(), gameWindowPosition.center().y() + 10);

        pCameraCenterHorizontalOuterLineItem->setLine(horizontalLine);
        pCameraCenterVerticalOuterLineItem->setLine(verticalLine);
        pCameraCenterHorizontalInnerLineItem->setLine(horizontalLine);
        pCameraCenterVerticalInnerLineItem->setLine(verticalLine);

        pCameraCenterHorizontalOuterLineItem->setZValue(1);
        pCameraCenterVerticalOuterLineItem->setZValue(1);
        pCameraCenterHorizontalInnerLineItem->setZValue(2);
        pCameraCenterVerticalInnerLineItem->setZValue(2);

        pCameraCenterGraphicsItemGroup->setZValue(99 * 1000000);
        pCameraCenterOpacityEffect->setOpacity(IsEnabled() ? 1.0 : 0.0);
    }
}

Location::CutsceneView::CutsceneView(Location *pLocation)
    : IDrawable(NULL)
    , IManipulationSurfaceView<Location>(pLocation, NULL)
{
    backgroundPixmap = QPixmap(pObject->backgroundFilePath, "png");

    for (QMap<QString, InteractionLocation *>::iterator iter = pObject->idToInteractionLocationMap.begin(); iter != pObject->idToInteractionLocationMap.end(); iter++)
    {
        IManipulatable *pInteractionLocationManipulatable = iter.value()->GetManipulationView(this, this);
        pInteractionLocationManipulatable->SetIsEnabled(false);

        manipulatableList.push_back(pInteractionLocationManipulatable);
    }

    for (QMap<QString, ForegroundElement *>::iterator iter = pObject->idToForegroundElementMap.begin(); iter != pObject->idToForegroundElementMap.end(); iter++)
    {
        IManipulatable *pForegroundElementManipulatable = iter.value()->GetManipulationView(this, this);
        pForegroundElementManipulatable->SetIsEnabled(false);

        manipulatableList.push_back(pForegroundElementManipulatable);
    }

    pCameraManipulatable = new Location::CutsceneView::CameraManipulatable(Vector2(backgroundPixmap.size()), this, this);
    QObject::connect(pCameraManipulatable, SIGNAL(WindowRectChanged(RectangleWH)), this, SLOT(CameraManipulatableWindowRectChanged(RectangleWH)));
    manipulatableList.push_back(pCameraManipulatable);

    additionalDrawablesList.clear();
}

Location::CutsceneView::~CutsceneView()
{
    backgroundPixmap = QPixmap();

    QObject::disconnect(pCameraManipulatable, SIGNAL(WindowRectChanged(RectangleWH)), this, SLOT(CameraManipulatableWindowRectChanged(RectangleWH)));

    for (Character::FieldInstance::ManipulationView *pCharacterManipulatable : characterManipulatableByStringMap.values())
    {
        QObject::disconnect(pCharacterManipulatable, SIGNAL(PositionOverrideChanged(Vector2)), this, SLOT(CharacterManipulatablePositionOverrideChanged(Vector2)));
    }

    for (IManipulatable *pManipulatable : manipulatableList)
    {
        delete pManipulatable;
    }

    manipulatableList.clear();
}

void Location::CutsceneView::SetToCutscene(const QString &cutsceneId)
{
    for (Character::FieldInstance::ManipulationView *pCharacterManipulatable : characterManipulatableByStringMap.values())
    {
        QObject::disconnect(pCharacterManipulatable, SIGNAL(PositionOverrideChanged(Vector2)), this, SLOT(CharacterManipulatablePositionOverrideChanged(Vector2)));
        delete pCharacterManipulatable;
    }

    characterManipulatableByStringMap.clear();

    pFieldCutscene = pObject->GetFieldCutsceneById(cutsceneId);

    for (QMap<QString, Character::FieldInstance *>::iterator iter = pFieldCutscene->GetIdToCharacterMap().begin(); iter != pFieldCutscene->GetIdToCharacterMap().end(); iter++)
    {
        Character::FieldInstance::ManipulationView *pCharacterManipulatable = iter.value()->GetManipulationView(this, this);
        pCharacterManipulatable->SetIsEnabled(true);
        pCharacterManipulatable->SetShouldDimIfDisabled(false);

        manipulatableList.push_back(pCharacterManipulatable);
        characterManipulatableByStringMap.insert(iter.key(), pCharacterManipulatable);

        QObject::connect(pCharacterManipulatable, SIGNAL(PositionOverrideChanged(Vector2)), this, SLOT(CharacterManipulatablePositionOverrideChanged(Vector2)));
    }

    pCameraManipulatable->SetWindowRectFromCenterPosition(pFieldCutscene->GetInitialCameraPosition());
    additionalDrawablesList.clear();
}

void Location::CutsceneView::GoToState(const FieldCutscene::State &cutsceneState)
{
    for (IDrawable *pDrawable : additionalDrawablesList)
    {
        pDrawable->Reset();
    }

    additionalDrawablesList.clear();

    for (const QString &characterId : characterManipulatableByStringMap.keys())
    {
        Character::FieldInstance::ManipulationView *pCharacterManipulationView = characterManipulatableByStringMap[characterId];

        pCharacterManipulationView->BlockUpdates();

        pCharacterManipulationView->SetPositionOverride(cutsceneState.GetCharacterPosition(characterId));
        pCharacterManipulationView->SetDirectionOverride(cutsceneState.GetCharacterDirection(characterId));
        pCharacterManipulationView->SetSpriteDirectionOverride(cutsceneState.GetFieldCharacterDirection(characterId));

        pCharacterManipulationView->SetIsEnabled(
            cutsceneState.GetActiveElement() == FieldCutscene::State::ActiveElement::Character &&
            cutsceneState.GetActiveCharacterId() == characterId);

        pCharacterManipulationView->AllowUpdates();
    }

    pCameraManipulatable->BlockUpdates();

    pCameraManipulatable->SetWindowRectFromCenterPosition(cutsceneState.GetCameraPosition());
    pCameraManipulatable->SetIsEnabled(cutsceneState.GetActiveElement() == FieldCutscene::State::ActiveElement::Camera);

    pCameraManipulatable->AllowUpdates();

    additionalDrawablesList = cutsceneState.GetAdditionalDrawablesList();

    for (IDrawable *pDrawable : additionalDrawablesList)
    {
        DrawAdditionalItem(pDrawable);
    }

    Update();
}

void Location::CutsceneView::DrawCore(QGraphicsScene *pScene, QList<QGraphicsItem *> &addedItems)
{
    IManipulationSurfaceView::DrawCore(pScene, addedItems);

    addedItems.push_back(pScene->addPixmap(backgroundPixmap));
}

void Location::CutsceneView::DrawChildren(QGraphicsScene *pScene)
{
    IManipulationSurfaceView::DrawChildren(pScene);

    for (IManipulatable *pManipulatable : manipulatableList)
    {
        pManipulatable->Draw(pScene);
    }

    pCameraManipulatable->Draw(pScene);

    for (IDrawable *pDrawable : additionalDrawablesList)
    {
        pDrawable->Draw(pScene);
    }
}

void Location::CutsceneView::UpdateChildren()
{
    IManipulationSurfaceView::UpdateChildren();

    for (IManipulatable *pManipulatable : manipulatableList)
    {
        pManipulatable->Update();
    }

    for (IDrawable *pDrawable : additionalDrawablesList)
    {
        pDrawable->Update();
    }
}

void Location::CutsceneView::ResetChildren()
{
    IManipulationSurfaceView::ResetChildren();

    for (IManipulatable *pManipulatable : manipulatableList)
    {
        pManipulatable->Reset();
    }

    for (IDrawable *pDrawable : additionalDrawablesList)
    {
        pDrawable->Reset();
    }
}

void Location::CutsceneView::UpdateState(const FieldCutscene::State &cutsceneState)
{
    if (cutsceneState.GetActiveElement() == FieldCutscene::State::ActiveElement::Character)
    {
        QString characterId = cutsceneState.GetActiveCharacterId();

        Character::FieldInstance::ManipulationView *pCharacterManipulationView = characterManipulatableByStringMap[characterId];

        pCharacterManipulationView->BlockUpdates();

        pCharacterManipulationView->SetDirectionOverride(cutsceneState.GetCharacterDirection(characterId));
        pCharacterManipulationView->SetSpriteDirectionOverride(cutsceneState.GetFieldCharacterDirection(characterId));

        pCharacterManipulationView->AllowUpdates();
        pCharacterManipulationView->Update();
    }
}

void Location::CutsceneView::DisableAllCharacters()
{
    for (IManipulatable *pCharacterManipulatable : characterManipulatableByStringMap.values())
    {
        pCharacterManipulatable->SetIsEnabled(false);
    }
}

void Location::CutsceneView::EnableOneCharacter(const QString &characterId)
{
    for (const QString &characterManipulatableId : characterManipulatableByStringMap.keys())
    {
        characterManipulatableByStringMap[characterManipulatableId]->SetIsEnabled(characterManipulatableId == characterId);
    }
}

void Location::CutsceneView::EnableAllCharacters()
{
    for (IManipulatable *pCharacterManipulatable : characterManipulatableByStringMap.values())
    {
        pCharacterManipulatable->SetIsEnabled(true);
    }
}

void Location::CutsceneView::CameraManipulatableWindowRectChangedImpl(RectangleWH newWindowRect)
{
    emit CameraRectChanged(newWindowRect);
}

void Location::CutsceneView::CharacterManipulatablePositionOverrideChangedImpl(Vector2 newPositionOverride)
{
    emit CharacterPositionChanged(newPositionOverride);
}

LocationSelector::LocationSelector(QWidget *parent)
    : QComboBox(parent)
{
    locationIds = CaseContent::GetInstance()->GetIds<Location>();

    addItems(locationIds);

    QObject::connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(CurrentIndexChanged(int)));
}

QString LocationSelector::GetId()
{
    return locationIds[currentIndex()];
}

void LocationSelector::SetToId(const QString &id)
{
    int indexOfCurrentLocation = locationIds.indexOf(QRegExp(id, Qt::CaseInsensitive));

    if (indexOfCurrentLocation >= 0)
    {
        setCurrentIndex(indexOfCurrentLocation);
    }
}

void LocationSelector::CurrentIndexChanged(int currentIndex)
{
    emit LocationSelected(locationIds[currentIndex]);
}
