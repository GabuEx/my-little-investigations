#ifndef CHARACTER_H
#define CHARACTER_H

#include <QList>
#include <QMap>
#include <QString>
#include <QStringList>

#include "Animation.h"
#include "Condition.h"
#include "HitBox.h"
#include "enums.h"

#include "CaseCreator/Utilities/Interfaces.h"
#include "CaseCreator/UIComponents/ManipulationViewBox.h"

#include "XmlStorableObject.h"

class ObjectManipulatorSlots;

namespace Staging
{
    class DialogCharacter;
    class FieldCharacter;
}

class Character : public XmlStorableObject
{
    BEGIN_XML_STORABLE_OBJECT(Character)
        XML_STORABLE_TEXT(id)
        XML_STORABLE_TEXT(name)
        XML_STORABLE_TEXT(defaultEmotionId)
        XML_STORABLE_CUSTOM_OBJECT(pHitBox, HitBox::CreateFromXml)
        XML_STORABLE_INT(anchorPosition)
        XML_STORABLE_CUSTOM_OBJECT(clickRect, RectangleWH::CreateFromXml)
        XML_STORABLE_CUSTOM_OBJECT_MAP(emotionByIdMap, DialogEmotion::CreateFromXml)
    END_XML_STORABLE_OBJECT()

public:
    Character();
    Character(QString id, QString name, QString defaultEmotionId);
    Character(Staging::DialogCharacter *pStagingDialogCharacter, Staging::FieldCharacter *pStagingFieldCharacter);
    virtual ~Character();

    static Character * CreateFromXml(XmlReader *pReader)
    {
        return new Character(pReader);
    }

    void SaveElementsToXml(XmlWriter *pWriter);
    void LoadElementsFromXml(XmlReader *pReader);

    static QString GetObjectAdditionString() { return QString("character"); }
    static QString GetListTitle() { return QString("Characters"); }
    static bool GetIsMainList() { return true; }
    static bool GetAllowsNewObjects() { return true; }

    QString GetId() const { return this->id; }

    QString GetName() const { return this->name; }
    void SetName(QString name) { this->name = name; }

    QString GetDisplayName() const { return this->name.length() > 0 ? this->name : this->id; }

    //// DIALOG CHARACTER PORTION ////
    QString GetDefaultEmotionId() const { return this->defaultEmotionId; }
    const QStringList& GetEmotionIds() { return this->emotionIdList; }

    class DialogEmotion : public XmlStorableObject
    {
        BEGIN_XML_STORABLE_OBJECT(DialogEmotion)
            XML_STORABLE_TEXT(id)
            XML_STORABLE_FILE_PATH(baseFilePath)
            XML_STORABLE_FILE_PATH_LIST(eyeFilePathList)
            XML_STORABLE_FILE_PATH_LIST(mouthFilePathList)
        END_XML_STORABLE_OBJECT()

    public:
        DialogEmotion()
        {
            lastMouthIndex = -1;
        }

        DialogEmotion(QString id)
            : DialogEmotion()
        {
            this->id = id;
        }

        virtual ~DialogEmotion() { }

        static DialogEmotion * CreateFromXml(XmlReader *pReader)
        {
            return new DialogEmotion(pReader);
        }

        static QString GetObjectAdditionString() { return QString("dialog sprite"); }
        static QString GetListTitle() { return QString("Dialog Sprites"); }
        static bool GetIsMainList() { return false; }
        static bool GetAllowsNewObjects() { return true; }

        QString GetId() const { return this->id; }
        void SetId(const QString &id) { this->id = id; }

        QString GetBaseFilePath();
        QString GetEyeFilePath(int eyeIndex);
        QStringList GetEyeFilePaths();
        QString GetMouthFilePath(int mouthIndex);
        QString GetRandomMouthFilePath();
        QStringList GetMouthFilePaths();

        int GetEyeDuration(int eyeIndex);
        int GetEyeFrameCount();
        int GetMouthFrameCount();

        void SetBaseFilePath(QString filePath);

        void AddEyeFrame(QString filePath);
        void SetEyeFrame(int index, QString filePath);
        void SetEyeFrames(QStringList filePaths);
        void InsertEyeFrame(int index, QString filePath);
        void RemoveEyeFrame(int index);

        void AddMouthFrame(QString filePath);
        void SetMouthFrame(int index, QString filePath);
        void SetMouthFrames(QStringList filePaths);
        void InsertMouthFrame(int index, QString filePath);
        void RemoveMouthFrame(int index);

        class DrawingView : public IDrawingView<Character::DialogEmotion>
        {
        public:
            DrawingView(Character::DialogEmotion *pEmotion, IDrawable *pParent);
            ~DrawingView();

            void DrawCore(QGraphicsScene *pScene, QList<QGraphicsItem *> &addedItems);
            void DrawChildren(QGraphicsScene *pScene);
            void UpdateCore() { }
            void UpdateChildren();
            void ResetChildren();

            void SetIsSpeaking(bool isSpeaking);

        private:
            class EyeDrawable : public IDrawable
            {
            public:
                EyeDrawable(Character::DialogEmotion *pEmotion, IDrawable *pParent);
                ~EyeDrawable() { }

                void DrawCore(QGraphicsScene *pScene, QList<QGraphicsItem *> &addedItems);
                void UpdateCore();
                void UpdateOnTimeout();

                qreal GetZOrder()
                {
                    return 1;
                }

            private:
                void UpdateTimeout();

                Character::DialogEmotion *pEmotion;

                int currentEyeIndex;
                QList<QGraphicsPixmapItem *> eyePixmapItems;
            };

            class MouthDrawable : public IDrawable
            {
            public:
                MouthDrawable(Character::DialogEmotion *pEmotion, IDrawable *pParent);
                ~MouthDrawable() { }

                void DrawCore(QGraphicsScene *pScene, QList<QGraphicsItem *> &addedItems);
                void UpdateCore();
                void UpdateOnTimeout();

                qreal GetZOrder()
                {
                    // The mouth should be on top of the eyes, so let's make sure of that
                    // by giving it a larger z-order.
                    return 2;
                }

                void SetIsSpeaking(bool isSpeaking);

            private:
                Character::DialogEmotion *pEmotion;

                int currentMouthIndex;
                QList<QGraphicsPixmapItem *> mouthPixmapItems;

                bool isSpeaking;
            };

            QString bodyBaseFilePath;
            QGraphicsPixmapItem *pBodyBasePixmapItem;
            EyeDrawable *pEyeDrawable;
            MouthDrawable *pMouthDrawable;
        };

        GET_DRAWING_VIEW_DEFINITION(Character::DialogEmotion)

    private:
        QString id;

        QString baseFilePath;
        QStringList eyeFilePathList;
        QStringList mouthFilePathList;

        int lastMouthIndex;
    };

    void AddDialogEmotion(QString emotionId);
    DialogEmotion * GetDialogEmotion(QString emotionId);
    void SetDialogEmotion(QString emotionId, DialogEmotion *pDialogEmotion);

    //// FIELD CHARACTER PORTION ////
    Animation * GetStandingAnimationForDirection(FieldCharacterDirection direction);
    Animation * GetWalkingAnimationForDirection(FieldCharacterDirection direction);
    Animation * GetRunningAnimationForDirection(FieldCharacterDirection direction);

    HitBox * GetHitBox() const { return pHitBox; }
    int GetAnchorPosition() const { return anchorPosition; }
    RectangleWH GetClickRect() const { return clickRect; }

    // This is a utility class used to expose the field animations in such a way that can be
    // enumerated in a list.
    class FieldSprite
    {
    public:
        FieldSprite() {}

        QString GetId() const { return this->id; }
        void SetId(const QString &id) { this->id = id; }

        static QString GetObjectAdditionString() { return QString("field sprite"); }
        static QString GetListTitle() { return QString("Field Sprites"); }
        static bool GetIsMainList() { return false; }
        static bool GetAllowsNewObjects() { return false; }

        Animation * GetAnimationForDirection(FieldCharacterDirection direction)
        {
            return ppAnimationsOnCharacter[direction] != NULL ? *ppAnimationsOnCharacter[direction] : NULL;
        }

        void SetAnimationForDirection(FieldCharacterDirection direction, Animation **ppAnimationOnCharacter)
        {
            ppAnimationsOnCharacter[direction] = ppAnimationOnCharacter;
        }

    private:
        QString id;
        Animation **ppAnimationsOnCharacter[FieldCharacterDirectionCount];
    };

    FieldSprite * GetStandingSprite() { return &standingSprite; }
    FieldSprite * GetWalkingSprite() { return &walkingSprite; }
    FieldSprite * GetRunningSprite() { return &runningSprite; }

    class FieldInstance : public IManipulatableObject, public XmlStorableObject
    {
        BEGIN_NAMED_XML_STORABLE_OBJECT(FieldInstance, CharacterFieldInstance)
            XML_STORABLE_CUSTOM_OBJECT(position, Vector2::CreateFromXml)
            XML_STORABLE_ENUM(direction, CharacterDirectionToString, StringToCharacterDirection)
            XML_STORABLE_ENUM(spriteDirection, FieldCharacterDirectionToString, StringToFieldCharacterDirection)
            XML_STORABLE_TEXT(clickEncounterId)
            XML_STORABLE_TEXT(clickCutsceneId)
            XML_STORABLE_CUSTOM_OBJECT(interactionLocation, Vector2::CreateFromXml)
            XML_STORABLE_BOOL(interactFromAnywhere)
            XML_STORABLE_CUSTOM_OBJECT(pCondition, Condition::CreateFromXml)
        END_XML_STORABLE_OBJECT()

    public:
        FieldInstance()
        {
            pCharacter = NULL;
            direction = CharacterDirectionNone;
            spriteDirection = FieldCharacterDirectionNone;
        }

        FieldInstance(Staging::FieldCharacter *pStagingFieldCharacter);
        virtual ~FieldInstance();

        static Character::FieldInstance * CreateFromXml(XmlReader *pReader)
        {
            return new Character::FieldInstance(pReader);
        }

        void WriteToCaseXml(XmlWriter *pWriter);

        QString GetId();

        QString GetName();
        void SetName(QString name);

        QString GetDisplayName();

        Vector2 GetPosition() const { return position; }
        void SetPosition(Vector2 position);

        Vector2 GetMidpoint() const;
        Vector2 GetMidpointForPosition(const Vector2 &position) const;

        CharacterDirection GetDirection() const { return direction; }
        FieldCharacterDirection GetSpriteDirection() const { return spriteDirection; }
        QString GetClickEncounterId() const { return clickEncounterId; }
        QString GetClickCutsceneId() const { return clickCutsceneId; }
        Vector2 GetInteractionLocation() const { return interactionLocation; }
        bool GetInteractFromAnywhere() const { return interactFromAnywhere; }
        Condition * GetCondition() const { return pCondition; }

        void SaveElementsToXml(XmlWriter *pWriter);
        void LoadElementsFromXml(XmlReader *pReader);

        virtual ObjectManipulatorSlots * GetManipulationWidget();

        class ManipulationView : public IManipulationView<Character::FieldInstance>
        {
        public:
            ManipulationView(Character::FieldInstance *pCharacterFieldInstance, IDrawable *pParent, UndoCommandSink *pUndoCommandSink);
            ~ManipulationView();

            void DrawCore(QGraphicsScene *pScene, QList<QGraphicsItem *> &addedItems);
            void DrawChildren(QGraphicsScene *pScene);
            void UpdateChildren();
            void ResetChildren();

            virtual QString GetMouseOverText()
            {
                return pObject->GetName();
            }

            Vector2 GetPosition()
            {
                if (positionIsOverridden)
                {
                    return positionOverride;
                }
                else
                {
                    return pObject->position;
                }
            }

            RectangleWH GetBoundingRect();
            qreal GetZOrder();

            qreal GetOpacity()
            {
                if (IsEnabled())
                {
                    return pParent->GetOpacity() * (pManipulationViewBox->GetIsBeingDragged() ? 0.5 : 1.0);
                }
                else
                {
                    return IManipulatable::GetOpacity();
                }
            }

            bool GetFlipHorizontal()
            {
                if (directionIsOverridden)
                {
                    return directionOverride == CharacterDirectionRight;
                }
                else
                {
                    return pObject->GetDirection() == CharacterDirectionRight;
                }
            }

            bool GetShouldDimIfDisabled()
            {
                return shouldDimIfDisabled;
            }

            void SetShouldDimIfDisabled(bool shouldDimIfDisabled);

            void SetPositionOverride(Vector2 position);
            void ClearPositionOverride();
            void SetDirectionOverride(CharacterDirection direction);
            void ClearDirectionOverride();
            void SetSpriteDirectionOverride(FieldCharacterDirection direction);
            void ClearSpriteDirectionOverride();

        protected:
            void OnGotFocus();
            void OnLostFocus();

            void OnMouseEnter();
            void OnMouseLeave();

            void OnMousePress();
            void OnMouseDrag(Vector2 delta);
            void OnMouseRelease();

        private:
            void SetPosition(Vector2 position);
            void SetIsMouseOver(bool isMouseOver);
            void SetIsSelected(bool isSelected);
            void SetIsBeingDragged(bool isBeingDragged);

            void UpdateAnimationDrawingView();

            Animation::DrawingView *pSpriteAnimationDrawingViews[FieldCharacterDirectionCount];
            ManipulationViewBox *pManipulationViewBox;

            bool shouldDimIfDisabled;

            Vector2 positionBeforeDrag;

            bool positionIsOverridden;
            bool directionIsOverridden;
            bool spriteDirectionIsOverridden;

            Vector2 positionOverride;
            CharacterDirection directionOverride;
            FieldCharacterDirection spriteDirectionOverride;
        };

        GET_MANIPULATION_VIEW_DEFINITION(Character::FieldInstance)

    private:
        Character *pCharacter;

        Vector2 position;
        CharacterDirection direction;
        FieldCharacterDirection spriteDirection;
        QString clickEncounterId;
        QString clickCutsceneId;
        Vector2 interactionLocation;
        bool interactFromAnywhere;
        Condition *pCondition;
    };

    class DialogDrawingView : public IDrawingView<Character>
    {
    public:
        DialogDrawingView(Character *pCharacter, IDrawable *pParent);
        ~DialogDrawingView();

        void DrawCore(QGraphicsScene * /*pScene */, QList<QGraphicsItem *> & /*addedItems*/) { }
        void DrawChildren(QGraphicsScene *pScene);
        void UpdateCore() { }
        void UpdateChildren();
        void ResetChildren();

        Vector2 GetPosition()
        {
            return position;
        }

        void SetPosition(Vector2 position)
        {
            this->position = position;
            Update();
        }

        bool GetFlipHorizontal()
        {
            return isFlipped;
        }

        void SetIsFlipped(bool isFlipped)
        {
            this->isFlipped = isFlipped;
            Update();
        }

        void SetEmotion(QString emotionId);
        void SetIsSpeaking(bool isSpeaking);

    private:
        Vector2 position;
        bool isFlipped;

        QString currentEmotionId;
        Character::DialogEmotion::DrawingView *pEmotionDrawingView;
    };

    Character::DialogDrawingView * GetDialogDrawingView(IDrawable *pParent)
    {
        return new Character::DialogDrawingView(this, pParent);
    }

private:
    QString id;
    QString name;

    //// DIALOG CHARACTER PORTION ////
    QString defaultEmotionId;
    QStringList emotionIdList;

    QMap<QString, DialogEmotion *> emotionByIdMap;

    //// FIELD CHARACTER PORTION ////
    Animation * standingAnimations[FieldCharacterDirectionCount];
    Animation * walkingAnimations[FieldCharacterDirectionCount];
    Animation * runningAnimations[FieldCharacterDirectionCount];

    FieldSprite standingSprite;
    FieldSprite walkingSprite;
    FieldSprite runningSprite;

    HitBox *pHitBox;
    int anchorPosition;
    RectangleWH clickRect;
};

class CharacterSelector : public QComboBox
{
    Q_OBJECT

public:
    explicit CharacterSelector(bool allowNoCharacter, QWidget *parent = 0);

    QString GetSelectedCharacterId();
    void SetToCharacterById(const QString &id);

signals:
    void CharacterSelected(const QString &characterDisplayName);

public slots:
    void CurrentIndexChanged(int);

private:
    QStringList characterIds;
    QStringList characterDisplayNames;
    bool allowNoCharacter;
};

#endif // CHARACTER_H
