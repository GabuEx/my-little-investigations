#ifndef CONVERSATION_H
#define CONVERSATION_H

#include <QString>
#include <QList>
#include <QStringList>
#include <QMap>
#include <QStack>

#include "Character.h"
#include "Condition.h"
#include "CaseCreator/Utilities/Interfaces.h"
#include "CaseCreator/UIComponents/BaseTypes/ObjectListWidget.h"
#include "CaseCreator/UIComponents/TemplateHelpers/NewObjectDialog.h"

#include "CaseCreator/CaseContent/Staging/Conversation.Staging.h"

#include "../UIComponents/BaseTypes/MultipleSelectionWidget.h"

#include "XmlStorableObject.h"

#include "SharedUtils.h"
#include "MLIException.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QTreeWidget>
#include <QLabel>
#include <QPlainTextEdit>

#include <QMouseEvent>
#include <QKeyEvent>
#include <QBrush>

#include <QGraphicsOpacityEffect>

#include <QGraphicsItemGroup>
#include <QGraphicsPixmapItem>
#include <QGraphicsSimpleTextItem>

#include <QElapsedTimer>

class Encounter;

class QLabel;
class QComboBox;

class ListItemTreeWidget;

class EvidenceSelector;
class BackgroundMusicSelector;

class CharacterSelectorMultipleSelectionSelectorWidget : public MultipleSelectionSelectorWidget
{
    Q_OBJECT

public:
    CharacterSelectorMultipleSelectionSelectorWidget(QWidget *parent = 0, Qt::WindowFlags flags = 0) : MultipleSelectionSelectorWidget(parent, flags) { }

    virtual QString GetString() override;
    virtual void SetToString(const QString &string) override;
    virtual QWidget * CreateSelector() override;

public slots:
    void CharacterSelectorCharacterSelected(const QString &characterId);

private:
    CharacterSelector *pCharacterSelector;
};

class EvidenceSelectorMultipleSelectionSelectorWidget : public MultipleSelectionSelectorWidget
{
    Q_OBJECT

public:
    EvidenceSelectorMultipleSelectionSelectorWidget(QWidget *parent = 0, Qt::WindowFlags flags = 0) : MultipleSelectionSelectorWidget(parent, flags) { }

    virtual QString GetString() override;
    virtual void SetToString(const QString &string) override;
    virtual QWidget * CreateSelector() override;

public slots:
    void EvidenceSelectorEvidenceSelected(const QString &evidenceId);

private:
    EvidenceSelector *pEvidenceSelector;
};

class LineEditMultipleSelectionSelectorWidget : public MultipleSelectionSelectorWidget
{
    Q_OBJECT

public:
    LineEditMultipleSelectionSelectorWidget(QWidget *parent = 0, Qt::WindowFlags flags = 0) : MultipleSelectionSelectorWidget(parent, flags) { }

    virtual QString GetString() override;
    virtual void SetToString(const QString &string) override;
    virtual QWidget * CreateSelector() override;

public slots:
    void LineEditTextEdited(const QString &evidenceId);

private:
    QLineEdit *pLineEdit;
};

class ConfrontationTopicMultipleSelectionSelectorWidget : public MultipleSelectionSelectorWidget
{
    Q_OBJECT

public:
    ConfrontationTopicMultipleSelectionSelectorWidget(QWidget *parent = 0, Qt::WindowFlags flags = 0) : MultipleSelectionSelectorWidget(parent, flags) { }

    virtual QString GetString() override;
    virtual void SetToString(const QString &string) override;
    virtual QWidget * CreateSelector() override;

    static QString CreateString(QString id, QString name, bool isEnabledAtStart);
    static void ParseString(QString string, QString *pId, QString *pName, bool *pIsEnabledAtStart);

public slots:
    void NameLineEditTextEdited(const QString &evidenceId);
    void IsEnabledCheckBoxToggled(bool isChecked);

private:
    QString GenerateString();

    QLineEdit *pNameLineEdit;
    QCheckBox *pIsEnabledCheckBox;
};

class ConversationSelector : public QComboBox
{
    Q_OBJECT

public:
    explicit ConversationSelector(QWidget *parent = 0);

    QString GetId();
    void SetToId(const QString &id);

    void Reset(Encounter *pParentEncounter);

signals:
    void ConversationSelected(const QString &conversationId);

public slots:
    void CurrentIndexChanged(int currentIndex);

private:
    QStringList conversationIds;
    int previousIndex;
};

class ConversationActionListItemSlots : public QObject
{
    Q_OBJECT

public:
    explicit ConversationActionListItemSlots(QObject *parent = 0) : QObject(parent) {}

signals:
    void Selected(ConversationActionListItemSlots *pSender);
};

class ConversationDialogSlots : public QObject
{
    Q_OBJECT

public:
    explicit ConversationDialogSlots(QObject *parent = 0) : QObject(parent) {}

public slots:
    void HandleTimeout()
    {
        IncrementPositionOnTimeout();
    }

protected:
    virtual void IncrementPositionOnTimeout() = 0;
};

class Conversation : public XmlStorableObject
{
    BEGIN_XML_STORABLE_OBJECT(Conversation)
        XML_STORABLE_TEXT(id)
        XML_STORABLE_TEXT(name)
        XML_STORABLE_CUSTOM_OBJECT_LIST(actions, Action::CreateFromXml)
    END_XML_STORABLE_OBJECT()

public:
    class Action;
    class DrawingView;

    class Dialog : public ConversationDialogSlots
    {
    public:
        Dialog();
        ~Dialog();

        void Reset();

        void SetParsedDialog(const QString &parsedDialog)
        {
            this->parsedDialog = parsedDialog;
        }

        void SetCharacters(
            QString leftCharacterId,
            QString leftCharacterInitialEmotionId,
            QString rightCharacterId,
            QString rightCharacterInitialEmotionId,
            CharacterPosition speakerPosition,
            QString speakingCharacterId)
        {
            this->leftCharacterId = leftCharacterId;
            this->leftCharacterInitialEmotionId = leftCharacterInitialEmotionId;
            this->rightCharacterId = rightCharacterId;
            this->rightCharacterInitialEmotionId = rightCharacterInitialEmotionId;
            this->speakerPosition = speakerPosition;
            this->speakingCharacterId = speakingCharacterId;
        }

        void SetAudio(QString dialogAudioFileName)
        {
            this->dialogAudioFileName = dialogAudioFileName;
        }

        void AddSpeedChangePosition(int position, double newMillisecondsPerCharacterUpdate);
        void AddEmotionChangePosition(int position, const QString &newEmotionId);
        void AddEmotionOtherChangePosition(int position, const QString &newEmotionId);
        void AddPausePosition(int position, double millisecondDuration);
        void AddAudioPausePosition(int position, double millisecondDuration);
        void AddMouthChangePosition(int position, bool mouthIsOn);

        void StartAside(int position);
        void EndAside(int position);
        void StartEmphasis(int position);
        void EndEmphasis(int position);

        void PlaySound(int position, const QString &id);

        QStringList GetHtmlLines(int lastTextPosition = -1) const;

        void PreloadAudio();

        void PlayOnDrawingView(Conversation::DrawingView *pDrawingView);
        void StopPlaying();

    private:
        void IncrementPositionOnTimeout();

        struct TextColorTransition
        {
            TextColorTransition(int position, QColor color, bool endsColor)
            {
                this->Position = position;
                this->Color = color;
                this->EndsColor = endsColor;
            }

            int Position;
            QColor Color;
            bool EndsColor;
        };

        class DialogEvent
        {
        public:
            DialogEvent(int position, Dialog *pOwningDialog)
            {
                this->position = position;
                this->pOwningDialog = pOwningDialog;
            }

            virtual ~DialogEvent() {}

            int GetPosition() { return position; }

            virtual void Raise() = 0;

        protected:
            Dialog *pOwningDialog;
            int position;
        };

        class SpeedChangeEvent : public DialogEvent
        {
        public:
            SpeedChangeEvent(int position, Dialog *pOwningDialog, double newMillisecondsPerCharacterUpdate)
                : DialogEvent(position, pOwningDialog)
            {
                this->newMillisecondsPerCharacterUpdate = newMillisecondsPerCharacterUpdate;
            }

            void Raise() override;

        private:
            double newMillisecondsPerCharacterUpdate;
        };

        class SpeakerEmotionChangeEvent : public DialogEvent
        {
        public:
            SpeakerEmotionChangeEvent(int position, Dialog *pOwningDialog, const QString &newEmotionId)
                : DialogEvent(position, pOwningDialog)
            {
                this->newEmotionId = newEmotionId;
            }

            void Raise() override;

        private:
            QString newEmotionId;
        };

        class OtherEmotionChangeEvent : public DialogEvent
        {
        public:
            OtherEmotionChangeEvent(int position, Dialog *pOwningDialog, const QString &newEmotionId)
                : DialogEvent(position, pOwningDialog)
            {
                this->newEmotionId = newEmotionId;
            }

            void Raise() override;

        private:
            QString newEmotionId;
        };

        class PauseEvent : public DialogEvent
        {
        public:
            PauseEvent(int position, Dialog *pOwningDialog, int msDuration)
                : DialogEvent(position, pOwningDialog)
            {
                this->msDuration = msDuration;
            }

            void Raise() override;

        private:
            int msDuration;
        };

        class AudioPauseEvent : public DialogEvent
        {
        public:
            AudioPauseEvent(int position, Dialog *pOwningDialog, int msDuration)
                : DialogEvent(position, pOwningDialog)
            {
                this->msDuration = msDuration;
            }

            void Raise() override;

        private:
            int msDuration;
        };

        class MouthChangeEvent : public DialogEvent
        {
        public:
            MouthChangeEvent(int position, Dialog *pOwningDialog, bool isMouthOpen)
                : DialogEvent(position, pOwningDialog)
            {
                this->isMouthOpen = isMouthOpen;
            }

            void Raise() override;

        private:
            bool isMouthOpen;
        };

        class PlaySoundEvent : public DialogEvent
        {
        public:
            PlaySoundEvent(int position, Dialog *pOwningDialog, const QString &id)
                : DialogEvent(position, pOwningDialog)
            {
                this->id = id;
            }

            void Raise() override;

        private:
            QString id;
        };

        QString parsedDialog;
        QList<DialogEvent *> events;
        QList<TextColorTransition> colorTransitions;

        QString leftCharacterId;
        QString leftCharacterInitialEmotionId;
        QString rightCharacterId;
        QString rightCharacterInitialEmotionId;
        CharacterPosition speakerPosition;
        QString speakingCharacterId;

        Conversation::DrawingView *pDrawingViewForPlaying;
        int currentPosition;
        QString leftCharacterEmotionId;
        QString rightCharacterEmotionId;
        int millisecondsPerCharacterUpdate;
        int mouthOffCount;

        QString dialogAudioFileName;
        QStringList soundEffectIds;

        QTimer playTimer;
        QElapsedTimer elapsedTimer;
        qint64 elapsedTimeLeftOver;
        int msPauseDuration;
        int msAudioPauseDuration;
        QList<DialogEvent *>::iterator eventIterator;
    };

    Conversation()
    {
        pOwningEncounter = NULL;
    }

    Conversation(QString id) : Conversation()
    {
        this->id = id;
    }

protected:
    Conversation(Staging::Conversation *pStagingConversation);

public:
    virtual ~Conversation();

    virtual void WriteToCaseXml(XmlWriter *pWriter);
    virtual void WriteToCaseXmlCore(XmlWriter *pWriter);

    static Conversation * CreateFromStaging(Staging::Conversation *pStagingConversation)
    {
        Conversation *pConversation = new Conversation(pStagingConversation);
        pConversation->PopulateActionsFromStaging(pConversation->actions, pStagingConversation->ActionList);

        return pConversation;
    }

    static Conversation * CreateFromXml(XmlReader *pReader)
    {
        return new Conversation(pReader);
    }

    static QString GetObjectAdditionString() { return QString("conversation"); }
    static QString GetListTitle() { return QString("Conversations"); }
    static bool GetIsMainList() { return false; }
    static bool GetAllowsNewObjects() { return true; }

    void PopulateActionsFromStaging(QList<Action *> &actions, const QList<Staging::Conversation::Action *> &stagingActions, int initialIndex = 0);
    void UpdateAndCacheConversationStates();
    void ReplaceAction(Action *pNewAction, Action *pOldAction);
    static void ReplaceAction(Action *pNewAction, Action *pOldAction, QList<Action *> &actionListToSearch);
    void PreloadAudio();

protected:
    virtual void PopulateActionFromStaging(QList<Action *> &actions, const QList<Staging::Conversation::Action *> &stagingActions, int &indexInStagingActions, int initialIndexInFullList);

public:
    QString GetId() { return this->id; }
    QString GetName() { return this->name; }
    QString GetDisplayName() { return this->name.length() > 0 ? this->name : this->id; }

    Encounter * GetOwningEncounter() { return pOwningEncounter; }
    void SetOwningEncounter(Encounter *pOwningEncounter) { this->pOwningEncounter = pOwningEncounter; }

    class DrawingView : public IDrawingView<Conversation>
    {
        friend class Conversation;

    public:
        DrawingView(Conversation *pConversation, IDrawable *pParent);
        virtual ~DrawingView();

        void DrawCore(QGraphicsScene *pScene, QList<QGraphicsItem *> &addedItems);
        void DrawChildren(QGraphicsScene *pScene);
        void UpdateCore();
        void UpdateChildren();
        void ResetChildren();

        void SetLeftCharacter(QString characterId, QString emotionId);
        void SetRightCharacter(QString characterId, QString emotionId);
        void SetDialog(CharacterPosition characterPosition, const QStringList &dialogHtmlLines, QString offscreenCharacterId, bool isTalking);

    private:
        QGraphicsPixmapItem *pDialogBackgroundPixmapItem;

        QString currentLeftCharacterId;
        QString currentLeftCharacterEmotionId;
        QString currentRightCharacterId;
        QString currentRightCharacterEmotionId;

        Character::DialogDrawingView *pLeftCharacterDialogDrawingView;
        Character::DialogDrawingView *pRightCharacterDialogDrawingView;

        QGraphicsItemGroup *pDialogItemGroup;
        QGraphicsOpacityEffect *pDialogItemGroupOpacityEffect;
        QGraphicsPixmapItem *pCharacterNameTabPixmapItem;
        QGraphicsSimpleTextItem *pCharacterNameSimpleTextItem;

        // Only five lines will actually fit on screen,
        // so we only need to allocate five text items.
        QGraphicsTextItem *pDialogLinesTextItems[5];
    };

    Conversation::DrawingView * GetDrawingView(IDrawable *pParent)
    {
        Conversation::DrawingView *pDrawingView = new Conversation::DrawingView(this, pParent);

        if (actions.length() > 0)
        {
            actions.first()->PushToDrawingView(pDrawingView);
        }

        return pDrawingView;
    }

    class UnlockCondition : public XmlStorableObject
    {
        BEGIN_XML_STORABLE_OBJECT(UnlockCondition)
        END_XML_STORABLE_OBJECT()

    public:
        enum class Type
        {
            FlagSet = 0,
            PartnerPresent = 1,
        };

        UnlockCondition() { }
        virtual ~UnlockCondition() { }

        virtual UnlockCondition::Type GetType() = 0;

        virtual UnlockCondition * Clone() = 0;
        virtual QString GetDisplayString() = 0;

        static UnlockCondition * LoadFromStagingObject(Staging::Conversation::UnlockCondition *pStagingUnlockCondition);
        static UnlockCondition * CreateFromXml(XmlReader *pReader);
    };

    class FlagSetUnlockCondition : public UnlockCondition
    {
        friend class LockConversationActionEditorDialogContents;

        BEGIN_DERIVED_XML_STORABLE_OBJECT(FlagSetUnlockCondition, UnlockCondition)
            XML_STORABLE_TEXT(flagId)
        END_XML_STORABLE_OBJECT()

    public:
        FlagSetUnlockCondition() { }
        FlagSetUnlockCondition(const QString &flagId) { this->flagId = flagId; }
        FlagSetUnlockCondition(Staging::Conversation::FlagSetUnlockCondition *pStagingFlagSetUnlockCondition);
        virtual ~FlagSetUnlockCondition() { }

        virtual UnlockCondition::Type GetType() override { return UnlockCondition::Type::FlagSet; }

        virtual UnlockCondition * Clone() override;
        virtual QString GetDisplayString() override;

    private:
        QString flagId;
    };

    class PartnerPresentUnlockCondition : public UnlockCondition
    {
        friend class LockConversationActionEditorDialogContents;

        BEGIN_DERIVED_XML_STORABLE_OBJECT(PartnerPresentUnlockCondition, UnlockCondition)
            XML_STORABLE_TEXT(partnerId)
        END_XML_STORABLE_OBJECT()

    public:
        PartnerPresentUnlockCondition() { }
        PartnerPresentUnlockCondition(const QString &partnerId) { this->partnerId = partnerId; }
        PartnerPresentUnlockCondition(Staging::Conversation::PartnerPresentUnlockCondition *pStagingPartnerPresentUnlockCondition);
        virtual ~PartnerPresentUnlockCondition() { }

        virtual UnlockCondition::Type GetType() override { return UnlockCondition::Type::PartnerPresent; }

        virtual UnlockCondition * Clone() override;
        virtual QString GetDisplayString() override;

    private:
        QString partnerId;
    };

    class State
    {
    public:
        State()
            : leftCharacterId("")
            , leftCharacterEmotionId("")
            , rightCharacterId("")
            , rightCharacterEmotionId("")
        {
        }

        State(
            const QString &initialLeftCharacterId,
            const QString &initialLeftCharacterEmotionId,
            const QString &initialRightCharacterId,
            const QString &initialRightCharacterEmotionId)
            : leftCharacterId(initialLeftCharacterId)
            , leftCharacterEmotionId(initialLeftCharacterEmotionId)
            , rightCharacterId(initialRightCharacterId)
            , rightCharacterEmotionId(initialRightCharacterEmotionId)
        {
        }

        State Clone() const
        {
            return State(
                this->leftCharacterId,
                this->leftCharacterEmotionId,
                this->rightCharacterId,
                this->rightCharacterEmotionId);
        }

        QString& GetLeftCharacterId() { return this->leftCharacterId; }
        QString& GetLeftCharacterEmotionId() { return this->leftCharacterEmotionId; }
        QString& GetRightCharacterId() { return this->rightCharacterId; }
        QString& GetRightCharacterEmotionId() { return this->rightCharacterEmotionId; }

        bool SetLeftCharacterId(const QString &id);
        bool SetLeftCharacterEmotionId(const QString &id);
        bool SetRightCharacterId(const QString &id);
        bool SetRightCharacterEmotionId(const QString &id);

    private:
        QString leftCharacterId;
        QString leftCharacterEmotionId;
        QString rightCharacterId;
        QString rightCharacterEmotionId;
    };

    enum class ActionType
    {
        CharacterChange,
        SetFlag,
        BranchOnCondition,
        ShowDialog,
        MustPresentEvidence,
        EnableConversation,
        EnableEvidence,
        UpdateEvidence,
        DisableEvidence,
        EnableCutscene,
        PlayBgm,
        PauseBgm,
        ResumeBgm,
        StopBgm,
        PlayAmbiance,
        PauseAmbiance,
        ResumeAmbiance,
        StopAmbiance,
        StartAnimation,
        StopAnimation,
        SetPartner,
        GoToPresentWrongEvidence,
        LockConversation,
        ExitEncounter,
        MoveToLocation,
        MoveToZoomedView,
        EndCase,
        MultipleChoice,
        ExitMultipleChoice,
        EnableFastForward,
        DisableFastForward,
        BeginBreakdown,
        EndBreakdown,
        InterrogationRepeat,
        ShowInterrogation,
        ExitInterrogationRepeat,
        ConfrontationTopicSelection,
        EnableTopic,
        RestartDecision,
        RestartConfrontation,
    };

    class Action : public XmlStorableObject, public ListItemObject<Conversation::State, Conversation::ActionType>
    {
        BEGIN_XML_STORABLE_OBJECT(Action)
        END_XML_STORABLE_OBJECT()

    public:
        Action() {}
        virtual ~Action() {}

        virtual void WriteToCaseXml(XmlWriter *pWriter, int& currentActionIndex) = 0;

        virtual ActionType GetType() = 0;

        Action * Clone();

        void CopyProperties(ListItemObject<Conversation::State, Conversation::ActionType> *pOther, bool isForEdit) override;
        void ExchangeListItemBaseOwnership(ListItemObject<Conversation::State, Conversation::ActionType> *pOther) override;
        State & GetStateDuringObject() override { return stateDuringAction; }

        ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> * GetListItemBase() { return pListItemBase; }
        void SetListItemBase(ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *pListItemBase) { this->pListItemBase = pListItemBase; }

        static void CloneActionList(QList<Action *> &destination, const QList<Action *> &source);
        static void ExchangeListItemBaseOwnership(const QList<Action *> &destination, const QList<Action *> &source);

        static Action * CreateFromXml(XmlReader *pReader);

        static QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> GetListItemsForActions(QList<Action *> &actions, int indentLevel);
        QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> GetListItems(int indentLevel);

        virtual void ReplaceAction(Action */*pNewAction*/, Action */*pOldAction*/)
        {
            throw new MLIException("ReplaceAction() should never be called on an action that has no children.");
        }

        virtual void UpdateAndCacheConversationState(State &currentState)
        {
            stateDuringAction = currentState.Clone();
        }

        virtual void PreloadAudio() { }

        virtual bool CanPlay() { return false; }
        virtual void PushToDrawingView(Conversation::DrawingView *pDrawingView);
        virtual void PlayOnDrawingView(Conversation::DrawingView */*pDrawingView*/) { }
        virtual void StopPlaying() { }

    protected:
        virtual void CopyPropertiesImpl(Action *pOther, bool isForEdit) = 0;
        virtual void ExchangeListItemBaseOwnershipImpl(Action */*pOther*/) { }
        virtual QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> GetListItemsImpl(int indentLevel) = 0;

    private:
        State stateDuringAction;
        ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *pListItemBase;

    public:
        template <class TAction>
        class ActionListItem : public ListItem<Conversation::Action, Conversation::State, Conversation::ActionType>
        {
        public:
            explicit ActionListItem(TAction *pAction, int indentLevel)
                : ListItem<Conversation::Action, Conversation::State, Conversation::ActionType>(indentLevel)
            {
                static_assert(is_base_of<Conversation::Action, TAction>::value, "Conversation::Action::ListItem must have a subclass of Conversation::Action as its template type.");

                SetObjectImpl(pAction);
            }

        protected:
            Conversation::Action * GetObjectImpl() override { return pAction; }

            void SetObjectImpl(Conversation::Action *pAction) override
            {
                this->pAction = dynamic_cast<TAction *>(pAction);

                if (pAction != NULL)
                {
                    pAction->SetListItemBase(this);
                }
            }

            TAction *pAction;
        };
    };

    class ActionWithNotification : public Action
    {
        BEGIN_DERIVED_XML_STORABLE_OBJECT(ActionWithNotification, Action)
            XML_STORABLE_BOOL(shouldNotify)
            XML_STORABLE_TEXT(rawNotificationText)
            XML_STORABLE_TEXT(oldEvidenceId)
            XML_STORABLE_TEXT(newEvidenceId)
            XML_STORABLE_TEXT(partnerId)
            XML_STORABLE_TEXT(locationWithCutsceneId)
            XML_STORABLE_TEXT(cutsceneId)
        END_XML_STORABLE_OBJECT()

    public:
        ActionWithNotification();
        virtual ~ActionWithNotification() { }

        void AddNotification(Staging::Conversation::NotificationAction *pStagingNotificationAction);

    protected:
        void CopyPropertiesImpl(Action *pOther, bool isForEdit);

        bool shouldNotify;
        QString rawNotificationText;
        QString oldEvidenceId;
        QString newEvidenceId;
        QString partnerId;
        QString locationWithCutsceneId;
        QString cutsceneId;
    };

    class CharacterChangeAction : public Action
    {
        friend class CharacterChangeActionListItem;
        friend class CharacterChangeActionEditorDialogContents;

        BEGIN_DERIVED_XML_STORABLE_OBJECT(CharacterChangeAction, Action)
            XML_STORABLE_ENUM(position, CharacterPositionToString, StringToCharacterPosition)
            XML_STORABLE_TEXT(characterIdToChangeTo)
            XML_STORABLE_TEXT(initialEmotionId)
            XML_STORABLE_TEXT(sfxId)
        END_XML_STORABLE_OBJECT()

    public:
        CharacterChangeAction();
        CharacterChangeAction(Staging::Conversation::CharacterChangeAction *pStagingCharacterChangeAction);

        void WriteToCaseXml(XmlWriter *pWriter, int& currentActionIndex) override;

        virtual ActionType GetType() override { return ActionType::CharacterChange; }

        virtual void UpdateAndCacheConversationState(State &currentState) override;

        static EditorDialogContents<Conversation::Action, Conversation::State> * CreateEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::Action *pActionToEdit);

    protected:
        virtual void CopyPropertiesImpl(Action *pOther, bool isForEdit) override;
        virtual QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> GetListItemsImpl(int indentLevel) override;

    private:
        CharacterPosition position;
        QString characterIdToChangeTo;
        QString initialEmotionId;
        QString sfxId;
    };

    class SetFlagAction : public ActionWithNotification
    {
        friend class SetFlagActionListItem;
        friend class SetFlagActionEditorDialogContents;

        BEGIN_DERIVED_XML_STORABLE_OBJECT(SetFlagAction, ActionWithNotification)
            XML_STORABLE_TEXT(flagId)
        END_XML_STORABLE_OBJECT()

    public:
        SetFlagAction() : ActionWithNotification() {}
        SetFlagAction(Staging::Conversation::SetFlagAction *pStagingSetFlagAction);

        void WriteToCaseXml(XmlWriter *pWriter, int& currentActionIndex) override;

        virtual ActionType GetType() override { return ActionType::SetFlag; }

        static EditorDialogContents<Conversation::Action, Conversation::State> * CreateEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::Action *pActionToEdit);

    protected:
        virtual void CopyPropertiesImpl(Action *pOther, bool isForEdit) override;
        virtual QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> GetListItemsImpl(int indentLevel) override;

    private:
        QString flagId;
    };

    class BranchOnConditionAction : public Action
    {
        friend class Conversation;
        friend class BranchOnConditionActionListItem;
        friend class BranchOnConditionActionEditorDialogContents;

        BEGIN_DERIVED_XML_STORABLE_OBJECT(BranchOnConditionAction, Action)
            XML_STORABLE_CUSTOM_OBJECT(pCondition, Condition::CreateFromXml)
            XML_STORABLE_CUSTOM_OBJECT_LIST(trueActions, Action::CreateFromXml)
            XML_STORABLE_CUSTOM_OBJECT_LIST(falseActions, Action::CreateFromXml)
        END_XML_STORABLE_OBJECT()

    public:
        BranchOnConditionAction();
        BranchOnConditionAction(Staging::Conversation::BranchOnConditionAction *pStagingBranchOnConditionAction);
        virtual ~BranchOnConditionAction();

        void WriteToCaseXml(XmlWriter *pWriter, int& currentActionIndex) override;

        virtual ActionType GetType() override { return ActionType::BranchOnCondition; }

        virtual void ReplaceAction(Action *pNewAction, Action *pOldAction) override;
        virtual void UpdateAndCacheConversationState(State &currentState) override;
        virtual void PreloadAudio() override;

        static EditorDialogContents<Conversation::Action, Conversation::State> * CreateEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::Action *pActionToEdit);

    protected:
        virtual void CopyPropertiesImpl(Action *pOther, bool isForEdit) override;
        virtual void ExchangeListItemBaseOwnershipImpl(Action *pOther) override;
        virtual QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> GetListItemsImpl(int indentLevel) override;

    private:
        Condition *pCondition;
        QList<Action *> trueActions;
        QList<Action *> falseActions;
    };

    class ShowDialogAction : public Action, public IDialogEventsOwner
    {
        friend class ShowDialogActionListItem;
        friend class ShowDialogActionEditorDialogContents;

        BEGIN_DERIVED_XML_STORABLE_OBJECT(ShowDialogAction, Action)
            XML_STORABLE_ENUM(speakerPosition, CharacterPositionToString, StringToCharacterPosition)
            XML_STORABLE_TEXT(characterId)
            XML_STORABLE_TEXT(rawDialog)
            XML_STORABLE_TEXT(voiceOverFilePath)
            XML_STORABLE_INT(leadInTime)
            XML_STORABLE_BOOL(shouldAdvanceAutomatically)
            XML_STORABLE_INT(delayBeforeContinuing)
        END_XML_STORABLE_OBJECT()

    public:
        ShowDialogAction();
        ShowDialogAction(Staging::Conversation::ShowDialogAction *pStagingShowDialogAction);

        void WriteToCaseXml(XmlWriter *pWriter, int& currentActionIndex) override;

        virtual ActionType GetType() override { return ActionType::ShowDialog; }

        void InitializeDialog();

        virtual void UpdateAndCacheConversationState(State &currentState) override;
        virtual void PreloadAudio() override;

        virtual bool CanPlay() { return true; }
        virtual void PushToDrawingView(Conversation::DrawingView *pDrawingView) override;
        virtual void PlayOnDrawingView(Conversation::DrawingView *pDrawingView) override;
        virtual void StopPlaying() override;

        virtual void LoadElementsFromXml(XmlReader *pReader)
        {
            XmlStorableObject::LoadElementsFromXml(pReader);
            InitializeDialog();
        }

        void AddSpeedChangePosition(int position, double newMillisecondsPerCharacterUpdate) override
        {
            parsedDialog.AddSpeedChangePosition(position, newMillisecondsPerCharacterUpdate);
        }

        void AddEmotionChangePosition(int position, const QString &newEmotionId) override
        {
            parsedDialog.AddEmotionChangePosition(position, newEmotionId);
        }

        void AddEmotionOtherChangePosition(int position, const QString &newEmotionId) override
        {
            parsedDialog.AddEmotionOtherChangePosition(position, newEmotionId);
        }

        void AddPausePosition(int position, double millisecondDuration) override
        {
            parsedDialog.AddPausePosition(position, millisecondDuration);
        }

        void AddAudioPausePosition(int position, double millisecondDuration) override
        {
            parsedDialog.AddAudioPausePosition(position, millisecondDuration);
        }

        void AddMouthChangePosition(int position, bool mouthIsOn) override
        {
            parsedDialog.AddMouthChangePosition(position, mouthIsOn);
        }

        void StartAside(int position) override
        {
            AddMouthChangePosition(position, false /* mouthIsOn */);
            parsedDialog.StartAside(position);
        }

        void EndAside(int position, int eventEnd, int parsedStringLength, QString *pStringToPrependOnNext) override
        {
            if (eventEnd + 1 == parsedStringLength)
            {
                *pStringToPrependOnNext = "{Mouth:On}";
            }
            else
            {
                AddMouthChangePosition(position, true /* mouthIsOn */);
            }

            parsedDialog.EndAside(position);
        }

        void StartEmphasis(int position) override
        {
            parsedDialog.StartEmphasis(position);
        }

        void EndEmphasis(int position) override
        {
            parsedDialog.EndEmphasis(position);
        }

        void AddPlaySoundPosition(int position, const QString &id) override
        {
            parsedDialog.PlaySound(position, id);
        }

        void AddShakePosition(int /*position*/) override
        {
            //this->dialogEventListOriginal.push_back(new ShakeEvent(position, this));
        }

        void AddScreenShakePosition(int /*position*/, double /*shakeIntensity*/) override
        {
            //this->dialogEventListOriginal.push_back(new ScreenShakeEvent(position, this, shakeIntensity));
        }

        void AddNextFramePosition(int /*position*/) override
        {
            //this->dialogEventListOriginal.push_back(new NextFrameEvent(position, this));
        }

        void AddPlayerDamagedPosition(int /*position*/) override
        {
            //this->dialogEventListOriginal.push_back(new PlayerDamagedEvent(position, this));
        }

        void AddOpponentDamagedPosition(int /*position*/) override
        {
            //this->dialogEventListOriginal.push_back(new OpponentDamagedEvent(position, this));
        }

        void AddPlayBgmPosition(int /*position*/, const QString &/*id*/) override
        {
            //this->dialogEventListOriginal.push_back(new PlayBgmEvent(position, this, id));
        }

        void AddPlayBgmPermanentlyPosition(int /*position*/, const QString &/*id*/) override
        {
            //this->dialogEventListOriginal.push_back(new PlayBgmEvent(position, this, id, true /* isPermanent */));
        }

        void AddStopBgmPosition(int /*position*/) override
        {
            //this->dialogEventListOriginal.push_back(new StopBgmEvent(position, this, false /* isInstant */, false /* isPermanent */));
        }

        void AddStopBgmPermanentlyPosition(int /*position*/) override
        {
            //this->dialogEventListOriginal.push_back(new StopBgmEvent(position, this, false /* isInstant */, true /* isPermanent */));
        }

        void AddStopBgmInstantlyPosition(int /*position*/) override
        {
            //this->dialogEventListOriginal.push_back(new StopBgmEvent(position, this, true /* isInstant */, false /* isPermanent */));
        }

        void AddStopBgmInstantlyPermanentlyPosition(int /*position*/) override
        {
            //this->dialogEventListOriginal.push_back(new StopBgmEvent(position, this, true /* isInstant */, true /* isPermanent */));
        }

        void AddZoomPosition(int /*position*/) override
        {
            //this->dialogEventListOriginal.push_back(new ZoomEvent(position, this));
        }

        void AddEndZoomPosition(int /*position*/) override
        {
            //this->dialogEventListOriginal.push_back(new EndZoomEvent(position, this));
        }

        void AddBeginBreakdownPosition(int /*position*/) override
        {
            //this->dialogEventListOriginal.push_back(new BeginBreakdownEvent(position, this));
        }

        void AddEndBreakdownPosition(int /*position*/) override
        {
            //this->dialogEventListOriginal.push_back(new EndBreakdownEvent(position, this));
        }

        static EditorDialogContents<Conversation::Action, Conversation::State> * CreateEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::Action *pActionToEdit);

    protected:
        virtual void CopyPropertiesImpl(Action *pOther, bool isForEdit) override;
        virtual QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> GetListItemsImpl(int indentLevel) override;

        CharacterPosition speakerPosition;
        QString characterId;
        QString rawDialog;
        Dialog parsedDialog;
        QString voiceOverFilePath;
        int leadInTime;
        bool shouldAdvanceAutomatically;
        int delayBeforeContinuing;

        State stateBeforeAction;
    };

    class MustPresentEvidenceAction : public ShowDialogAction
    {
        friend class Conversation;
        friend class MustPresentEvidenceActionListItem;
        friend class ShowDialogActionEditorDialogContents;

        BEGIN_DERIVED_XML_STORABLE_OBJECT(MustPresentEvidenceAction, ShowDialogAction)
            XML_STORABLE_TEXT_LIST(correctEvidenceIdList)
            XML_STORABLE_CUSTOM_OBJECT_LIST(correctEvidencePresentedActions, Action::CreateFromXml)
            XML_STORABLE_CUSTOM_OBJECT_LIST(wrongEvidencePresentedActions, Action::CreateFromXml)
            XML_STORABLE_CUSTOM_OBJECT_LIST(endRequestedActions, Action::CreateFromXml)
            XML_STORABLE_BOOL(canEndBeRequested)
        END_XML_STORABLE_OBJECT()

    public:
        MustPresentEvidenceAction();
        MustPresentEvidenceAction(Staging::Conversation::MustPresentEvidenceAction *pStagingMustPresentEvidenceAction);
        virtual ~MustPresentEvidenceAction();

        void WriteToCaseXml(XmlWriter *pWriter, int& currentActionIndex) override;

        virtual ActionType GetType() override { return ActionType::MustPresentEvidence; }

        virtual void ReplaceAction(Action *pNewAction, Action *pOldAction) override;
        virtual void UpdateAndCacheConversationState(State &currentState) override;
        virtual void PreloadAudio() override;

        static EditorDialogContents<Conversation::Action, Conversation::State> * CreateEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::Action *pActionToEdit);

    protected:
        virtual void CopyPropertiesImpl(Action *pOther, bool isForEdit) override;
        virtual void ExchangeListItemBaseOwnershipImpl(Action *pOther) override;
        virtual QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> GetListItemsImpl(int indentLevel) override;

    private:
        QList<QString> correctEvidenceIdList;

        QList<Action *> correctEvidencePresentedActions;
        QList<Action *> wrongEvidencePresentedActions;
        QList<Action *> endRequestedActions;

        bool canEndBeRequested;
    };

    class EnableConversationAction : public ActionWithNotification
    {
        friend class EnableConversationActionListItem;
        friend class EnableConversationActionEditorDialogContents;

        BEGIN_DERIVED_XML_STORABLE_OBJECT(EnableConversationAction, ActionWithNotification)
            XML_STORABLE_TEXT(encounterId)
            XML_STORABLE_TEXT(conversationId)
        END_XML_STORABLE_OBJECT()

    public:
        EnableConversationAction() : ActionWithNotification() { }
        EnableConversationAction(Staging::Conversation::EnableConversationAction *pStagingEnableConversationAction);

        void WriteToCaseXml(XmlWriter *pWriter, int& currentActionIndex) override;

        virtual ActionType GetType() override { return ActionType::EnableConversation; }

        static EditorDialogContents<Conversation::Action, Conversation::State> * CreateEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::Action *pActionToEdit);

    protected:
        virtual void CopyPropertiesImpl(Action *pOther, bool isForEdit) override;
        virtual QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> GetListItemsImpl(int indentLevel) override;

    private:
        QString encounterId;
        QString conversationId;
    };

    class EnableEvidenceAction : public ActionWithNotification
    {
        friend class EnableEvidenceActionListItem;
        friend class EnableEvidenceActionEditorDialogContents;

        BEGIN_DERIVED_XML_STORABLE_OBJECT(EnableEvidenceAction, ActionWithNotification)
        END_XML_STORABLE_OBJECT()

    public:
        EnableEvidenceAction() : ActionWithNotification() { }
        EnableEvidenceAction(Staging::Conversation::EnableEvidenceAction *pStagingEnableEvidenceAction);

        void WriteToCaseXml(XmlWriter *pWriter, int& currentActionIndex) override;

        virtual ActionType GetType() override { return ActionType::EnableEvidence; }

        static EditorDialogContents<Conversation::Action, Conversation::State> * CreateEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::Action *pActionToEdit);

    protected:
        virtual void CopyPropertiesImpl(Action *pOther, bool isForEdit) override;
        virtual QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> GetListItemsImpl(int indentLevel);
    };

    class UpdateEvidenceAction : public ActionWithNotification
    {
        friend class UpdateEvidenceActionListItem;
        friend class UpdateEvidenceActionEditorDialogContents;

        BEGIN_DERIVED_XML_STORABLE_OBJECT(UpdateEvidenceAction, ActionWithNotification)
        END_XML_STORABLE_OBJECT()

    public:
        UpdateEvidenceAction() : ActionWithNotification() { }
        UpdateEvidenceAction(Staging::Conversation::UpdateEvidenceAction *pStagingUpdateEvidenceAction);

        void WriteToCaseXml(XmlWriter *pWriter, int& currentActionIndex) override;

        virtual ActionType GetType() override { return ActionType::UpdateEvidence; }

        static EditorDialogContents<Conversation::Action, Conversation::State> * CreateEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::Action *pActionToEdit);

    protected:
        virtual void CopyPropertiesImpl(Action *pOther, bool isForEdit) override;
        virtual QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> GetListItemsImpl(int indentLevel) override;
    };

    class DisableEvidenceAction : public ActionWithNotification
    {
        friend class DisableEvidenceActionListItem;
        friend class DisableEvidenceActionEditorDialogContents;

        BEGIN_DERIVED_XML_STORABLE_OBJECT(DisableEvidenceAction, ActionWithNotification)
        END_XML_STORABLE_OBJECT()

    public:
        DisableEvidenceAction() : ActionWithNotification() { }
        DisableEvidenceAction(Staging::Conversation::DisableEvidenceAction *pStagingDisableEvidenceAction);

        void WriteToCaseXml(XmlWriter *pWriter, int& currentActionIndex) override;

        virtual ActionType GetType() override { return ActionType::DisableEvidence; }

        static EditorDialogContents<Conversation::Action, Conversation::State> * CreateEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::Action *pActionToEdit);

    protected:
        virtual void CopyPropertiesImpl(Action *pOther, bool isForEdit) override;
        virtual QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> GetListItemsImpl(int indentLevel) override;
    };

    class EnableCutsceneAction : public ActionWithNotification
    {
        friend class EnableCutsceneActionListItem;
        friend class EnableCutsceneActionEditorDialogContents;

        BEGIN_DERIVED_XML_STORABLE_OBJECT(EnableCutsceneAction, ActionWithNotification)
        END_XML_STORABLE_OBJECT()

    public:
        EnableCutsceneAction() : ActionWithNotification() { }
        EnableCutsceneAction(Staging::Conversation::EnableCutsceneAction *pStagingEnableCutsceneAction);

        void WriteToCaseXml(XmlWriter *pWriter, int& currentActionIndex) override;

        virtual ActionType GetType() override { return ActionType::EnableCutscene; }

        static EditorDialogContents<Conversation::Action, Conversation::State> * CreateEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::Action *pActionToEdit);

    protected:
        virtual void CopyPropertiesImpl(Action *pOther, bool isForEdit) override;
        virtual QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> GetListItemsImpl(int indentLevel) override;
    };

    class PlayBgmAction : public Action
    {
        friend class PlayBgmActionListItem;
        friend class PlayBgmActionEditorDialogContents;

        BEGIN_DERIVED_XML_STORABLE_OBJECT(PlayBgmAction, Action)
            XML_STORABLE_TEXT(bgmId)
            XML_STORABLE_BOOL(preserveBgm)
        END_XML_STORABLE_OBJECT()

    public:
        PlayBgmAction();
        PlayBgmAction(Staging::Conversation::PlayBgmAction *pStagingPlayBgmAction);

        void WriteToCaseXml(XmlWriter *pWriter, int& currentActionIndex) override;

        virtual ActionType GetType() override { return ActionType::PlayBgm; }

        static EditorDialogContents<Conversation::Action, Conversation::State> * CreateEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::Action *pActionToEdit);

    protected:
        virtual void CopyPropertiesImpl(Action *pOther, bool isForEdit) override;
        virtual QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> GetListItemsImpl(int indentLevel) override;

    private:
        QString bgmId;
        bool preserveBgm;
    };

    class PauseBgmAction : public Action
    {
        friend class PauseBgmActionListItem;
        friend class PauseBgmActionEditorDialogContents;

        BEGIN_DERIVED_XML_STORABLE_OBJECT(PauseBgmAction, Action)
        END_XML_STORABLE_OBJECT()

    public:
        PauseBgmAction() : Action() {}
        PauseBgmAction(Staging::Conversation::PauseBgmAction */*pStagingPauseBgmAction*/) : PauseBgmAction() {}

        void WriteToCaseXml(XmlWriter *pWriter, int& currentActionIndex) override;

        virtual ActionType GetType() override { return ActionType::PauseBgm; }

        static EditorDialogContents<Conversation::Action, Conversation::State> * CreateEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::Action *pActionToEdit);

    protected:
        virtual void CopyPropertiesImpl(Action *pOther, bool isForEdit) override;
        virtual QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> GetListItemsImpl(int indentLevel) override;
    };

    class ResumeBgmAction : public Action
    {
        friend class ResumeBgmActionListItem;
        friend class ResumeBgmActionEditorDialogContents;

        BEGIN_DERIVED_XML_STORABLE_OBJECT(ResumeBgmAction, Action)
        END_XML_STORABLE_OBJECT()

    public:
        ResumeBgmAction() : Action() {}
        ResumeBgmAction(Staging::Conversation::ResumeBgmAction */*pStagingResumeBgmAction*/) : ResumeBgmAction() {}

        void WriteToCaseXml(XmlWriter *pWriter, int& currentActionIndex) override;

        virtual ActionType GetType() override { return ActionType::ResumeBgm; }

        static EditorDialogContents<Conversation::Action, Conversation::State> * CreateEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::Action *pActionToEdit);

    protected:
        virtual void CopyPropertiesImpl(Action *pOther, bool isForEdit) override;
        virtual QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> GetListItemsImpl(int indentLevel) override;
    };

    class StopBgmAction : public Action
    {
        friend class StopBgmActionListItem;
        friend class StopBgmActionEditorDialogContents;

        BEGIN_DERIVED_XML_STORABLE_OBJECT(StopBgmAction, Action)
            XML_STORABLE_BOOL(isInstant)
            XML_STORABLE_BOOL(preserveBgm)
        END_XML_STORABLE_OBJECT()

    public:
        StopBgmAction();
        StopBgmAction(Staging::Conversation::StopBgmAction *pStagingStopBgmAction);

        void WriteToCaseXml(XmlWriter *pWriter, int& currentActionIndex) override;

        virtual ActionType GetType() override { return ActionType::StopBgm; }

        static EditorDialogContents<Conversation::Action, Conversation::State> * CreateEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::Action *pActionToEdit);

    protected:
        virtual void CopyPropertiesImpl(Action *pOther, bool isForEdit) override;
        virtual QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> GetListItemsImpl(int indentLevel) override;

    private:
        bool isInstant;
        bool preserveBgm;
    };

    class PlayAmbianceAction : public Action
    {
        friend class PlayAmbianceActionListItem;
        friend class PlayAmbianceActionEditorDialogContents;

        BEGIN_DERIVED_XML_STORABLE_OBJECT(PlayAmbianceAction, Action)
            XML_STORABLE_TEXT(ambianceSfxId)
            XML_STORABLE_BOOL(preserveAmbiance)
        END_XML_STORABLE_OBJECT()

    public:
        PlayAmbianceAction();
        PlayAmbianceAction(Staging::Conversation::PlayAmbianceAction *pStagingPlayAmbianceAction);

        void WriteToCaseXml(XmlWriter *pWriter, int& currentActionIndex) override;

        virtual ActionType GetType() override { return ActionType::PlayAmbiance; }

        static EditorDialogContents<Conversation::Action, Conversation::State> * CreateEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::Action *pActionToEdit);

    protected:
        virtual void CopyPropertiesImpl(Action *pOther, bool isForEdit) override;
        virtual QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> GetListItemsImpl(int indentLevel) override;

    private:
        QString ambianceSfxId;
        bool preserveAmbiance;
    };

    class PauseAmbianceAction : public Action
    {
        friend class PauseAmbianceActionListItem;
        friend class PauseAmbianceActionEditorDialogContents;

        BEGIN_DERIVED_XML_STORABLE_OBJECT(PauseAmbianceAction, Action)
        END_XML_STORABLE_OBJECT()

    public:
        PauseAmbianceAction() : Action() {}
        PauseAmbianceAction(Staging::Conversation::PauseAmbianceAction */*pStagingPauseAmbianceAction*/) : PauseAmbianceAction() {}

        void WriteToCaseXml(XmlWriter *pWriter, int& currentActionIndex) override;

        virtual ActionType GetType() override { return ActionType::PauseAmbiance; }

        static EditorDialogContents<Conversation::Action, Conversation::State> * CreateEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::Action *pActionToEdit);

    protected:
        virtual void CopyPropertiesImpl(Action *pOther, bool isForEdit) override;
        virtual QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> GetListItemsImpl(int indentLevel) override;
    };

    class ResumeAmbianceAction : public Action
    {
        friend class ResumeAmbianceActionListItem;
        friend class ResumeAmbianceActionEditorDialogContents;

        BEGIN_DERIVED_XML_STORABLE_OBJECT(ResumeAmbianceAction, Action)
        END_XML_STORABLE_OBJECT()

    public:
        ResumeAmbianceAction() : Action() {}
        ResumeAmbianceAction(Staging::Conversation::ResumeAmbianceAction */*pStagingResumeAmbianceAction*/) : ResumeAmbianceAction() {}

        void WriteToCaseXml(XmlWriter *pWriter, int& currentActionIndex) override;

        virtual ActionType GetType() override { return ActionType::ResumeAmbiance; }

        static EditorDialogContents<Conversation::Action, Conversation::State> * CreateEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::Action *pActionToEdit);

    protected:
        virtual void CopyPropertiesImpl(Action *pOther, bool isForEdit) override;
        virtual QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> GetListItemsImpl(int indentLevel) override;
    };

    class StopAmbianceAction : public Action
    {
        friend class StopAmbianceActionListItem;
        friend class StopAmbianceActionEditorDialogContents;

        BEGIN_DERIVED_XML_STORABLE_OBJECT(StopAmbianceAction, Action)
            XML_STORABLE_BOOL(isInstant)
            XML_STORABLE_BOOL(preserveAmbiance)
        END_XML_STORABLE_OBJECT()

    public:
        StopAmbianceAction();
        StopAmbianceAction(Staging::Conversation::StopAmbianceAction *pStagingStopAmbianceAction);

        void WriteToCaseXml(XmlWriter *pWriter, int& currentActionIndex) override;

        virtual ActionType GetType() override { return ActionType::StopAmbiance; }

        static EditorDialogContents<Conversation::Action, Conversation::State> * CreateEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::Action *pActionToEdit);

    protected:
        virtual void CopyPropertiesImpl(Action *pOther, bool isForEdit) override;
        virtual QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> GetListItemsImpl(int indentLevel) override;

    private:
        bool isInstant;
        bool preserveAmbiance;
    };

    class StartAnimationAction : public Action
    {
        friend class StartAnimationActionListItem;
        friend class StartAnimationActionEditorDialogContents;

        BEGIN_DERIVED_XML_STORABLE_OBJECT(StartAnimationAction, Action)
            XML_STORABLE_TEXT(animationId)
        END_XML_STORABLE_OBJECT()

    public:
        StartAnimationAction() : Action() {}
        StartAnimationAction(Staging::Conversation::StartAnimationAction *pStartAnimationAction);

        void WriteToCaseXml(XmlWriter *pWriter, int& currentActionIndex) override;

        virtual ActionType GetType() override { return ActionType::StartAnimation; }

        static EditorDialogContents<Conversation::Action, Conversation::State> * CreateEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::Action *pActionToEdit);

    protected:
        virtual void CopyPropertiesImpl(Action *pOther, bool isForEdit) override;
        virtual QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> GetListItemsImpl(int indentLevel) override;

    private:
        QString animationId;
    };

    class StopAnimationAction : public Action
    {
        friend class StopAnimationActionListItem;
        friend class StopAnimationActionEditorDialogContents;

        BEGIN_DERIVED_XML_STORABLE_OBJECT(StopAnimationAction, Action)
        END_XML_STORABLE_OBJECT()

    public:
        StopAnimationAction() : Action() {}
        StopAnimationAction(Staging::Conversation::StopAnimationAction */*pStopAnimationAction*/) {}

        void WriteToCaseXml(XmlWriter *pWriter, int& currentActionIndex) override;

        virtual ActionType GetType() override { return ActionType::StopAnimation; }

        static EditorDialogContents<Conversation::Action, Conversation::State> * CreateEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::Action *pActionToEdit);

    protected:
        virtual void CopyPropertiesImpl(Action *pOther, bool isForEdit) override;
        virtual QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> GetListItemsImpl(int indentLevel) override;
    };

    class SetPartnerAction : public ActionWithNotification
    {
        friend class SetPartnerActionListItem;
        friend class SetPartnerActionEditorDialogContents;

        BEGIN_DERIVED_XML_STORABLE_OBJECT(SetPartnerAction, ActionWithNotification)
        END_XML_STORABLE_OBJECT()

    public:
        SetPartnerAction() : ActionWithNotification() {}
        SetPartnerAction(Staging::Conversation::SetPartnerAction *pSetPartnerAction);

        void WriteToCaseXml(XmlWriter *pWriter, int& currentActionIndex) override;

        virtual ActionType GetType() override { return ActionType::SetPartner; }

        static EditorDialogContents<Conversation::Action, Conversation::State> * CreateEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::Action *pActionToEdit);

    protected:
        virtual void CopyPropertiesImpl(Action *pOther, bool isForEdit) override;
        virtual QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> GetListItemsImpl(int indentLevel) override;
    };

    class GoToPresentWrongEvidenceAction : public Action
    {
        friend class GoToPresentWrongEvidenceActionListItem;
        friend class GoToPresentWrongEvidenceActionEditorDialogContents;

        BEGIN_DERIVED_XML_STORABLE_OBJECT(GoToPresentWrongEvidenceAction, Action)
        END_XML_STORABLE_OBJECT()

    public:
        GoToPresentWrongEvidenceAction() : Action() {}
        GoToPresentWrongEvidenceAction(Staging::Conversation::GoToPresentWrongEvidenceAction */*pGoToPresentWrongEvidenceAction*/) {}

        void WriteToCaseXml(XmlWriter *pWriter, int& currentActionIndex) override;

        virtual ActionType GetType() override { return ActionType::GoToPresentWrongEvidence; }

        static EditorDialogContents<Conversation::Action, Conversation::State> * CreateEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::Action *pActionToEdit);

    protected:
        virtual void CopyPropertiesImpl(Action *pOther, bool isForEdit) override;
        virtual QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> GetListItemsImpl(int indentLevel) override;
    };

    class LockConversationAction : public Action
    {
        friend class LockConversationActionListItem;
        friend class LockConversationActionEditorDialogContents;

        BEGIN_DERIVED_XML_STORABLE_OBJECT(LockConversationAction, Action)
            XML_STORABLE_CUSTOM_OBJECT(pUnlockCondition, UnlockCondition::CreateFromXml)
        END_XML_STORABLE_OBJECT()

    public:
        LockConversationAction();
        LockConversationAction(Staging::Conversation::LockConversationAction *pLockConversationAction);
        virtual ~LockConversationAction();

        void WriteToCaseXml(XmlWriter *pWriter, int& currentActionIndex) override;

        virtual ActionType GetType() override { return ActionType::LockConversation; }

        static EditorDialogContents<Conversation::Action, Conversation::State> * CreateEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::Action *pActionToEdit);

    protected:
        virtual void CopyPropertiesImpl(Action *pOther, bool isForEdit) override;
        virtual QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> GetListItemsImpl(int indentLevel) override;

    private:
        UnlockCondition *pUnlockCondition;
    };

    class ExitEncounterAction : public Action
    {
        friend class ExitEncounterActionListItem;
        friend class ExitEncounterActionEditorDialogContents;

        BEGIN_DERIVED_XML_STORABLE_OBJECT(ExitEncounterAction, Action)
        END_XML_STORABLE_OBJECT()

    public:
        ExitEncounterAction() : Action() {}
        ExitEncounterAction(Staging::Conversation::ExitEncounterAction */*pExitEncounterAction*/) {}

        void WriteToCaseXml(XmlWriter *pWriter, int& currentActionIndex) override;

        virtual ActionType GetType() override { return ActionType::ExitEncounter; }

        static EditorDialogContents<Conversation::Action, Conversation::State> * CreateEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::Action *pActionToEdit);

    protected:
        virtual void CopyPropertiesImpl(Action *pOther, bool isForEdit) override;
        virtual QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> GetListItemsImpl(int indentLevel) override;
    };

    class MoveToLocationAction : public Action
    {
        friend class MoveToLocationActionListItem;
        friend class MoveToLocationActionEditorDialogContents;

        BEGIN_DERIVED_XML_STORABLE_OBJECT(MoveToLocationAction, Action)
            XML_STORABLE_TEXT(newAreaId)
            XML_STORABLE_TEXT(newLocationId)
            XML_STORABLE_TEXT(transitionId)
        END_XML_STORABLE_OBJECT()

    public:
        MoveToLocationAction() : Action() {}
        MoveToLocationAction(Staging::Conversation::MoveToLocationAction *pMoveToLocationAction);

        void WriteToCaseXml(XmlWriter *pWriter, int& currentActionIndex) override;

        virtual ActionType GetType() override { return ActionType::MoveToLocation; }

        static EditorDialogContents<Conversation::Action, Conversation::State> * CreateEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::Action *pActionToEdit);

    protected:
        virtual void CopyPropertiesImpl(Action *pOther, bool isForEdit) override;
        virtual QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> GetListItemsImpl(int indentLevel) override;

    private:
        QString newAreaId;
        QString newLocationId;
        QString transitionId;
    };

    class MoveToZoomedViewAction : public Action
    {
        friend class MoveToZoomedViewActionListItem;
        friend class MoveToZoomedViewActionEditorDialogContents;

        BEGIN_DERIVED_XML_STORABLE_OBJECT(MoveToZoomedViewAction, Action)
            XML_STORABLE_TEXT(zoomedViewId)
        END_XML_STORABLE_OBJECT()

    public:
        MoveToZoomedViewAction() : Action() {}
        MoveToZoomedViewAction(Staging::Conversation::MoveToZoomedViewAction *pMoveToZoomedViewAction);

        void WriteToCaseXml(XmlWriter *pWriter, int& currentActionIndex) override;

        virtual ActionType GetType() override { return ActionType::MoveToZoomedView; }

        static EditorDialogContents<Conversation::Action, Conversation::State> * CreateEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::Action *pActionToEdit);

    protected:
        virtual void CopyPropertiesImpl(Action *pOther, bool isForEdit) override;
        virtual QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> GetListItemsImpl(int indentLevel) override;

    private:
        QString zoomedViewId;
    };

    class EndCaseAction : public Action
    {
        friend class EndCaseActionListItem;
        friend class EndCaseActionEditorDialogContents;

        BEGIN_DERIVED_XML_STORABLE_OBJECT(EndCaseAction, Action)
            XML_STORABLE_BOOL(completesCase)
        END_XML_STORABLE_OBJECT()

    public:
        EndCaseAction();
        EndCaseAction(Staging::Conversation::EndCaseAction *pEndCaseAction);

        void WriteToCaseXml(XmlWriter *pWriter, int& currentActionIndex) override;

        virtual ActionType GetType() override { return ActionType::EndCase; }

        static EditorDialogContents<Conversation::Action, Conversation::State> * CreateEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::Action *pActionToEdit);

    protected:
        virtual void CopyPropertiesImpl(Action *pOther, bool isForEdit) override;
        virtual QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> GetListItemsImpl(int indentLevel) override;

    private:
        bool completesCase;
    };

    class MultipleChoiceAction : public Action
    {
        friend class Confrontation;
        friend class MultipleChoiceActionListItem;
        friend class MultipleChoiceActionEditorDialogContents;

        BEGIN_DERIVED_XML_STORABLE_OBJECT(MultipleChoiceAction, Action)
            XML_STORABLE_CUSTOM_OBJECT_LIST(options, Option::CreateFromXml)
        END_XML_STORABLE_OBJECT()

    public:
        MultipleChoiceAction() : Action() {}
        MultipleChoiceAction(Staging::Conversation::BeginMultipleChoiceAction */*pBeginMultipleChoiceAction*/) : MultipleChoiceAction() {}
        virtual ~MultipleChoiceAction();

        void WriteToCaseXml(XmlWriter *pWriter, int& currentActionIndex) override;

        virtual ActionType GetType() override { return ActionType::MultipleChoice; }

        void AddOption(const QString &text, const QList<Action *> &actions);

        static EditorDialogContents<Conversation::Action, Conversation::State> * CreateEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::Action *pActionToEdit);

    protected:
        virtual void CopyPropertiesImpl(Action *pOther, bool isForEdit) override;
        virtual void ExchangeListItemBaseOwnershipImpl(Action *pOther) override;
        virtual QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> GetListItemsImpl(int indentLevel) override;

    private:
        class Option : public XmlStorableObject
        {
            friend class Confrontation;

            BEGIN_XML_STORABLE_OBJECT(Option)
                XML_STORABLE_TEXT(text)
                XML_STORABLE_CUSTOM_OBJECT_LIST(actions, Action::CreateFromXml)
            END_XML_STORABLE_OBJECT()

        public:
            Option() {}
            Option(const QString &text);
            Option(const QString &text, const QList<Action *> &actions);
            virtual ~Option();

            Option * Clone();

            QString GetText() { return text; }
            void SetText(const QString &text) { this->text = text; }

            QList<Action *> & GetActions() { return actions; }

            void ReplaceAction(Action *pNewAction, Action *pOldAction);
            void ExchangeListItemBaseOwnership(Option *pOther);

            static Option * CreateFromXml(XmlReader *pReader)
            {
                return new Option(pReader);
            }

        private:
            QString text;
            QList<Action *> actions;
        };

        QList<Option *> options;
    };

    class ExitMultipleChoiceAction : public Action
    {
        friend class ExitMultipleChoiceActionListItem;
        friend class ExitMultipleChoiceActionEditorDialogContents;

        BEGIN_DERIVED_XML_STORABLE_OBJECT(ExitMultipleChoiceAction, Action)
        END_XML_STORABLE_OBJECT()

    public:
        ExitMultipleChoiceAction() {}
        ExitMultipleChoiceAction(Staging::Conversation::ExitMultipleChoiceAction */*pExitMultipleChoiceAction*/) : ExitMultipleChoiceAction() {}

        void WriteToCaseXml(XmlWriter *pWriter, int& currentActionIndex) override;

        virtual ActionType GetType() override { return ActionType::ExitMultipleChoice; }

        static EditorDialogContents<Conversation::Action, Conversation::State> * CreateEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::Action *pActionToEdit);

    protected:
        virtual void CopyPropertiesImpl(Action *pOther, bool isForEdit) override;
        virtual QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> GetListItemsImpl(int indentLevel) override;
    };

    class EnableFastForwardAction : public Action
    {
        friend class EnableFastForwardActionListItem;
        friend class EnableFastForwardActionEditorDialogContents;

        BEGIN_DERIVED_XML_STORABLE_OBJECT(EnableFastForwardAction, Action)
        END_XML_STORABLE_OBJECT()

    public:
        EnableFastForwardAction() : Action() {}
        EnableFastForwardAction(Staging::Conversation::EnableFastForwardAction */*pEnableFastForwardAction*/) : EnableFastForwardAction() {}

        void WriteToCaseXml(XmlWriter *pWriter, int& currentActionIndex) override;

        virtual ActionType GetType() override { return ActionType::EnableFastForward; }

        static EditorDialogContents<Conversation::Action, Conversation::State> * CreateEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::Action *pActionToEdit);

    protected:
        virtual void CopyPropertiesImpl(Action *pOther, bool isForEdit) override;
        virtual QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> GetListItemsImpl(int indentLevel) override;
    };

    class DisableFastForwardAction : public Action
    {
        friend class DisableFastForwardActionListItem;
        friend class DisableFastForwardActionEditorDialogContents;

        BEGIN_DERIVED_XML_STORABLE_OBJECT(DisableFastForwardAction, Action)
        END_XML_STORABLE_OBJECT()

    public:
        DisableFastForwardAction() : Action() {}
        DisableFastForwardAction(Staging::Conversation::DisableFastForwardAction */*pDisableFastForwardAction*/) : DisableFastForwardAction() {}

        void WriteToCaseXml(XmlWriter *pWriter, int& currentActionIndex) override;

        virtual ActionType GetType() override { return ActionType::DisableFastForward; }

        static EditorDialogContents<Conversation::Action, Conversation::State> * CreateEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::Action *pActionToEdit);

    protected:
        virtual void CopyPropertiesImpl(Action *pOther, bool isForEdit) override;
        virtual QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> GetListItemsImpl(int indentLevel) override;
    };

    class BeginBreakdownAction : public Action
    {
        friend class BeginBreakdownActionListItem;
        friend class BeginBreakdownActionEditorDialogContents;

        BEGIN_DERIVED_XML_STORABLE_OBJECT(BeginBreakdownAction, Action)
            XML_STORABLE_ENUM(characterPosition, CharacterPositionToString, StringToCharacterPosition)
        END_XML_STORABLE_OBJECT()

    public:
        BeginBreakdownAction();
        BeginBreakdownAction(Staging::Conversation::BeginBreakdownAction *pBeginBreakdownAction);

        void WriteToCaseXml(XmlWriter *pWriter, int& currentActionIndex) override;

        virtual ActionType GetType() override { return ActionType::BeginBreakdown; }

        static EditorDialogContents<Conversation::Action, Conversation::State> * CreateEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::Action *pActionToEdit);

    protected:
        virtual void CopyPropertiesImpl(Action *pOther, bool isForEdit) override;
        virtual QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> GetListItemsImpl(int indentLevel) override;

    private:
        CharacterPosition characterPosition;
    };

    class EndBreakdownAction : public Action
    {
        friend class EndBreakdownActionListItem;
        friend class EndBreakdownActionEditorDialogContents;

        BEGIN_DERIVED_XML_STORABLE_OBJECT(EndBreakdownAction, Action)
        END_XML_STORABLE_OBJECT()

    public:
        EndBreakdownAction() {}
        EndBreakdownAction(Staging::Conversation::EndBreakdownAction */*pEndBreakdownAction*/) : EndBreakdownAction() {}

        void WriteToCaseXml(XmlWriter *pWriter, int& currentActionIndex) override;

        virtual ActionType GetType() override { return ActionType::EndBreakdown; }

        static EditorDialogContents<Conversation::Action, Conversation::State> * CreateEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::Action *pActionToEdit);

    protected:
        virtual void CopyPropertiesImpl(Action *pOther, bool isForEdit) override;
        virtual QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> GetListItemsImpl(int indentLevel) override;
    };

    ObjectListWidget<Conversation::Action, Conversation::State, Conversation::ActionType> * GetObjectListWidget();

protected:
    QString id;
    QString name;

    QList<Action *> actions;

    Encounter *pOwningEncounter;
};

class Interrogation : public Conversation
{
    BEGIN_DERIVED_XML_STORABLE_OBJECT(Interrogation, Conversation)
    END_XML_STORABLE_OBJECT()

public:
    class ShowInterrogationAction;

    Interrogation() : Conversation() { }
    Interrogation(QString id) : Conversation(id) { }

protected:
    Interrogation(Staging::Interrogation *pStagingInterrogation);

public:
    static Interrogation * CreateFromStaging(Staging::Interrogation *pStagingInterrogation)
    {
        Interrogation *pInterrogation = new Interrogation(pStagingInterrogation);
        pInterrogation->PopulateActionsFromStaging(pInterrogation->actions, pStagingInterrogation->ActionList);

        return pInterrogation;
    }

    static Interrogation * CreateFromXml(XmlReader *pReader)
    {
        return new Interrogation(pReader);
    }

    virtual void WriteToCaseXml(XmlWriter *pWriter);

    static QString GetObjectAdditionString() { return QString("interrogation"); }
    static QString GetListTitle() { return QString("Interrogations"); }
    static bool GetIsMainList() { return false; }
    static bool GetAllowsNewObjects() { return true; }

protected:
    virtual void PopulateActionFromStaging(QList<Action *> &actions, const QList<Staging::Conversation::Action *> &stagingActions, int &indexInStagingActions, int initialIndexInFullList);

public:
    class InterrogationRepeatAction : public Action
    {
        friend class Interrogation;
        friend class InterrogationRepeatActionListItem;
        friend class InterrogationRepeatActionDialogContents;

        BEGIN_DERIVED_XML_STORABLE_OBJECT(InterrogationRepeatAction, Action)
            XML_STORABLE_CUSTOM_OBJECT_LIST(actionsToRepeat, Action::CreateFromXml)
            XML_STORABLE_CUSTOM_OBJECT_LIST(wrongEvidencePresentedActions, Action::CreateFromXml)
            XML_STORABLE_CUSTOM_OBJECT_LIST(wrongPartnerUsedActions, Action::CreateFromXml)
            XML_STORABLE_CUSTOM_OBJECT_LIST(endRequestedActions, Action::CreateFromXml)
        END_XML_STORABLE_OBJECT()

    public:
        InterrogationRepeatAction() : Action() {}
        InterrogationRepeatAction(Staging::Interrogation::BeginInterrogationRepeatAction */*pBeginInterrogationRepeatAction*/) : InterrogationRepeatAction() {}
        virtual ~InterrogationRepeatAction();

        void WriteToCaseXml(XmlWriter *pWriter, int& currentActionIndex) override;

        virtual ActionType GetType() override { return ActionType::InterrogationRepeat; }

        virtual void ReplaceAction(Action *pNewAction, Action *pOldAction) override;
        virtual void UpdateAndCacheConversationState(State &currentState) override;
        virtual void PreloadAudio() override;

        static EditorDialogContents<Conversation::Action, Conversation::State> * CreateEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::Action *pActionToEdit);

    protected:
        virtual void CopyPropertiesImpl(Action *pOther, bool isForEdit) override;
        virtual void ExchangeListItemBaseOwnershipImpl(Action *pOther) override;
        virtual QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> GetListItemsImpl(int indentLevel) override;

    private:
        QList<Action *> actionsToRepeat;
        QList<Action *> wrongEvidencePresentedActions;
        QList<Action *> wrongPartnerUsedActions;
        QList<Action *> endRequestedActions;
    };

    class ShowInterrogationAction : public ShowDialogAction
    {
        friend class Interrogation;
        friend class ShowDialogActionListItem;
        friend class ShowDialogActionEditorDialogContents;
        friend class ShowInterrogationActionListItem;

        BEGIN_DERIVED_XML_STORABLE_OBJECT(ShowInterrogationAction, ShowDialogAction)
            XML_STORABLE_TEXT(conditionFlag)
            XML_STORABLE_CUSTOM_OBJECT_LIST(pressForInfoActions, Action::CreateFromXml)
        END_XML_STORABLE_OBJECT()

    public:
        ShowInterrogationAction() : ShowDialogAction() {}
        ShowInterrogationAction(Staging::Interrogation::ShowInterrogationAction *pShowInterrogationAction);
        virtual ~ShowInterrogationAction();

        void WriteToCaseXml(XmlWriter *pWriter, int& currentActionIndex) override;

        virtual ActionType GetType() override { return ActionType::ShowInterrogation; }

        virtual void ReplaceAction(Action *pNewAction, Action *pOldAction) override;
        virtual void UpdateAndCacheConversationState(State &currentState) override;
        virtual void PreloadAudio() override;

        void SaveElementsToXml(XmlWriter *pWriter);
        void LoadElementsFromXml(XmlReader *pReader);

        static EditorDialogContents<Conversation::Action, Conversation::State> * CreateEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::Action *pActionToEdit);

    protected:
        virtual void CopyPropertiesImpl(Action *pOther, bool isForEdit) override;
        virtual void ExchangeListItemBaseOwnershipImpl(Action *pOther) override;
        virtual QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> GetListItemsImpl(int indentLevel) override;

    private:
        QList<Action *> pressForInfoActions;
        QMap<QString, QList<Action *> > evidencePresentedActionsByEvidenceId;
        QMap<QString, QList<Action *> > partnerUsedActionsByPartnerId;

        QString conditionFlag;

        QList<QString> originalEvidenceIdList;
        QMap<QString, QString> originalEvidenceIdToNewEvidenceIdMap;
        QList<QString> originalPartnerIdList;
        QMap<QString, QString> originalPartnerIdToNewPartnerIdMap;
    };

    class ExitInterrogationRepeatAction : public Action
    {
        friend class ExitInterrogationRepeatActionListItem;
        friend class ExitInterrogationRepeatActionDialogContents;

        BEGIN_DERIVED_XML_STORABLE_OBJECT(ExitInterrogationRepeatAction, Action)
        END_XML_STORABLE_OBJECT()

    public:
        ExitInterrogationRepeatAction() : Action() {}
        ExitInterrogationRepeatAction(Staging::Interrogation::ExitInterrogationRepeatAction */*pExitInterrogationRepeatAction*/) : ExitInterrogationRepeatAction() {}

        void WriteToCaseXml(XmlWriter *pWriter, int& currentActionIndex) override;

        virtual ActionType GetType() override { return ActionType::ExitInterrogationRepeat; }

        static EditorDialogContents<Conversation::Action, Conversation::State> * CreateEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::Action *pActionToEdit);

    protected:
        virtual void CopyPropertiesImpl(Action *pOther, bool isForEdit) override;
        virtual QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> GetListItemsImpl(int indentLevel) override;
    };
};

class Confrontation : public Interrogation
{
    friend class Conversation;

    BEGIN_DERIVED_XML_STORABLE_OBJECT(Confrontation, Interrogation)
        XML_STORABLE_TEXT(playerCharacterId)
        XML_STORABLE_CUSTOM_OBJECT(playerIconOffset, Vector2::CreateFromXml)
        XML_STORABLE_INT(playerInitialHealth)
        XML_STORABLE_TEXT(opponentCharacterId)
        XML_STORABLE_CUSTOM_OBJECT(opponentIconOffset, Vector2::CreateFromXml)
        XML_STORABLE_INT(opponentInitialHealth)
    END_XML_STORABLE_OBJECT()

public:
    Confrontation() : Interrogation() { }
    Confrontation(QString id) : Interrogation(id) { }

private:
    Confrontation(Staging::Confrontation *pStagingConfrontation);

public:
    virtual void WriteToCaseXml(XmlWriter *pWriter);
    virtual void WriteToCaseXmlCore(XmlWriter *pWriter);

public:
    static Confrontation * CreateFromStaging(Staging::Confrontation *pStagingConfrontation)
    {
        Confrontation *pConfrontation = new Confrontation(pStagingConfrontation);
        pConfrontation->PopulateActionsFromStaging(pConfrontation->actions, pStagingConfrontation->ActionList);

        return pConfrontation;
    }

    static Confrontation * CreateFromXml(XmlReader *pReader)
    {
        return new Confrontation(pReader);
    }

    static QString GetObjectAdditionString() { return QString("confrontation"); }
    static QString GetListTitle() { return QString("Confrontations"); }
    static bool GetIsMainList() { return false; }
    static bool GetAllowsNewObjects() { return true; }

protected:
    virtual void PopulateActionFromStaging(QList<Action *> &actions, const QList<Staging::Conversation::Action *> &stagingActions, int &indexInStagingActions, int initialIndexInFullList);

public:
    class Topic : public XmlStorableObject
    {
        BEGIN_NAMED_XML_STORABLE_OBJECT(Topic, ConfrontationTopic)
            XML_STORABLE_TEXT(id)
            XML_STORABLE_TEXT(name)
            XML_STORABLE_BOOL(isEnabledAtStart)
            XML_STORABLE_CUSTOM_OBJECT_LIST(actions, Action::CreateFromXml)
        END_XML_STORABLE_OBJECT()

    public:
        Topic();
        Topic(QString id, QString name, bool isEnabledAtStart);
        Topic(Staging::Confrontation::Topic *pStagingTopic);
        virtual ~Topic();

        Topic * Clone();

        void AddActions(const QList<Action *> &actions);

        QString GetId() { return id; }
        void SetId(QString id) { this->id = id; }

        QString GetName() { return name; }
        void SetName(QString name) { this->name = name; }

        bool GetIsEnabledAtStart() { return isEnabledAtStart; }
        void SetIsEnabledAtStart(bool isEnabledAtStart) { this->isEnabledAtStart = isEnabledAtStart; }

        QList<Action *> & GetActions() { return actions; }

        void ReplaceAction(Action *pNewAction, Action *pOldAction);
        void ExchangeListItemBaseOwnership(Topic *pOther);
        void UpdateAndCacheConversationState(State &currentState);
        void PreloadAudio();

        static Topic * CreateFromXml(XmlReader *pReader)
        {
            return new Topic(pReader);
        }

    private:
        QString id;
        QString name;
        bool isEnabledAtStart;
        QList<Action *> actions;
    };

    class ConfrontationTopicSelectionAction : public Action
    {
        friend class Confrontation;
        friend class ConfrontationTopicSelectionActionListItem;
        friend class ConfrontationTopicSelectionActionEditorDialogContents;

        BEGIN_DERIVED_XML_STORABLE_OBJECT(ConfrontationTopicSelectionAction, Action)
            XML_STORABLE_CUSTOM_OBJECT(pInitialTopic, Topic::CreateFromXml)
            XML_STORABLE_CUSTOM_OBJECT_LIST(topics, Topic::CreateFromXml)
            XML_STORABLE_CUSTOM_OBJECT_LIST(playerDefeatedActions, Action::CreateFromXml)
        END_XML_STORABLE_OBJECT()

    public:
        ConfrontationTopicSelectionAction();
        ConfrontationTopicSelectionAction(Staging::Confrontation::BeginConfrontationTopicSelectionAction */*pStagingBeginConfrontationTopicSelectionAction*/) : ConfrontationTopicSelectionAction() {}
        virtual ~ConfrontationTopicSelectionAction();

        void WriteToCaseXml(XmlWriter *pWriter, int& currentActionIndex) override;

        virtual ActionType GetType() override { return ActionType::ConfrontationTopicSelection; }

        void AddTopic(Staging::Confrontation::Topic *pStagingTopic, const QList<Action *> &actions, bool isInitialTopic);

        virtual void ReplaceAction(Action *pNewAction, Action *pOldAction) override;
        virtual void UpdateAndCacheConversationState(State &currentState) override;
        virtual void PreloadAudio() override;

        static EditorDialogContents<Conversation::Action, Conversation::State> * CreateEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::Action *pActionToEdit);

    protected:
        virtual void CopyPropertiesImpl(Action *pOther, bool isForEdit) override;
        virtual void ExchangeListItemBaseOwnershipImpl(Action *pOther) override;
        virtual QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> GetListItemsImpl(int indentLevel) override;

    private:
        Topic *pInitialTopic;
        QList<Topic *> topics;

        QList<Action *> playerDefeatedActions;
    };

    class EnableTopicAction : public Action
    {
        friend class EnableTopicActionListItem;
        friend class EnableTopicActionEditorDialogContents;

        BEGIN_DERIVED_XML_STORABLE_OBJECT(EnableTopicAction, Action)
            XML_STORABLE_TEXT(topicId)
        END_XML_STORABLE_OBJECT()

    public:
        EnableTopicAction() : Action() {}
        EnableTopicAction(Staging::Confrontation::EnableTopicAction *pEnableTopicAction);

        void WriteToCaseXml(XmlWriter *pWriter, int& currentActionIndex) override;

        virtual ActionType GetType() override { return ActionType::EnableTopic; }

        static EditorDialogContents<Conversation::Action, Conversation::State> * CreateEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::Action *pActionToEdit);

    protected:
        virtual void CopyPropertiesImpl(Action *pOther, bool isForEdit) override;
        virtual QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> GetListItemsImpl(int indentLevel) override;

    private:
        QString topicId;
    };

    class RestartDecisionAction : public Action
    {
        friend class Confrontation;
        friend class RestartDecisionActionListItem;
        friend class RestartDecisionActionEditorDialogContents;

        BEGIN_DERIVED_XML_STORABLE_OBJECT(RestartDecisionAction, Action)
            XML_STORABLE_CUSTOM_OBJECT_LIST(restartActions, Action::CreateFromXml)
            XML_STORABLE_CUSTOM_OBJECT_LIST(quitActions, Action::CreateFromXml)
        END_XML_STORABLE_OBJECT()

    public:
        RestartDecisionAction() : Action() {}
        RestartDecisionAction(Staging::Conversation::BeginMultipleChoiceAction */*pBeginMultipleChoiceAction*/) : RestartDecisionAction() {}
        virtual ~RestartDecisionAction();

        void WriteToCaseXml(XmlWriter *pWriter, int& currentActionIndex) override;

        virtual ActionType GetType() override { return ActionType::RestartDecision; }

        virtual void ReplaceAction(Action *pNewAction, Action *pOldAction) override;
        virtual void UpdateAndCacheConversationState(State &currentState) override;
        virtual void PreloadAudio() override;

        static EditorDialogContents<Conversation::Action, Conversation::State> * CreateEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::Action *pActionToEdit);

    protected:
        virtual void CopyPropertiesImpl(Action *pOther, bool isForEdit) override;
        virtual void ExchangeListItemBaseOwnershipImpl(Action *pOther) override;
        virtual QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> GetListItemsImpl(int indentLevel) override;

    private:
        QList<Action *> restartActions;
        QList<Action *> quitActions;
    };

    class RestartConfrontationAction : public Action
    {
        friend class RestartConfrontationActionListItem;
        friend class RestartConfrontationActionEditorDialogContents;

        BEGIN_DERIVED_XML_STORABLE_OBJECT(RestartConfrontationAction, Action)
        END_XML_STORABLE_OBJECT()

    public:
        RestartConfrontationAction() : Action() {}
        RestartConfrontationAction(Staging::Confrontation::RestartConfrontationAction */*pRestartConfrontationAction*/) : RestartConfrontationAction() {}

        void WriteToCaseXml(XmlWriter *pWriter, int& currentActionIndex) override;

        virtual ActionType GetType() override { return ActionType::RestartConfrontation; }

        static EditorDialogContents<Conversation::Action, Conversation::State> * CreateEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::Action *pActionToEdit);

    protected:
        virtual void CopyPropertiesImpl(Action *pOther, bool isForEdit) override;
        virtual QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> GetListItemsImpl(int indentLevel) override;
    };

private:
    QString playerCharacterId;
    Vector2 playerIconOffset;
    int playerInitialHealth;
    QString opponentCharacterId;
    Vector2 opponentIconOffset;
    int opponentInitialHealth;
};

class ConversationActionEditorDialogContentsImplementation : public EditorDialogContents<Conversation::Action, Conversation::State>
{
public:
    ConversationActionEditorDialogContentsImplementation(const Conversation::State &stateBeforeAction, Conversation::Action */*pActionToEdit*/)
        : EditorDialogContents<Conversation::Action, Conversation::State>(stateBeforeAction, NULL)
    {
    }

    static EditorDialogContents<Conversation::Action, Conversation::State> * Create(const Conversation::State &stateBeforeAction, Conversation::Action *pActionToEdit)
    {
        return new ConversationActionEditorDialogContentsImplementation(stateBeforeAction, pActionToEdit);
    }

    bool ValidateFields() override { return true; }
};

class CharacterChangeActionListItem : public Conversation::Action::ActionListItem<Conversation::CharacterChangeAction>
{
    Q_OBJECT

public:
    explicit CharacterChangeActionListItem(Conversation::CharacterChangeAction *pAction, int indentLevel)
        : Conversation::Action::ActionListItem<Conversation::CharacterChangeAction>(pAction, indentLevel)
    {
    }

    QString GetDisplayString() override;
};

class CharacterChangeActionEditorDialogContents : public EditorDialogContents<Conversation::Action, Conversation::State>
{
    Q_OBJECT

public:
    CharacterChangeActionEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::CharacterChangeAction *pActionToEdit = NULL);

public slots:
    void CharacterPositionComboBoxCurrentIndexChanged(const QString& newPosition);
    void CharacterSelectorCharacterSelected(const QString &newCharacterId);
    void CharacterEmotionComboBoxCurrentIndexChanged(const QString& newEmotionId);

    bool ValidateFields() override;

private:
    Conversation::CharacterChangeAction *pCharacterChangeAction;

    QString currentCharacterId;

    QLabel *pCharacterEmotionLabel;
    QComboBox *pCharacterEmotionComboBox;
};

class SetFlagActionListItem : public Conversation::Action::ActionListItem<Conversation::SetFlagAction>
{
    Q_OBJECT

public:
    explicit SetFlagActionListItem(Conversation::SetFlagAction *pAction, int indentLevel)
        : Conversation::Action::ActionListItem<Conversation::SetFlagAction>(pAction, indentLevel)
    {
    }

    QString GetDisplayString() override;
};

class SetFlagActionEditorDialogContents : public EditorDialogContents<Conversation::Action, Conversation::State>
{
    Q_OBJECT

public:
    SetFlagActionEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::SetFlagAction *pActionToEdit = NULL);

public slots:
    void FlagEditorFlagSelected(const QString& newFlagId);

protected:
    bool ValidateFields() override;

private:
    Conversation::SetFlagAction *pSetFlagAction;
};

class BranchOnConditionActionListItem : public Conversation::Action::ActionListItem<Conversation::BranchOnConditionAction>
{
    Q_OBJECT

public:
    explicit BranchOnConditionActionListItem(Conversation::BranchOnConditionAction *pAction, int indentLevel)
        : Conversation::Action::ActionListItem<Conversation::BranchOnConditionAction>(pAction, indentLevel)
    {
    }

    bool GetCanHaveChildren() override { return true; }
    QString GetDisplayString() override;
};

class BranchOnConditionActionEditorDialogContents : public EditorDialogContents<Conversation::Action, Conversation::State>
{
    Q_OBJECT

public:
    BranchOnConditionActionEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::BranchOnConditionAction *pActionToEdit = NULL);

protected:
    bool ValidateFields() override;
    void FinalizeObject() override;

private:
    Conversation::BranchOnConditionAction *pBranchOnConditionAction;

    ConditionEditor *pConditionEditor;
};

class ShowDialogActionListItem : public Conversation::Action::ActionListItem<Conversation::ShowDialogAction>
{
    Q_OBJECT

public:
    explicit ShowDialogActionListItem(Conversation::ShowDialogAction *pAction, int indentLevel)
        : Conversation::Action::ActionListItem<Conversation::ShowDialogAction>(pAction, indentLevel)
    {
    }

    QString GetDisplayString() override;

protected:
    virtual QString GetHeaderString(const QString &speakerName);
    virtual QString GetFooterString();
};

class ShowDialogActionEditorDialogContents : public EditorDialogContents<Conversation::Action, Conversation::State>
{
    Q_OBJECT

public:
    enum class Type
    {
        ShowDialog,
        MustPresentEvidence,
        ShowInterrogation,
    };

    ShowDialogActionEditorDialogContents(const Conversation::State &stateBeforeAction, Type context, Conversation::ShowDialogAction *pActionToEdit = NULL);

protected:
    bool ValidateFields() override;
    void OnAccepted() override;

public slots:
    void SpeakerComboBoxCurrentIndexChanged(int newIndex);
    void OffscreenCharacterSelectorCharacterSelected(const QString &newCharacterId);
    void RawDialogTextEditTextChanged();
    void VoiceOverCheckBoxToggled(bool isChecked);
    void SelectVoiceOverFilePushButtonClicked();
    void LeadInTimeCheckBoxToggled(bool isChecked);
    void LeadInTimeAmountLineEditEditingFinished();
    void AutomaticallyContinueCheckBoxToggled(bool isChecked);
    void AutomaticallyContinueAmountLineEditEditingFinished();

    void EvidenceSelectorStringChanged(int index, const QString &string);
    void EvidenceSelectorStringAdded(int index, const QString &string);
    void EvidenceSelectorStringRemoved(int index);

    void InterrogationAcceptEvidenceCheckBoxToggled(bool isChecked);
    void InterrogationEvidenceSelectorStringChanged(int index, const QString &string);
    void InterrogationEvidenceSelectorStringAdded(int index, const QString &string);
    void InterrogationEvidenceSelectorStringRemoved(int index);

    void InterrogationAcceptPartnerCheckBoxToggled(bool isChecked);
    void InterrogationPartnerSelectorStringChanged(int index, const QString &string);
    void InterrogationPartnerSelectorStringAdded(int index, const QString &string);
    void InterrogationPartnerSelectorStringRemoved(int index);

protected:
    Conversation::MustPresentEvidenceAction * GetMustPresentEvidenceAction()
    {
        return dynamic_cast<Conversation::MustPresentEvidenceAction *>(pShowDialogAction);
    }

    Interrogation::ShowInterrogationAction * GetShowInterrogationAction()
    {
        return dynamic_cast<Interrogation::ShowInterrogationAction *>(pShowDialogAction);
    }

private:
    Conversation::ShowDialogAction *pShowDialogAction;

    CharacterSelector *pOffscreenCharacterSelector;
    QPlainTextEdit *pRawDialogTextEdit;
    QPushButton *pSelectVoiceOverFilePushButton;
    QLabel *pVoiceOverFilePathLabel;
    QLineEdit *pLeadInTimeAmountLineEdit;
    QLabel *pLeadInTimeMillisecondsLabel;
    QLineEdit *pAutomaticallyContinueAmountLineEdit;
    QLabel *pAutomaticallyContinueMillisecondsLabel;

    MultipleSelectionWidget<EvidenceSelectorMultipleSelectionSelectorWidget> *pAcceptedEvidenceSelector;
    QStringList acceptedEvidenceList;
    MultipleSelectionWidget<CharacterSelectorMultipleSelectionSelectorWidget> *pAcceptedPartnersSelector;
    QStringList acceptedPartnerList;
};

class MustPresentEvidenceActionListItem : public ShowDialogActionListItem
{
    Q_OBJECT

public:
    explicit MustPresentEvidenceActionListItem(Conversation::ShowDialogAction *pAction, int indentLevel)
        : ShowDialogActionListItem(pAction, indentLevel)
    {
    }

    bool GetCanHaveChildren() override { return true; }

protected:
    QString GetHeaderString(const QString &speakerName) override;
    QString GetFooterString() override;

private:
    Conversation::MustPresentEvidenceAction * GetMustPresentEvidenceAction()
    {
        return static_cast<Conversation::MustPresentEvidenceAction *>(pAction);
    }
};

class EnableConversationActionListItem : public Conversation::Action::ActionListItem<Conversation::EnableConversationAction>
{
    Q_OBJECT

public:
    explicit EnableConversationActionListItem(Conversation::EnableConversationAction *pAction, int indentLevel)
        : Conversation::Action::ActionListItem<Conversation::EnableConversationAction>(pAction, indentLevel)
    {
    }

    QString GetDisplayString() override;
};

class EnableConversationActionEditorDialogContents : public EditorDialogContents<Conversation::Action, Conversation::State>
{
    Q_OBJECT

public:
    EnableConversationActionEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::EnableConversationAction *pActionToEdit = NULL);

public slots:
    void EncounterSelectorEncounterSelected(const QString &encounterId);
    void ConversationSelectorConverationSelected(const QString &conversationId);

protected:
    bool ValidateFields() override;

private:
    Conversation::EnableConversationAction *pEnableConversationAction;

    ConversationSelector *pConversationSelector;
};

class EnableEvidenceActionListItem : public Conversation::Action::ActionListItem<Conversation::EnableEvidenceAction>
{
    Q_OBJECT

public:
    explicit EnableEvidenceActionListItem(Conversation::EnableEvidenceAction *pAction, int indentLevel)
        : Conversation::Action::ActionListItem<Conversation::EnableEvidenceAction>(pAction, indentLevel)
    {
    }

    QString GetDisplayString() override;
};

class EnableEvidenceActionEditorDialogContents : public EditorDialogContents<Conversation::Action, Conversation::State>
{
    Q_OBJECT

public:
    EnableEvidenceActionEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::EnableEvidenceAction *pActionToEdit = NULL);

public slots:
    void EvidenceSelectorEvidenceSelected(const QString &evidenceId);

protected:
    bool ValidateFields() override;

private:
    Conversation::EnableEvidenceAction *pEnableEvidenceAction;

    EvidenceSelector *pEvidenceSelector;
};

class UpdateEvidenceActionListItem : public Conversation::Action::ActionListItem<Conversation::UpdateEvidenceAction>
{
    Q_OBJECT

public:
    explicit UpdateEvidenceActionListItem(Conversation::UpdateEvidenceAction *pAction, int indentLevel)
        : Conversation::Action::ActionListItem<Conversation::UpdateEvidenceAction>(pAction, indentLevel)
    {
    }

    QString GetDisplayString() override;
};

class UpdateEvidenceActionEditorDialogContents : public EditorDialogContents<Conversation::Action, Conversation::State>
{
    Q_OBJECT

public:
    UpdateEvidenceActionEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::UpdateEvidenceAction *pActionToEdit = NULL);

public slots:
    void OldEvidenceSelectorEvidenceSelected(const QString &evidenceId);
    void NewEvidenceSelectorEvidenceSelected(const QString &evidenceId);

protected:
    bool ValidateFields() override;

private:
    Conversation::UpdateEvidenceAction *pUpdateEvidenceAction;

    EvidenceSelector *pOldEvidenceSelector;
    EvidenceSelector *pNewEvidenceSelector;
};

class DisableEvidenceActionListItem : public Conversation::Action::ActionListItem<Conversation::DisableEvidenceAction>
{
    Q_OBJECT

public:
    explicit DisableEvidenceActionListItem(Conversation::DisableEvidenceAction *pAction, int indentLevel)
        : Conversation::Action::ActionListItem<Conversation::DisableEvidenceAction>(pAction, indentLevel)
    {
    }

    QString GetDisplayString() override;
};

class DisableEvidenceActionEditorDialogContents : public EditorDialogContents<Conversation::Action, Conversation::State>
{
    Q_OBJECT

public:
    DisableEvidenceActionEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::DisableEvidenceAction *pActionToEdit = NULL);

public slots:
    void EvidenceSelectorEvidenceSelected(const QString &evidenceId);

protected:
    bool ValidateFields() override;

private:
    Conversation::DisableEvidenceAction *pDisableEvidenceAction;

    EvidenceSelector *pEvidenceSelector;
};

class EnableCutsceneActionListItem : public Conversation::Action::ActionListItem<Conversation::EnableCutsceneAction>
{
    Q_OBJECT

public:
    explicit EnableCutsceneActionListItem(Conversation::EnableCutsceneAction *pAction, int indentLevel)
        : Conversation::Action::ActionListItem<Conversation::EnableCutsceneAction>(pAction, indentLevel)
    {
    }

    QString GetDisplayString() override;
};

class EnableCutsceneActionEditorDialogContents : public EditorDialogContents<Conversation::Action, Conversation::State>
{
    Q_OBJECT

public:
    EnableCutsceneActionEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::EnableCutsceneAction *pActionToEdit = NULL);

public slots:
    //void EvidenceSelectorEvidenceSelected(const QString &evidenceId);

protected:
    bool ValidateFields() override;

private:
    Conversation::EnableCutsceneAction *pEnableCutsceneAction;

    //EvidenceSelector *pEvidenceSelector;
};

class PlayBgmActionListItem : public Conversation::Action::ActionListItem<Conversation::PlayBgmAction>
{
    Q_OBJECT

public:
    explicit PlayBgmActionListItem(Conversation::PlayBgmAction *pAction, int indentLevel)
        : Conversation::Action::ActionListItem<Conversation::PlayBgmAction>(pAction, indentLevel)
    {
    }

    QString GetDisplayString() override;
};

class PlayBgmActionEditorDialogContents : public EditorDialogContents<Conversation::Action, Conversation::State>
{
    Q_OBJECT

public:
    PlayBgmActionEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::PlayBgmAction *pActionToEdit = NULL);

public slots:
    void BackgroundMusicSelectorBackgroundMusicSelected(const QString &bgmId);
    void PreserveCheckBoxToggled(bool isChecked);

protected:
    bool ValidateFields() override;

private:
    Conversation::PlayBgmAction *pPlayBgmAction;
};

class PauseBgmActionListItem : public Conversation::Action::ActionListItem<Conversation::PauseBgmAction>
{
    Q_OBJECT

public:
    explicit PauseBgmActionListItem(Conversation::PauseBgmAction *pAction, int indentLevel)
        : Conversation::Action::ActionListItem<Conversation::PauseBgmAction>(pAction, indentLevel)
    {
    }

    QString GetDisplayString() override;
};

class PauseBgmActionEditorDialogContents : public EditorDialogContents<Conversation::Action, Conversation::State>
{
    Q_OBJECT

public:
    PauseBgmActionEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::PauseBgmAction *pActionToEdit = NULL);

protected:
    bool ValidateFields() override;

private:
    Conversation::PauseBgmAction *pPauseBgmAction;
};

class ResumeBgmActionListItem : public Conversation::Action::ActionListItem<Conversation::ResumeBgmAction>
{
    Q_OBJECT

public:
    explicit ResumeBgmActionListItem(Conversation::ResumeBgmAction *pAction, int indentLevel)
        : Conversation::Action::ActionListItem<Conversation::ResumeBgmAction>(pAction, indentLevel)
    {
    }

    QString GetDisplayString() override;
};

class ResumeBgmActionEditorDialogContents : public EditorDialogContents<Conversation::Action, Conversation::State>
{
    Q_OBJECT

public:
    ResumeBgmActionEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::ResumeBgmAction *pActionToEdit = NULL);

protected:
    bool ValidateFields() override;

private:
    Conversation::ResumeBgmAction *pResumeBgmAction;
};

class StopBgmActionListItem : public Conversation::Action::ActionListItem<Conversation::StopBgmAction>
{
    Q_OBJECT

public:
    explicit StopBgmActionListItem(Conversation::StopBgmAction *pAction, int indentLevel)
        : Conversation::Action::ActionListItem<Conversation::StopBgmAction>(pAction, indentLevel)
    {
    }

    QString GetDisplayString() override;
};

class StopBgmActionEditorDialogContents : public EditorDialogContents<Conversation::Action, Conversation::State>
{
    Q_OBJECT

public:
    StopBgmActionEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::StopBgmAction *pActionToEdit = NULL);

public slots:
    void IsInstantCheckBoxToggled(bool isChecked);
    void PreserveCheckBoxToggled(bool isChecked);

protected:
    bool ValidateFields() override;

private:
    Conversation::StopBgmAction *pStopBgmAction;
};

class PlayAmbianceActionListItem : public Conversation::Action::ActionListItem<Conversation::PlayAmbianceAction>
{
    Q_OBJECT

public:
    explicit PlayAmbianceActionListItem(Conversation::PlayAmbianceAction *pAction, int indentLevel)
        : Conversation::Action::ActionListItem<Conversation::PlayAmbianceAction>(pAction, indentLevel)
    {
    }

    QString GetDisplayString() override;
};

class PlayAmbianceActionEditorDialogContents : public EditorDialogContents<Conversation::Action, Conversation::State>
{
    Q_OBJECT

public:
    PlayAmbianceActionEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::PlayAmbianceAction *pActionToEdit = NULL);

public slots:
    void SoundEffectSelectorSoundEffectSelected(const QString &sfxId);
    void PreserveCheckBoxToggled(bool isChecked);

protected:
    bool ValidateFields() override;

private:
    Conversation::PlayAmbianceAction *pPlayAmbianceAction;
};

class PauseAmbianceActionListItem : public Conversation::Action::ActionListItem<Conversation::PauseAmbianceAction>
{
    Q_OBJECT

public:
    explicit PauseAmbianceActionListItem(Conversation::PauseAmbianceAction *pAction, int indentLevel)
        : Conversation::Action::ActionListItem<Conversation::PauseAmbianceAction>(pAction, indentLevel)
    {
    }

    QString GetDisplayString() override;
};

class PauseAmbianceActionEditorDialogContents : public EditorDialogContents<Conversation::Action, Conversation::State>
{
    Q_OBJECT

public:
    PauseAmbianceActionEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::PauseAmbianceAction *pActionToEdit = NULL);

protected:
    bool ValidateFields() override;

private:
    Conversation::PauseAmbianceAction *pPauseAmbianceAction;
};

class ResumeAmbianceActionListItem : public Conversation::Action::ActionListItem<Conversation::ResumeAmbianceAction>
{
    Q_OBJECT

public:
    explicit ResumeAmbianceActionListItem(Conversation::ResumeAmbianceAction *pAction, int indentLevel)
        : Conversation::Action::ActionListItem<Conversation::ResumeAmbianceAction>(pAction, indentLevel)
    {
    }

    QString GetDisplayString() override;
};

class ResumeAmbianceActionEditorDialogContents : public EditorDialogContents<Conversation::Action, Conversation::State>
{
    Q_OBJECT

public:
    ResumeAmbianceActionEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::ResumeAmbianceAction *pActionToEdit = NULL);

protected:
    bool ValidateFields() override;

private:
    Conversation::ResumeAmbianceAction *pResumeAmbianceAction;
};

class StopAmbianceActionListItem : public Conversation::Action::ActionListItem<Conversation::StopAmbianceAction>
{
    Q_OBJECT

public:
    explicit StopAmbianceActionListItem(Conversation::StopAmbianceAction *pAction, int indentLevel)
        : Conversation::Action::ActionListItem<Conversation::StopAmbianceAction>(pAction, indentLevel)
    {
    }

    QString GetDisplayString() override;
};

class StopAmbianceActionEditorDialogContents : public EditorDialogContents<Conversation::Action, Conversation::State>
{
    Q_OBJECT

public:
    StopAmbianceActionEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::StopAmbianceAction *pActionToEdit = NULL);

public slots:
    void IsInstantCheckBoxToggled(bool isChecked);
    void PreserveCheckBoxToggled(bool isChecked);

protected:
    bool ValidateFields() override;

private:
    Conversation::StopAmbianceAction *pStopAmbianceAction;
};

class StartAnimationActionListItem : public Conversation::Action::ActionListItem<Conversation::StartAnimationAction>
{
    Q_OBJECT

public:
    explicit StartAnimationActionListItem(Conversation::StartAnimationAction *pAction, int indentLevel)
        : Conversation::Action::ActionListItem<Conversation::StartAnimationAction>(pAction, indentLevel)
    {
    }

    QString GetDisplayString() override;
};

class StartAnimationActionEditorDialogContents : public EditorDialogContents<Conversation::Action, Conversation::State>
{
    Q_OBJECT

public:
    StartAnimationActionEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::StartAnimationAction *pActionToEdit = NULL);

protected:
    bool ValidateFields() override;

private:
    Conversation::StartAnimationAction *pStartAnimationAction;
};

class StopAnimationActionListItem : public Conversation::Action::ActionListItem<Conversation::StopAnimationAction>
{
    Q_OBJECT

public:
    explicit StopAnimationActionListItem(Conversation::StopAnimationAction *pAction, int indentLevel)
        : Conversation::Action::ActionListItem<Conversation::StopAnimationAction>(pAction, indentLevel)
    {
    }

    QString GetDisplayString() override;
};

class StopAnimationActionEditorDialogContents : public EditorDialogContents<Conversation::Action, Conversation::State>
{
    Q_OBJECT

public:
    StopAnimationActionEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::StopAnimationAction *pActionToEdit = NULL);

protected:
    bool ValidateFields() override;

private:
    Conversation::StopAnimationAction *pStopAnimationAction;
};

class SetPartnerActionListItem : public Conversation::Action::ActionListItem<Conversation::SetPartnerAction>
{
    Q_OBJECT

public:
    explicit SetPartnerActionListItem(Conversation::SetPartnerAction *pAction, int indentLevel)
        : Conversation::Action::ActionListItem<Conversation::SetPartnerAction>(pAction, indentLevel)
    {
    }

    QString GetDisplayString() override;
};

class SetPartnerActionEditorDialogContents : public EditorDialogContents<Conversation::Action, Conversation::State>
{
    Q_OBJECT

public:
    SetPartnerActionEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::SetPartnerAction *pActionToEdit = NULL);

public slots:
    void PartnerSelectorCharacterSelected(const QString &partnerId);

protected:
    bool ValidateFields() override;

private:
    Conversation::SetPartnerAction *pSetPartnerAction;
};

class GoToPresentWrongEvidenceActionListItem : public Conversation::Action::ActionListItem<Conversation::GoToPresentWrongEvidenceAction>
{
    Q_OBJECT

public:
    explicit GoToPresentWrongEvidenceActionListItem(Conversation::GoToPresentWrongEvidenceAction *pAction, int indentLevel)
        : Conversation::Action::ActionListItem<Conversation::GoToPresentWrongEvidenceAction>(pAction, indentLevel)
    {
    }

    QString GetDisplayString() override;
};

class GoToPresentWrongEvidenceActionEditorDialogContents : public EditorDialogContents<Conversation::Action, Conversation::State>
{
    Q_OBJECT

public:
    GoToPresentWrongEvidenceActionEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::GoToPresentWrongEvidenceAction *pActionToEdit = NULL);

protected:
    bool ValidateFields() override;

private:
    Conversation::GoToPresentWrongEvidenceAction *pGoToPresentWrongEvidenceAction;
};

class LockConversationActionListItem : public Conversation::Action::ActionListItem<Conversation::LockConversationAction>
{
    Q_OBJECT

public:
    explicit LockConversationActionListItem(Conversation::LockConversationAction *pAction, int indentLevel)
        : Conversation::Action::ActionListItem<Conversation::LockConversationAction>(pAction, indentLevel)
    {
    }

    QString GetDisplayString() override;
};

class LockConversationActionEditorDialogContents : public EditorDialogContents<Conversation::Action, Conversation::State>
{
    Q_OBJECT

public:
    LockConversationActionEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::LockConversationAction *pActionToEdit = NULL);

public slots:
    void TypeSelectorComboBoxCurrentItemChanged(int selectedIndex);
    void FlagEditorFlagSelected(const QString &flagId);
    void PartnerSelectorCharacterSelected(const QString &partnerId);

protected:
    bool ValidateFields() override;

private:
    Conversation::LockConversationAction *pLockConversationAction;

    FlagEditor *pFlagEditor;
    CharacterSelector *pPartnerSelector;
};

class ExitEncounterActionListItem : public Conversation::Action::ActionListItem<Conversation::ExitEncounterAction>
{
    Q_OBJECT

public:
    explicit ExitEncounterActionListItem(Conversation::ExitEncounterAction *pAction, int indentLevel)
        : Conversation::Action::ActionListItem<Conversation::ExitEncounterAction>(pAction, indentLevel)
    {
    }

    QString GetDisplayString() override;
};

class ExitEncounterActionEditorDialogContents : public EditorDialogContents<Conversation::Action, Conversation::State>
{
    Q_OBJECT

public:
    ExitEncounterActionEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::ExitEncounterAction *pActionToEdit = NULL);

protected:
    bool ValidateFields() override;

private:
    Conversation::ExitEncounterAction *pExitEncounterAction;
};

class MoveToLocationActionListItem : public Conversation::Action::ActionListItem<Conversation::MoveToLocationAction>
{
    Q_OBJECT

public:
    explicit MoveToLocationActionListItem(Conversation::MoveToLocationAction *pAction, int indentLevel)
        : Conversation::Action::ActionListItem<Conversation::MoveToLocationAction>(pAction, indentLevel)
    {
    }

    QString GetDisplayString() override;
};

class MoveToLocationActionEditorDialogContents : public EditorDialogContents<Conversation::Action, Conversation::State>
{
    Q_OBJECT

public:
    MoveToLocationActionEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::MoveToLocationAction *pActionToEdit = NULL);

public slots:
    void LocationSelectorLocationSelected(const QString &locationId);
    void TransitionIdComboBoxCurrentItemChanged(const QString &transitionId);

protected:
    bool ValidateFields() override;

private:
    Conversation::MoveToLocationAction *pMoveToLocationAction;
};

class MoveToZoomedViewActionListItem : public Conversation::Action::ActionListItem<Conversation::MoveToZoomedViewAction>
{
    Q_OBJECT

public:
    explicit MoveToZoomedViewActionListItem(Conversation::MoveToZoomedViewAction *pAction, int indentLevel)
        : Conversation::Action::ActionListItem<Conversation::MoveToZoomedViewAction>(pAction, indentLevel)
    {
    }

    QString GetDisplayString() override;
};

class MoveToZoomedViewActionEditorDialogContents : public EditorDialogContents<Conversation::Action, Conversation::State>
{
    Q_OBJECT

public:
    MoveToZoomedViewActionEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::MoveToZoomedViewAction *pActionToEdit = NULL);

public slots:
    void ZoomedViewIdComboBoxCurrentItemChanged(const QString &zoomedViewId);

protected:
    bool ValidateFields() override;

private:
    Conversation::MoveToZoomedViewAction *pMoveToZoomedViewAction;
};

class EndCaseActionListItem : public Conversation::Action::ActionListItem<Conversation::EndCaseAction>
{
    Q_OBJECT

public:
    explicit EndCaseActionListItem(Conversation::EndCaseAction *pAction, int indentLevel)
        : Conversation::Action::ActionListItem<Conversation::EndCaseAction>(pAction, indentLevel)
    {
    }

    QString GetDisplayString() override;
};

class EndCaseActionEditorDialogContents : public EditorDialogContents<Conversation::Action, Conversation::State>
{
    Q_OBJECT

public:
    EndCaseActionEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::EndCaseAction *pActionToEdit = NULL);

public slots:
    void CompletesCaseCheckBoxToggled(bool isChecked);

protected:
    bool ValidateFields() override;

private:
    Conversation::EndCaseAction *pEndCaseAction;
};

class MultipleChoiceActionListItem : public Conversation::Action::ActionListItem<Conversation::MultipleChoiceAction>
{
    Q_OBJECT

public:
    explicit MultipleChoiceActionListItem(Conversation::MultipleChoiceAction *pAction, int indentLevel)
        : Conversation::Action::ActionListItem<Conversation::MultipleChoiceAction>(pAction, indentLevel)
    {
    }

    bool GetCanHaveChildren() override { return true; }
    QString GetDisplayString() override;
};

class MultipleChoiceActionEditorDialogContents : public EditorDialogContents<Conversation::Action, Conversation::State>
{
    Q_OBJECT

public:
    MultipleChoiceActionEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::MultipleChoiceAction *pActionToEdit = NULL);

public slots:
    void OptionSelectorStringChanged(int index, const QString &string);
    void OptionSelectorStringAdded(int index, const QString &string);
    void OptionSelectorStringRemoved(int index);

protected:
    bool ValidateFields() override;

private:
    Conversation::MultipleChoiceAction *pMultipleChoiceAction;
};

class ExitMultipleChoiceActionListItem : public Conversation::Action::ActionListItem<Conversation::ExitMultipleChoiceAction>
{
    Q_OBJECT

public:
    explicit ExitMultipleChoiceActionListItem(Conversation::ExitMultipleChoiceAction *pAction, int indentLevel)
        : Conversation::Action::ActionListItem<Conversation::ExitMultipleChoiceAction>(pAction, indentLevel)
    {
    }

    QString GetDisplayString() override;
};

class ExitMultipleChoiceActionEditorDialogContents : public EditorDialogContents<Conversation::Action, Conversation::State>
{
    Q_OBJECT

public:
    ExitMultipleChoiceActionEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::ExitMultipleChoiceAction *pActionToEdit = NULL);

protected:
    bool ValidateFields() override;

private:
    Conversation::ExitMultipleChoiceAction *pExitMultipleChoiceAction;
};

class EnableFastForwardActionListItem : public Conversation::Action::ActionListItem<Conversation::EnableFastForwardAction>
{
    Q_OBJECT

public:
    explicit EnableFastForwardActionListItem(Conversation::EnableFastForwardAction *pAction, int indentLevel)
        : Conversation::Action::ActionListItem<Conversation::EnableFastForwardAction>(pAction, indentLevel)
    {
    }

    QString GetDisplayString() override;
};

class EnableFastForwardActionEditorDialogContents : public EditorDialogContents<Conversation::Action, Conversation::State>
{
    Q_OBJECT

public:
    EnableFastForwardActionEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::EnableFastForwardAction *pActionToEdit = NULL);

protected:
    bool ValidateFields() override;

private:
    Conversation::EnableFastForwardAction *pEnableFastForwardAction;
};

class DisableFastForwardActionListItem : public Conversation::Action::ActionListItem<Conversation::DisableFastForwardAction>
{
    Q_OBJECT

public:
    explicit DisableFastForwardActionListItem(Conversation::DisableFastForwardAction *pAction, int indentLevel)
        : Conversation::Action::ActionListItem<Conversation::DisableFastForwardAction>(pAction, indentLevel)
    {
    }

    QString GetDisplayString() override;
};

class DisableFastForwardActionEditorDialogContents : public EditorDialogContents<Conversation::Action, Conversation::State>
{
    Q_OBJECT

public:
    DisableFastForwardActionEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::DisableFastForwardAction *pActionToEdit = NULL);

protected:
    bool ValidateFields() override;

private:
    Conversation::DisableFastForwardAction *pDisableFastForwardAction;
};

class BeginBreakdownActionListItem : public Conversation::Action::ActionListItem<Conversation::BeginBreakdownAction>
{
    Q_OBJECT

public:
    explicit BeginBreakdownActionListItem(Conversation::BeginBreakdownAction *pAction, int indentLevel)
        : Conversation::Action::ActionListItem<Conversation::BeginBreakdownAction>(pAction, indentLevel)
    {
    }

    QString GetDisplayString() override;
};

class BeginBreakdownActionEditorDialogContents : public EditorDialogContents<Conversation::Action, Conversation::State>
{
    Q_OBJECT

public:
    BeginBreakdownActionEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::BeginBreakdownAction *pActionToEdit = NULL);

public slots:
    void CharacterComboBoxCurrentIndexChanged(int newIndex);

protected:
    bool ValidateFields() override;

private:
    Conversation::BeginBreakdownAction *pBeginBreakdownAction;
};

class EndBreakdownActionListItem : public Conversation::Action::ActionListItem<Conversation::EndBreakdownAction>
{
    Q_OBJECT

public:
    explicit EndBreakdownActionListItem(Conversation::EndBreakdownAction *pAction, int indentLevel)
        : Conversation::Action::ActionListItem<Conversation::EndBreakdownAction>(pAction, indentLevel)
    {
    }

    QString GetDisplayString() override;
};

class EndBreakdownActionEditorDialogContents : public EditorDialogContents<Conversation::Action, Conversation::State>
{
    Q_OBJECT

public:
    EndBreakdownActionEditorDialogContents(const Conversation::State &stateBeforeAction, Conversation::EndBreakdownAction *pActionToEdit = NULL);

protected:
    bool ValidateFields() override;

private:
    Conversation::EndBreakdownAction *pEndBreakdownAction;
};

class InterrogationRepeatActionListItem : public Conversation::Action::ActionListItem<Interrogation::InterrogationRepeatAction>
{
    Q_OBJECT

public:
    explicit InterrogationRepeatActionListItem(Interrogation::InterrogationRepeatAction *pAction, int indentLevel)
        : Conversation::Action::ActionListItem<Interrogation::InterrogationRepeatAction>(pAction, indentLevel)
    {
    }

    bool GetCanHaveChildren() override { return true; }
    QString GetDisplayString() override;
};

class InterrogationRepeatActionEditorDialogContents : public EditorDialogContents<Conversation::Action, Conversation::State>
{
    Q_OBJECT

public:
    InterrogationRepeatActionEditorDialogContents(const Conversation::State &stateBeforeAction, Interrogation::InterrogationRepeatAction *pActionToEdit = NULL);

protected:
    bool ValidateFields() override;

private:
    Interrogation::InterrogationRepeatAction *pInterrogationRepeatAction;
};

class ShowInterrogationActionListItem : public ShowDialogActionListItem
{
    Q_OBJECT

public:
    explicit ShowInterrogationActionListItem(Conversation::ShowDialogAction *pAction, int indentLevel)
        : ShowDialogActionListItem(pAction, indentLevel)
    {
    }

    bool GetCanHaveChildren() override { return true; }

protected:
    QString GetHeaderString(const QString &speakerName) override;
    QString GetFooterString() override;

private:
    Interrogation::ShowInterrogationAction * GetShowInterrogationAction()
    {
        return static_cast<Interrogation::ShowInterrogationAction *>(pAction);
    }
};

class ExitInterrogationRepeatActionListItem : public Conversation::Action::ActionListItem<Interrogation::ExitInterrogationRepeatAction>
{
    Q_OBJECT

public:
    explicit ExitInterrogationRepeatActionListItem(Interrogation::ExitInterrogationRepeatAction *pAction, int indentLevel)
        : Conversation::Action::ActionListItem<Interrogation::ExitInterrogationRepeatAction>(pAction, indentLevel)
    {
    }

    QString GetDisplayString() override;
};

class ExitInterrogationRepeatActionEditorDialogContents : public EditorDialogContents<Conversation::Action, Conversation::State>
{
    Q_OBJECT

public:
    ExitInterrogationRepeatActionEditorDialogContents(const Conversation::State &stateBeforeAction, Interrogation::ExitInterrogationRepeatAction *pActionToEdit = NULL);

protected:
    bool ValidateFields() override;

private:
    Interrogation::ExitInterrogationRepeatAction *pExitInterrogationRepeatAction;
};

class ConfrontationTopicSelectionActionListItem : public Conversation::Action::ActionListItem<Confrontation::ConfrontationTopicSelectionAction>
{
    Q_OBJECT

public:
    explicit ConfrontationTopicSelectionActionListItem(Confrontation::ConfrontationTopicSelectionAction *pAction, int indentLevel)
        : Conversation::Action::ActionListItem<Confrontation::ConfrontationTopicSelectionAction>(pAction, indentLevel)
    {
    }

    bool GetCanHaveChildren() override { return true; }
    QString GetDisplayString() override;
};

class ConfrontationTopicSelectionActionEditorDialogContents : public EditorDialogContents<Conversation::Action, Conversation::State>
{
    Q_OBJECT

public:
    ConfrontationTopicSelectionActionEditorDialogContents(const Conversation::State &stateBeforeAction, Confrontation::ConfrontationTopicSelectionAction *pActionToEdit = NULL);

public slots:
    void TopicSelectorStringChanged(int index, const QString &string);
    void TopicSelectorStringAdded(int index, const QString &string);
    void TopicSelectorStringRemoved(int index);
    void InitialTopicCheckBoxToggled(bool checked);

protected:
    bool ValidateFields() override;

private:
    Confrontation::ConfrontationTopicSelectionAction *pConfrontationTopicSelectionAction;
};

class EnableTopicActionListItem : public Conversation::Action::ActionListItem<Confrontation::EnableTopicAction>
{
    Q_OBJECT

public:
    explicit EnableTopicActionListItem(Confrontation::EnableTopicAction *pAction, int indentLevel)
        : Conversation::Action::ActionListItem<Confrontation::EnableTopicAction>(pAction, indentLevel)
    {
    }

    QString GetDisplayString() override;
};

class EnableTopicActionEditorDialogContents : public EditorDialogContents<Conversation::Action, Conversation::State>
{
    Q_OBJECT

public:
    EnableTopicActionEditorDialogContents(const Conversation::State &stateBeforeAction, Confrontation::EnableTopicAction *pActionToEdit = NULL);

public slots:
    void ConfrontationTopicComboBoxCurrentTextChanged(const QString &topicId);

protected:
    bool ValidateFields() override;

private:
    Confrontation::EnableTopicAction *pEnableTopicAction;
};

class RestartDecisionActionListItem : public Conversation::Action::ActionListItem<Confrontation::RestartDecisionAction>
{
    Q_OBJECT

public:
    explicit RestartDecisionActionListItem(Confrontation::RestartDecisionAction *pAction, int indentLevel)
        : Conversation::Action::ActionListItem<Confrontation::RestartDecisionAction>(pAction, indentLevel)
    {
    }

    bool GetCanHaveChildren() override { return true; }
    QString GetDisplayString() override;
};

class RestartDecisionActionEditorDialogContents : public EditorDialogContents<Conversation::Action, Conversation::State>
{
    Q_OBJECT

public:
    RestartDecisionActionEditorDialogContents(const Conversation::State &stateBeforeAction, Confrontation::RestartDecisionAction *pActionToEdit = NULL);

protected:
    bool ValidateFields() override;

private:
    Confrontation::RestartDecisionAction *pRestartDecisionAction;
};

class RestartConfrontationActionListItem : public Conversation::Action::ActionListItem<Confrontation::RestartConfrontationAction>
{
    Q_OBJECT

public:
    explicit RestartConfrontationActionListItem(Confrontation::RestartConfrontationAction *pAction, int indentLevel)
        : Conversation::Action::ActionListItem<Confrontation::RestartConfrontationAction>(pAction, indentLevel)
    {
    }

    QString GetDisplayString() override;
};

class RestartConfrontationActionEditorDialogContents : public EditorDialogContents<Conversation::Action, Conversation::State>
{
    Q_OBJECT

public:
    RestartConfrontationActionEditorDialogContents(const Conversation::State &stateBeforeAction, Confrontation::RestartConfrontationAction *pActionToEdit = NULL);

protected:
    bool ValidateFields() override;

private:
    Confrontation::RestartConfrontationAction *pRestartConfrontationAction;
};

Q_DECLARE_METATYPE(ConversationActionListItemSlots *)

#endif // CONVERSATION_H
