#ifndef LOCATION_H
#define LOCATION_H

#include "enums.h"
#include "Character.h"
#include "InteractionLocation.h"
#include "ForegroundElement.h"
#include "FieldCutscene.h"
#include "ZoomedView.h"

#include "CaseCreator/Utilities/Interfaces.h"
#include "XmlStorableObject.h"

#include <QString>
#include <QGraphicsScene>
#include <QPixmap>
#include <QRectF>
#include <QStringList>

namespace Staging
{
    class FieldCharacter;
    class Location;
}

class Location : public XmlStorableObject
{
    friend class LocationCutsceneView;

    BEGIN_XML_STORABLE_OBJECT(Location)
        XML_STORABLE_TEXT(id)
        XML_STORABLE_FILE_PATH(backgroundFilePath)
        XML_STORABLE_CUSTOM_OBJECT(pAreaHitBox, HitBox::CreateFromXml)
        XML_STORABLE_CUSTOM_OBJECT_MAP(idToInteractionLocationMap, InteractionLocation::CreateFromXml)
        XML_STORABLE_CUSTOM_OBJECT_MAP(idToForegroundElementMap, ForegroundElement::CreateFromXml)
        XML_STORABLE_CUSTOM_OBJECT_MAP(idToCharacterInstanceMap, Character::FieldInstance::CreateFromXml)
        XML_STORABLE_CUSTOM_OBJECT_MAP(idToFieldCutsceneMap, FieldCutscene::CreateFromXml)
        XML_STORABLE_CUSTOM_OBJECT_MAP(idToZoomedViewMap, ZoomedView::CreateFromXml)
    END_XML_STORABLE_OBJECT()

    friend class OverviewView;
    friend class CutsceneView;

public:
    Location()
    {
        pAreaHitBox = NULL;
    }

    Location(QString id, QString backgroundFilePath)
        : Location()
    {
        this->id = id;
        this->backgroundFilePath = backgroundFilePath;
    }

    Location(Staging::Location *pStagingLocation);
    virtual ~Location();

    static Location * CreateFromXml(XmlReader *pReader)
    {
        return new Location(pReader);
    }

    void LoadElementsFromXml(XmlReader *pReader);

    void WriteToCaseXml(XmlWriter *pWriter);

    void AddInteractionLocation(InteractionLocation *pElement);
    InteractionLocation * GetInteractionLocationById(QString id);

    QStringList GetInteractionLocationIds();
    QStringList GetInteractionLocationDisplayNames();

    void AddForegroundElement(ForegroundElement *pElement);
    ForegroundElement * GetForegroundElementById(QString id);

    QStringList GetForegroundElementIds();
    QStringList GetForegroundElementDisplayNames();

    void AddCharacterInstance(Character::FieldInstance *pElement);
    Character::FieldInstance * GetCharacterInstanceById(QString id);

    void AddFieldCutscene(FieldCutscene *pElement);
    FieldCutscene * GetFieldCutsceneById(QString id);

    QStringList GetFieldCutsceneIds();
    QStringList GetFieldCutsceneDisplayNames();

    void AddZoomedView(ZoomedView *pElement);
    ZoomedView * GetZoomedViewById(QString id);

    QStringList GetZoomedViewIds();
    QStringList GetZoomedViewDisplayNames();

    static QString GetObjectAdditionString() { return QString("location"); }
    static QString GetListTitle() { return QString("Locations"); }
    static bool GetIsMainList() { return true; }
    static bool GetAllowsNewObjects() { return true; }

    QString GetId() { return this->id; }
    QString GetDisplayName() { return this->id; }
    QString GetBackgroundFilePath() { return this->backgroundFilePath; }

    class OverviewView : public IManipulationSurfaceView<Location>
    {
        friend class Location;

    public:
        OverviewView(Location *pLocation);
        ~OverviewView();

        void DrawCore(QGraphicsScene *pScene, QList<QGraphicsItem *> &addedItems);
        void DrawChildren(QGraphicsScene *pScene);
        void UpdateChildren();
        void ResetChildren();

        QRectF GetSceneRect()
        {
            QSize pixmapSize = backgroundPixmap.size();
            return QRectF(0, 0, pixmapSize.width(), pixmapSize.height());
        }

        void SetAreInteractionLocationsEnabled(bool areEnabled);
        void SetAreForegroundElementsEnabled(bool areEnabled);
        void SetAreCharactersEnabled(bool areEnabled);
        void SetAreHitBoxesEnabled(bool areEnabled);

    protected:
        QList<IManipulatable *> & GetManipulatableList()
        {
            return manipulatableList;
        }

    private:
        QPixmap backgroundPixmap;

        QList<IManipulatable *> manipulatableList;

        QList<IManipulatable *> interactionLocationManipulatableList;
        QList<IManipulatable *> foregroundElementManipulatableList;
        QList<IManipulatable *> characterManipulatableList;
        QList<IManipulatable *> hitBoxManipulatableList;
    };

    Location::OverviewView * GetOverviewView()
    {
        return new Location::OverviewView(this);
    }

    class CutsceneView : public IManipulationSurfaceView<Location>
    {
        friend class Location;

        class CameraManipulatable : public IManipulatable
        {
        public:
            CameraManipulatable(const Vector2 &backgroundSize, IDrawable *pParent, UndoCommandSink *pUndoCommandSink = NULL);

            virtual RectangleWH GetBoundingRect()
            {
                return windowRect;
            }

            void SetWindowRect(RectangleWH windowRect);

            void SetWindowRectFromCenterPosition(const Vector2 &centerPosition)
            {
                SetWindowRect(
                    RectangleWH(
                        centerPosition.GetX() - gameWindowSize.GetX() / 2, centerPosition.GetY() - gameWindowSize.GetY() / 2,
                        gameWindowSize.GetX(), gameWindowSize.GetY()));
            }

            bool GetShouldDimIfDisabled()
            {
                return false;
            }

        protected:
            void DrawCore(QGraphicsScene * /*pScene*/, QList<QGraphicsItem *> & /*addedItems*/) override;
            void UpdateCore() override;

            void OnMousePress() override
            {
                windowRectBeforeDrag = windowRect;
            }

            void OnMouseDrag(Vector2 delta) override
            {
                SetWindowRect(windowRect + delta);
            }

            void OnMouseRelease() override
            {
                AddUndoCommand(new CameraMoveUndoCommand(this, windowRectBeforeDrag, windowRect));
            }

        private:
            class CameraMoveUndoCommand : public QUndoCommand
            {
            public:
                CameraMoveUndoCommand(CameraManipulatable *pCameraManipulatable, RectangleWH oldWindowRect, RectangleWH newWindowRect)
                    : pCameraManipulatable(pCameraManipulatable)
                    , oldWindowRect(oldWindowRect)
                    , newWindowRect(newWindowRect)
                {
                }

                void undo()
                {
                    pCameraManipulatable->SetWindowRect(oldWindowRect);
                }

                void redo()
                {
                    pCameraManipulatable->SetWindowRect(newWindowRect);
                }

            private:
                CameraManipulatable *pCameraManipulatable;

                RectangleWH oldWindowRect;
                RectangleWH newWindowRect;
            };

            RectangleWH windowRect;
            RectangleWH windowRectBeforeDrag;
            Vector2 backgroundSize;

            QGraphicsPathItem *pOutsideCameraPathItem;
            QGraphicsLineItem *pCameraCenterHorizontalOuterLineItem;
            QGraphicsLineItem *pCameraCenterVerticalOuterLineItem;
            QGraphicsLineItem *pCameraCenterHorizontalInnerLineItem;
            QGraphicsLineItem *pCameraCenterVerticalInnerLineItem;
            QGraphicsItemGroup *pCameraCenterGraphicsItemGroup;
            QGraphicsOpacityEffect *pCameraCenterOpacityEffect;
        };

    public:
        CutsceneView(Location *pLocation);
        ~CutsceneView();

        void SetToCutscene(const QString &cutsceneId);
        void GoToState(const FieldCutscene::State &cutsceneState);
        void UpdateState(const FieldCutscene::State &cutsceneState);

        void DrawCore(QGraphicsScene *pScene, QList<QGraphicsItem *> &addedItems);
        void DrawChildren(QGraphicsScene *pScene);
        void UpdateChildren();
        void ResetChildren();

        QRectF GetSceneRect()
        {
            QSize pixmapSize = backgroundPixmap.size();
            return QRectF(0, 0, pixmapSize.width(), pixmapSize.height());
        }

        void DisableAllCharacters();
        void EnableOneCharacter(const QString &characterId);
        void EnableAllCharacters();

    protected:
        QList<IManipulatable *> & GetManipulatableList()
        {
            return manipulatableList;
        }

        void CameraManipulatableWindowRectChangedImpl(RectangleWH newWindowRect) override;
        void CharacterManipulatablePositionOverrideChangedImpl(Vector2 newPositionOverride) override;

    private:
        QPixmap backgroundPixmap;

        CameraManipulatable *pCameraManipulatable;

        FieldCutscene *pFieldCutscene;
        QList<IManipulatable *> manipulatableList;
        QMap<QString, Character::FieldInstance::ManipulationView *> characterManipulatableByStringMap;
        QList<IDrawable *> additionalDrawablesList;
    };

    Location::CutsceneView * GetCutsceneView()
    {
        return new Location::CutsceneView(this);
    }

private:
    QString id;
    QString backgroundFilePath;

    HitBox *pAreaHitBox;

    QMap<QString, InteractionLocation *> idToInteractionLocationMap;
    QMap<QString, ForegroundElement *> idToForegroundElementMap;
    QMap<QString, Character::FieldInstance *> idToCharacterInstanceMap;
    QMap<QString, FieldCutscene *> idToFieldCutsceneMap;
    QMap<QString, ZoomedView *> idToZoomedViewMap;
};

class LocationSelector : public QComboBox
{
    Q_OBJECT

public:
    explicit LocationSelector(QWidget *parent = 0);

    QString GetId();
    void SetToId(const QString &id);

signals:
    void LocationSelected(const QString &locationId);

public slots:
    void CurrentIndexChanged(int);

private:
    QStringList locationIds;
};

#endif // LOCATION_H
