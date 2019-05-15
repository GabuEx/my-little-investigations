#ifndef FIELDCUTSCENE_H
#define FIELDCUTSCENE_H

#include "XmlStorableObject.h"

#include "Animation.h"
#include "Character.h"
#include "Encounter.h"
#include "Staging/FieldCutscene.Staging.h"

#include "CaseCreator/UIComponents/BaseTypes/ObjectListWidget.h"

#include "enums.h"
#include "Vector2.h"

#include <memory>

class Location;

class FieldCutscene : public XmlStorableObject
{
public:
    class State
    {
    public:
        enum class ActiveElement
        {
            None,
            Character,
            Camera,
        };

        State() { }

        State(
            const QMap<QString, CharacterDirection> &characterIdToOriginalCharacterDirectionMap,
            const QMap<QString, FieldCharacterDirection> &characterIdToOriginalFieldCharacterDirectionMap,
            const QMap<QString, Vector2> &characterIdToOriginalPositionMap,
            const Vector2 &initialCameraPosition,
            const QString &initialBackgroundSpritePath,
            const QString &initialBgmId,
            const QString &initialAmbianceSfxId,
            const QMap<QString, Character::FieldInstance *> &characterIdToCharacterMap)
            : characterIdToCharacterDirectionMap(characterIdToOriginalCharacterDirectionMap)
            , characterIdToFieldCharacterDirectionMap(characterIdToOriginalFieldCharacterDirectionMap)
            , characterIdToPositionMap(characterIdToOriginalPositionMap)
            , cameraPosition(initialCameraPosition)
            , backgroundSpritePath(initialBackgroundSpritePath)
            , bgmId(initialBgmId)
            , ambianceSfxId(initialAmbianceSfxId)
            , characterIdToCharacterMap(characterIdToCharacterMap)
        {
        }

        State(const State &other)
            : characterIdToCharacterDirectionMap(other.characterIdToCharacterDirectionMap)
            , characterIdToFieldCharacterDirectionMap(other.characterIdToFieldCharacterDirectionMap)
            , characterIdToPositionMap(other.characterIdToPositionMap)
            , cameraPosition(other.cameraPosition)
            , backgroundSpritePath(other.backgroundSpritePath)
            , bgmId(other.bgmId)
            , ambianceSfxId(other.ambianceSfxId)
            , activeElement(other.activeElement)
            , activeCharacterId(other.activeCharacterId)
            , additionalDrawablesList(other.additionalDrawablesList)
            , characterIdToCharacterMap(other.characterIdToCharacterMap)
            , previousState(other.previousState ? unique_ptr<State>(new State(*other.previousState)) : unique_ptr<State>())
        {
        }

        State& operator=(const State& other)
        {
            if (this != &other)
            {
                characterIdToCharacterDirectionMap = other.characterIdToCharacterDirectionMap;
                characterIdToFieldCharacterDirectionMap = other.characterIdToFieldCharacterDirectionMap;
                characterIdToPositionMap = other.characterIdToPositionMap;
                cameraPosition = other.cameraPosition;
                backgroundSpritePath = other.backgroundSpritePath;
                bgmId = other.bgmId;
                ambianceSfxId = other.ambianceSfxId;
                activeElement = other.activeElement;
                activeCharacterId = other.activeCharacterId;
                additionalDrawablesList = other.additionalDrawablesList;
                characterIdToCharacterMap = other.characterIdToCharacterMap;
                previousState = other.previousState ? unique_ptr<State>(new State(*other.previousState)) : unique_ptr<State>();
            }

            return *this;
        }

        State Clone() const
        {
            return State(*this);
        }

        CharacterDirection GetCharacterDirection(const QString &characterId) const;
        FieldCharacterDirection GetFieldCharacterDirection(const QString &characterId) const;
        Vector2 GetCharacterPosition(const QString &characterId) const;
        Vector2 GetCharacterCameraCenterPosition(const QString &characterId) const;
        Vector2 GetCameraPosition() const { return this->cameraPosition; }
        QString GetBackgroundSpritePath() const { return this->backgroundSpritePath; }
        QString GetBgmId() const { return this->bgmId; }
        QString GetAmbianceSfxId() const { return this->ambianceSfxId; }
        ActiveElement GetActiveElement() const { return this->activeElement; }
        QString GetActiveCharacterId() const { return this->activeCharacterId; }
        QList<IDrawable *> GetAdditionalDrawablesList() const { return this->additionalDrawablesList; }

        Character::FieldInstance * GetCharacter(const QString &characterId) const;

        State & GetPreviousState() const { return *this->previousState; }

        void SetCharacterDirection(const QString &characterId, CharacterDirection direction);
        void SetFieldCharacterDirection(const QString &characterId, FieldCharacterDirection direction);
        void SetCharacterPosition(const QString &characterId, Vector2 position);
        void SetCameraPosition(Vector2 position) { this->cameraPosition = position; }
        void SetBackgroundSpritePath(const QString &backgroundSpritePath) { this->backgroundSpritePath = backgroundSpritePath; }
        void SetBgmId(const QString &bgmId) { this->bgmId = bgmId; }
        void SetAmbianceSfxId(const QString &ambianceSfxId) { this->ambianceSfxId = ambianceSfxId; }
        void SetActiveElement(ActiveElement activeElement) { this->activeElement = activeElement; }
        void SetActiveCharacterId(const QString &activeCharacterId);
        void AddAdditionalDrawable(IDrawable *pAdditionalDrawable) { this->additionalDrawablesList.append(pAdditionalDrawable); }

        void AdvanceToNextPhase()
        {
            previousState = unique_ptr<State>(new State(*this));
            previousState->previousState = NULL;

            additionalDrawablesList.clear();
        }

    private:
        QMap<QString, CharacterDirection> characterIdToCharacterDirectionMap;
        QMap<QString, FieldCharacterDirection> characterIdToFieldCharacterDirectionMap;
        QMap<QString, Vector2> characterIdToPositionMap;
        Vector2 cameraPosition;
        QString backgroundSpritePath;
        QString bgmId;
        QString ambianceSfxId;
        ActiveElement activeElement;
        QString activeCharacterId;
        QList<IDrawable *> additionalDrawablesList;

        QMap<QString, Character::FieldInstance *> characterIdToCharacterMap;

        unique_ptr<State> previousState;
    };

    enum class PhaseType
    {
        Conversation,
        Movement,
        Orient,
        SetBackground,
        CenterCamera,
        Pause,
        PlayBgm,
        StopBgm,
        PlayAmbiance,
        StopAmbiance,
        EndCase,
    };

    class FieldCutscenePhase : public XmlStorableObject, public ListItemObject<FieldCutscene::State, FieldCutscene::PhaseType>
    {
    public:
        static FieldCutscenePhase * CreateFromXml(XmlReader *pReader);
        static QList<FieldCutscene::FieldCutscenePhase *> CreateFromStaging(Staging::FieldCutscene::FieldCutscenePhase *pStagingFieldCutscenePhase);

        FieldCutscenePhase() { }

        virtual ~FieldCutscenePhase() {}

        virtual FieldCutscene::PhaseType GetType() = 0;
        virtual void UpdateCharacterPosition(Vector2 /*newCharacterPosition*/) { }
        virtual void UpdateCameraPosition(Vector2 /*newCameraPosition*/) { }
        virtual void InitializeForDrawable(IDrawable * /*pParentDrawable*/) { }

        FieldCutscenePhase * Clone();
        void UpdateAndCacheCutsceneState(FieldCutscene::State &currentState);

        void CopyProperties(ListItemObject<FieldCutscene::State, FieldCutscene::PhaseType> *pOther, bool isForEdit)
        {
            if (!IsSameType(pOther))
            {
                throw new MLIException("Can only copy properties between phases of the same type.");
            }

            CopyPropertiesImpl(dynamic_cast<FieldCutscene::FieldCutscenePhase *>(pOther), isForEdit);
        }

        ListItemBase<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType> * GetListItemBase() { return pListItemBase; }
        void SetListItemBase(ListItemBase<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType> *pListItemBase) { this->pListItemBase = pListItemBase; }

        QList<ListItemBase<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType> *> GetListItems(int indentLevel)
        {
            return GetListItemsImpl(indentLevel);
        }

        virtual void ExchangeListItemBaseOwnership(ListItemObject<FieldCutscene::State, FieldCutscene::PhaseType> *pOther)
        {
            ListItemBase<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType> *pListItemBase = GetListItemBase();

            if (pListItemBase != NULL)
            {
                FieldCutscene::FieldCutscenePhase *pPhase = dynamic_cast<FieldCutscene::FieldCutscenePhase *>(pOther);
                pListItemBase->SetObject(pPhase);
            }
        }

        FieldCutscene::State & GetStateBeforeObject()
        {
            return stateDuringPhase.GetPreviousState();
        }

        virtual FieldCutscene::State & GetStateDuringObject() override
        {
            return stateDuringPhase;
        }

        template <class TPhase>
        class PhaseListItem : public ListItem<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType>
        {
        public:
            explicit PhaseListItem(TPhase *pPhase, int indentLevel)
                : ListItem<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType>(indentLevel)
            {
                static_assert(is_base_of<FieldCutscene::FieldCutscenePhase, TPhase>::value, "FieldCutscene::FieldCutscenePhase::PhaseListItem must have a subclass of FieldCutscene::FieldCutscenePhase as its template type.");

                SetObjectImpl(pPhase);
            }

        protected:
            FieldCutscene::FieldCutscenePhase * GetObjectImpl() override { return pPhase; }

            void SetObjectImpl(FieldCutscene::FieldCutscenePhase *pPhase) override
            {
                this->pPhase = dynamic_cast<TPhase *>(pPhase);

                if (pPhase != NULL)
                {
                    pPhase->SetListItemBase(this);
                }
            }

            TPhase *pPhase;
        };

    protected:
        virtual void UpdateCutsceneState(FieldCutscene::State & /*currentState*/) { }
        virtual void CopyPropertiesImpl(FieldCutscene::FieldCutscenePhase *pOther, bool isForEdit) = 0;
        virtual QList<ListItemBase<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType> *> GetListItemsImpl(int indentLevel) = 0;

        int millisecondDelayBeforeBegin;
        FieldCutscene::State stateDuringPhase;

    private:
        ListItemBase<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType> *pListItemBase;
    };

    class FieldCutsceneConversation : public FieldCutscenePhase
    {
        BEGIN_XML_STORABLE_OBJECT(FieldCutsceneConversation)
            XML_STORABLE_INT(millisecondDelayBeforeBegin)
        END_XML_STORABLE_OBJECT()

    public:
        FieldCutsceneConversation()
        {
            pEncounter = make_shared<Encounter>();
        }

        FieldCutsceneConversation(Staging::FieldCutscene::FieldCutsceneConversation *pStagingFieldCutsceneConversation);

        ~FieldCutsceneConversation();

        void SaveElementsToXml(XmlWriter *pWriter);
        void LoadElementsFromXml(XmlReader *pReader);

        FieldCutscene::PhaseType GetType() override { return FieldCutscene::PhaseType::Conversation; }

        static EditorDialogContents<FieldCutscene::FieldCutscenePhase, FieldCutscene::State> * CreateEditorDialogContents(
                const FieldCutscene::State &stateBeforeObject,
                FieldCutscene::FieldCutscenePhase *pPhaseToEdit);

    protected:
        void CopyPropertiesImpl(FieldCutscene::FieldCutscenePhase *pOther, bool isForEdit) override;
        QList<ListItemBase<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType> *> GetListItemsImpl(int indentLevel) override;

    private:
        shared_ptr<Encounter> pEncounter;
    };

    class FieldCutsceneMovement : public FieldCutscenePhase
    {
        friend class FieldCutsceneMovementListItem;
        friend class FieldCutsceneMovementEditorDialogContents;

        class DirectionArrow : public IDrawable
        {
        public:
            DirectionArrow(IDrawable *pParent)
                : IDrawable(pParent)
                , pOriginOuterEllipse(NULL)
                , pPathOuterLine(NULL)
                , pArrowTopOuterLine(NULL)
                , pArrowBottomOuterLine(NULL)
                , pOriginInnerEllipse(NULL)
                , pPathInnerLine(NULL)
                , pArrowTopInnerLine(NULL)
                , pArrowBottomInnerLine(NULL)
            {
            }

            DirectionArrow(IDrawable *pParent, const Vector2 &startPosition, const Vector2 &endPosition)
                : DirectionArrow(pParent)
            {
                this->startPosition = startPosition;
                this->endPosition = endPosition;
            }

            void SetPositions(const Vector2 &startPosition, const Vector2 &endPosition);

        protected:
            void DrawCore(QGraphicsScene *pScene, QList<QGraphicsItem *> &addedItems) override;
            void UpdateCore() override;

        private:
            Vector2 startPosition;
            Vector2 endPosition;

            QGraphicsEllipseItem *pOriginOuterEllipse;
            QGraphicsLineItem *pPathOuterLine;
            QGraphicsLineItem *pArrowTopOuterLine;
            QGraphicsLineItem *pArrowBottomOuterLine;
            QGraphicsEllipseItem *pOriginInnerEllipse;
            QGraphicsLineItem *pPathInnerLine;
            QGraphicsLineItem *pArrowTopInnerLine;
            QGraphicsLineItem *pArrowBottomInnerLine;
            QGraphicsItemGroup *pOuterArrowGroup;
            QGraphicsItemGroup *pInnerArrowGroup;
            QGraphicsItemGroup *pArrowGroup;
        };

        BEGIN_XML_STORABLE_OBJECT(FieldCutsceneMovement)
            XML_STORABLE_INT(millisecondDelayBeforeBegin)
            XML_STORABLE_TEXT(characterId)
            XML_STORABLE_CUSTOM_OBJECT(targetPosition, Vector2::CreateFromXml)
            XML_STORABLE_ENUM(movementState, FieldCharacterStateToString, StringToFieldCharacterState)
        END_XML_STORABLE_OBJECT()

    public:
        FieldCutsceneMovement()
        {
            pGhostedCharacterDrawingView = NULL;
            pDirectionArrow = NULL;
        }

        FieldCutsceneMovement(Staging::FieldCutscene::FieldCutsceneMovement *pStagingFieldCutsceneMovement);
        virtual ~FieldCutsceneMovement();

        FieldCutscene::PhaseType GetType() override { return FieldCutscene::PhaseType::Movement; }
        void UpdateCharacterPosition(Vector2 newCharacterPosition) override;
        void InitializeForDrawable(IDrawable *pParentDrawable) override;

        static EditorDialogContents<FieldCutscene::FieldCutscenePhase, FieldCutscene::State> * CreateEditorDialogContents(
                const FieldCutscene::State &stateBeforeObject,
                FieldCutscene::FieldCutscenePhase *pPhaseToEdit);

    protected:
        void UpdateCutsceneState(FieldCutscene::State &currentState) override;
        void CopyPropertiesImpl(FieldCutscene::FieldCutscenePhase *pOther, bool isForEdit) override;
        QList<ListItemBase<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType> *> GetListItemsImpl(int indentLevel) override;

    private:
        QString characterId;
        Vector2 targetPosition;
        FieldCharacterState movementState;

        Animation::DrawingView *pGhostedCharacterDrawingView;
        DirectionArrow *pDirectionArrow;
    };

    class FieldCutsceneOrient : public FieldCutscenePhase
    {
        friend class FieldCutsceneOrientListItem;
        friend class FieldCutsceneOrientEditorDialogContents;

        BEGIN_XML_STORABLE_OBJECT(FieldCutsceneOrient)
            XML_STORABLE_INT(millisecondDelayBeforeBegin)
            XML_STORABLE_TEXT(characterId)
            XML_STORABLE_ENUM(direction, CharacterDirectionToString, StringToCharacterDirection)
            XML_STORABLE_ENUM(spriteDirection, FieldCharacterDirectionToString, StringToFieldCharacterDirection)
        END_XML_STORABLE_OBJECT()

    public:
        FieldCutsceneOrient() { }
        FieldCutsceneOrient(Staging::FieldCutscene::FieldCutsceneOrient *pStagingFieldCutsceneOrient);

        FieldCutscene::PhaseType GetType() override { return FieldCutscene::PhaseType::Orient; }

        static EditorDialogContents<FieldCutscene::FieldCutscenePhase, FieldCutscene::State> * CreateEditorDialogContents(
                const FieldCutscene::State &stateBeforeObject,
                FieldCutscene::FieldCutscenePhase *pPhaseToEdit);

    protected:
        void UpdateCutsceneState(FieldCutscene::State &currentState) override;
        void CopyPropertiesImpl(FieldCutscene::FieldCutscenePhase *pOther, bool isForEdit) override;
        QList<ListItemBase<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType> *> GetListItemsImpl(int indentLevel) override;

    private:
        QString characterId;
        CharacterDirection direction;
        FieldCharacterDirection spriteDirection;
    };

    class FieldCutsceneSetBackground : public FieldCutscenePhase
    {
        friend class FieldCutsceneSetBackgroundListItem;
        friend class FieldCutsceneSetBackgroundEditorDialogContents;

        BEGIN_XML_STORABLE_OBJECT(FieldCutsceneSetBackground)
            XML_STORABLE_FILE_PATH(backgroundSpriteFilePath)
            XML_STORABLE_INT(msFadeDuration)
        END_XML_STORABLE_OBJECT()

    public:
        FieldCutsceneSetBackground() { }
        FieldCutsceneSetBackground(Staging::FieldCutscene::FieldCutsceneSetBackground *pStagingFieldCutsceneSetBackground);

        FieldCutscene::PhaseType GetType() override { return FieldCutscene::PhaseType::SetBackground; }

        static EditorDialogContents<FieldCutscene::FieldCutscenePhase, FieldCutscene::State> * CreateEditorDialogContents(
                const FieldCutscene::State &stateBeforeObject,
                FieldCutscene::FieldCutscenePhase *pPhaseToEdit);

    protected:
        void UpdateCutsceneState(FieldCutscene::State &currentState) override;
        void CopyPropertiesImpl(FieldCutscene::FieldCutscenePhase *pOther, bool isForEdit) override;
        QList<ListItemBase<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType> *> GetListItemsImpl(int indentLevel) override;

    private:
        QString backgroundSpriteFilePath;
        int msFadeDuration;
    };

    class FieldCutsceneCenterCamera : public FieldCutscenePhase
    {
        friend class FieldCutsceneCenterCameraListItem;
        friend class FieldCutsceneCenterCameraEditorDialogContents;

        BEGIN_XML_STORABLE_OBJECT(FieldCutsceneCenterCamera)
            XML_STORABLE_INT(millisecondDelayBeforeBegin)
            XML_STORABLE_TEXT(characterId)
            XML_STORABLE_CUSTOM_OBJECT(cameraCenterPosition, Vector2::CreateFromXml)
        END_XML_STORABLE_OBJECT()

    public:
        FieldCutsceneCenterCamera() { }
        FieldCutsceneCenterCamera(Staging::FieldCutscene::FieldCutsceneCenterCamera *pStagingFieldCutsceneCenterCamera);

        FieldCutscene::PhaseType GetType() override { return FieldCutscene::PhaseType::CenterCamera; }
        void UpdateCameraPosition(Vector2 newCameraPosition) override;

        static EditorDialogContents<FieldCutscene::FieldCutscenePhase, FieldCutscene::State> * CreateEditorDialogContents(
                const FieldCutscene::State &stateBeforeObject,
                FieldCutscene::FieldCutscenePhase *pPhaseToEdit);

    protected:
        void UpdateCutsceneState(FieldCutscene::State &currentState) override;
        void CopyPropertiesImpl(FieldCutscene::FieldCutscenePhase *pOther, bool isForEdit) override;
        QList<ListItemBase<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType> *> GetListItemsImpl(int indentLevel) override;

    private:
        QString characterId;
        Vector2 cameraCenterPosition;
    };

    class FieldCutscenePause : public FieldCutscenePhase
    {
        friend class FieldCutscenePauseListItem;
        friend class FieldCutscenePauseDialogContents;

        BEGIN_XML_STORABLE_OBJECT(FieldCutscenePause)
            XML_STORABLE_INT(msPauseDuration)
        END_XML_STORABLE_OBJECT()

    public:
        FieldCutscenePause() { }
        FieldCutscenePause(Staging::FieldCutscene::FieldCutscenePause *pStagingFieldCutscenePause);

        FieldCutscene::PhaseType GetType() override { return FieldCutscene::PhaseType::Pause; }

        static EditorDialogContents<FieldCutscene::FieldCutscenePhase, FieldCutscene::State> * CreateEditorDialogContents(
                const FieldCutscene::State &stateBeforeObject,
                FieldCutscene::FieldCutscenePhase *pPhaseToEdit);

    protected:
        void CopyPropertiesImpl(FieldCutscene::FieldCutscenePhase *pOther, bool isForEdit) override;
        QList<ListItemBase<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType> *> GetListItemsImpl(int indentLevel) override;

    private:
        int msPauseDuration;
    };

    class FieldCutscenePlayBgm : public FieldCutscenePhase
    {
        friend class FieldCutscenePlayBgmListItem;
        friend class FieldCutscenePlayBgmEditorDialogContents;

        BEGIN_XML_STORABLE_OBJECT(FieldCutscenePlayBgm)
            XML_STORABLE_TEXT(bgmId)
        END_XML_STORABLE_OBJECT()

    public:
        FieldCutscenePlayBgm() { }
        FieldCutscenePlayBgm(Staging::FieldCutscene::FieldCutscenePlayBgm *pFieldCutscenePlayBgm);

        FieldCutscene::PhaseType GetType() override { return FieldCutscene::PhaseType::PlayBgm; }

        static EditorDialogContents<FieldCutscene::FieldCutscenePhase, FieldCutscene::State> * CreateEditorDialogContents(
                const FieldCutscene::State &stateBeforeObject,
                FieldCutscene::FieldCutscenePhase *pPhaseToEdit);

    protected:
        void UpdateCutsceneState(FieldCutscene::State &currentState) override;
        void CopyPropertiesImpl(FieldCutscene::FieldCutscenePhase *pOther, bool isForEdit) override;
        QList<ListItemBase<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType> *> GetListItemsImpl(int indentLevel) override;

    private:
        QString bgmId;
    };

    class FieldCutsceneStopBgm : public FieldCutscenePhase
    {
        friend class FieldCutsceneStopBgmListItem;
        friend class FieldCutsceneStopBgmEditorDialogContents;

        BEGIN_XML_STORABLE_OBJECT(FieldCutsceneStopBgm)
            XML_STORABLE_BOOL(isInstant)
        END_XML_STORABLE_OBJECT()

    public:
        FieldCutsceneStopBgm() { }
        FieldCutsceneStopBgm(Staging::FieldCutscene::FieldCutsceneStopBgm *pFieldCutsceneStopBgm);

        FieldCutscene::PhaseType GetType() override { return FieldCutscene::PhaseType::StopBgm; }

        static EditorDialogContents<FieldCutscene::FieldCutscenePhase, FieldCutscene::State> * CreateEditorDialogContents(
                const FieldCutscene::State &stateBeforeObject,
                FieldCutscene::FieldCutscenePhase *pPhaseToEdit);

    protected:
        void UpdateCutsceneState(FieldCutscene::State &currentState) override;
        void CopyPropertiesImpl(FieldCutscene::FieldCutscenePhase *pOther, bool isForEdit) override;
        QList<ListItemBase<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType> *> GetListItemsImpl(int indentLevel) override;

    private:
        bool isInstant;
    };

    class FieldCutscenePlayAmbiance : public FieldCutscenePhase
    {
        friend class FieldCutscenePlayAmbianceListItem;
        friend class FieldCutscenePlayAmbianceEditorDialogContents;

        BEGIN_XML_STORABLE_OBJECT(FieldCutscenePlayAmbiance)
            XML_STORABLE_TEXT(ambianceSfxId)
        END_XML_STORABLE_OBJECT()

    public:
        FieldCutscenePlayAmbiance() { }
        FieldCutscenePlayAmbiance(Staging::FieldCutscene::FieldCutscenePlayAmbiance *pFieldCutscenePlayAmbiance);

        FieldCutscene::PhaseType GetType() override { return FieldCutscene::PhaseType::PlayAmbiance; }

        static EditorDialogContents<FieldCutscene::FieldCutscenePhase, FieldCutscene::State> * CreateEditorDialogContents(
                const FieldCutscene::State &stateBeforeObject,
                FieldCutscene::FieldCutscenePhase *pPhaseToEdit);

    protected:
        void UpdateCutsceneState(FieldCutscene::State &currentState) override;
        void CopyPropertiesImpl(FieldCutscene::FieldCutscenePhase *pOther, bool isForEdit) override;
        QList<ListItemBase<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType> *> GetListItemsImpl(int indentLevel) override;

    private:
        QString ambianceSfxId;
    };

    class FieldCutsceneStopAmbiance : public FieldCutscenePhase
    {
        friend class FieldCutsceneStopAmbianceListItem;
        friend class FieldCutsceneStopAmbianceEditorDialogContents;

        BEGIN_XML_STORABLE_OBJECT(FieldCutsceneStopAmbiance)
            XML_STORABLE_BOOL(isInstant)
        END_XML_STORABLE_OBJECT()

    public:
        FieldCutsceneStopAmbiance() { }
        FieldCutsceneStopAmbiance(Staging::FieldCutscene::FieldCutsceneStopAmbiance *pFieldCutsceneStopAmbiance);

        FieldCutscene::PhaseType GetType() override { return FieldCutscene::PhaseType::StopAmbiance; }

        static EditorDialogContents<FieldCutscene::FieldCutscenePhase, FieldCutscene::State> * CreateEditorDialogContents(
                const FieldCutscene::State &stateBeforeObject,
                FieldCutscene::FieldCutscenePhase *pPhaseToEdit);

    protected:
        void UpdateCutsceneState(FieldCutscene::State &currentState) override;
        void CopyPropertiesImpl(FieldCutscene::FieldCutscenePhase *pOther, bool isForEdit) override;
        QList<ListItemBase<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType> *> GetListItemsImpl(int indentLevel) override;

    private:
        bool isInstant;
    };

    class FieldCutsceneEndCase : public FieldCutscenePhase
    {
        friend class FieldCutsceneEndCaseListItem;
        friend class FieldCutsceneEndCaseEditorDialogContents;

        BEGIN_XML_STORABLE_OBJECT(FieldCutsceneEndCase)
            XML_STORABLE_BOOL(completesCase)
        END_XML_STORABLE_OBJECT()

    public:
        FieldCutsceneEndCase() { }
        FieldCutsceneEndCase(Staging::FieldCutscene::FieldCutsceneEndCase *pFieldCutsceneEndCase);

        FieldCutscene::PhaseType GetType() override { return FieldCutscene::PhaseType::EndCase; }

        static EditorDialogContents<FieldCutscene::FieldCutscenePhase, FieldCutscene::State> * CreateEditorDialogContents(
                const FieldCutscene::State &stateBeforeObject,
                FieldCutscene::FieldCutscenePhase *pPhaseToEdit);

    protected:
        void CopyPropertiesImpl(FieldCutscene::FieldCutscenePhase *pOther, bool isForEdit) override;
        QList<ListItemBase<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType> *> GetListItemsImpl(int indentLevel) override;

    private:
        bool completesCase;
    };

    BEGIN_XML_STORABLE_OBJECT(FieldCutscene)
        XML_STORABLE_TEXT(id)
        XML_STORABLE_CUSTOM_OBJECT_LIST(phaseList, FieldCutscenePhase::CreateFromXml)
        XML_STORABLE_CUSTOM_OBJECT_MAP(idToCharacterMap, Character::FieldInstance::CreateFromXml)
        XML_STORABLE_ENUM_MAP(characterIdToOriginalCharacterDirectionMap, CharacterDirectionToString, StringToCharacterDirection)
        XML_STORABLE_ENUM_MAP(characterIdToOriginalFieldCharacterDirectionMap, FieldCharacterDirectionToString, StringToFieldCharacterDirection)
        XML_STORABLE_CUSTOM_OBJECT_MAP(characterIdToOriginalPositionMap, Vector2::CreateFromXml)
        XML_STORABLE_CUSTOM_OBJECT(initialCameraPosition, Vector2::CreateFromXml)
        XML_STORABLE_BOOL(isEnabled)
        XML_STORABLE_BOOL(preventsInitialBgm)
        XML_STORABLE_TEXT(initialBgmReplacement)
        XML_STORABLE_BOOL(preventsInitialAmbiance)
        XML_STORABLE_TEXT(initialAmbianceReplacement)
    END_XML_STORABLE_OBJECT()

public:
    FieldCutscene()
    {
        pOwningLocation = NULL;
    }

    FieldCutscene(const QString &id);
    FieldCutscene(Staging::FieldCutscene *pStagingFieldCutscene);

    virtual ~FieldCutscene();

    static FieldCutscene * CreateFromXml(XmlReader *pReader)
    {
        return new FieldCutscene(pReader);
    }

    void LoadElementsFromXml(XmlReader *pReader) override;

    QString GetId() { return id; }
    void SetIdFromIndex(int indexInScene);

    QString GetDisplayName() { return id; }

    static QString GetObjectAdditionString() { return QString("cutscene"); }
    static QString GetListTitle() { return QString("Cutscenes"); }
    static bool GetIsMainList() { return false; }
    static bool GetAllowsNewObjects() { return true; }

    Location * GetOwningLocation() { return pOwningLocation; }
    void SetOwningLocation(Location *pOwningLocation) { this->pOwningLocation = pOwningLocation; }

    QMap<QString, Character::FieldInstance *> & GetIdToCharacterMap() { return idToCharacterMap; }
    Vector2 & GetInitialCameraPosition() { return initialCameraPosition; }

    void InitializeForDrawable(IDrawable *pParentDrawable);
    void UpdateAndCacheCutsceneStates();
    void ReplacePhase(FieldCutscenePhase *pNewPhase, FieldCutscenePhase *pOldPhase);

    ObjectListWidget<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType> * GetObjectListWidget();

private:
    QString id;

    QList<FieldCutscenePhase *> phaseList;
    QMap<QString, Character::FieldInstance *> idToCharacterMap;

    QMap<QString, CharacterDirection> characterIdToOriginalCharacterDirectionMap;
    QMap<QString, FieldCharacterDirection> characterIdToOriginalFieldCharacterDirectionMap;
    QMap<QString, Vector2> characterIdToOriginalPositionMap;

    Vector2 initialCameraPosition;

    bool isEnabled;
    bool preventsInitialBgm;
    QString initialBgmReplacement;
    bool preventsInitialAmbiance;
    QString initialAmbianceReplacement;

    Location *pOwningLocation;
};

typedef ObjectListWidget<FieldCutscene::FieldCutscenePhase, FieldCutscene::State, FieldCutscene::PhaseType> FieldCutscenePhaseListWidget;

class FieldCutsceneConversationListItem : public FieldCutscene::FieldCutscenePhase::PhaseListItem<FieldCutscene::FieldCutsceneConversation>
{
    Q_OBJECT

public:
    explicit FieldCutsceneConversationListItem(FieldCutscene::FieldCutsceneConversation *pPhase, int indentLevel)
        : FieldCutscene::FieldCutscenePhase::PhaseListItem<FieldCutscene::FieldCutsceneConversation>(pPhase, indentLevel)
    {
    }

    QString GetDisplayString() override;
};

class FieldCutsceneConversationEditorDialogContents : public EditorDialogContents<FieldCutscene::FieldCutscenePhase, FieldCutscene::State>
{
    Q_OBJECT

public:
    FieldCutsceneConversationEditorDialogContents(const FieldCutscene::State &stateBeforeAction, FieldCutscene::FieldCutsceneConversation *pPhaseToEdit = NULL);

public slots:
    bool ValidateFields() override;

private:
    FieldCutscene::FieldCutsceneConversation *pFieldCutsceneConversation;
};

class FieldCutsceneMovementListItem : public FieldCutscene::FieldCutscenePhase::PhaseListItem<FieldCutscene::FieldCutsceneMovement>
{
    Q_OBJECT

public:
    explicit FieldCutsceneMovementListItem(FieldCutscene::FieldCutsceneMovement *pPhase, int indentLevel)
        : FieldCutscene::FieldCutscenePhase::PhaseListItem<FieldCutscene::FieldCutsceneMovement>(pPhase, indentLevel)
    {
    }

    QString GetDisplayString() override;
};

class FieldCutsceneMovementEditorDialogContents : public EditorDialogContents<FieldCutscene::FieldCutscenePhase, FieldCutscene::State>
{
    Q_OBJECT

public:
    FieldCutsceneMovementEditorDialogContents(const FieldCutscene::State &stateBeforeAction, FieldCutscene::FieldCutsceneMovement *pPhaseToEdit = NULL);

public slots:
    bool ValidateFields() override;

private:
    FieldCutscene::FieldCutsceneMovement *pFieldCutsceneMovement;
};

class FieldCutsceneOrientListItem : public FieldCutscene::FieldCutscenePhase::PhaseListItem<FieldCutscene::FieldCutsceneOrient>
{
    Q_OBJECT

public:
    explicit FieldCutsceneOrientListItem(FieldCutscene::FieldCutsceneOrient *pPhase, int indentLevel)
        : FieldCutscene::FieldCutscenePhase::PhaseListItem<FieldCutscene::FieldCutsceneOrient>(pPhase, indentLevel)
    {
    }

    QString GetDisplayString() override;
};

class FieldCutsceneOrientEditorDialogContents : public EditorDialogContents<FieldCutscene::FieldCutscenePhase, FieldCutscene::State>
{
    Q_OBJECT

public:
    FieldCutsceneOrientEditorDialogContents(const FieldCutscene::State &stateBeforeAction, FieldCutscene::FieldCutsceneOrient *pPhaseToEdit = NULL);

public slots:
    bool ValidateFields() override;

private:
    FieldCutscene::FieldCutsceneOrient *pFieldCutsceneOrient;
};

class FieldCutsceneSetBackgroundListItem : public FieldCutscene::FieldCutscenePhase::PhaseListItem<FieldCutscene::FieldCutsceneSetBackground>
{
    Q_OBJECT

public:
    explicit FieldCutsceneSetBackgroundListItem(FieldCutscene::FieldCutsceneSetBackground *pPhase, int indentLevel)
        : FieldCutscene::FieldCutscenePhase::PhaseListItem<FieldCutscene::FieldCutsceneSetBackground>(pPhase, indentLevel)
    {
    }

    QString GetDisplayString() override;
};

class FieldCutsceneSetBackgroundEditorDialogContents : public EditorDialogContents<FieldCutscene::FieldCutscenePhase, FieldCutscene::State>
{
    Q_OBJECT

public:
    FieldCutsceneSetBackgroundEditorDialogContents(const FieldCutscene::State &stateBeforeAction, FieldCutscene::FieldCutsceneSetBackground *pPhaseToEdit = NULL);

public slots:
    bool ValidateFields() override;

private:
    FieldCutscene::FieldCutsceneSetBackground *pFieldCutsceneSetBackground;
};

class FieldCutsceneCenterCameraListItem : public FieldCutscene::FieldCutscenePhase::PhaseListItem<FieldCutscene::FieldCutsceneCenterCamera>
{
    Q_OBJECT

public:
    explicit FieldCutsceneCenterCameraListItem(FieldCutscene::FieldCutsceneCenterCamera *pPhase, int indentLevel)
        : FieldCutscene::FieldCutscenePhase::PhaseListItem<FieldCutscene::FieldCutsceneCenterCamera>(pPhase, indentLevel)
    {
    }

    QString GetDisplayString() override;
};

class FieldCutsceneCenterCameraEditorDialogContents : public EditorDialogContents<FieldCutscene::FieldCutscenePhase, FieldCutscene::State>
{
    Q_OBJECT

public:
    FieldCutsceneCenterCameraEditorDialogContents(const FieldCutscene::State &stateBeforeAction, FieldCutscene::FieldCutsceneCenterCamera *pPhaseToEdit = NULL);

public slots:
    bool ValidateFields() override;

private:
    FieldCutscene::FieldCutsceneCenterCamera *pFieldCutsceneCenterCamera;
};

class FieldCutscenePauseListItem : public FieldCutscene::FieldCutscenePhase::PhaseListItem<FieldCutscene::FieldCutscenePause>
{
    Q_OBJECT

public:
    explicit FieldCutscenePauseListItem(FieldCutscene::FieldCutscenePause *pPhase, int indentLevel)
        : FieldCutscene::FieldCutscenePhase::PhaseListItem<FieldCutscene::FieldCutscenePause>(pPhase, indentLevel)
    {
    }

    QString GetDisplayString() override;
};

class FieldCutscenePauseEditorDialogContents : public EditorDialogContents<FieldCutscene::FieldCutscenePhase, FieldCutscene::State>
{
    Q_OBJECT

public:
    FieldCutscenePauseEditorDialogContents(const FieldCutscene::State &stateBeforeAction, FieldCutscene::FieldCutscenePause *pPhaseToEdit = NULL);

public slots:
    bool ValidateFields() override;

private:
    FieldCutscene::FieldCutscenePause *pFieldCutscenePause;
};

class FieldCutscenePlayBgmListItem : public FieldCutscene::FieldCutscenePhase::PhaseListItem<FieldCutscene::FieldCutscenePlayBgm>
{
    Q_OBJECT

public:
    explicit FieldCutscenePlayBgmListItem(FieldCutscene::FieldCutscenePlayBgm *pPhase, int indentLevel)
        : FieldCutscene::FieldCutscenePhase::PhaseListItem<FieldCutscene::FieldCutscenePlayBgm>(pPhase, indentLevel)
    {
    }

    QString GetDisplayString() override;
};

class FieldCutscenePlayBgmEditorDialogContents : public EditorDialogContents<FieldCutscene::FieldCutscenePhase, FieldCutscene::State>
{
    Q_OBJECT

public:
    FieldCutscenePlayBgmEditorDialogContents(const FieldCutscene::State &stateBeforeAction, FieldCutscene::FieldCutscenePlayBgm *pPhaseToEdit = NULL);

public slots:
    bool ValidateFields() override;

private:
    FieldCutscene::FieldCutscenePlayBgm *pFieldCutscenePlayBgm;
};

class FieldCutsceneStopBgmListItem : public FieldCutscene::FieldCutscenePhase::PhaseListItem<FieldCutscene::FieldCutsceneStopBgm>
{
    Q_OBJECT

public:
    explicit FieldCutsceneStopBgmListItem(FieldCutscene::FieldCutsceneStopBgm *pPhase, int indentLevel)
        : FieldCutscene::FieldCutscenePhase::PhaseListItem<FieldCutscene::FieldCutsceneStopBgm>(pPhase, indentLevel)
    {
    }

    QString GetDisplayString() override;
};

class FieldCutsceneStopBgmEditorDialogContents : public EditorDialogContents<FieldCutscene::FieldCutscenePhase, FieldCutscene::State>
{
    Q_OBJECT

public:
    FieldCutsceneStopBgmEditorDialogContents(const FieldCutscene::State &stateBeforeAction, FieldCutscene::FieldCutsceneStopBgm *pPhaseToEdit = NULL);

public slots:
    bool ValidateFields() override;

private:
    FieldCutscene::FieldCutsceneStopBgm *pFieldCutsceneStopBgm;
};

class FieldCutscenePlayAmbianceListItem : public FieldCutscene::FieldCutscenePhase::PhaseListItem<FieldCutscene::FieldCutscenePlayAmbiance>
{
    Q_OBJECT

public:
    explicit FieldCutscenePlayAmbianceListItem(FieldCutscene::FieldCutscenePlayAmbiance *pPhase, int indentLevel)
        : FieldCutscene::FieldCutscenePhase::PhaseListItem<FieldCutscene::FieldCutscenePlayAmbiance>(pPhase, indentLevel)
    {
    }

    QString GetDisplayString() override;
};

class FieldCutscenePlayAmbianceEditorDialogContents : public EditorDialogContents<FieldCutscene::FieldCutscenePhase, FieldCutscene::State>
{
    Q_OBJECT

public:
    FieldCutscenePlayAmbianceEditorDialogContents(const FieldCutscene::State &stateBeforeAction, FieldCutscene::FieldCutscenePlayAmbiance *pPhaseToEdit = NULL);

public slots:
    bool ValidateFields() override;

private:
    FieldCutscene::FieldCutscenePlayAmbiance *pFieldCutscenePlayAmbiance;
};

class FieldCutsceneStopAmbianceListItem : public FieldCutscene::FieldCutscenePhase::PhaseListItem<FieldCutscene::FieldCutsceneStopAmbiance>
{
    Q_OBJECT

public:
    explicit FieldCutsceneStopAmbianceListItem(FieldCutscene::FieldCutsceneStopAmbiance *pPhase, int indentLevel)
        : FieldCutscene::FieldCutscenePhase::PhaseListItem<FieldCutscene::FieldCutsceneStopAmbiance>(pPhase, indentLevel)
    {
    }

    QString GetDisplayString() override;
};

class FieldCutsceneStopAmbianceEditorDialogContents : public EditorDialogContents<FieldCutscene::FieldCutscenePhase, FieldCutscene::State>
{
    Q_OBJECT

public:
    FieldCutsceneStopAmbianceEditorDialogContents(const FieldCutscene::State &stateBeforeAction, FieldCutscene::FieldCutsceneStopAmbiance *pPhaseToEdit = NULL);

public slots:
    bool ValidateFields() override;

private:
    FieldCutscene::FieldCutsceneStopAmbiance *pFieldCutsceneStopAmbiance;
};

class FieldCutsceneEndCaseListItem : public FieldCutscene::FieldCutscenePhase::PhaseListItem<FieldCutscene::FieldCutsceneEndCase>
{
    Q_OBJECT

public:
    explicit FieldCutsceneEndCaseListItem(FieldCutscene::FieldCutsceneEndCase *pPhase, int indentLevel)
        : FieldCutscene::FieldCutscenePhase::PhaseListItem<FieldCutscene::FieldCutsceneEndCase>(pPhase, indentLevel)
    {
    }

    QString GetDisplayString() override;
};

class FieldCutsceneEndCaseEditorDialogContents : public EditorDialogContents<FieldCutscene::FieldCutscenePhase, FieldCutscene::State>
{
    Q_OBJECT

public:
    FieldCutsceneEndCaseEditorDialogContents(const FieldCutscene::State &stateBeforeAction, FieldCutscene::FieldCutsceneEndCase *pPhaseToEdit = NULL);

public slots:
    bool ValidateFields() override;

private:
    FieldCutscene::FieldCutsceneEndCase *pFieldCutsceneEndCase;
};

#endif // FIELDCUTSCENE_H
