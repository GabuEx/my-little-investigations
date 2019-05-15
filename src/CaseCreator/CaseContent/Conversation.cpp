#include "Conversation.h"

#include "CaseContent.h"
#include "Encounter.h"
#include "Character.h"
#include "Evidence.h"
#include "Flag.h"
#include "Location.h"
#include "BackgroundMusic.h"
#include "SoundEffect.h"

#include "XmlReader.h"
#include "XmlWriter.h"

#include "SharedUtils.h"
#include "CaseCreator/Utilities/Utilities.h"

#include "Staging/Conversation.Staging.h"

#include "CaseCreator/Utilities/AudioPlayer.h"
#include "CaseCreator/Utilities/HtmlDelegate.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QListWidget>

#include <QComboBox>
#include <QPushButton>
#include <QFileDialog>

const QColor AsideTextColor(135, 206, 235);
const QColor EmphasisTextColor(255, 51, 51);

const double DefaultMillisecondsPerCharacterUpdate = 33;

template <>
void CaseContent::Add<Conversation>(QString, Conversation *pConversation, void *pParentObject)
{
    Encounter *pEncounter = reinterpret_cast<Encounter *>(pParentObject);

    if (pEncounter != NULL)
    {
        pEncounter->AddConversation(pConversation, pConversation->GetId());
    }
}

template <>
Conversation * CaseContent::GetById<Conversation>(QString id, void *pParentObject)
{
    Encounter *pEncounter = reinterpret_cast<Encounter *>(pParentObject);

    if (pEncounter != NULL)
    {
        return pEncounter->GetConversationById(id);
    }
    else
    {
        return NULL;
    }
}

template <>
QStringList CaseContent::GetIds<Conversation>(void *pParentObject)
{
    Encounter *pEncounter = reinterpret_cast<Encounter *>(pParentObject);

    if (pEncounter != NULL)
    {
        return pEncounter->GetConversationIds();
    }
    else
    {
        return QStringList();
    }
}

template <>
QStringList CaseContent::GetDisplayNames<Conversation>(void *pParentObject)
{
    Encounter *pEncounter = reinterpret_cast<Encounter *>(pParentObject);

    if (pEncounter != NULL)
    {
        return pEncounter->GetConversationDisplayNames();
    }
    else
    {
        return QStringList();
    }
}

template <>
void CaseContent::Add<Interrogation>(QString, Interrogation *pInterrogation, void *pParentObject)
{
    Encounter *pEncounter = reinterpret_cast<Encounter *>(pParentObject);

    if (pEncounter != NULL)
    {
        pEncounter->AddConversation(pInterrogation, pInterrogation->GetId());
    }
}

template <>
Interrogation * CaseContent::GetById<Interrogation>(QString id, void *pParentObject)
{
    Encounter *pEncounter = reinterpret_cast<Encounter *>(pParentObject);

    if (pEncounter != NULL)
    {
        return pEncounter->GetInterrogationById(id);
    }
    else
    {
        return NULL;
    }
}

template <>
QStringList CaseContent::GetIds<Interrogation>(void *pParentObject)
{
    Encounter *pEncounter = reinterpret_cast<Encounter *>(pParentObject);

    if (pEncounter != NULL)
    {
        return pEncounter->GetInterrogationIds();
    }
    else
    {
        return QStringList();
    }
}

template <>
QStringList CaseContent::GetDisplayNames<Interrogation>(void *pParentObject)
{
    Encounter *pEncounter = reinterpret_cast<Encounter *>(pParentObject);

    if (pEncounter != NULL)
    {
        return pEncounter->GetInterrogationDisplayNames();
    }
    else
    {
        return QStringList();
    }
}

template <>
void CaseContent::Add<Confrontation>(QString, Confrontation *pConfrontation, void *pParentObject)
{
    Encounter *pEncounter = reinterpret_cast<Encounter *>(pParentObject);

    if (pEncounter != NULL)
    {
        pEncounter->AddConversation(pConfrontation, pConfrontation->GetId());
    }
}

template <>
Confrontation * CaseContent::GetById<Confrontation>(QString id, void *pParentObject)
{
    Encounter *pEncounter = reinterpret_cast<Encounter *>(pParentObject);

    if (pEncounter != NULL)
    {
        return pEncounter->GetConfrontationById(id);
    }
    else
    {
        return NULL;
    }
}

template <>
QStringList CaseContent::GetIds<Confrontation>(void *pParentObject)
{
    Encounter *pEncounter = reinterpret_cast<Encounter *>(pParentObject);

    if (pEncounter != NULL)
    {
        return pEncounter->GetConfrontationIds();
    }
    else
    {
        return QStringList();
    }
}

template <>
QStringList CaseContent::GetDisplayNames<Confrontation>(void *pParentObject)
{
    Encounter *pEncounter = reinterpret_cast<Encounter *>(pParentObject);

    if (pEncounter != NULL)
    {
        return pEncounter->GetConfrontationDisplayNames();
    }
    else
    {
        return QStringList();
    }
}

QString CharacterSelectorMultipleSelectionSelectorWidget::GetString()
{
    return pCharacterSelector->GetSelectedCharacterId();
}

void CharacterSelectorMultipleSelectionSelectorWidget::SetToString(const QString &string)
{
    pCharacterSelector->SetToCharacterById(string);
}

QWidget * CharacterSelectorMultipleSelectionSelectorWidget::CreateSelector()
{
    pCharacterSelector = new CharacterSelector(false /* allowNoCharacter */);
    QObject::connect(pCharacterSelector, SIGNAL(CharacterSelected(QString)), this, SLOT(CharacterSelectorCharacterSelected(QString)));
    return pCharacterSelector;
}

void CharacterSelectorMultipleSelectionSelectorWidget::CharacterSelectorCharacterSelected(const QString &characterId)
{
    emit StringSelected(characterId);
}

QString EvidenceSelectorMultipleSelectionSelectorWidget::GetString()
{
    return pEvidenceSelector->GetId();
}

void EvidenceSelectorMultipleSelectionSelectorWidget::SetToString(const QString &string)
{
    pEvidenceSelector->SetToId(string);
}

QWidget * EvidenceSelectorMultipleSelectionSelectorWidget::CreateSelector()
{
    pEvidenceSelector = new EvidenceSelector();
    QObject::connect(pEvidenceSelector, SIGNAL(EvidenceSelected(QString)), this, SLOT(EvidenceSelectorEvidenceSelected(QString)));
    return pEvidenceSelector;
}

void EvidenceSelectorMultipleSelectionSelectorWidget::EvidenceSelectorEvidenceSelected(const QString &evidenceId)
{
    emit StringSelected(this, evidenceId);
}

QString LineEditMultipleSelectionSelectorWidget::GetString()
{
    return pLineEdit->text();
}

void LineEditMultipleSelectionSelectorWidget::SetToString(const QString &string)
{
    pLineEdit->setText(string);
}

QWidget * LineEditMultipleSelectionSelectorWidget::CreateSelector()
{
    pLineEdit = new QLineEdit();
    pLineEdit->setMinimumWidth(300);
    QObject::connect(pLineEdit, SIGNAL(textEdited(QString)), this, SLOT(LineEditTextEdited(QString)));
    return pLineEdit;
}

void LineEditMultipleSelectionSelectorWidget::LineEditTextEdited(const QString &string)
{
    emit StringSelected(this, string);
}

QString ConfrontationTopicMultipleSelectionSelectorWidget::GetString()
{
    return GenerateString();
}

void ConfrontationTopicMultipleSelectionSelectorWidget::SetToString(const QString &string)
{
    QString id;
    QString name;
    bool isEnabledAtStart;

    ConfrontationTopicMultipleSelectionSelectorWidget::ParseString(string, &id, &name, &isEnabledAtStart);

    pNameLineEdit->setText(name);
    pIsEnabledCheckBox->setChecked(isEnabledAtStart);
}

QWidget * ConfrontationTopicMultipleSelectionSelectorWidget::CreateSelector()
{
    QWidget *pSelector = new QWidget();
    QHBoxLayout *pLayout = new QHBoxLayout();

    pNameLineEdit = new QLineEdit();
    pNameLineEdit->setMinimumWidth(300);
    QObject::connect(pNameLineEdit, SIGNAL(textEdited(QString)), this, SLOT(NameLineEditTextEdited(QString)));
    pLayout->addWidget(pNameLineEdit);

    pIsEnabledCheckBox = new QCheckBox();
    pIsEnabledCheckBox->setText("Enabled at start");
    QObject::connect(pIsEnabledCheckBox, SIGNAL(toggled(bool)), this, SLOT(IsEnabledCheckBoxToggled(bool)));
    pLayout->addWidget(pIsEnabledCheckBox);

    pSelector->setLayout(pLayout);
    return pSelector;
}

QString ConfrontationTopicMultipleSelectionSelectorWidget::CreateString(QString id, QString name, bool isEnabledAtStart)
{
    return id + "\t" + name + "\t" + (isEnabledAtStart ? "true" : "false");
}

void ConfrontationTopicMultipleSelectionSelectorWidget::ParseString(QString string, QString *pId, QString *pName, bool *pIsEnabledAtStart)
{
    QStringList stringList = string.split("\t");

    if (stringList.size() != 3)
    {
        throw new MLIException("Expected three elements in a confrontation topic selector widget string: the ID, the name, and the enabled-at-start state.");
    }

    *pId = stringList[0];
    *pName = stringList[1];
    *pIsEnabledAtStart = stringList[2] == "true";
}

void ConfrontationTopicMultipleSelectionSelectorWidget::NameLineEditTextEdited(const QString &/*evidenceId*/)
{
    emit StringSelected(this, GenerateString());
}

void ConfrontationTopicMultipleSelectionSelectorWidget::IsEnabledCheckBoxToggled(bool /*isChecked*/)
{
    emit StringSelected(this, GenerateString());
}

QString ConfrontationTopicMultipleSelectionSelectorWidget::GenerateString()
{
    QString name = pNameLineEdit->text();
    QString id = name.replace(QRegExp("[^a-zA-Z0-9]"), "");

    return ConfrontationTopicMultipleSelectionSelectorWidget::CreateString(id, name, pIsEnabledCheckBox->isChecked());
}

Conversation::Dialog::Dialog()
    : ConversationDialogSlots()
{
    pDrawingViewForPlaying = NULL;
    currentPosition = -1;
    millisecondsPerCharacterUpdate = DefaultMillisecondsPerCharacterUpdate;

    playTimer.setSingleShot(false);
    QObject::connect(&playTimer, SIGNAL(timeout()), this, SLOT(HandleTimeout()));

    events.clear();
    colorTransitions.clear();
}

Conversation::Dialog::~Dialog()
{
    Reset();
}

void Conversation::Dialog::Reset()
{
    for (DialogEvent *pEvent : events)
    {
        delete pEvent;
    }

    events.clear();
    colorTransitions.clear();
}

void Conversation::Dialog::AddSpeedChangePosition(int position, double newMillisecondsPerCharacterUpdate)
{
    events.push_back(new SpeedChangeEvent(position, this, newMillisecondsPerCharacterUpdate));
}

void Conversation::Dialog::AddEmotionChangePosition(int position, const QString &newEmotionId)
{
    events.push_back(new SpeakerEmotionChangeEvent(position, this, newEmotionId));
}

void Conversation::Dialog::AddEmotionOtherChangePosition(int position, const QString &newEmotionId)
{
    events.push_back(new OtherEmotionChangeEvent(position, this, newEmotionId));
}

void Conversation::Dialog::AddPausePosition(int position, double millisecondDuration)
{
    int intMillisecondDuration = (int)millisecondDuration;
    int pauseDuration = 0;

    if (millisecondDuration > 500)
    {
        pauseDuration = intMillisecondDuration - 300;
    }
    else
    {
        pauseDuration = intMillisecondDuration * 2 / 5;
    }

    events.push_back(new PauseEvent(position, this, pauseDuration));
    events.push_back(new AudioPauseEvent(position, this, intMillisecondDuration - pauseDuration));
}

void Conversation::Dialog::AddAudioPausePosition(int position, double millisecondDuration)
{
    events.push_back(new AudioPauseEvent(position, this, (int)millisecondDuration));
}

void Conversation::Dialog::AddMouthChangePosition(int position, bool mouthIsOn)
{
    events.push_back(new MouthChangeEvent(position, this, mouthIsOn));
}

void Conversation::Dialog::StartAside(int position)
{
    colorTransitions.push_back(TextColorTransition(position, AsideTextColor, false /* endsColor */));
}

void Conversation::Dialog::EndAside(int position)
{
    colorTransitions.push_back(TextColorTransition(position, AsideTextColor, true /* endsColor */));
}

void Conversation::Dialog::StartEmphasis(int position)
{
    colorTransitions.push_back(TextColorTransition(position, EmphasisTextColor, false /* endsColor */));
}

void Conversation::Dialog::EndEmphasis(int position)
{
    colorTransitions.push_back(TextColorTransition(position, EmphasisTextColor, true /* endsColor */));
}

void Conversation::Dialog::PlaySound(int position, const QString &id)
{
    events.push_back(new PlaySoundEvent(position, this, id));
    soundEffectIds.push_back(id);
}

QStringList Conversation::Dialog::GetHtmlLines(int lastTextPosition) const
{
    // If lastTextPosition < 0, then we'll retrieve all the text.
    if (lastTextPosition < 0)
    {
        lastTextPosition = parsedDialog.length();
    }

    int lineOffset = 0;
    int htmlOffset = 0;
    QStack<QString> spansOpen;

    QStringList originalLines = split(parsedDialog.left(lastTextPosition), '\n');
    QStringList htmlLines = originalLines;

    QStringList::iterator originalIter = originalLines.begin();
    QStringList::iterator htmlIter = htmlLines.begin();

    for (TextColorTransition colorTransition : colorTransitions)
    {
        if (colorTransition.Position > lastTextPosition)
        {
            break;
        }
        else
        {
            while (colorTransition.Position > lineOffset + (*originalIter).length())
            {
                lineOffset += (*originalIter).length() + 1; // + 1 for the newline character.
                htmlOffset = 0;

                for (int i = 0; i < spansOpen.length(); i++)
                {
                    (*htmlIter).append("</span>");
                }

                originalIter++;
                htmlIter++;

                if (htmlIter == htmlLines.end())
                {
                    break;
                }

                for (QString span : spansOpen)
                {
                    (*htmlIter).insert(0, span);
                    htmlOffset += span.length();
                }
            }
        }

        if (htmlIter == htmlLines.end())
        {
            break;
        }

        QString htmlToAdd;

        if (colorTransition.EndsColor)
        {
            htmlToAdd = "</span>";
            spansOpen.pop();
        }
        else
        {
            int r, g, b;
            colorTransition.Color.getRgb(&r, &g, &b);

            char rgbString[256];
            sprintf(rgbString, "%02x%02x%02x", r, g, b);

            htmlToAdd = QString("<span style='color:#") + rgbString + QString("'>");
            spansOpen.push(htmlToAdd);
        }

        (*htmlIter).insert(colorTransition.Position - lineOffset + htmlOffset, htmlToAdd);
        htmlOffset += htmlToAdd.length();
    }

    while (htmlIter != htmlLines.end())
    {
        for (int i = 0; i < spansOpen.length(); i++)
        {
            (*htmlIter).append("</span>");
        }

        htmlIter++;

        if (htmlIter != htmlLines.end())
        {
            for (QString span : spansOpen)
            {
                (*htmlIter).insert(0, span);
                htmlOffset += span.length();
            }
        }
    }

    return htmlLines;
}

void Conversation::Dialog::PreloadAudio()
{
    AudioPlayer::PreloadDialog(dialogAudioFileName, dialogAudioFileName);

    for (QString id : soundEffectIds)
    {
        if (CaseContent::GetInstance()->GetIds<SoundEffect>().contains(id))
        {
            CaseContent::GetInstance()->GetById<SoundEffect>(id)->Preload();
        }
    }
}

void Conversation::Dialog::PlayOnDrawingView(Conversation::DrawingView *pDrawingView)
{
    pDrawingViewForPlaying = pDrawingView;
    currentPosition = 0;
    leftCharacterEmotionId = leftCharacterInitialEmotionId;
    rightCharacterEmotionId = rightCharacterInitialEmotionId;
    millisecondsPerCharacterUpdate = DefaultMillisecondsPerCharacterUpdate;
    mouthOffCount = 0;

    eventIterator = events.begin();

    if (dialogAudioFileName.length() > 0)
    {
        AudioPlayer::PlayDialog(dialogAudioFileName);
    }

    playTimer.start(16);
    elapsedTimer.start();
    elapsedTimeLeftOver = 0;
    msPauseDuration = 0;
    msAudioPauseDuration = 0;
    IncrementPositionOnTimeout();
}

void Conversation::Dialog::StopPlaying()
{
    playTimer.stop();
    AudioPlayer::StopDialog();

    if (pDrawingViewForPlaying != NULL)
    {
        pDrawingViewForPlaying->SetDialog(speakerPosition, GetHtmlLines(), speakingCharacterId, false);
        pDrawingViewForPlaying = NULL;
    }
}

void Conversation::Dialog::IncrementPositionOnTimeout()
{
    qint64 elapsedTime = elapsedTimer.elapsed() + elapsedTimeLeftOver;
    elapsedTimer.start();

    while (msAudioPauseDuration > 0 || msPauseDuration > 0)
    {
        if (msAudioPauseDuration > 0)
        {
            if (msAudioPauseDuration > elapsedTime)
            {
                msAudioPauseDuration -= elapsedTime;
                elapsedTime = 0;
            }
            else
            {
                msAudioPauseDuration = 0;
                elapsedTime -= msAudioPauseDuration;
            }
        }

        if (msAudioPauseDuration == 0 && msPauseDuration > 0)
        {
            if (msPauseDuration > elapsedTime)
            {
                msPauseDuration -= elapsedTime;
                elapsedTime = 0;
            }
            else
            {
                msPauseDuration = 0;
                elapsedTime -= msPauseDuration;
            }
        }

        if (msAudioPauseDuration > 0 || msPauseDuration > 0)
        {
            break;
        }

        while (eventIterator != events.end() && (*eventIterator)->GetPosition() <= currentPosition && msAudioPauseDuration == 0 && msPauseDuration == 0)
        {
            (*eventIterator)->Raise();
            eventIterator++;
        }
    }

    while (msAudioPauseDuration == 0 && msPauseDuration == 0 && elapsedTime >= millisecondsPerCharacterUpdate && currentPosition < parsedDialog.length())
    {
        currentPosition++;
        elapsedTime -= millisecondsPerCharacterUpdate;
    }

    elapsedTimeLeftOver = elapsedTime;

    while (eventIterator != events.end() && (*eventIterator)->GetPosition() <= currentPosition && msAudioPauseDuration == 0 && msPauseDuration == 0)
    {
        (*eventIterator)->Raise();
        eventIterator++;
    }

    pDrawingViewForPlaying->SetLeftCharacter(
        leftCharacterId,
        leftCharacterEmotionId);

    pDrawingViewForPlaying->SetRightCharacter(
        rightCharacterId,
        rightCharacterEmotionId);

    bool isTalking =
        msAudioPauseDuration == 0 &&
        (currentPosition < parsedDialog.length() ||
            (AudioPlayer::GetCurrentPlayingDialog() == dialogAudioFileName && dialogAudioFileName.length() > 0)) &&
        mouthOffCount == 0;

    pDrawingViewForPlaying->SetDialog(speakerPosition, GetHtmlLines(currentPosition), speakingCharacterId, isTalking);

    if (currentPosition >= parsedDialog.length() && AudioPlayer::GetCurrentPlayingDialog() != dialogAudioFileName)
    {
        StopPlaying();
    }
}

void Conversation::Dialog::SpeedChangeEvent::Raise()
{
    this->pOwningDialog->millisecondsPerCharacterUpdate = newMillisecondsPerCharacterUpdate;
}

void Conversation::Dialog::SpeakerEmotionChangeEvent::Raise()
{
    if (this->pOwningDialog->speakerPosition == CharacterPositionLeft)
    {
        this->pOwningDialog->leftCharacterEmotionId = newEmotionId;
    }
    else if (this->pOwningDialog->speakerPosition == CharacterPositionRight)
    {
        this->pOwningDialog->rightCharacterEmotionId = newEmotionId;
    }
}

void Conversation::Dialog::OtherEmotionChangeEvent::Raise()
{
    if (this->pOwningDialog->speakerPosition == CharacterPositionLeft)
    {
        this->pOwningDialog->rightCharacterEmotionId = newEmotionId;
    }
    else if (this->pOwningDialog->speakerPosition == CharacterPositionRight)
    {
        this->pOwningDialog->leftCharacterEmotionId = newEmotionId;
    }
}

void Conversation::Dialog::PauseEvent::Raise()
{
    this->pOwningDialog->msPauseDuration += msDuration;
}

void Conversation::Dialog::AudioPauseEvent::Raise()
{
    this->pOwningDialog->msAudioPauseDuration += msDuration;
}

void Conversation::Dialog::MouthChangeEvent::Raise()
{
    if (isMouthOpen)
    {
        if (this->pOwningDialog->mouthOffCount > 0)
        {
            this->pOwningDialog->mouthOffCount--;
        }
    }
    else
    {
        this->pOwningDialog->mouthOffCount++;
    }
}

void Conversation::Dialog::PlaySoundEvent::Raise()
{
    if (CaseContent::GetInstance()->GetIds<SoundEffect>().contains(id))
    {
        CaseContent::GetInstance()->GetById<SoundEffect>(id)->Play();
    }
}

Conversation::Conversation(Staging::Conversation *pStagingConversation)
    : Conversation()
{
    SplitConversationIdFromCaseFile(pStagingConversation->Id, NULL, &id);
    name = pStagingConversation->Name;
}

Conversation::~Conversation()
{
    pOwningEncounter = NULL;
}

void Conversation::WriteToCaseXml(XmlWriter *pWriter)
{
    pWriter->StartElement("Conversation");
    WriteToCaseXmlCore(pWriter);
    pWriter->EndElement();
}

void Conversation::WriteToCaseXmlCore(XmlWriter *pWriter)
{
    pWriter->WriteBooleanElement("IsEnabled", false); // Placeholder
    pWriter->WriteBooleanElement("HasBeenCompleted", false); // Placeholder

    pWriter->StartElement("ActionList");

    for (Action *pAction : actions)
    {
        pWriter->StartElement("Entry");
        pAction->WriteToCaseXml(pWriter);
        pWriter->EndElement();
    }

    pWriter->EndElement();
}

void Conversation::PopulateActionsFromStaging(QList<Action *> &actions, const QList<Staging::Conversation::Action *> &stagingActions, int initialIndex)
{
    for (int i = 0; i < stagingActions.size(); i++)
    {
        PopulateActionFromStaging(actions, stagingActions, i, initialIndex);
    }
}

void Conversation::PopulateActionFromStaging(QList<Action *> &actions, const QList<Staging::Conversation::Action *> &stagingActions, int &indexInStagingActions, int initialIndexInFullList)
{
    Staging::Conversation::Action *pAction = stagingActions[indexInStagingActions];

    switch (pAction->GetType())
    {
    case Staging::ConversationActionType_MultipleCharacterChange:
        {
            Staging::Conversation::MultipleCharacterChangeAction *pActionSubclass =
                    static_cast<Staging::Conversation::MultipleCharacterChangeAction *>(pAction);

            for (Staging::Conversation::CharacterChangeAction *pCharacterChangeAction : pActionSubclass->CharacterChangeList)
            {
                actions.append(new CharacterChangeAction(pCharacterChangeAction));
            }
        }
        break;

    case Staging::ConversationActionType_CharacterChange:
        actions.append(new CharacterChangeAction(static_cast<Staging::Conversation::CharacterChangeAction *>(pAction)));
        break;

    case Staging::ConversationActionType_SetFlag:
        {
            SetFlagAction *pSetFlagAction = new SetFlagAction(static_cast<Staging::Conversation::SetFlagAction *>(pAction));

            if (indexInStagingActions < stagingActions.count() - 1)
            {
                Staging::Conversation::Action *pNextAction = stagingActions[indexInStagingActions + 1];

                if (pNextAction->GetType() == Staging::ConversationActionType_Notification)
                {
                    pSetFlagAction->AddNotification(static_cast<Staging::Conversation::NotificationAction *>(pNextAction));
                }
            }

            actions.append(pSetFlagAction);
        }
        break;

    case Staging::ConversationActionType_BranchOnCondition:
        {
            Staging::Conversation::BranchOnConditionAction *pStagingBranchOnConditionAction =
                    static_cast<Staging::Conversation::BranchOnConditionAction *>(pAction);

            BranchOnConditionAction *pBranchOnConditionAction = new BranchOnConditionAction(pStagingBranchOnConditionAction);

            if (indexInStagingActions >= stagingActions.count() - 1)
            {
                throw new MLIException("Expected a BranchIfTrueAction to follow a BranchOnConditionAction.");
            }

            Staging::Conversation::Action *pNextAction = stagingActions[indexInStagingActions + 1];

            if (pNextAction->GetType() != Staging::ConversationActionType_BranchIfTrue &&
                    pNextAction->GetType() != Staging::ConversationActionType_BranchIfFalse)
            {
                throw new MLIException("Expected a BranchIfTrueAction or a BranchIfFalseAction to follow a BranchOnConditionAction.");
            }

            int endIndex = 0;

            if (pNextAction->GetType() == Staging::ConversationActionType_BranchIfTrue)
            {
                Staging::Conversation::BranchIfTrueAction *pStagingBranchIfTrueAction =
                        static_cast<Staging::Conversation::BranchIfTrueAction *>(pNextAction);

                endIndex = pStagingBranchIfTrueAction->EndIndex - initialIndexInFullList;
            }
            else
            {
                Staging::Conversation::BranchIfFalseAction *pStagingBranchIfFalseAction =
                        static_cast<Staging::Conversation::BranchIfFalseAction *>(pNextAction);

                endIndex = pStagingBranchIfFalseAction->EndIndex - initialIndexInFullList;
            }

            QList<Staging::Conversation::Action *> branchIfTrueActions;
            QList<Staging::Conversation::Action *> branchIfFalseActions;

            if (pStagingBranchOnConditionAction->TrueIndex < pStagingBranchOnConditionAction->FalseIndex)
            {
                for (int j = pStagingBranchOnConditionAction->TrueIndex - initialIndexInFullList; j < pStagingBranchOnConditionAction->FalseIndex - initialIndexInFullList; j++)
                {
                    branchIfTrueActions.append(stagingActions[j]);
                }

                for (int j = pStagingBranchOnConditionAction->FalseIndex - initialIndexInFullList; j < endIndex; j++)
                {
                    branchIfFalseActions.append(stagingActions[j]);
                }
            }
            else
            {
                for (int j = pStagingBranchOnConditionAction->FalseIndex - initialIndexInFullList; j < pStagingBranchOnConditionAction->TrueIndex - initialIndexInFullList; j++)
                {
                    branchIfFalseActions.append(stagingActions[j]);
                }

                for (int j = pStagingBranchOnConditionAction->TrueIndex - initialIndexInFullList; j < endIndex; j++)
                {
                    branchIfTrueActions.append(stagingActions[j]);
                }
            }

            PopulateActionsFromStaging(pBranchOnConditionAction->trueActions, branchIfTrueActions, pStagingBranchOnConditionAction->TrueIndex - initialIndexInFullList);
            PopulateActionsFromStaging(pBranchOnConditionAction->falseActions, branchIfFalseActions, pStagingBranchOnConditionAction->FalseIndex - initialIndexInFullList);

            actions.append(pBranchOnConditionAction);
            indexInStagingActions = endIndex - 1;
        }
        break;

    case Staging::ConversationActionType_ShowDialog:
    case Staging::ConversationActionType_ShowDialogAutomatic:
        actions.append(new ShowDialogAction(static_cast<Staging::Conversation::ShowDialogAction *>(pAction)));
        break;

    case Staging::ConversationActionType_MustPresentEvidence:
        {
            Staging::Conversation::MustPresentEvidenceAction *pStagingMustPresentEvidenceAction =
                    static_cast<Staging::Conversation::MustPresentEvidenceAction *>(pAction);

            MustPresentEvidenceAction *pMustPresentEvidenceAction = new MustPresentEvidenceAction(pStagingMustPresentEvidenceAction);

            int endIndex = -1;

            // Track the number of MustPresentEvidenceActions that we've seen,
            // so we're sure that we're matching with the right end action.
            int mustPresentEvidenceCount = 1;

            for (int j = indexInStagingActions + 1; j < stagingActions.count(); j++)
            {
                if (stagingActions[j]->GetType() == Staging::ConversationActionType_EndMustPresentEvidence)
                {
                    mustPresentEvidenceCount--;

                    if (mustPresentEvidenceCount == 0)
                    {
                        endIndex = j;
                        break;
                    }
                }
                else if (stagingActions[j]->GetType() == Staging::ConversationActionType_MustPresentEvidence)
                {
                    mustPresentEvidenceCount++;
                }
            }

            if (endIndex < 0)
            {
                throw new MLIException("Expected an EndMustPresentEvidenceAction to follow a MustPresentEvidenceAction.");
            }

            QList<Staging::Conversation::Action *> correctEvidencePresentedStagingActions;
            QList<Staging::Conversation::Action *> wrongEvidencePresentedStagingActions;
            QList<Staging::Conversation::Action *> endRequestedStagingActions;

            int firstIndex = pStagingMustPresentEvidenceAction->CorrectEvidencePresentedIndex;
            int secondIndex = -1;
            int thirdIndex = -1;
            QList<Staging::Conversation::Action *> *pFirstStagingActions = &correctEvidencePresentedStagingActions;
            QList<Staging::Conversation::Action *> *pSecondStagingActions = NULL;
            QList<Staging::Conversation::Action *> *pThirdStagingActions = NULL;

            if (pStagingMustPresentEvidenceAction->WrongEvidencePresentedIndex >= 0)
            {
                if (pStagingMustPresentEvidenceAction->WrongEvidencePresentedIndex < firstIndex)
                {
                    secondIndex = firstIndex;
                    pSecondStagingActions = pFirstStagingActions;

                    firstIndex = pStagingMustPresentEvidenceAction->WrongEvidencePresentedIndex;
                    pFirstStagingActions = &wrongEvidencePresentedStagingActions;
                }
                else
                {
                    secondIndex = pStagingMustPresentEvidenceAction->WrongEvidencePresentedIndex;
                    pSecondStagingActions = &wrongEvidencePresentedStagingActions;
                }
            }

            if (pStagingMustPresentEvidenceAction->EndRequestedIndex >= 0)
            {
                if (pStagingMustPresentEvidenceAction->EndRequestedIndex < firstIndex)
                {
                    thirdIndex = secondIndex;
                    pThirdStagingActions = pSecondStagingActions;

                    secondIndex = firstIndex;
                    pSecondStagingActions = pFirstStagingActions;

                    firstIndex = pStagingMustPresentEvidenceAction->EndRequestedIndex;
                    pFirstStagingActions = &endRequestedStagingActions;
                }
                else if (pStagingMustPresentEvidenceAction->EndRequestedIndex < secondIndex)
                {
                    thirdIndex = secondIndex;
                    pThirdStagingActions = pSecondStagingActions;

                    secondIndex = pStagingMustPresentEvidenceAction->EndRequestedIndex;
                    pSecondStagingActions = &endRequestedStagingActions;
                }
                else
                {
                    thirdIndex = pStagingMustPresentEvidenceAction->EndRequestedIndex;
                    pThirdStagingActions = &endRequestedStagingActions;
                }
            }

            if (firstIndex >= 0)
            {
                int nextIndex = (secondIndex >= 0 ? secondIndex - initialIndexInFullList : (thirdIndex >= 0 ? thirdIndex - initialIndexInFullList : endIndex));

                for (int j = firstIndex - initialIndexInFullList; j < nextIndex; j++)
                {
                    pFirstStagingActions->append(stagingActions[j]);
                }
            }

            if (secondIndex >= 0)
            {
                int nextIndex = (thirdIndex >= 0 ? thirdIndex - initialIndexInFullList : endIndex);

                for (int j = secondIndex - initialIndexInFullList; j < nextIndex; j++)
                {
                    pSecondStagingActions->append(stagingActions[j]);
                }
            }

            if (thirdIndex >= 0)
            {
                for (int j = thirdIndex - initialIndexInFullList; j < endIndex; j++)
                {
                    pThirdStagingActions->append(stagingActions[j]);
                }
            }

            if (pStagingMustPresentEvidenceAction->CorrectEvidencePresentedIndex >= 0)
            {
                PopulateActionsFromStaging(pMustPresentEvidenceAction->correctEvidencePresentedActions, correctEvidencePresentedStagingActions, pStagingMustPresentEvidenceAction->CorrectEvidencePresentedIndex - initialIndexInFullList);
            }

            if (pStagingMustPresentEvidenceAction->WrongEvidencePresentedIndex >= 0)
            {
                PopulateActionsFromStaging(pMustPresentEvidenceAction->wrongEvidencePresentedActions, wrongEvidencePresentedStagingActions, pStagingMustPresentEvidenceAction->WrongEvidencePresentedIndex - initialIndexInFullList);
            }

            if (pStagingMustPresentEvidenceAction->EndRequestedIndex >= 0)
            {
                PopulateActionsFromStaging(pMustPresentEvidenceAction->endRequestedActions, endRequestedStagingActions, pStagingMustPresentEvidenceAction->EndRequestedIndex - initialIndexInFullList);
            }

            actions.append(pMustPresentEvidenceAction);
            indexInStagingActions = endIndex - 1;
        }
        break;

    case Staging::ConversationActionType_EnableConversation:
        {
            EnableConversationAction *pEnableConversationAction = new EnableConversationAction(static_cast<Staging::Conversation::EnableConversationAction *>(pAction));

            if (indexInStagingActions < stagingActions.count() - 1)
            {
                Staging::Conversation::Action *pNextAction = stagingActions[indexInStagingActions + 1];

                if (pNextAction->GetType() == Staging::ConversationActionType_Notification)
                {
                    pEnableConversationAction->AddNotification(static_cast<Staging::Conversation::NotificationAction *>(pNextAction));
                }
            }

            actions.append(pEnableConversationAction);
        }
        break;

    case Staging::ConversationActionType_EnableEvidence:
        {
            EnableEvidenceAction *pEnableEvidenceAction = new EnableEvidenceAction(static_cast<Staging::Conversation::EnableEvidenceAction *>(pAction));

            if (indexInStagingActions < stagingActions.count() - 1)
            {
                Staging::Conversation::Action *pNextAction = stagingActions[indexInStagingActions + 1];

                if (pNextAction->GetType() == Staging::ConversationActionType_Notification)
                {
                    pEnableEvidenceAction->AddNotification(static_cast<Staging::Conversation::NotificationAction *>(pNextAction));
                }
            }

            actions.append(pEnableEvidenceAction);
        }
        break;

    case Staging::ConversationActionType_UpdateEvidence:
        {
            UpdateEvidenceAction *pUpdateEvidenceAction = new UpdateEvidenceAction(static_cast<Staging::Conversation::UpdateEvidenceAction *>(pAction));

            if (indexInStagingActions < stagingActions.count() - 1)
            {
                Staging::Conversation::Action *pNextAction = stagingActions[indexInStagingActions + 1];

                if (pNextAction->GetType() == Staging::ConversationActionType_Notification)
                {
                    pUpdateEvidenceAction->AddNotification(static_cast<Staging::Conversation::NotificationAction *>(pNextAction));
                }
            }

            actions.append(pUpdateEvidenceAction);
        }
        break;

    case Staging::ConversationActionType_DisableEvidence:
        {
            DisableEvidenceAction *pDisableEvidenceAction = new DisableEvidenceAction(static_cast<Staging::Conversation::DisableEvidenceAction *>(pAction));

            if (indexInStagingActions < stagingActions.count() - 1)
            {
                Staging::Conversation::Action *pNextAction = stagingActions[indexInStagingActions + 1];

                if (pNextAction->GetType() == Staging::ConversationActionType_Notification)
                {
                    pDisableEvidenceAction->AddNotification(static_cast<Staging::Conversation::NotificationAction *>(pNextAction));
                }
            }

            actions.append(pDisableEvidenceAction);
        }
        break;

    case Staging::ConversationActionType_EnableCutscene:
        {
            EnableCutsceneAction *pEnableCutsceneAction = new EnableCutsceneAction(static_cast<Staging::Conversation::EnableCutsceneAction *>(pAction));

            if (indexInStagingActions < stagingActions.count() - 1)
            {
                Staging::Conversation::Action *pNextAction = stagingActions[indexInStagingActions + 1];

                if (pNextAction->GetType() == Staging::ConversationActionType_Notification)
                {
                    pEnableCutsceneAction->AddNotification(static_cast<Staging::Conversation::NotificationAction *>(pNextAction));
                }
            }

            actions.append(pEnableCutsceneAction);
        }
        break;

    case Staging::ConversationActionType_PlayBgm:
        actions.append(new PlayBgmAction(static_cast<Staging::Conversation::PlayBgmAction *>(pAction)));
        break;

    case Staging::ConversationActionType_PauseBgm:
        actions.append(new PauseBgmAction(static_cast<Staging::Conversation::PauseBgmAction *>(pAction)));
        break;

    case Staging::ConversationActionType_ResumeBgm:
        actions.append(new ResumeBgmAction(static_cast<Staging::Conversation::ResumeBgmAction *>(pAction)));
        break;

    case Staging::ConversationActionType_StopBgm:
        actions.append(new StopBgmAction(static_cast<Staging::Conversation::StopBgmAction *>(pAction)));
        break;

    case Staging::ConversationActionType_PlayAmbiance:
        actions.append(new PlayAmbianceAction(static_cast<Staging::Conversation::PlayAmbianceAction *>(pAction)));
        break;

    case Staging::ConversationActionType_PauseAmbiance:
        actions.append(new PauseAmbianceAction(static_cast<Staging::Conversation::PauseAmbianceAction *>(pAction)));
        break;

    case Staging::ConversationActionType_ResumeAmbiance:
        actions.append(new ResumeAmbianceAction(static_cast<Staging::Conversation::ResumeAmbianceAction *>(pAction)));
        break;

    case Staging::ConversationActionType_StopAmbiance:
        actions.append(new StopAmbianceAction(static_cast<Staging::Conversation::StopAmbianceAction *>(pAction)));
        break;

    case Staging::ConversationActionType_StartAnimation:
        actions.append(new StartAnimationAction(static_cast<Staging::Conversation::StartAnimationAction *>(pAction)));
        break;

    case Staging::ConversationActionType_StopAnimation:
        actions.append(new StopAnimationAction(static_cast<Staging::Conversation::StopAnimationAction *>(pAction)));
        break;

    case Staging::ConversationActionType_SetPartner:
        actions.append(new SetPartnerAction(static_cast<Staging::Conversation::SetPartnerAction *>(pAction)));
        break;

    case Staging::ConversationActionType_GoToPresentWrongEvidence:
        actions.append(new GoToPresentWrongEvidenceAction(static_cast<Staging::Conversation::GoToPresentWrongEvidenceAction *>(pAction)));
        break;

    case Staging::ConversationActionType_LockConversation:
        actions.append(new LockConversationAction(static_cast<Staging::Conversation::LockConversationAction *>(pAction)));
        break;

    case Staging::ConversationActionType_ExitEncounter:
        actions.append(new ExitEncounterAction(static_cast<Staging::Conversation::ExitEncounterAction *>(pAction)));
        break;

    case Staging::ConversationActionType_MoveToLocation:
        actions.append(new MoveToLocationAction(static_cast<Staging::Conversation::MoveToLocationAction *>(pAction)));
        break;

    case Staging::ConversationActionType_MoveToZoomedView:
        actions.append(new MoveToZoomedViewAction(static_cast<Staging::Conversation::MoveToZoomedViewAction *>(pAction)));
        break;

    case Staging::ConversationActionType_EndCase:
        actions.append(new EndCaseAction(static_cast<Staging::Conversation::EndCaseAction *>(pAction)));
        break;

    case Staging::ConversationActionType_BeginMultipleChoice:
        {
            Staging::Conversation::BeginMultipleChoiceAction *pStagingBeginMultipleChoiceAction =
                    static_cast<Staging::Conversation::BeginMultipleChoiceAction *>(pAction);

            if (pStagingBeginMultipleChoiceAction->OptionTexts.count() != pStagingBeginMultipleChoiceAction->OptionIndexes.count())
            {
                throw new MLIException("Expected there to be an equal number of option texts and option indexes in MultipleChoiceActions.");
            }

            MultipleChoiceAction *pMultipleChoiceAction = new MultipleChoiceAction(pStagingBeginMultipleChoiceAction);

            int endIndex = -1;

            // Track the number of BeginMultipleChoiceActions that we've seen,
            // so we're sure that we're matching with the right end action.
            int beginMultipleChoiceCount = 1;

            for (int j = indexInStagingActions + 1; j < stagingActions.count(); j++)
            {
                if (stagingActions[j]->GetType() == Staging::ConversationActionType_EndMultipleChoice)
                {
                    beginMultipleChoiceCount--;

                    if (beginMultipleChoiceCount == 0)
                    {
                        endIndex = j;
                        break;
                    }
                }
                else if (stagingActions[j]->GetType() == Staging::ConversationActionType_BeginMultipleChoice)
                {
                    beginMultipleChoiceCount++;
                }
            }

            if (endIndex < 0)
            {
                throw new MLIException("Expected a EndMultipleChoiceAction to follow a BeginMultipleChoiceAction.");
            }

            for (int j = 0; j < pStagingBeginMultipleChoiceAction->OptionIndexes.count(); j++)
            {
                QList<Action *> optionActions;
                QList<Staging::Conversation::Action *> optionStagingActions;

                for (int k = pStagingBeginMultipleChoiceAction->OptionIndexes[j] - initialIndexInFullList;
                     k < (j < pStagingBeginMultipleChoiceAction->OptionIndexes.count() - 1 ?
                          pStagingBeginMultipleChoiceAction->OptionIndexes[j + 1] - initialIndexInFullList :
                          endIndex);
                     k++)
                {
                    optionStagingActions.append(stagingActions[k]);
                }

                PopulateActionsFromStaging(optionActions, optionStagingActions, pStagingBeginMultipleChoiceAction->OptionIndexes[j] - initialIndexInFullList);
                pMultipleChoiceAction->AddOption(pStagingBeginMultipleChoiceAction->OptionTexts[j], optionActions);
            }

            actions.append(pMultipleChoiceAction);
            indexInStagingActions = endIndex - 1;
        }
        break;

    case Staging::ConversationActionType_ExitMultipleChoice:
        actions.append(new ExitMultipleChoiceAction(static_cast<Staging::Conversation::ExitMultipleChoiceAction *>(pAction)));
        break;

    case Staging::ConversationActionType_EnableFastForward:
        actions.append(new EnableFastForwardAction(static_cast<Staging::Conversation::EnableFastForwardAction *>(pAction)));
        break;

    case Staging::ConversationActionType_DisableFastForward:
        actions.append(new DisableFastForwardAction(static_cast<Staging::Conversation::DisableFastForwardAction *>(pAction)));
        break;

    case Staging::ConversationActionType_BeginBreakdown:
        actions.append(new BeginBreakdownAction(static_cast<Staging::Conversation::BeginBreakdownAction *>(pAction)));
        break;

    case Staging::ConversationActionType_EndBreakdown:
        actions.append(new EndBreakdownAction(static_cast<Staging::Conversation::EndBreakdownAction *>(pAction)));
        break;

    default:
        qDebug("Ignoring conversation action of type '%s'.", ConversationActionTypeToString(pAction->GetType()).toStdString().c_str());
        break;
    }
}

void Conversation::UpdateAndCacheConversationStates()
{
    State state;

    if (pOwningEncounter != NULL)
    {
        state.SetLeftCharacterId(pOwningEncounter->GetInitialLeftCharacterId());
        state.SetLeftCharacterEmotionId(pOwningEncounter->GetInitialLeftCharacterEmotionId());
        state.SetRightCharacterId(pOwningEncounter->GetInitialRightCharacterId());
        state.SetRightCharacterEmotionId(pOwningEncounter->GetInitialRightCharacterEmotionId());
    }

    for (Action *pAction : actions)
    {
        pAction->UpdateAndCacheConversationState(state);
    }
}

void Conversation::ReplaceAction(Action *pNewAction, Action *pOldAction)
{
    Conversation::ReplaceAction(pNewAction, pOldAction, actions);
}

void Conversation::ReplaceAction(Action *pNewAction, Action *pOldAction, QList<Action *> &actionListToSearch)
{
    for (int i = 0; i < actionListToSearch.length(); i++)
    {
        if (pOldAction == actionListToSearch[i])
        {
             actionListToSearch[i] = pNewAction;
             delete pOldAction;
             break;
        }
    }
}

void Conversation::PreloadAudio()
{
    for (Action *pAction : actions)
    {
        pAction->PreloadAudio();
    }
}

ObjectListWidget<Conversation::Action, Conversation::State, Conversation::ActionType> * Conversation::GetObjectListWidget()
{
    ObjectListWidget<Conversation::Action, Conversation::State, Conversation::ActionType> *pListWidget = new ObjectListWidget<Conversation::Action, Conversation::State, Conversation::ActionType>();

    pListWidget->BeginListItemChanges();
    pListWidget->AddListItems(Conversation::Action::GetListItemsForActions(actions, 0));
    pListWidget->NotifyListItemChangesComplete();
    pListWidget->UpdateListItemStatesBeforeObject();

    return pListWidget;
}

Conversation::DrawingView::DrawingView(Conversation *pConversation, IDrawable *pParent)
    : IDrawable(pParent)
    , IDrawingView<Conversation>(pConversation, pParent)
{
    pDialogBackgroundPixmapItem = new QGraphicsPixmapItem(QPixmap(CaseContent::GetInstance()->RelativePathToAbsolutePath("Images/Misc/DialogBackground.png")));

    pLeftCharacterDialogDrawingView = NULL;
    pRightCharacterDialogDrawingView = NULL;

    pDialogItemGroup = new QGraphicsItemGroup();
    pDialogItemGroupOpacityEffect = new QGraphicsOpacityEffect();
    pDialogItemGroupOpacityEffect->setOpacity(0);
    pDialogItemGroup->setGraphicsEffect(pDialogItemGroupOpacityEffect);

    pCharacterNameTabPixmapItem = new QGraphicsPixmapItem(QPixmap(CaseContent::GetInstance()->RelativePathToAbsolutePath("Images/Misc/Tab.png")));
    pCharacterNameTabPixmapItem->setY(363 - pCharacterNameTabPixmapItem->pixmap().height());
    pCharacterNameSimpleTextItem = new QGraphicsSimpleTextItem("");
    pCharacterNameSimpleTextItem->setFont(QFont("Celestia Redux", 15));
    pCharacterNameSimpleTextItem->setBrush(QBrush(QColor(255, 255, 255)));

    pDialogItemGroup->addToGroup(pCharacterNameTabPixmapItem);
    pDialogItemGroup->addToGroup(pCharacterNameSimpleTextItem);

    QFont dialogFont("FayesMousewriting", 28);
    QFontMetrics dialogFontMetrics(dialogFont);

    for (int i = 0; i < 5; i++)
    {
        pDialogLinesTextItems[i] = new QGraphicsTextItem("");
        pDialogLinesTextItems[i]->setFont(dialogFont);
        pDialogLinesTextItems[i]->setDefaultTextColor(QColor(255, 255, 255));
        pDialogLinesTextItems[i]->setX(dialogTextArea.GetX() + dialogPadding);
        pDialogLinesTextItems[i]->setY(dialogTextArea.GetY() + dialogPadding + i * dialogFontMetrics.height());
        pDialogItemGroup->addToGroup(pDialogLinesTextItems[i]);
    }
}

Conversation::DrawingView::~DrawingView()
{
    Reset();

    /*delete pDialogBackgroundPixmapItem;
    pDialogBackgroundPixmapItem = NULL;

    delete pLeftCharacterDialogDrawingView;
    pLeftCharacterDialogDrawingView = NULL;

    delete pRightCharacterDialogDrawingView;
    pRightCharacterDialogDrawingView = NULL;

    delete pDialogItemGroup;
    pDialogItemGroup = NULL;*/
}

void Conversation::DrawingView::DrawCore(QGraphicsScene *pScene, QList<QGraphicsItem *> &addedItems)
{
    pScene->addItem(pDialogBackgroundPixmapItem);
    addedItems.append(pDialogBackgroundPixmapItem);

    pScene->addItem(pDialogItemGroup);
    addedItems.append(pDialogItemGroup);
}

void Conversation::DrawingView::DrawChildren(QGraphicsScene *pScene)
{
    if (pLeftCharacterDialogDrawingView != NULL)
    {
        pLeftCharacterDialogDrawingView->Draw(pScene);
    }

    if (pRightCharacterDialogDrawingView != NULL)
    {
        pRightCharacterDialogDrawingView->Draw(pScene);
    }
}

void Conversation::DrawingView::UpdateCore()
{
    pDialogBackgroundPixmapItem->setZValue(1);
    pDialogItemGroup->setZValue(2);
}

void Conversation::DrawingView::UpdateChildren()
{
    if (pLeftCharacterDialogDrawingView != NULL)
    {
        pLeftCharacterDialogDrawingView->Update();
    }

    if (pRightCharacterDialogDrawingView != NULL)
    {
        pRightCharacterDialogDrawingView->Update();
    }
}

void Conversation::DrawingView::ResetChildren()
{
    if (pLeftCharacterDialogDrawingView != NULL)
    {
        pLeftCharacterDialogDrawingView->Reset();
    }

    if (pRightCharacterDialogDrawingView != NULL)
    {
        pRightCharacterDialogDrawingView->Reset();
    }
}

void Conversation::DrawingView::SetLeftCharacter(QString characterId, QString emotionId)
{
    if (characterId != currentLeftCharacterId || emotionId != currentLeftCharacterEmotionId)
    {
        if (characterId != currentLeftCharacterId)
        {
            delete pLeftCharacterDialogDrawingView;

            if (characterId.length() > 0)
            {
                Character *pNewCharacter = CaseContent::GetInstance()->GetById<Character>(characterId);
                pLeftCharacterDialogDrawingView = pNewCharacter->GetDialogDrawingView(this);
                pLeftCharacterDialogDrawingView->SetEmotion(emotionId);
                pLeftCharacterDialogDrawingView->SetPosition(Vector2(0, 0));
                pLeftCharacterDialogDrawingView->SetIsFlipped(true);
            }
            else
            {
                pLeftCharacterDialogDrawingView = NULL;
            }
        }
        else if (pLeftCharacterDialogDrawingView != NULL)
        {
            pLeftCharacterDialogDrawingView->SetEmotion(emotionId);
        }

        currentLeftCharacterId = characterId;
        currentLeftCharacterEmotionId = emotionId;

        Redraw();
    }
}

void Conversation::DrawingView::SetRightCharacter(QString characterId, QString emotionId)
{
    if (characterId != currentRightCharacterId || emotionId != currentRightCharacterEmotionId)
    {
        if (characterId != currentRightCharacterId)
        {
            delete pRightCharacterDialogDrawingView;

            if (characterId.length() > 0)
            {
                Character *pNewCharacter = CaseContent::GetInstance()->GetById<Character>(characterId);
                pRightCharacterDialogDrawingView = pNewCharacter->GetDialogDrawingView(this);
                pRightCharacterDialogDrawingView->SetEmotion(emotionId);
                pRightCharacterDialogDrawingView->SetPosition(Vector2(480, 0));
            }
            else
            {
                pRightCharacterDialogDrawingView = NULL;
            }
        }
        else if (pRightCharacterDialogDrawingView != NULL)
        {
            pRightCharacterDialogDrawingView->SetEmotion(emotionId);
        }

        currentRightCharacterId = characterId;
        currentRightCharacterEmotionId = emotionId;

        Redraw();
    }
}

void Conversation::DrawingView::SetDialog(CharacterPosition characterPosition, const QStringList &dialogHtmlLines, QString offscreenCharacterId, bool isTalking)
{
    if (pLeftCharacterDialogDrawingView != NULL)
    {
        if (characterPosition == CharacterPositionLeft)
        {
            pLeftCharacterDialogDrawingView->SetIsSpeaking(isTalking);
        }
        else
        {
            pLeftCharacterDialogDrawingView->SetIsSpeaking(false);
        }
    }

    if (pRightCharacterDialogDrawingView != NULL)
    {
        if (characterPosition == CharacterPositionRight)
        {
            pRightCharacterDialogDrawingView->SetIsSpeaking(isTalking);
        }
        else
        {
            pRightCharacterDialogDrawingView->SetIsSpeaking(false);
        }
    }

    if (characterPosition == CharacterPositionNone)
    {
        pDialogItemGroupOpacityEffect->setOpacity(0);
    }
    else
    {
        QString characterId;

        if (characterPosition == CharacterPositionLeft)
        {
            characterId = currentLeftCharacterId;
        }
        else if (characterPosition == CharacterPositionRight)
        {
            characterId = currentRightCharacterId;
        }
        else if (characterPosition == CharacterPositionOffscreen)
        {
            characterId = offscreenCharacterId;
        }

        if (characterId.length() > 0)
        {
            pCharacterNameSimpleTextItem->setText(CaseContent::GetInstance()->GetById<Character>(characterId)->GetDisplayName().toUpper());
        }
        else
        {
            pCharacterNameSimpleTextItem->setText("????");
        }

        QFontMetrics characterNameFontMetrics(pCharacterNameSimpleTextItem->font());
        pCharacterNameSimpleTextItem->setX(pCharacterNameTabPixmapItem->pixmap().width() / 2 - characterNameFontMetrics.width(pCharacterNameSimpleTextItem->text()) / 2);
        pCharacterNameSimpleTextItem->setY(pCharacterNameTabPixmapItem->y() + 1);

        for (int i = 0; i < Min(dialogHtmlLines.length(), 5); i++)
        {
            pDialogLinesTextItems[i]->setHtml(dialogHtmlLines[i]);
        }

        for (int i = Min(dialogHtmlLines.length(), 5); i < 5; i++)
        {
            pDialogLinesTextItems[i]->setHtml("");
        }

        pDialogItemGroupOpacityEffect->setOpacity(1);
    }
}

Conversation::UnlockCondition * Conversation::UnlockCondition::LoadFromStagingObject(Staging::Conversation::UnlockCondition *pStagingUnlockCondition)
{
    switch (pStagingUnlockCondition->GetType())
    {
    case Staging::UnlockConditionType_FlagSet:
        return new Conversation::FlagSetUnlockCondition(static_cast<Staging::Conversation::FlagSetUnlockCondition *>(pStagingUnlockCondition));

    case Staging::UnlockConditionType_PartnerPresent:
        return new Conversation::PartnerPresentUnlockCondition(static_cast<Staging::Conversation::PartnerPresentUnlockCondition *>(pStagingUnlockCondition));

    default:
        throw new MLIException("Unexpected unlock condition type.");
    }
}

Conversation::UnlockCondition * Conversation::UnlockCondition::CreateFromXml(XmlReader *pReader)
{
    if (pReader->ElementExists(Conversation::FlagSetUnlockCondition::GetNameForXml()))
    {
        return new Conversation::FlagSetUnlockCondition(pReader);
    }
    else if (pReader->ElementExists(Conversation::PartnerPresentUnlockCondition::GetNameForXml()))
    {
        return new Conversation::PartnerPresentUnlockCondition(pReader);
    }
    else
    {
        throw new MLIException("Unexpected unlock condition type.");
    }
}

Conversation::FlagSetUnlockCondition::FlagSetUnlockCondition(Staging::Conversation::FlagSetUnlockCondition *pStagingFlagSetUnlockCondition)
{
    flagId = pStagingFlagSetUnlockCondition->FlagId;
}

Conversation::UnlockCondition * Conversation::FlagSetUnlockCondition::Clone()
{
    Conversation::FlagSetUnlockCondition *pClonedCondition = new Conversation::FlagSetUnlockCondition();

    pClonedCondition->flagId = flagId;

    return pClonedCondition;
}

QString Conversation::FlagSetUnlockCondition::GetDisplayString()
{
    return QString("Lock this conversation until the flag ") + UnderlineString(flagId) + QString(" is set.");
}

Conversation::PartnerPresentUnlockCondition::PartnerPresentUnlockCondition(Staging::Conversation::PartnerPresentUnlockCondition *pStagingPartnerPresentUnlockCondition)
{
    partnerId = pStagingPartnerPresentUnlockCondition->PartnerId;
}

Conversation::UnlockCondition * Conversation::PartnerPresentUnlockCondition::Clone()
{
    Conversation::PartnerPresentUnlockCondition *pClonedCondition = new Conversation::PartnerPresentUnlockCondition();

    pClonedCondition->partnerId = partnerId;

    return pClonedCondition;
}

QString Conversation::PartnerPresentUnlockCondition::GetDisplayString()
{
    return QString("Lock this conversation until the partner ") + UnderlineString(partnerId) + QString(" is present.");
}

bool Conversation::State::SetLeftCharacterId(const QString &id)
{
    Character *pCharacter = CaseContent::GetInstance()->GetById<Character>(id);

    if (pCharacter != NULL)
    {
        this->leftCharacterId = id;
        return true;
    }
    else
    {
        return false;
    }
}

bool Conversation::State::SetLeftCharacterEmotionId(const QString &id)
{
    Character *pCharacter = CaseContent::GetInstance()->GetById<Character>(leftCharacterId);

    if (pCharacter != NULL)
    {
        QString newEmotionId = "";

        for (const QString &emotionId : pCharacter->GetEmotionIds())
        {
            if (QRegExp(emotionId, Qt::CaseInsensitive).exactMatch(id))
            {
                newEmotionId = id;
                break;
            }
        }

        if (newEmotionId.length() > 0)
        {
            this->leftCharacterEmotionId = newEmotionId;
            return true;
        }
    }

    return false;
}

bool Conversation::State::SetRightCharacterId(const QString &id)
{
    Character *pCharacter = CaseContent::GetInstance()->GetById<Character>(id);

    if (pCharacter != NULL)
    {
        this->rightCharacterId = id;
        return true;
    }
    else
    {
        return false;
    }
}

bool Conversation::State::SetRightCharacterEmotionId(const QString &id)
{
    Character *pCharacter = CaseContent::GetInstance()->GetById<Character>(rightCharacterId);

    if (pCharacter != NULL)
    {
        QString newEmotionId = "";

        for (const QString &emotionId : pCharacter->GetEmotionIds())
        {
            if (QRegExp(emotionId, Qt::CaseInsensitive).exactMatch(id))
            {
                newEmotionId = id;
                break;
            }
        }

        if (newEmotionId.length() > 0)
        {
            this->rightCharacterEmotionId = newEmotionId;
            return true;
        }
    }

    return false;
}

Conversation::Action * Conversation::Action::Clone()
{
    Conversation::Action *pClonedAction = NULL;

    switch(GetType())
    {
    case Conversation::ActionType::CharacterChange:
        pClonedAction = new Conversation::CharacterChangeAction();
        break;
    case Conversation::ActionType::SetFlag:
        pClonedAction = new Conversation::SetFlagAction();
        break;
    case Conversation::ActionType::BranchOnCondition:
        pClonedAction = new Conversation::BranchOnConditionAction();
        break;
    case Conversation::ActionType::ShowDialog:
        pClonedAction = new Conversation::ShowDialogAction();
        break;
    case Conversation::ActionType::MustPresentEvidence:
        pClonedAction = new Conversation::MustPresentEvidenceAction();
        break;
    case Conversation::ActionType::EnableConversation:
        pClonedAction = new Conversation::EnableConversationAction();
        break;
    case Conversation::ActionType::EnableEvidence:
        pClonedAction = new Conversation::EnableEvidenceAction();
        break;
    case Conversation::ActionType::UpdateEvidence:
        pClonedAction = new Conversation::UpdateEvidenceAction();
        break;
    case Conversation::ActionType::DisableEvidence:
        pClonedAction = new Conversation::DisableEvidenceAction();
        break;
    case Conversation::ActionType::EnableCutscene:
        pClonedAction = new Conversation::EnableCutsceneAction();
        break;
    case Conversation::ActionType::PlayBgm:
        pClonedAction = new Conversation::PlayBgmAction();
        break;
    case Conversation::ActionType::PauseBgm:
        pClonedAction = new Conversation::PauseBgmAction();
        break;
    case Conversation::ActionType::ResumeBgm:
        pClonedAction = new Conversation::ResumeBgmAction();
        break;
    case Conversation::ActionType::StopBgm:
        pClonedAction = new Conversation::StopBgmAction();
        break;
    case Conversation::ActionType::PlayAmbiance:
        pClonedAction = new Conversation::PlayAmbianceAction();
        break;
    case Conversation::ActionType::PauseAmbiance:
        pClonedAction = new Conversation::PauseAmbianceAction();
        break;
    case Conversation::ActionType::ResumeAmbiance:
        pClonedAction = new Conversation::ResumeAmbianceAction();
        break;
    case Conversation::ActionType::StopAmbiance:
        pClonedAction = new Conversation::StopAmbianceAction();
        break;
    case Conversation::ActionType::StartAnimation:
        pClonedAction = new Conversation::StartAnimationAction();
        break;
    case Conversation::ActionType::StopAnimation:
        pClonedAction = new Conversation::StopAnimationAction();
        break;
    case Conversation::ActionType::SetPartner:
        pClonedAction = new Conversation::SetPartnerAction();
        break;
    case Conversation::ActionType::GoToPresentWrongEvidence:
        pClonedAction = new Conversation::GoToPresentWrongEvidenceAction();
        break;
    case Conversation::ActionType::LockConversation:
        pClonedAction = new Conversation::LockConversationAction();
        break;
    case Conversation::ActionType::ExitEncounter:
        pClonedAction = new Conversation::ExitEncounterAction();
        break;
    case Conversation::ActionType::MoveToLocation:
        pClonedAction = new Conversation::MoveToLocationAction();
        break;
    case Conversation::ActionType::MoveToZoomedView:
        pClonedAction = new Conversation::MoveToZoomedViewAction();
        break;
    case Conversation::ActionType::EndCase:
        pClonedAction = new Conversation::EndCaseAction();
        break;
    case Conversation::ActionType::MultipleChoice:
        pClonedAction = new Conversation::MultipleChoiceAction();
        break;
    case Conversation::ActionType::ExitMultipleChoice:
        pClonedAction = new Conversation::ExitMultipleChoiceAction();
        break;
    case Conversation::ActionType::EnableFastForward:
        pClonedAction = new Conversation::EnableFastForwardAction();
        break;
    case Conversation::ActionType::DisableFastForward:
        pClonedAction = new Conversation::DisableFastForwardAction();
        break;
    case Conversation::ActionType::BeginBreakdown:
        pClonedAction = new Conversation::BeginBreakdownAction();
        break;
    case Conversation::ActionType::EndBreakdown:
        pClonedAction = new Conversation::EndBreakdownAction();
        break;
    case Conversation::ActionType::InterrogationRepeat:
        pClonedAction = new Interrogation::InterrogationRepeatAction();
        break;
    case Conversation::ActionType::ShowInterrogation:
        pClonedAction = new Interrogation::ShowInterrogationAction();
        break;
    case Conversation::ActionType::ExitInterrogationRepeat:
        pClonedAction = new Interrogation::ExitInterrogationRepeatAction();
        break;
    case Conversation::ActionType::ConfrontationTopicSelection:
        pClonedAction = new Confrontation::ConfrontationTopicSelectionAction();
        break;
    case Conversation::ActionType::EnableTopic:
        pClonedAction = new Confrontation::EnableTopicAction();
        break;
    case Conversation::ActionType::RestartDecision:
        pClonedAction = new Confrontation::RestartDecisionAction();
        break;
    case Conversation::ActionType::RestartConfrontation:
        pClonedAction = new Confrontation::RestartConfrontationAction();
        break;
    default:
        throw new MLIException("Unknown action type");
    }

    CopyProperties(pClonedAction, true /* isForEdit */);
    return pClonedAction;
}

void Conversation::Action::CopyProperties(ListItemObject<Conversation::State, Conversation::ActionType> *pOther, bool isForEdit)
{
    if (!IsSameType(pOther))
    {
        throw new MLIException("Can only copy properties between actions of the same type.");
    }

    CopyPropertiesImpl(dynamic_cast<Conversation::Action *>(pOther), isForEdit);
}

void Conversation::Action::ExchangeListItemBaseOwnership(ListItemObject<Conversation::State, Conversation::ActionType> *pOther)
{
    if (GetType() != pOther->GetType())
    {
        throw new MLIException("Can only exchange ListItemBase ownership between actions of the same type.");
    }

    ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *pListItemBase = GetListItemBase();
    Conversation::Action *pAction = dynamic_cast<Conversation::Action *>(pOther);

    if (pListItemBase != NULL)
    {
        pListItemBase->SetObject(pAction);
    }

    ExchangeListItemBaseOwnershipImpl(dynamic_cast<Conversation::Action *>(pOther));
}

void Conversation::Action::CloneActionList(QList<Action *> &destination, const QList<Action *> &source)
{
    for (Action *pAction : destination)
    {
        delete pAction;
    }

    destination.clear();

    for (Action *pAction : source)
    {
        destination.push_back(pAction->Clone());
    }
}

void Conversation::Action::ExchangeListItemBaseOwnership(const QList<Action *> &destination, const QList<Action *> &source)
{
    if (destination.size() != source.size())
    {
        throw new MLIException("ExchangeListItemBaseOwnership must only be called on cloned lists of actions.");
    }

    for (int i = 0; i < destination.size(); i++)
    {
        Action *pSourceAction = source[i];
        Action *pDestinationAction = destination[i];

        pSourceAction->ExchangeListItemBaseOwnership(pDestinationAction);
    }
}

Conversation::Action * Conversation::Action::CreateFromXml(XmlReader *pReader)
{
    if (pReader->ElementExists(CharacterChangeAction::GetNameForXml()))
    {
        return new CharacterChangeAction(pReader);
    }
    else if (pReader->ElementExists(SetFlagAction::GetNameForXml()))
    {
        return new SetFlagAction(pReader);
    }
    else if (pReader->ElementExists(BranchOnConditionAction::GetNameForXml()))
    {
        return new BranchOnConditionAction(pReader);
    }
    else if (pReader->ElementExists(ShowDialogAction::GetNameForXml()))
    {
        return new ShowDialogAction(pReader);
    }
    else if (pReader->ElementExists(MustPresentEvidenceAction::GetNameForXml()))
    {
        return new MustPresentEvidenceAction(pReader);
    }
    else if (pReader->ElementExists(EnableConversationAction::GetNameForXml()))
    {
        return new EnableConversationAction(pReader);
    }
    else if (pReader->ElementExists(EnableEvidenceAction::GetNameForXml()))
    {
        return new EnableEvidenceAction(pReader);
    }
    else if (pReader->ElementExists(UpdateEvidenceAction::GetNameForXml()))
    {
        return new UpdateEvidenceAction(pReader);
    }
    else if (pReader->ElementExists(DisableEvidenceAction::GetNameForXml()))
    {
        return new DisableEvidenceAction(pReader);
    }
    else if (pReader->ElementExists(EnableCutsceneAction::GetNameForXml()))
    {
        return new EnableCutsceneAction(pReader);
    }
    else if (pReader->ElementExists(PlayBgmAction::GetNameForXml()))
    {
        return new PlayBgmAction(pReader);
    }
    else if (pReader->ElementExists(PauseBgmAction::GetNameForXml()))
    {
        return new PauseBgmAction(pReader);
    }
    else if (pReader->ElementExists(ResumeBgmAction::GetNameForXml()))
    {
        return new ResumeBgmAction(pReader);
    }
    else if (pReader->ElementExists(StopBgmAction::GetNameForXml()))
    {
        return new StopBgmAction(pReader);
    }
    else if (pReader->ElementExists(PlayAmbianceAction::GetNameForXml()))
    {
        return new PlayAmbianceAction(pReader);
    }
    else if (pReader->ElementExists(PauseAmbianceAction::GetNameForXml()))
    {
        return new PauseAmbianceAction(pReader);
    }
    else if (pReader->ElementExists(ResumeAmbianceAction::GetNameForXml()))
    {
        return new ResumeAmbianceAction(pReader);
    }
    else if (pReader->ElementExists(StopAmbianceAction::GetNameForXml()))
    {
        return new StopAmbianceAction(pReader);
    }
    else if (pReader->ElementExists(StartAnimationAction::GetNameForXml()))
    {
        return new StartAnimationAction(pReader);
    }
    else if (pReader->ElementExists(StopAnimationAction::GetNameForXml()))
    {
        return new StopAnimationAction(pReader);
    }
    else if (pReader->ElementExists(SetPartnerAction::GetNameForXml()))
    {
        return new SetPartnerAction(pReader);
    }
    else if (pReader->ElementExists(GoToPresentWrongEvidenceAction::GetNameForXml()))
    {
        return new GoToPresentWrongEvidenceAction(pReader);
    }
    else if (pReader->ElementExists(LockConversationAction::GetNameForXml()))
    {
        return new LockConversationAction(pReader);
    }
    else if (pReader->ElementExists(ExitEncounterAction::GetNameForXml()))
    {
        return new ExitEncounterAction(pReader);
    }
    else if (pReader->ElementExists(MoveToLocationAction::GetNameForXml()))
    {
        return new MoveToLocationAction(pReader);
    }
    else if (pReader->ElementExists(MoveToZoomedViewAction::GetNameForXml()))
    {
        return new MoveToZoomedViewAction(pReader);
    }
    else if (pReader->ElementExists(EndCaseAction::GetNameForXml()))
    {
        return new EndCaseAction(pReader);
    }
    else if (pReader->ElementExists(MultipleChoiceAction::GetNameForXml()))
    {
        return new MultipleChoiceAction(pReader);
    }
    else if (pReader->ElementExists(ExitMultipleChoiceAction::GetNameForXml()))
    {
        return new ExitMultipleChoiceAction(pReader);
    }
    else if (pReader->ElementExists(EnableFastForwardAction::GetNameForXml()))
    {
        return new EnableFastForwardAction(pReader);
    }
    else if (pReader->ElementExists(DisableFastForwardAction::GetNameForXml()))
    {
        return new DisableFastForwardAction(pReader);
    }
    else if (pReader->ElementExists(BeginBreakdownAction::GetNameForXml()))
    {
        return new BeginBreakdownAction(pReader);
    }
    else if (pReader->ElementExists(EndBreakdownAction::GetNameForXml()))
    {
        return new EndBreakdownAction(pReader);
    }
    else if (pReader->ElementExists(Interrogation::InterrogationRepeatAction::GetNameForXml()))
    {
        return new Interrogation::InterrogationRepeatAction(pReader);
    }
    else if (pReader->ElementExists(Interrogation::ShowInterrogationAction::GetNameForXml()))
    {
        return new Interrogation::ShowInterrogationAction(pReader);
    }
    else if (pReader->ElementExists(Interrogation::ExitInterrogationRepeatAction::GetNameForXml()))
    {
        return new Interrogation::ExitInterrogationRepeatAction(pReader);
    }
    else if (pReader->ElementExists(Confrontation::ConfrontationTopicSelectionAction::GetNameForXml()))
    {
        return new Confrontation::ConfrontationTopicSelectionAction(pReader);
    }
    else if (pReader->ElementExists(Confrontation::EnableTopicAction::GetNameForXml()))
    {
        return new Confrontation::EnableTopicAction(pReader);
    }
    else if (pReader->ElementExists(Confrontation::RestartDecisionAction::GetNameForXml()))
    {
        return new Confrontation::RestartDecisionAction(pReader);
    }
    else if (pReader->ElementExists(Confrontation::RestartConfrontationAction::GetNameForXml()))
    {
        return new Confrontation::RestartConfrontationAction(pReader);
    }
    else
    {
        throw new MLIException("Unknown conversation action type.");
    }
}

QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> Conversation::Action::GetListItemsForActions(QList<Conversation::Action *> &actions, int indentLevel)
{
    QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> listItems;

    listItems.append(new TopSeparatorListItem<Conversation::Action, Conversation::State, Conversation::ActionType>(indentLevel, &actions, 0, true /* shouldInsert */));

    if (actions.length() > 0)
    {
        int currentPosition = 0;
        int itemCount = actions.length();

        for (Conversation::Action *pAction : actions)
        {
            for (ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *pChildListItem : pAction->GetListItems(indentLevel))
            {
                listItems.append(pChildListItem);

                if (pChildListItem->GetIndentLevel() == indentLevel)
                {
                    pChildListItem->SetContainingObjectList(&actions, currentPosition, false /* shouldInsert */);
                }
            }

            currentPosition++;

            if (currentPosition != itemCount)
            {
                 listItems.append(new MiddleSeparatorListItem<Conversation::Action, Conversation::State, Conversation::ActionType>(indentLevel, &actions, currentPosition, true /* shouldInsert */));
            }
        }

        listItems.append(new BottomSeparatorListItem<Conversation::Action, Conversation::State, Conversation::ActionType>(indentLevel, &actions, itemCount, true /* shouldInsert */));
    }

    return listItems;
}

QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> Conversation::Action::GetListItems(int indentLevel)
{
    return GetListItemsImpl(indentLevel);
}

void Conversation::Action::PushToDrawingView(Conversation::DrawingView *pDrawingView)
{
    pDrawingView->SetLeftCharacter(
        stateDuringAction.GetLeftCharacterId(),
        stateDuringAction.GetLeftCharacterEmotionId());

    pDrawingView->SetRightCharacter(
        stateDuringAction.GetRightCharacterId(),
        stateDuringAction.GetRightCharacterEmotionId());

    pDrawingView->SetDialog(CharacterPositionNone, QStringList(), "", false);
}

Conversation::ActionWithNotification::ActionWithNotification()
    : Action()
{
    shouldNotify = false;
}

void Conversation::ActionWithNotification::AddNotification(Staging::Conversation::NotificationAction *pStagingNotificationAction)
{
    shouldNotify = true;
    rawNotificationText = pStagingNotificationAction->RawNotificationText;
    oldEvidenceId = pStagingNotificationAction->OldEvidenceId;
    newEvidenceId = pStagingNotificationAction->NewEvidenceId;
    partnerId = pStagingNotificationAction->PartnerId;
    locationWithCutsceneId = pStagingNotificationAction->LocationWithCutsceneId;
    cutsceneId = pStagingNotificationAction->CutsceneId;
}

void Conversation::ActionWithNotification::CopyPropertiesImpl(Conversation::Conversation::Action *pOther, bool /*isForEdit*/)
{
    Conversation::ActionWithNotification *pAction = static_cast<Conversation::ActionWithNotification *>(pOther);

    pAction->shouldNotify = shouldNotify;
    pAction->rawNotificationText = rawNotificationText;
    pAction->oldEvidenceId = oldEvidenceId;
    pAction->newEvidenceId = newEvidenceId;
    pAction->partnerId = partnerId;
    pAction->locationWithCutsceneId = locationWithCutsceneId;
    pAction->cutsceneId = cutsceneId;
}

Conversation::CharacterChangeAction::CharacterChangeAction()
    : Action()
{
    position = CharacterPositionNone;
}

Conversation::CharacterChangeAction::CharacterChangeAction(Staging::Conversation::CharacterChangeAction *pStagingCharacterChangeAction)
    : Action()
{
    position = (CharacterPosition)(pStagingCharacterChangeAction->Position);
    characterIdToChangeTo = pStagingCharacterChangeAction->CharacterIdToChangeTo;
    initialEmotionId = pStagingCharacterChangeAction->InitialEmotionId;
    sfxId = pStagingCharacterChangeAction->SfxId;
}

void Conversation::CharacterChangeAction::WriteToCaseXml(XmlWriter *pWriter, int& currentActionIndex)
{
    pWriter->StartElement("CharacterChangeAction");
    pWriter->WriteTextElement("Position", CharacterPositionToString(position));
    pWriter->WriteTextElement("CharacterIdToChangeTo", characterIdToChangeTo);
    pWriter->WriteTextElement("InitialEmotionId", initialEmotionId);

    if (sfxId.length() > 0)
    {
        pWriter->WriteTextElement("SfxId", sfxId);
    }

    pWriter->EndElement();

    currentActionIndex++;
}

void Conversation::CharacterChangeAction::UpdateAndCacheConversationState(State &currentState)
{
    if (position == CharacterPositionLeft)
    {
        currentState.SetLeftCharacterId(characterIdToChangeTo);
        currentState.SetLeftCharacterEmotionId(initialEmotionId);
    }
    else
    {
        currentState.SetRightCharacterId(characterIdToChangeTo);
        currentState.SetRightCharacterEmotionId(initialEmotionId);
    }

    Conversation::Action::UpdateAndCacheConversationState(currentState);
}

EditorDialogContents<Conversation::Action, Conversation::State> * Conversation::CharacterChangeAction::CreateEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::Action *pActionToEdit)
{
    return new CharacterChangeActionEditorDialogContents(stateBeforeObject, dynamic_cast<Conversation::CharacterChangeAction *>(pActionToEdit));
}

void Conversation::CharacterChangeAction::CopyPropertiesImpl(Conversation::Action *pOther, bool /*isForEdit*/)
{
    Conversation::CharacterChangeAction *pAction = static_cast<Conversation::CharacterChangeAction *>(pOther);

    pAction->position = position;
    pAction->characterIdToChangeTo = characterIdToChangeTo;
    pAction->initialEmotionId = initialEmotionId;
    pAction->sfxId = sfxId;
}

QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> Conversation::CharacterChangeAction::GetListItemsImpl(int indentLevel)
{
    QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> editorList;
    editorList.append(new CharacterChangeActionListItem(this, indentLevel));
    return editorList;
}

QString CharacterChangeActionListItem::GetDisplayString()
{
    QString displayString = "Change the ";
    displayString += UnderlineString(CharacterPositionToString(pAction->position));
    displayString += " character to be ";

    Character *pCharacter = CaseContent::GetInstance()->GetById<Character>(pAction->characterIdToChangeTo);

    if (pCharacter != NULL)
    {
        displayString += UnderlineString(pCharacter->GetDisplayName());
        displayString += " with the emotion ";
        displayString += UnderlineString(pAction->initialEmotionId);
    }
    else
    {
        displayString += UnderlineString("no character");
    }

    displayString += ".";
    return displayString;
}

CharacterChangeActionEditorDialogContents::CharacterChangeActionEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::CharacterChangeAction *pActionToEdit)
    : EditorDialogContents<Conversation::Action, Conversation::State>(stateBeforeObject, pActionToEdit)
{
    if (pActionToEdit != NULL)
    {
        pCharacterChangeAction = static_cast<Conversation::CharacterChangeAction *>(pActionToEdit->Clone());
    }
    else
    {
        pCharacterChangeAction = new Conversation::CharacterChangeAction();
    }

    pObject = pCharacterChangeAction;

    QHBoxLayout *pLayout = new QHBoxLayout();

    QLabel *pLabel1 = new QLabel("Change the ");
    pLayout->addWidget(pLabel1);

    QComboBox *pCharacterPositionComboBox = new QComboBox();
    pCharacterPositionComboBox->addItem("Left");
    pCharacterPositionComboBox->addItem("Right");
    pLayout->addWidget(pCharacterPositionComboBox);

    if (pActionToEdit != NULL)
    {
        pCharacterPositionComboBox->setCurrentIndex(pCharacterChangeAction->position == CharacterPositionRight ? 1 : 0);
    }

    QLabel *pLabel2 = new QLabel(" character to be ");
    pLayout->addWidget(pLabel2);

    CharacterSelector *pCharacterSelector = new CharacterSelector(true /* allowNoCharacter */);
    pLayout->addWidget(pCharacterSelector);

    Character *pCharacter = NULL;

    if (pActionToEdit != NULL)
    {
        pCharacter = CaseContent::GetInstance()->GetById<Character>(pCharacterChangeAction->characterIdToChangeTo);
    }

    pCharacterEmotionLabel = new QLabel(" with the emotion ");
    pCharacterEmotionLabel->setGraphicsEffect(new QGraphicsOpacityEffect());
    pLayout->addWidget(pCharacterEmotionLabel);

    pCharacterEmotionComboBox = new QComboBox();
    pCharacterEmotionComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    pLayout->addWidget(pCharacterEmotionComboBox);

    if (pCharacter != NULL)
    {
        currentCharacterId = pCharacter->GetId();
        pCharacterSelector->SetToCharacterById(pCharacter->GetId());

        if (pCharacterSelector->GetSelectedCharacterId() > 0)
        {
            const QStringList emotionIds = pCharacter->GetEmotionIds();
            pCharacterEmotionComboBox->addItems(emotionIds);

            int indexOfCurrentEmotion = 0;

            if (pActionToEdit != NULL)
            {
                indexOfCurrentEmotion = emotionIds.indexOf(QRegExp(pCharacterChangeAction->initialEmotionId, Qt::CaseInsensitive));
            }

            if (indexOfCurrentEmotion >= 0)
            {
                pCharacterEmotionComboBox->setCurrentIndex(indexOfCurrentEmotion);
            }
            else
            {
                pCharacterEmotionComboBox->setCurrentText(pCharacter->GetDefaultEmotionId());
            }
        }
        else
        {
            SetOpacity(pCharacterEmotionLabel, 0.0);
            SetOpacity(pCharacterEmotionComboBox, 0.0);
        }
    }
    else
    {
        pCharacterSelector->SetToCharacterById("");

        SetOpacity(pCharacterEmotionLabel, 0.0);
        SetOpacity(pCharacterEmotionComboBox, 0.0);
    }

    QObject::connect(pCharacterPositionComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(CharacterPositionComboBoxCurrentIndexChanged(QString)));
    QObject::connect(pCharacterSelector, SIGNAL(CharacterSelected(QString)), this, SLOT(CharacterSelectorCharacterSelected(QString)));
    QObject::connect(pCharacterEmotionComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(CharacterEmotionComboBoxCurrentIndexChanged(QString)));

    // Call these to initialize everything properly.
    CharacterPositionComboBoxCurrentIndexChanged(pCharacterPositionComboBox->currentText());
    CharacterSelectorCharacterSelected(pCharacterSelector->GetSelectedCharacterId());
    CharacterEmotionComboBoxCurrentIndexChanged(pCharacterEmotionComboBox->currentText());

    pLayout->addStretch(1);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(pLayout);
}

bool CharacterChangeActionEditorDialogContents::ValidateFields()
{
    return true;
}

void CharacterChangeActionEditorDialogContents::CharacterPositionComboBoxCurrentIndexChanged(const QString& newPosition)
{
    pCharacterChangeAction->position = StringToCharacterPosition(newPosition);
}

void CharacterChangeActionEditorDialogContents::CharacterSelectorCharacterSelected(const QString &newCharacterId)
{
    if (newCharacterId.length() > 0)
    {
        Character *pCharacter = CaseContent::GetInstance()->GetById<Character>(newCharacterId);

        if (pCharacter != NULL)
        {
            if (currentCharacterId != newCharacterId)
            {
                currentCharacterId = newCharacterId;
                pCharacterChangeAction->characterIdToChangeTo = newCharacterId;

                const QStringList emotionIds = pCharacter->GetEmotionIds();
                pCharacterEmotionComboBox->clear();
                pCharacterEmotionComboBox->addItems(emotionIds);

                pCharacterEmotionComboBox->setCurrentText(pCharacter->GetDefaultEmotionId());
            }

            SetOpacity(pCharacterEmotionLabel, 1.0);
            SetOpacity(pCharacterEmotionComboBox, 1.0);
        }
        else
        {
            pCharacterChangeAction->characterIdToChangeTo = "";

            SetOpacity(pCharacterEmotionLabel, 0.0);
            SetOpacity(pCharacterEmotionComboBox, 0.0);
        }
    }
    else
    {
        pCharacterChangeAction->characterIdToChangeTo = "";

        SetOpacity(pCharacterEmotionLabel, 0.0);
        SetOpacity(pCharacterEmotionComboBox, 0.0);
    }
}

void CharacterChangeActionEditorDialogContents::CharacterEmotionComboBoxCurrentIndexChanged(const QString& newEmotionId)
{
    pCharacterChangeAction->initialEmotionId = newEmotionId;
}

Conversation::SetFlagAction::SetFlagAction(Staging::Conversation::SetFlagAction *pStagingSetFlagAction)
    : ActionWithNotification()
{
    flagId = pStagingSetFlagAction->FlagId;
}

void Conversation::SetFlagAction::WriteToCaseXml(XmlWriter *pWriter, int& currentActionIndex)
{
    pWriter->StartElement("SetFlagAction");
    pWriter->WriteTextElement("FlagId", flagId);
    pWriter->EndElement();

    currentActionIndex++;
}

EditorDialogContents<Conversation::Action, Conversation::State> * Conversation::SetFlagAction::CreateEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::Action *pActionToEdit)
{
    return new SetFlagActionEditorDialogContents(stateBeforeObject, dynamic_cast<Conversation::SetFlagAction *>(pActionToEdit));
}

QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> Conversation::SetFlagAction::GetListItemsImpl(int indentLevel)
{
    QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> editorList;
    editorList.append(new SetFlagActionListItem(this, indentLevel));
    return editorList;
}

void Conversation::SetFlagAction::CopyPropertiesImpl(Conversation::Action *pOther, bool isForEdit)
{
    Conversation::SetFlagAction *pAction = static_cast<Conversation::SetFlagAction *>(pOther);

    Conversation::ActionWithNotification::CopyPropertiesImpl(pOther, isForEdit);
    pAction->flagId = flagId;
}

QString SetFlagActionListItem::GetDisplayString()
{
    QString displayString = "Set the flag ";
    displayString += UnderlineString(pAction->flagId);
    displayString += " to true.";
    return displayString;
}

SetFlagActionEditorDialogContents::SetFlagActionEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::SetFlagAction *pActionToEdit)
    : EditorDialogContents<Conversation::Action, Conversation::State>(stateBeforeObject, pActionToEdit)
{
    if (pActionToEdit != NULL)
    {
        pSetFlagAction = static_cast<Conversation::SetFlagAction *>(pActionToEdit->Clone());
    }
    else
    {
        pSetFlagAction = new Conversation::SetFlagAction();
    }

    pObject = pSetFlagAction;

    QHBoxLayout *pLayout = new QHBoxLayout();

    QLabel *pLabel1 = new QLabel("Set the flag ");
    pLayout->addWidget(pLabel1);

    FlagEditor *pFlagEditor = new FlagEditor();

    if (pActionToEdit != NULL)
    {
        pFlagEditor->SetToId(pActionToEdit->flagId);
    }

    pLayout->addWidget(pFlagEditor);

    QLabel *pLabel2 = new QLabel(" to true.");
    pLayout->addWidget(pLabel2);

    QObject::connect(pFlagEditor, SIGNAL(FlagSelected(QString)), this, SLOT(FlagEditorFlagSelected(QString)));

    // Call these to initialize everything properly.
    FlagEditorFlagSelected(pFlagEditor->currentText());

    pLayout->addStretch(1);
    pLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(pLayout);
}

bool SetFlagActionEditorDialogContents::ValidateFields()
{
    return true;
}

void SetFlagActionEditorDialogContents::FlagEditorFlagSelected(const QString& newFlagId)
{
    pSetFlagAction->flagId = newFlagId;
}

Conversation::BranchOnConditionAction::BranchOnConditionAction()
    : Action()
{
    pCondition = NULL;
}

Conversation::BranchOnConditionAction::BranchOnConditionAction(Staging::Conversation::BranchOnConditionAction *pStagingBranchOnConditionAction)
    : Action()
{
    pCondition = new Condition(pStagingBranchOnConditionAction->pCondition);
}

Conversation::BranchOnConditionAction::~BranchOnConditionAction()
{
    delete pCondition;
    pCondition = NULL;

    for (Action *pAction : trueActions)
    {
        delete pAction;
    }

    trueActions.clear();

    for (Action *pAction : falseActions)
    {
        delete pAction;
    }

    falseActions.clear();
}

void Conversation::BranchOnConditionAction::WriteToCaseXml(XmlWriter *pWriter, int& currentActionIndex)
{
    pWriter->StartElement("BranchOnConditionAction");
    pWriter->StartElement("Condition");
    pCondition->WriteToCaseXml(pWriter);
    pWriter->EndElement();
    pWriter->WriteIntElement("TrueIndex", currentActionIndex + 1);
    pWriter->WriteIntElement("FalseIndex", currentActionIndex + trueActions.length() + 1);
    pWriter->EndElement();
}

void Conversation::BranchOnConditionAction::ReplaceAction(Conversation::Action *pNewAction, Conversation::Action *pOldAction)
{
    Conversation::ReplaceAction(pNewAction, pOldAction, trueActions);
    Conversation::ReplaceAction(pNewAction, pOldAction, falseActions);
}

void Conversation::BranchOnConditionAction::UpdateAndCacheConversationState(State &currentState)
{
    Conversation::Action::UpdateAndCacheConversationState(currentState);
    State trueActionsBranchState = currentState.Clone();
    State falseActionsBranchState = currentState.Clone();

    for (Conversation::Action *pAction : trueActions)
    {
        pAction->UpdateAndCacheConversationState(trueActionsBranchState);
    }

    for (Conversation::Action *pAction : falseActions)
    {
        pAction->UpdateAndCacheConversationState(falseActionsBranchState);
    }

    currentState = trueActionsBranchState.Clone();
}

void Conversation::BranchOnConditionAction::PreloadAudio()
{
    for (Conversation::Action *pAction : trueActions)
    {
        pAction->PreloadAudio();
    }

    for (Conversation::Action *pAction : falseActions)
    {
        pAction->PreloadAudio();
    }
}

EditorDialogContents<Conversation::Action, Conversation::State> * Conversation::BranchOnConditionAction::CreateEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::Action *pActionToEdit)
{
    return new BranchOnConditionActionEditorDialogContents(stateBeforeObject, dynamic_cast<Conversation::BranchOnConditionAction *>(pActionToEdit));
}

void Conversation::BranchOnConditionAction::CopyPropertiesImpl(Conversation::Action *pOther, bool /*isForEdit*/)
{
    Conversation::BranchOnConditionAction *pAction = static_cast<Conversation::BranchOnConditionAction *>(pOther);

    delete pAction->pCondition;
    pAction->pCondition = pCondition->Clone();

    Conversation::Action::CloneActionList(pAction->trueActions, trueActions);
    Conversation::Action::CloneActionList(pAction->falseActions, falseActions);
}

void Conversation::BranchOnConditionAction::ExchangeListItemBaseOwnershipImpl(Action *pOther)
{
    Conversation::BranchOnConditionAction *pAction = static_cast<Conversation::BranchOnConditionAction *>(pOther);

    Conversation::Action::ExchangeListItemBaseOwnership(pAction->trueActions, trueActions);
    Conversation::Action::ExchangeListItemBaseOwnership(pAction->falseActions, falseActions);
}

QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> Conversation::BranchOnConditionAction::GetListItemsImpl(int indentLevel)
{
    QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> editorList;

    editorList.append(new BranchOnConditionActionListItem(this, indentLevel));
    editorList.append(new TextDisplay<Conversation::Action, Conversation::State, Conversation::ActionType>("If condition is true:", indentLevel + 1));
    editorList.append(GetListItemsForActions(trueActions, indentLevel + 2));
    editorList.append(new TextDisplay<Conversation::Action, Conversation::State, Conversation::ActionType>("If condition is false:", indentLevel + 1));
    editorList.append(GetListItemsForActions(falseActions, indentLevel + 2));

    return editorList;
}

QString BranchOnConditionActionListItem::GetDisplayString()
{
    QString displayString = "Branch on condition: ";
    displayString += UnderlineString(pAction->pCondition->ToString());
    return displayString;
}

BranchOnConditionActionEditorDialogContents::BranchOnConditionActionEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::BranchOnConditionAction *pActionToEdit)
    : EditorDialogContents<Conversation::Action, Conversation::State>(stateBeforeObject, pActionToEdit)
{
    if (pActionToEdit != NULL)
    {
        pBranchOnConditionAction = static_cast<Conversation::BranchOnConditionAction *>(pActionToEdit->Clone());
    }
    else
    {
        pBranchOnConditionAction = new Conversation::BranchOnConditionAction();
    }

    pObject = pBranchOnConditionAction;

    QVBoxLayout *pLayout = new QVBoxLayout();

    pLayout->addWidget(new QLabel("Branch on condition:"));
    pLayout->addSpacing(10);

    if (pActionToEdit != NULL)
    {
        pConditionEditor = pBranchOnConditionAction->pCondition->GetEditor();
    }
    else
    {
        pConditionEditor = new ConditionEditor();
    }

    pLayout->addWidget(pConditionEditor);

    pLayout->setSpacing(0);
    pLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(pLayout);
}

bool BranchOnConditionActionEditorDialogContents::ValidateFields()
{
    return true;
}

void BranchOnConditionActionEditorDialogContents::FinalizeObject()
{
    delete pBranchOnConditionAction->pCondition;
    pBranchOnConditionAction->pCondition = pConditionEditor->GetCondition();
}

Conversation::ShowDialogAction::ShowDialogAction()
    : Action()
{
    speakerPosition = CharacterPositionNone;
    leadInTime = 0;
    shouldAdvanceAutomatically = false;
    delayBeforeContinuing = 0;
}

Conversation::ShowDialogAction::ShowDialogAction(Staging::Conversation::ShowDialogAction *pStagingShowDialogAction)
    : ShowDialogAction()
{
    speakerPosition = (CharacterPosition)pStagingShowDialogAction->SpeakerPosition;
    characterId = pStagingShowDialogAction->CharacterId;
    rawDialog = pStagingShowDialogAction->RawDialog;
    voiceOverFilePath = "Audio/" + pStagingShowDialogAction->FilePath;
    leadInTime = pStagingShowDialogAction->LeadInTime;

    if (pStagingShowDialogAction->GetType() == Staging::ConversationActionType_ShowDialogAutomatic)
    {
        Staging::Conversation::ShowDialogAutomaticAction *pStagingShowDialogAutomaticAction =
                static_cast<Staging::Conversation::ShowDialogAutomaticAction *>(pStagingShowDialogAction);

        shouldAdvanceAutomatically = true;
        delayBeforeContinuing = pStagingShowDialogAutomaticAction->DelayBeforeContinuing;
    }

    InitializeDialog();
}

void Conversation::ShowDialogAction::InitializeDialog()
{
    parsedDialog.Reset();
    parsedDialog.SetParsedDialog(ParseRawDialog(this, rawDialog, dialogTextArea, dialogPadding, QFont("FayesMousewriting", 28)));
    parsedDialog.SetAudio(voiceOverFilePath);
}

void Conversation::ShowDialogAction::UpdateAndCacheConversationState(State &currentState)
{
    stateBeforeAction = currentState.Clone();

    QRegularExpression eventsRegularExpression("{(.*?)}");
    QRegularExpressionMatchIterator eventsIterator = eventsRegularExpression.globalMatch(rawDialog);

    while (eventsIterator.hasNext())
    {
        QRegularExpressionMatch eventMatch = eventsIterator.next();
        QString eventString = eventMatch.captured(1);
        QStringList eventComponents = eventString.split(':');

        if (eventComponents.size() == 2)
        {
            if (eventComponents[0].toLower() == "emotion")
            {
                if (speakerPosition == CharacterPositionLeft)
                {
                    currentState.SetLeftCharacterEmotionId(eventComponents[1]);
                }
                else
                {
                    currentState.SetRightCharacterEmotionId(eventComponents[1]);
                }
            }
            else if (eventComponents[0].toLower() == "otheremotion")
            {
                if (speakerPosition == CharacterPositionLeft)
                {
                    currentState.SetRightCharacterEmotionId(eventComponents[1]);
                }
                else
                {
                    currentState.SetLeftCharacterEmotionId(eventComponents[1]);
                }
            }
        }
    }

    Conversation::Action::UpdateAndCacheConversationState(currentState);

    parsedDialog.SetCharacters(
        stateBeforeAction.GetLeftCharacterId(),
        stateBeforeAction.GetLeftCharacterEmotionId(),
        stateBeforeAction.GetRightCharacterId(),
        stateBeforeAction.GetRightCharacterEmotionId(),
        speakerPosition,
        characterId);
}

void Conversation::ShowDialogAction::PreloadAudio()
{
    parsedDialog.PreloadAudio();
}

void Conversation::ShowDialogAction::PushToDrawingView(Conversation::DrawingView *pDrawingView)
{
    Conversation::Action::PushToDrawingView(pDrawingView);
    pDrawingView->SetDialog(speakerPosition, parsedDialog.GetHtmlLines(), characterId, false);
}

void Conversation::ShowDialogAction::PlayOnDrawingView(Conversation::DrawingView *pDrawingView)
{
    parsedDialog.PlayOnDrawingView(pDrawingView);
}

void Conversation::ShowDialogAction::StopPlaying()
{
    parsedDialog.StopPlaying();
}

EditorDialogContents<Conversation::Action, Conversation::State> * Conversation::ShowDialogAction::CreateEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::Action *pActionToEdit)
{
    return new ShowDialogActionEditorDialogContents(stateBeforeObject, ShowDialogActionEditorDialogContents::Type::ShowDialog, dynamic_cast<Conversation::ShowDialogAction *>(pActionToEdit));
}

void Conversation::ShowDialogAction::CopyPropertiesImpl(Conversation::Action *pOther, bool /*isForEdit*/)
{
    Conversation::ShowDialogAction *pAction = static_cast<Conversation::ShowDialogAction *>(pOther);

    pAction->speakerPosition = speakerPosition;
    pAction->characterId = characterId;
    pAction->rawDialog = rawDialog;
    pAction->voiceOverFilePath = voiceOverFilePath;
    pAction->leadInTime = leadInTime;
    pAction->shouldAdvanceAutomatically = shouldAdvanceAutomatically;
    pAction->delayBeforeContinuing = delayBeforeContinuing;
    pAction->InitializeDialog();
}

QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> Conversation::ShowDialogAction::GetListItemsImpl(int indentLevel)
{
    QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> editorList;
    editorList.append(new ShowDialogActionListItem(this, indentLevel));
    return editorList;
}

QString ShowDialogActionListItem::GetDisplayString()
{
    QString speakerName;

    Character *pLeftCharacter = CaseContent::GetInstance()->GetById<Character>(stateBeforeObject.GetLeftCharacterId());
    Character *pRightCharacter = CaseContent::GetInstance()->GetById<Character>(stateBeforeObject.GetRightCharacterId());

    switch (pAction->speakerPosition)
    {
    case CharacterPositionLeft:
        speakerName = (pLeftCharacter != NULL ? pLeftCharacter->GetDisplayName() : "no one") + " (the left character)";
        break;
    case CharacterPositionRight:
        speakerName = (pRightCharacter != NULL ? pRightCharacter->GetDisplayName() : "no one") + " (the right character)";
        break;
    case CharacterPositionOffscreen:
        speakerName = CaseContent::GetInstance()->GetById<Character>(pAction->characterId)->GetDisplayName() + " (offscreen)";
        break;
    case CharacterPositionUnknown:
        speakerName = "an unseen character";
        break;
    default:
        throw new MLIException("Unknown speaker position.");
    }

    QString displayString = GetHeaderString(speakerName);
    displayString += "<p>\"" + StripDialogEvents(pAction->rawDialog) + "\"</p>";

    if (pAction->voiceOverFilePath.length() > 0)
    {
        displayString += "<p>With associated voice-over file: " + pAction->voiceOverFilePath + ".ogg</p>";
    }

    if (pAction->leadInTime > 0 && pAction->shouldAdvanceAutomatically)
    {
        displayString += "<p>Wait ";
        displayString += UnderlineString(QString::number(pAction->leadInTime) + " milliseconds");
        displayString += " before beginning the line, and then wait ";
        displayString += UnderlineString(QString::number(pAction->delayBeforeContinuing) + " milliseconds");
        displayString += " before automatically continuing.</p>";
    }
    else if (pAction->leadInTime > 0)
    {
        displayString += "<p>Wait ";
        displayString += UnderlineString(QString::number(pAction->leadInTime) + " milliseconds");
        displayString += " before beginning the line.</p>";
    }
    else if (pAction->shouldAdvanceAutomatically)
    {
        displayString += "<p>Wait ";
        displayString += UnderlineString(QString::number(pAction->delayBeforeContinuing) + " milliseconds");
        displayString += " before automatically continuing.</p>";
    }

    displayString += GetFooterString();
    return displayString;
}

QString ShowDialogActionListItem::GetHeaderString(const QString &speakerName)
{
    QString headerString = "<p>Show ";
    headerString += UnderlineString(speakerName);
    headerString += " saying this:</p>";

    return headerString;
}

QString ShowDialogActionListItem::GetFooterString()
{
    return "";
}

ShowDialogActionEditorDialogContents::ShowDialogActionEditorDialogContents(const Conversation::State &stateBeforeObject, ShowDialogActionEditorDialogContents::Type type, Conversation::ShowDialogAction *pActionToEdit)
    : EditorDialogContents<Conversation::Action, Conversation::State>(stateBeforeObject, pActionToEdit)
{
    if (pActionToEdit != NULL)
    {
        pShowDialogAction = static_cast<Conversation::ShowDialogAction *>(pActionToEdit->Clone());

        switch (type)
        {
        case ShowDialogActionEditorDialogContents::Type::ShowInterrogation:
            {
                Interrogation::ShowInterrogationAction *pShowInterrogationAction = static_cast<Interrogation::ShowInterrogationAction *>(pActionToEdit);

                for (QString evidenceId : pShowInterrogationAction->evidencePresentedActionsByEvidenceId.keys())
                {
                    GetShowInterrogationAction()->originalEvidenceIdList.append(evidenceId);
                    GetShowInterrogationAction()->originalEvidenceIdToNewEvidenceIdMap.insert(evidenceId, evidenceId);
                }

                for (QString partnerId : pShowInterrogationAction->partnerUsedActionsByPartnerId.keys())
                {
                    GetShowInterrogationAction()->originalPartnerIdList.append(partnerId);
                    GetShowInterrogationAction()->originalPartnerIdToNewPartnerIdMap.insert(partnerId, partnerId);
                }
            }

            break;

        default:
            break;
        }
    }
    else
    {
        switch (type)
        {
        case ShowDialogActionEditorDialogContents::Type::ShowDialog:
            pShowDialogAction = new Conversation::ShowDialogAction();
            break;

        case ShowDialogActionEditorDialogContents::Type::MustPresentEvidence:
            pShowDialogAction = new Conversation::MustPresentEvidenceAction();
            break;

        case ShowDialogActionEditorDialogContents::Type::ShowInterrogation:
            pShowDialogAction = new Interrogation::ShowInterrogationAction();
            break;

        default:
            throw new MLIException("Unknown action type.");
        }
    }

    pObject = pShowDialogAction;

    QVBoxLayout *pLayout = new QVBoxLayout();

    QHBoxLayout *pSpeakerLayout = new QHBoxLayout();

    QLabel *pSpeakerLabelStart = new QLabel("Show ");
    pSpeakerLayout->addWidget(pSpeakerLabelStart);

    QComboBox *pSpeakerComboBox = new QComboBox();

    Character *pLeftCharacter = CaseContent::GetInstance()->GetById<Character>(this->stateBeforeObject.GetLeftCharacterId());
    Character *pRightCharacter = CaseContent::GetInstance()->GetById<Character>(this->stateBeforeObject.GetRightCharacterId());

    pSpeakerComboBox->addItem((pLeftCharacter != NULL ? pLeftCharacter->GetDisplayName() : "no one") + " (the left character)");
    pSpeakerComboBox->addItem((pRightCharacter != NULL ? pRightCharacter->GetDisplayName() : "no one") + " (the right character)");
    pSpeakerComboBox->addItem("an offscreen character...");
    pSpeakerComboBox->addItem("an unseen character");
    pSpeakerLayout->addWidget(pSpeakerComboBox);

    if (pActionToEdit != NULL)
    {
        switch (pActionToEdit->speakerPosition)
        {
        case CharacterPositionLeft:
            pSpeakerComboBox->setCurrentIndex(0);
            break;
        case CharacterPositionRight:
            pSpeakerComboBox->setCurrentIndex(1);
            break;
        case CharacterPositionOffscreen:
            pSpeakerComboBox->setCurrentIndex(2);
            break;
        case CharacterPositionUnknown:
            pSpeakerComboBox->setCurrentIndex(3);
            break;
        default:
            throw new MLIException("Unknown speaker position.");
        }
    }

    pOffscreenCharacterSelector = new CharacterSelector(false /* allowNoCharacter */);
    pSpeakerLayout->addWidget(pOffscreenCharacterSelector);

    QLabel *pSpeakerLabelEnd = new QLabel(" saying this:");
    pSpeakerLayout->addWidget(pSpeakerLabelEnd);

    pSpeakerLayout->addStretch(1);
    pLayout->addLayout(pSpeakerLayout);

    pRawDialogTextEdit = new QPlainTextEdit();
    pRawDialogTextEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    pRawDialogTextEdit->setSizeAdjustPolicy(QPlainTextEdit::AdjustToContents);
    pLayout->addWidget(pRawDialogTextEdit);

    if (pActionToEdit != NULL)
    {
        pRawDialogTextEdit->setPlainText(pActionToEdit->rawDialog);
    }

    QHBoxLayout *pVoiceOverFileLayout = new QHBoxLayout();

    QCheckBox *pVoiceOverCheckBox = new QCheckBox("Associate voice-over file... ");
    pVoiceOverFileLayout->addWidget(pVoiceOverCheckBox);

    pSelectVoiceOverFilePushButton = new QPushButton("Select file");
    pVoiceOverFileLayout->addWidget(pSelectVoiceOverFilePushButton);

    pVoiceOverFilePathLabel = new QLabel();
    pVoiceOverFilePathLabel->setMinimumWidth(300);
    pVoiceOverFileLayout->addWidget(pVoiceOverFilePathLabel);

    if (pActionToEdit != NULL)
    {
        pVoiceOverCheckBox->setChecked(pActionToEdit->voiceOverFilePath.length() > 0);
        pVoiceOverFilePathLabel->setText(pActionToEdit->voiceOverFilePath + ".ogg");
    }

    pVoiceOverFileLayout->addStretch(1);
    pLayout->addLayout(pVoiceOverFileLayout);

    QHBoxLayout *pLeadInTimeLayout = new QHBoxLayout();

    QCheckBox *pLeadInTimeCheckBox = new QCheckBox("Wait before starting audio playback... ");
    pLeadInTimeLayout->addWidget(pLeadInTimeCheckBox);

    pLeadInTimeAmountLineEdit = new QLineEdit();
    pLeadInTimeAmountLineEdit->setValidator(new QRegExpValidator(QRegExp("[0-9]+")));
    pLeadInTimeLayout->addWidget(pLeadInTimeAmountLineEdit);

    pLeadInTimeMillisecondsLabel = new QLabel(" milliseconds");
    pLeadInTimeLayout->addWidget(pLeadInTimeMillisecondsLabel);

    if (pActionToEdit != NULL)
    {
        pLeadInTimeCheckBox->setChecked(pActionToEdit->leadInTime > 0);
        pLeadInTimeAmountLineEdit->setText(QString::number(pActionToEdit->leadInTime));
    }
    else
    {
        pLeadInTimeAmountLineEdit->setText("0");
    }

    pLeadInTimeLayout->addStretch(1);
    pLayout->addLayout(pLeadInTimeLayout);

    QHBoxLayout *pAutomaticallyContinueLayout = new QHBoxLayout();

    QCheckBox *pAutomaticallyContinueCheckBox = new QCheckBox("Automatically continue after time elapses... ");
    pAutomaticallyContinueLayout->addWidget(pAutomaticallyContinueCheckBox);

    pAutomaticallyContinueAmountLineEdit = new QLineEdit();
    pAutomaticallyContinueAmountLineEdit->setValidator(new QRegExpValidator(QRegExp("[0-9]+")));
    pAutomaticallyContinueLayout->addWidget(pAutomaticallyContinueAmountLineEdit);

    pAutomaticallyContinueMillisecondsLabel = new QLabel(" milliseconds");
    pAutomaticallyContinueLayout->addWidget(pAutomaticallyContinueMillisecondsLabel);

    if (pActionToEdit != NULL)
    {
        pAutomaticallyContinueCheckBox->setChecked(pActionToEdit->shouldAdvanceAutomatically);
        pAutomaticallyContinueAmountLineEdit->setText(QString::number(pActionToEdit->delayBeforeContinuing));
    }
    else
    {
        pAutomaticallyContinueAmountLineEdit->setText("0");
    }

    pAutomaticallyContinueLayout->addStretch(1);
    pLayout->addLayout(pAutomaticallyContinueLayout);

    QObject::connect(pSpeakerComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(SpeakerComboBoxCurrentIndexChanged(int)));
    QObject::connect(pOffscreenCharacterSelector, SIGNAL(CharacterSelected(QString)), this, SLOT(OffscreenCharacterSelectorCharacterSelected(QString)));
    QObject::connect(pRawDialogTextEdit, SIGNAL(textChanged()), this, SLOT(RawDialogTextEditTextChanged()));
    QObject::connect(pVoiceOverCheckBox, SIGNAL(toggled(bool)), this, SLOT(VoiceOverCheckBoxToggled(bool)));
    QObject::connect(pSelectVoiceOverFilePushButton, SIGNAL(clicked()), this, SLOT(SelectVoiceOverFilePushButtonClicked()));
    QObject::connect(pLeadInTimeCheckBox, SIGNAL(toggled(bool)), this, SLOT(LeadInTimeCheckBoxToggled(bool)));
    QObject::connect(pLeadInTimeAmountLineEdit, SIGNAL(editingFinished()), this, SLOT(LeadInTimeAmountLineEditEditingFinished()));
    QObject::connect(pAutomaticallyContinueCheckBox, SIGNAL(toggled(bool)), this, SLOT(AutomaticallyContinueCheckBoxToggled(bool)));
    QObject::connect(pAutomaticallyContinueAmountLineEdit, SIGNAL(editingFinished()), this, SLOT(AutomaticallyContinueAmountLineEditEditingFinished()));

    // Call these to initialize everything properly.
    SpeakerComboBoxCurrentIndexChanged(pSpeakerComboBox->currentIndex());
    OffscreenCharacterSelectorCharacterSelected(pOffscreenCharacterSelector->GetSelectedCharacterId());
    VoiceOverCheckBoxToggled(pVoiceOverCheckBox->isChecked());
    LeadInTimeCheckBoxToggled(pLeadInTimeCheckBox->isChecked());
    AutomaticallyContinueCheckBoxToggled(pAutomaticallyContinueCheckBox->isChecked());

    if (type == ShowDialogActionEditorDialogContents::Type::MustPresentEvidence)
    {
        QLabel *pAcceptedEvidenceLabel = new QLabel();
        pAcceptedEvidenceLabel->setText("Require the player to present one of these pieces of evidence:");
        pLayout->addWidget(pAcceptedEvidenceLabel);

        MultipleSelectionWidget<EvidenceSelectorMultipleSelectionSelectorWidget> *pEvidenceSelector = new MultipleSelectionWidget<EvidenceSelectorMultipleSelectionSelectorWidget>();

        QObject::connect(pEvidenceSelector, SIGNAL(StringChanged(int,QString)), this, SLOT(EvidenceSelectorStringChanged(int,QString)));
        QObject::connect(pEvidenceSelector, SIGNAL(StringAdded(int,QString)), this, SLOT(EvidenceSelectorStringAdded(int,QString)));
        QObject::connect(pEvidenceSelector, SIGNAL(StringRemoved(int)), this, SLOT(EvidenceSelectorStringRemoved(int)));

        Conversation::MustPresentEvidenceAction *pMustPresentEvidenceAction = GetMustPresentEvidenceAction();

        if (pMustPresentEvidenceAction != NULL && pMustPresentEvidenceAction->correctEvidenceIdList.size() > 0)
        {
            pEvidenceSelector->SetSelections(pMustPresentEvidenceAction->correctEvidenceIdList);
        }
        else
        {
            QStringList stringList;
            stringList.append("");

            pEvidenceSelector->SetSelections(stringList);
        }

        pLayout->addWidget(pEvidenceSelector);
    }
    else if (type == ShowDialogActionEditorDialogContents::Type::ShowInterrogation)
    {
        Interrogation::ShowInterrogationAction *pShowInterrogationAction = GetShowInterrogationAction();

        QCheckBox *pAcceptEvidenceCheckBox = new QCheckBox("Allow the player to present evidence here");
        pLayout->addWidget(pAcceptEvidenceCheckBox);

        QObject::connect(pAcceptEvidenceCheckBox, SIGNAL(toggled(bool)), this, SLOT(InterrogationAcceptEvidenceCheckBoxToggled(bool)));

        pAcceptedEvidenceSelector = new MultipleSelectionWidget<EvidenceSelectorMultipleSelectionSelectorWidget>();
        pLayout->addWidget(pAcceptedEvidenceSelector);

        QObject::connect(pAcceptedEvidenceSelector, SIGNAL(StringChanged(int,QString)), this, SLOT(InterrogationEvidenceSelectorStringChanged(int,QString)));
        QObject::connect(pAcceptedEvidenceSelector, SIGNAL(StringAdded(int,QString)), this, SLOT(InterrogationEvidenceSelectorStringAdded(int,QString)));
        QObject::connect(pAcceptedEvidenceSelector, SIGNAL(StringRemoved(int)), this, SLOT(InterrogationEvidenceSelectorStringRemoved(int)));

        if (pShowInterrogationAction->evidencePresentedActionsByEvidenceId.keys().size() > 0)
        {
            pAcceptEvidenceCheckBox->setChecked(true);
            pAcceptedEvidenceSelector->SetSelections(pShowInterrogationAction->evidencePresentedActionsByEvidenceId.keys());
        }
        else
        {
            QStringList stringList;
            stringList.append("");

            pAcceptEvidenceCheckBox->setChecked(false);
            pAcceptedEvidenceSelector->SetSelections(stringList);
            pAcceptedEvidenceSelector->hide();
        }

        acceptedEvidenceList.append(pAcceptedEvidenceSelector->GetSelections());

        QCheckBox *pAcceptPartnersCheckBox = new QCheckBox("Allow the player to use a partner here");
        pLayout->addWidget(pAcceptPartnersCheckBox);

        QObject::connect(pAcceptPartnersCheckBox, SIGNAL(toggled(bool)), this, SLOT(InterrogationAcceptPartnerCheckBoxToggled(bool)));

        pAcceptedPartnersSelector = new MultipleSelectionWidget<CharacterSelectorMultipleSelectionSelectorWidget>();
        pLayout->addWidget(pAcceptedPartnersSelector);

        QObject::connect(pAcceptedPartnersSelector, SIGNAL(StringChanged(int,QString)), this, SLOT(InterrogationPartnerSelectorStringChanged(int,QString)));
        QObject::connect(pAcceptedPartnersSelector, SIGNAL(StringAdded(int,QString)), this, SLOT(InterrogationPartnerSelectorStringAdded(int,QString)));
        QObject::connect(pAcceptedPartnersSelector, SIGNAL(StringRemoved(int)), this, SLOT(InterrogationPartnerSelectorStringRemoved(int)));

        if (pShowInterrogationAction->partnerUsedActionsByPartnerId.keys().size() > 0)
        {
            pAcceptPartnersCheckBox->setChecked(true);
            pAcceptedPartnersSelector->SetSelections(pShowInterrogationAction->partnerUsedActionsByPartnerId.keys());
        }
        else
        {
            QStringList stringList;
            stringList.append("");

            pAcceptPartnersCheckBox->setChecked(false);
            pAcceptedPartnersSelector->SetSelections(stringList);
            pAcceptedPartnersSelector->hide();
        }

        acceptedPartnerList.append(pAcceptedPartnersSelector->GetSelections());
    }

    pLayout->addStretch(1);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(pLayout);
}

bool ShowDialogActionEditorDialogContents::ValidateFields()
{
    return true;
}

void ShowDialogActionEditorDialogContents::OnAccepted()
{
    pShowDialogAction->InitializeDialog();
    pShowDialogAction->PreloadAudio();
}

void ShowDialogActionEditorDialogContents::SpeakerComboBoxCurrentIndexChanged(int newIndex)
{
    switch (newIndex)
    {
    case 0:
        pShowDialogAction->speakerPosition = CharacterPositionLeft;
        pOffscreenCharacterSelector->hide();
        break;
    case 1:
        pShowDialogAction->speakerPosition = CharacterPositionRight;
        pOffscreenCharacterSelector->hide();
        break;
    case 2:
        pShowDialogAction->speakerPosition = CharacterPositionOffscreen;
        pOffscreenCharacterSelector->show();
        break;
    case 3:
        pShowDialogAction->speakerPosition = CharacterPositionUnknown;
        pOffscreenCharacterSelector->hide();
        break;
    default:
        throw new MLIException("Unexpected SpeakerComboBox index selected.");
    }
}

void ShowDialogActionEditorDialogContents::OffscreenCharacterSelectorCharacterSelected(const QString &newCharacterId)
{
    pShowDialogAction->characterId = newCharacterId;
}

void ShowDialogActionEditorDialogContents::RawDialogTextEditTextChanged()
{
    pShowDialogAction->rawDialog = pRawDialogTextEdit->toPlainText();
}

void ShowDialogActionEditorDialogContents::VoiceOverCheckBoxToggled(bool isChecked)
{
    if (isChecked)
    {
        SetOpacity(pSelectVoiceOverFilePushButton, 1.0);
        SetOpacity(pVoiceOverFilePathLabel, 1.0);
    }
    else
    {
        SetOpacity(pSelectVoiceOverFilePushButton, 0.0);
        SetOpacity(pVoiceOverFilePathLabel, 0.0);
    }
}

void ShowDialogActionEditorDialogContents::SelectVoiceOverFilePushButtonClicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Select voice-over file"), QString(), tr("Voice-over files (*.ogg)"));

    if (filePath.length() > 0)
    {
        filePath = CaseContent::GetInstance()->AbsolutePathToRelativePath(filePath);
        pVoiceOverFilePathLabel->setText(filePath);

        // Remove the ".ogg" from the stored file path - that's implied.
        pShowDialogAction->voiceOverFilePath = filePath.left(filePath.length() - 4);
    }
}

void ShowDialogActionEditorDialogContents::LeadInTimeCheckBoxToggled(bool isChecked)
{
    if (isChecked)
    {
        SetOpacity(pLeadInTimeAmountLineEdit, 1.0);
        SetOpacity(pLeadInTimeMillisecondsLabel, 1.0);
    }
    else
    {
        SetOpacity(pLeadInTimeAmountLineEdit, 0.0);
        SetOpacity(pLeadInTimeMillisecondsLabel, 0.0);
    }
}

void ShowDialogActionEditorDialogContents::LeadInTimeAmountLineEditEditingFinished()
{
    pShowDialogAction->leadInTime = pLeadInTimeAmountLineEdit->text().toInt();
}

void ShowDialogActionEditorDialogContents::AutomaticallyContinueCheckBoxToggled(bool isChecked)
{
    pShowDialogAction->shouldAdvanceAutomatically = isChecked;

    if (isChecked)
    {
        SetOpacity(pAutomaticallyContinueAmountLineEdit, 1.0);
        SetOpacity(pAutomaticallyContinueMillisecondsLabel, 1.0);
    }
    else
    {
        SetOpacity(pAutomaticallyContinueAmountLineEdit, 0.0);
        SetOpacity(pAutomaticallyContinueMillisecondsLabel, 0.0);
    }
}

void ShowDialogActionEditorDialogContents::AutomaticallyContinueAmountLineEditEditingFinished()
{
    pShowDialogAction->leadInTime = pAutomaticallyContinueAmountLineEdit->text().toInt();
}

void ShowDialogActionEditorDialogContents::EvidenceSelectorStringChanged(int index, const QString &string)
{
    GetMustPresentEvidenceAction()->correctEvidenceIdList[index] = string;
}

void ShowDialogActionEditorDialogContents::EvidenceSelectorStringAdded(int index, const QString &string)
{
    GetMustPresentEvidenceAction()->correctEvidenceIdList.insert(index, string);
}

void ShowDialogActionEditorDialogContents::EvidenceSelectorStringRemoved(int index)
{
    GetMustPresentEvidenceAction()->correctEvidenceIdList.removeAt(index);
}

void ShowDialogActionEditorDialogContents::InterrogationAcceptEvidenceCheckBoxToggled(bool isChecked)
{
    if (isChecked)
    {
        pAcceptedEvidenceSelector->show();

        for (const QString &string : pAcceptedEvidenceSelector->GetSelections())
        {
            GetShowInterrogationAction()->evidencePresentedActionsByEvidenceId.insert(string, QList<Conversation::Action *>());
        }
    }
    else
    {
        pAcceptedEvidenceSelector->hide();
        GetShowInterrogationAction()->evidencePresentedActionsByEvidenceId.clear();
    }
}

void ShowDialogActionEditorDialogContents::InterrogationEvidenceSelectorStringChanged(int index, const QString &string)
{
    QString previousString = acceptedEvidenceList[index];
    acceptedEvidenceList[index] = string;

    GetShowInterrogationAction()->evidencePresentedActionsByEvidenceId[string] = GetShowInterrogationAction()->evidencePresentedActionsByEvidenceId[previousString];
    GetShowInterrogationAction()->evidencePresentedActionsByEvidenceId.remove(previousString);

    GetShowInterrogationAction()->originalEvidenceIdToNewEvidenceIdMap[GetShowInterrogationAction()->originalEvidenceIdList[index]] = string;
}

void ShowDialogActionEditorDialogContents::InterrogationEvidenceSelectorStringAdded(int index, const QString &string)
{
    acceptedEvidenceList.insert(index, string);

    GetShowInterrogationAction()->evidencePresentedActionsByEvidenceId.insert(string, QList<Conversation::Action *>());
}

void ShowDialogActionEditorDialogContents::InterrogationEvidenceSelectorStringRemoved(int index)
{
    QString previousString = acceptedEvidenceList[index];
    acceptedEvidenceList.removeAt(index);

    GetShowInterrogationAction()->evidencePresentedActionsByEvidenceId.remove(previousString);
}

void ShowDialogActionEditorDialogContents::InterrogationAcceptPartnerCheckBoxToggled(bool isChecked)
{
    if (isChecked)
    {
        pAcceptedPartnersSelector->show();

        for (const QString &string : pAcceptedPartnersSelector->GetSelections())
        {
            GetShowInterrogationAction()->partnerUsedActionsByPartnerId.insert(string, QList<Conversation::Action *>());
        }
    }
    else
    {
        pAcceptedPartnersSelector->hide();
        GetShowInterrogationAction()->partnerUsedActionsByPartnerId.clear();
    }
}

void ShowDialogActionEditorDialogContents::InterrogationPartnerSelectorStringChanged(int index, const QString &string)
{
    QString previousString = acceptedPartnerList[index];
    acceptedPartnerList[index] = string;

    GetShowInterrogationAction()->partnerUsedActionsByPartnerId[string] = GetShowInterrogationAction()->partnerUsedActionsByPartnerId[previousString];
    GetShowInterrogationAction()->partnerUsedActionsByPartnerId.remove(previousString);

    GetShowInterrogationAction()->originalPartnerIdToNewPartnerIdMap[GetShowInterrogationAction()->originalPartnerIdList[index]] = string;
}

void ShowDialogActionEditorDialogContents::InterrogationPartnerSelectorStringAdded(int index, const QString &string)
{
    acceptedPartnerList.insert(index, string);

    GetShowInterrogationAction()->partnerUsedActionsByPartnerId.insert(string, QList<Conversation::Action *>());
}

void ShowDialogActionEditorDialogContents::InterrogationPartnerSelectorStringRemoved(int index)
{
    QString previousString = acceptedPartnerList[index];
    acceptedPartnerList.removeAt(index);

    GetShowInterrogationAction()->partnerUsedActionsByPartnerId.remove(previousString);
}

Conversation::MustPresentEvidenceAction::MustPresentEvidenceAction()
    : ShowDialogAction()
{
    canEndBeRequested = false;
}

Conversation::MustPresentEvidenceAction::MustPresentEvidenceAction(Staging::Conversation::MustPresentEvidenceAction *pStagingMustPresentEvidenceAction)
    : ShowDialogAction(pStagingMustPresentEvidenceAction)
{
    for (QString evidenceId : pStagingMustPresentEvidenceAction->CorrectEvidenceIdList)
    {
        correctEvidenceIdList.append(evidenceId);
    }

    canEndBeRequested = pStagingMustPresentEvidenceAction->EndRequestedIndex >= 0;
}

Conversation::MustPresentEvidenceAction::~MustPresentEvidenceAction()
{
    for (Action *pAction : correctEvidencePresentedActions)
    {
        delete pAction;
    }

    correctEvidencePresentedActions.clear();

    for (Action *pAction : wrongEvidencePresentedActions)
    {
        delete pAction;
    }

    wrongEvidencePresentedActions.clear();

    for (Action *pAction : endRequestedActions)
    {
        delete pAction;
    }

    endRequestedActions.clear();
}

void Conversation::MustPresentEvidenceAction::ReplaceAction(Conversation::Action *pNewAction, Conversation::Action *pOldAction)
{
    Conversation::ReplaceAction(pNewAction, pOldAction, correctEvidencePresentedActions);
    Conversation::ReplaceAction(pNewAction, pOldAction, wrongEvidencePresentedActions);
    Conversation::ReplaceAction(pNewAction, pOldAction, endRequestedActions);
}

void Conversation::MustPresentEvidenceAction::UpdateAndCacheConversationState(State &currentState)
{
    Conversation::ShowDialogAction::UpdateAndCacheConversationState(currentState);

    State correctActionsBranchState = currentState.Clone();
    State wrongActionsBranchState = currentState.Clone();
    State endRequestedActionsBranchState = currentState.Clone();

    for (Conversation::Action *pAction : correctEvidencePresentedActions)
    {
        pAction->UpdateAndCacheConversationState(correctActionsBranchState);
    }

    for (Conversation::Action *pAction : wrongEvidencePresentedActions)
    {
        pAction->UpdateAndCacheConversationState(wrongActionsBranchState);
    }

    for (Conversation::Action *pAction : endRequestedActions)
    {
        pAction->UpdateAndCacheConversationState(endRequestedActionsBranchState);
    }

    currentState = correctActionsBranchState.Clone();
}

void Conversation::MustPresentEvidenceAction::PreloadAudio()
{
    Conversation::ShowDialogAction::PreloadAudio();

    for (Conversation::Action *pAction : correctEvidencePresentedActions)
    {
        pAction->PreloadAudio();
    }

    for (Conversation::Action *pAction : wrongEvidencePresentedActions)
    {
        pAction->PreloadAudio();
    }

    for (Conversation::Action *pAction : endRequestedActions)
    {
        pAction->PreloadAudio();
    }
}

EditorDialogContents<Conversation::Action, Conversation::State> * Conversation::MustPresentEvidenceAction::CreateEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::Action *pActionToEdit)
{
    return new ShowDialogActionEditorDialogContents(stateBeforeObject, ShowDialogActionEditorDialogContents::Type::MustPresentEvidence, dynamic_cast<Conversation::MustPresentEvidenceAction *>(pActionToEdit));
}

void Conversation::MustPresentEvidenceAction::CopyPropertiesImpl(Conversation::Action *pOther, bool isForEdit)
{
    Conversation::MustPresentEvidenceAction *pAction = static_cast<Conversation::MustPresentEvidenceAction *>(pOther);

    Conversation::ShowDialogAction::CopyPropertiesImpl(pAction, isForEdit);

    pAction->correctEvidenceIdList.clear();

    for (QString evidenceId : correctEvidenceIdList)
    {
        pAction->correctEvidenceIdList.push_back(evidenceId);
    }

    Conversation::Action::CloneActionList(pAction->correctEvidencePresentedActions, correctEvidencePresentedActions);
    Conversation::Action::CloneActionList(pAction->wrongEvidencePresentedActions, wrongEvidencePresentedActions);
    Conversation::Action::CloneActionList(pAction->endRequestedActions, endRequestedActions);

    pAction->canEndBeRequested = canEndBeRequested;
}

void Conversation::MustPresentEvidenceAction::ExchangeListItemBaseOwnershipImpl(Action *pOther)
{
    Conversation::MustPresentEvidenceAction *pAction = static_cast<Conversation::MustPresentEvidenceAction *>(pOther);

    Conversation::Action::ExchangeListItemBaseOwnership(pAction->correctEvidencePresentedActions, correctEvidencePresentedActions);
    Conversation::Action::ExchangeListItemBaseOwnership(pAction->wrongEvidencePresentedActions, wrongEvidencePresentedActions);
    Conversation::Action::ExchangeListItemBaseOwnership(pAction->endRequestedActions, endRequestedActions);
}

QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> Conversation::MustPresentEvidenceAction::GetListItemsImpl(int indentLevel)
{
    QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> editorList;

    editorList.append(new MustPresentEvidenceActionListItem(this, indentLevel));
    editorList.append(new TextDisplay<Conversation::Action, Conversation::State, Conversation::ActionType>("If correct evidence is presented:", indentLevel + 1));
    editorList.append(GetListItemsForActions(correctEvidencePresentedActions, indentLevel + 2));
    editorList.append(new TextDisplay<Conversation::Action, Conversation::State, Conversation::ActionType>("If incorrect evidence is presented:", indentLevel + 1));
    editorList.append(GetListItemsForActions(wrongEvidencePresentedActions, indentLevel + 2));
    editorList.append(new TextDisplay<Conversation::Action, Conversation::State, Conversation::ActionType>("If the player gives up:", indentLevel + 1));
    editorList.append(GetListItemsForActions(endRequestedActions, indentLevel + 2));

    return editorList;
}

QString MustPresentEvidenceActionListItem::GetHeaderString(const QString &speakerName)
{
    QString headerString = "<p>Require the player to present evidence after showing ";
    headerString += UnderlineString(speakerName);
    headerString += " saying this:</p>";

    return headerString;
}

QString MustPresentEvidenceActionListItem::GetFooterString()
{
    Conversation::MustPresentEvidenceAction *pMustPresentEvidenceAction = GetMustPresentEvidenceAction();
    QString footerString = "<p>Accept the following evidence as correct: " + ConcatenateStringList(pMustPresentEvidenceAction->correctEvidenceIdList) + "</p>";

    if (pMustPresentEvidenceAction->canEndBeRequested)
    {
        footerString += "<p>Allow the player to exit without correctly presenting evidence.</p>";
    }

    return footerString;
}

Conversation::EnableConversationAction::EnableConversationAction(Staging::Conversation::EnableConversationAction *pStagingEnableConversationAction)
    : EnableConversationAction()
{
    SplitConversationIdFromCaseFile(pStagingEnableConversationAction->ConversationId, &encounterId, &conversationId);
}

EditorDialogContents<Conversation::Action, Conversation::State> * Conversation::EnableConversationAction::CreateEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::Action *pActionToEdit)
{
    return new EnableConversationActionEditorDialogContents(stateBeforeObject, static_cast<Conversation::EnableConversationAction *>(pActionToEdit));
}

void Conversation::EnableConversationAction::CopyPropertiesImpl(Conversation::Action *pOther, bool isForEdit)
{
    Conversation::EnableConversationAction *pAction = static_cast<Conversation::EnableConversationAction *>(pOther);

    Conversation::ActionWithNotification::CopyPropertiesImpl(pAction, isForEdit);

    pAction->encounterId = encounterId;
    pAction->conversationId = conversationId;
}

QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> Conversation::EnableConversationAction::GetListItemsImpl(int indentLevel)
{
    QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> editorList;
    editorList.append(new EnableConversationActionListItem(this, indentLevel));
    return editorList;
}

QString EnableConversationActionListItem::GetDisplayString()
{
    QString displayString = "In encounter ";
    displayString += UnderlineString(pAction->encounterId);
    displayString += ", enable the conversation ";
    displayString += UnderlineString(pAction->conversationId);
    displayString += ".";

    return displayString;
}

EnableConversationActionEditorDialogContents::EnableConversationActionEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::EnableConversationAction *pActionToEdit)
    : EditorDialogContents<Conversation::Action, Conversation::State>(stateBeforeObject, pActionToEdit)
{
    if (pActionToEdit != NULL)
    {
        pEnableConversationAction = static_cast<Conversation::EnableConversationAction *>(pActionToEdit->Clone());
    }
    else
    {
        pEnableConversationAction = new Conversation::EnableConversationAction();
    }

    pObject = pEnableConversationAction;

    QHBoxLayout *pLayout = new QHBoxLayout();

    pLayout->addWidget(new QLabel("In encounter "));

    EncounterSelector *pEncounterSelector = new EncounterSelector();
    pEncounterSelector->Reset();
    pLayout->addWidget(pEncounterSelector);

    if (pActionToEdit != NULL)
    {
        pEncounterSelector->SetToId(pActionToEdit->encounterId);
    }

    Encounter *pEncounter = CaseContent::GetInstance()->GetById<Encounter>(pEncounterSelector->GetId());

    pLayout->addWidget(new QLabel(" enable the conversation "));

    pConversationSelector = new ConversationSelector();
    pConversationSelector->Reset(pEncounter);
    pLayout->addWidget(pConversationSelector);

    if (pActionToEdit != NULL)
    {
        pConversationSelector->SetToId(pActionToEdit->conversationId);
    }

    pLayout->addStretch(1);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(pLayout);

    if (pActionToEdit == NULL)
    {
        pEnableConversationAction->encounterId = pEncounterSelector->GetId();
        pEnableConversationAction->conversationId = pConversationSelector->GetId();
    }

    QObject::connect(pEncounterSelector, SIGNAL(EncounterSelected(QString)), this, SLOT(EncounterSelectorEncounterSelected(QString)));
    QObject::connect(pConversationSelector, SIGNAL(ConversationSelected(QString)), this, SLOT(ConversationSelectorConverationSelected(QString)));
}

void EnableConversationActionEditorDialogContents::EncounterSelectorEncounterSelected(const QString &encounterId)
{
    pEnableConversationAction->encounterId = encounterId;

    Encounter *pEncounter = CaseContent::GetInstance()->GetById<Encounter>(encounterId);

    pConversationSelector->Reset(pEncounter);
    ConversationSelectorConverationSelected(pConversationSelector->GetId());
}

void EnableConversationActionEditorDialogContents::ConversationSelectorConverationSelected(const QString &conversationId)
{
    pEnableConversationAction->conversationId = conversationId;
}

bool EnableConversationActionEditorDialogContents::ValidateFields()
{
    return true;
}

Conversation::EnableEvidenceAction::EnableEvidenceAction(Staging::Conversation::EnableEvidenceAction *pStagingEnableEvidenceAction)
{
    newEvidenceId = pStagingEnableEvidenceAction->EvidenceId;
}

EditorDialogContents<Conversation::Action, Conversation::State> * Conversation::EnableEvidenceAction::CreateEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::Action *pActionToEdit)
{
    return new EnableEvidenceActionEditorDialogContents(stateBeforeObject, static_cast<Conversation::EnableEvidenceAction *>(pActionToEdit));
}

void Conversation::EnableEvidenceAction::CopyPropertiesImpl(Conversation::Action *pOther, bool isForEdit)
{
    Conversation::EnableEvidenceAction *pAction = static_cast<Conversation::EnableEvidenceAction *>(pOther);

    Conversation::ActionWithNotification::CopyPropertiesImpl(pAction, isForEdit);

    pAction->newEvidenceId = newEvidenceId;
}

QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> Conversation::EnableEvidenceAction::GetListItemsImpl(int indentLevel)
{
    QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> editorList;
    editorList.append(new EnableEvidenceActionListItem(this, indentLevel));
    return editorList;
}

QString EnableEvidenceActionListItem::GetDisplayString()
{
    QString displayString = "Enable evidence ";
    displayString += UnderlineString(pAction->newEvidenceId);
    displayString += ".";

    return displayString;
}

EnableEvidenceActionEditorDialogContents::EnableEvidenceActionEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::EnableEvidenceAction *pActionToEdit)
    : EditorDialogContents<Conversation::Action, Conversation::State>(stateBeforeObject, pActionToEdit)
{
    if (pActionToEdit != NULL)
    {
        pEnableEvidenceAction = static_cast<Conversation::EnableEvidenceAction *>(pActionToEdit->Clone());
    }
    else
    {
        pEnableEvidenceAction = new Conversation::EnableEvidenceAction();
    }

    pObject = pEnableEvidenceAction;

    QHBoxLayout *pLayout = new QHBoxLayout();

    pLayout->addWidget(new QLabel("Enable evidence "));

    pEvidenceSelector = new EvidenceSelector();
    pLayout->addWidget(pEvidenceSelector);

    if (pActionToEdit != NULL)
    {
        pEvidenceSelector->SetToId(pActionToEdit->newEvidenceId);
    }

    pLayout->addStretch(1);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(pLayout);

    if (pActionToEdit == NULL)
    {
        pEnableEvidenceAction->newEvidenceId = pEvidenceSelector->GetId();
    }

    QObject::connect(pEvidenceSelector, SIGNAL(EvidenceSelected(QString)), this, SLOT(EvidenceSelectorEvidenceSelected(QString)));
}

void EnableEvidenceActionEditorDialogContents::EvidenceSelectorEvidenceSelected(const QString &evidenceId)
{
    pEnableEvidenceAction->newEvidenceId = evidenceId;
}

bool EnableEvidenceActionEditorDialogContents::ValidateFields()
{
    return true;
}

Conversation::UpdateEvidenceAction::UpdateEvidenceAction(Staging::Conversation::UpdateEvidenceAction *pStagingUpdateEvidenceAction)
{
    oldEvidenceId = pStagingUpdateEvidenceAction->EvidenceId;
    newEvidenceId = pStagingUpdateEvidenceAction->NewEvidenceId;
}

EditorDialogContents<Conversation::Action, Conversation::State> * Conversation::UpdateEvidenceAction::CreateEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::Action *pActionToEdit)
{
    return new UpdateEvidenceActionEditorDialogContents(stateBeforeObject, static_cast<Conversation::UpdateEvidenceAction *>(pActionToEdit));
}

void Conversation::UpdateEvidenceAction::CopyPropertiesImpl(Conversation::Action *pOther, bool isForEdit)
{
    Conversation::UpdateEvidenceAction *pAction = static_cast<Conversation::UpdateEvidenceAction *>(pOther);

    Conversation::ActionWithNotification::CopyPropertiesImpl(pAction, isForEdit);

    pAction->oldEvidenceId = oldEvidenceId;
    pAction->newEvidenceId = newEvidenceId;
}

QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> Conversation::UpdateEvidenceAction::GetListItemsImpl(int indentLevel)
{
    QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> editorList;
    editorList.append(new UpdateEvidenceActionListItem(this, indentLevel));
    return editorList;
}

QString UpdateEvidenceActionListItem::GetDisplayString()
{
    QString displayString = "Update evidence ";
    displayString += UnderlineString(pAction->oldEvidenceId);
    displayString += " to ";
    displayString += UnderlineString(pAction->newEvidenceId);
    displayString += ".";

    return displayString;
}

UpdateEvidenceActionEditorDialogContents::UpdateEvidenceActionEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::UpdateEvidenceAction *pActionToEdit)
    : EditorDialogContents<Conversation::Action, Conversation::State>(stateBeforeObject, pActionToEdit)
{
    if (pActionToEdit != NULL)
    {
        pUpdateEvidenceAction = static_cast<Conversation::UpdateEvidenceAction *>(pActionToEdit->Clone());
    }
    else
    {
        pUpdateEvidenceAction = new Conversation::UpdateEvidenceAction();
    }

    pObject = pUpdateEvidenceAction;

    QHBoxLayout *pLayout = new QHBoxLayout();

    pLayout->addWidget(new QLabel("Update evidence "));

    pOldEvidenceSelector = new EvidenceSelector();
    pLayout->addWidget(pOldEvidenceSelector);

    if (pActionToEdit != NULL)
    {
        pOldEvidenceSelector->SetToId(pActionToEdit->oldEvidenceId);
    }

    pLayout->addWidget(new QLabel(" to "));

    pNewEvidenceSelector = new EvidenceSelector();
    pLayout->addWidget(pNewEvidenceSelector);

    if (pActionToEdit != NULL)
    {
        pNewEvidenceSelector->SetToId(pActionToEdit->newEvidenceId);
    }

    pLayout->addStretch(1);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(pLayout);

    if (pActionToEdit == NULL)
    {
        pUpdateEvidenceAction->oldEvidenceId = pOldEvidenceSelector->GetId();
        pUpdateEvidenceAction->newEvidenceId = pNewEvidenceSelector->GetId();
    }

    QObject::connect(pOldEvidenceSelector, SIGNAL(EvidenceSelected(QString)), this, SLOT(OldEvidenceSelectorEvidenceSelected(QString)));
    QObject::connect(pNewEvidenceSelector, SIGNAL(EvidenceSelected(QString)), this, SLOT(NewEvidenceSelectorEvidenceSelected(QString)));
}

void UpdateEvidenceActionEditorDialogContents::OldEvidenceSelectorEvidenceSelected(const QString &evidenceId)
{
    pUpdateEvidenceAction->newEvidenceId = evidenceId;
}

void UpdateEvidenceActionEditorDialogContents::NewEvidenceSelectorEvidenceSelected(const QString &evidenceId)
{
    pUpdateEvidenceAction->newEvidenceId = evidenceId;
}

bool UpdateEvidenceActionEditorDialogContents::ValidateFields()
{
    return true;
}

Conversation::DisableEvidenceAction::DisableEvidenceAction(Staging::Conversation::DisableEvidenceAction *pStagingDisableEvidenceAction)
{
    newEvidenceId = pStagingDisableEvidenceAction->EvidenceId;
}

EditorDialogContents<Conversation::Action, Conversation::State> * Conversation::DisableEvidenceAction::CreateEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::Action *pActionToEdit)
{
    return new DisableEvidenceActionEditorDialogContents(stateBeforeObject, static_cast<Conversation::DisableEvidenceAction *>(pActionToEdit));
}

void Conversation::DisableEvidenceAction::CopyPropertiesImpl(Conversation::Action *pOther, bool isForEdit)
{
    Conversation::DisableEvidenceAction *pAction = static_cast<Conversation::DisableEvidenceAction *>(pOther);

    Conversation::ActionWithNotification::CopyPropertiesImpl(pAction, isForEdit);

    pAction->newEvidenceId = newEvidenceId;
}

QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> Conversation::DisableEvidenceAction::GetListItemsImpl(int indentLevel)
{
    QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> editorList;
    editorList.append(new DisableEvidenceActionListItem(this, indentLevel));
    return editorList;
}

QString DisableEvidenceActionListItem::GetDisplayString()
{
    QString displayString = "Disable evidence ";
    displayString += UnderlineString(pAction->newEvidenceId);
    displayString += ".";

    return displayString;
}

DisableEvidenceActionEditorDialogContents::DisableEvidenceActionEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::DisableEvidenceAction *pActionToEdit)
    : EditorDialogContents<Conversation::Action, Conversation::State>(stateBeforeObject, pActionToEdit)
{
    if (pActionToEdit != NULL)
    {
        pDisableEvidenceAction = static_cast<Conversation::DisableEvidenceAction *>(pActionToEdit->Clone());
    }
    else
    {
        pDisableEvidenceAction = new Conversation::DisableEvidenceAction();
    }

    pObject = pDisableEvidenceAction;

    QHBoxLayout *pLayout = new QHBoxLayout();

    pLayout->addWidget(new QLabel("Disable evidence "));

    pEvidenceSelector = new EvidenceSelector();
    pLayout->addWidget(pEvidenceSelector);

    if (pActionToEdit != NULL)
    {
        pEvidenceSelector->SetToId(pActionToEdit->newEvidenceId);
    }

    pLayout->addStretch(1);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(pLayout);

    if (pActionToEdit == NULL)
    {
        pDisableEvidenceAction->newEvidenceId = pEvidenceSelector->GetId();
    }

    QObject::connect(pEvidenceSelector, SIGNAL(EvidenceSelected(QString)), this, SLOT(EvidenceSelectorEvidenceSelected(QString)));
}

void DisableEvidenceActionEditorDialogContents::EvidenceSelectorEvidenceSelected(const QString &evidenceId)
{
    pDisableEvidenceAction->newEvidenceId = evidenceId;
}

bool DisableEvidenceActionEditorDialogContents::ValidateFields()
{
    return true;
}

Conversation::EnableCutsceneAction::EnableCutsceneAction(Staging::Conversation::EnableCutsceneAction *pStagingEnableCutsceneAction)
{
    cutsceneId = pStagingEnableCutsceneAction->CutsceneId;
}

EditorDialogContents<Conversation::Action, Conversation::State> * Conversation::EnableCutsceneAction::CreateEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::Action *pActionToEdit)
{
    return new EnableCutsceneActionEditorDialogContents(stateBeforeObject, static_cast<Conversation::EnableCutsceneAction *>(pActionToEdit));
}

void Conversation::EnableCutsceneAction::CopyPropertiesImpl(Conversation::Action *pOther, bool isForEdit)
{
    Conversation::EnableCutsceneAction *pAction = static_cast<Conversation::EnableCutsceneAction *>(pOther);

    Conversation::ActionWithNotification::CopyPropertiesImpl(pAction, isForEdit);

    pAction->cutsceneId = cutsceneId;
}

QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> Conversation::EnableCutsceneAction::GetListItemsImpl(int indentLevel)
{
    QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> editorList;
    editorList.append(new EnableCutsceneActionListItem(this, indentLevel));
    return editorList;
}

QString EnableCutsceneActionListItem::GetDisplayString()
{
    QString displayString = "Enable cutscene ";
    displayString += UnderlineString(pAction->cutsceneId);
    displayString += ".";

    return displayString;
}

EnableCutsceneActionEditorDialogContents::EnableCutsceneActionEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::EnableCutsceneAction *pActionToEdit)
    : EditorDialogContents<Conversation::Action, Conversation::State>(stateBeforeObject, pActionToEdit)
{
    if (pActionToEdit != NULL)
    {
        pEnableCutsceneAction = static_cast<Conversation::EnableCutsceneAction *>(pActionToEdit->Clone());
    }
    else
    {
        pEnableCutsceneAction = new Conversation::EnableCutsceneAction();
    }

    pObject = pEnableCutsceneAction;

    QHBoxLayout *pLayout = new QHBoxLayout();

    pLayout->addWidget(new QLabel("Enable cutscene "));

    /*pEvidenceSelector = new EvidenceSelector();
    pLayout->addWidget(pEvidenceSelector);

    if (pActionToEdit != NULL)
    {
        pEvidenceSelector->SetToId(pActionToEdit->newEvidenceId);
    }*/

    pLayout->addStretch(1);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(pLayout);

    /*if (pActionToEdit == NULL)
    {
        pEnableCutsceneAction->newEvidenceId = pEvidenceSelector->GetId();
    }

    QObject::connect(pEvidenceSelector, SIGNAL(EvidenceSelected(QString)), this, SLOT(EvidenceSelectorEvidenceSelected(QString)));*/
}

/*void EnableCutsceneActionEditorDialogContents::EvidenceSelectorEvidenceSelected(const QString &evidenceId)
{
    pEnableCutsceneAction->newEvidenceId = evidenceId;
}*/

bool EnableCutsceneActionEditorDialogContents::ValidateFields()
{
    return true;
}

Conversation::PlayBgmAction::PlayBgmAction()
    : Action()
{
    preserveBgm = false;
}

Conversation::PlayBgmAction::PlayBgmAction(Staging::Conversation::PlayBgmAction *pStagingPlayBgmAction)
    : PlayBgmAction()
{
    bgmId = pStagingPlayBgmAction->BgmId;
    preserveBgm = pStagingPlayBgmAction->PreserveBgm;
}

EditorDialogContents<Conversation::Action, Conversation::State> * Conversation::PlayBgmAction::CreateEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::Action *pActionToEdit)
{
    return new PlayBgmActionEditorDialogContents(stateBeforeObject, static_cast<Conversation::PlayBgmAction *>(pActionToEdit));
}

void Conversation::PlayBgmAction::CopyPropertiesImpl(Conversation::Action *pOther, bool /*isForEdit*/)
{
    Conversation::PlayBgmAction *pAction = static_cast<Conversation::PlayBgmAction *>(pOther);

    pAction->bgmId = bgmId;
    pAction->preserveBgm = preserveBgm;
}

QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> Conversation::PlayBgmAction::GetListItemsImpl(int indentLevel)
{
    QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> editorList;
    editorList.append(new PlayBgmActionListItem(this, indentLevel));
    return editorList;
}

QString PlayBgmActionListItem::GetDisplayString()
{
    QString displayString = "Play the background music ";
    displayString += UnderlineString(pAction->bgmId);

    if (pAction->preserveBgm)
    {
        displayString += " and let it continue after the conversation finishes.";
    }
    else
    {
        displayString += " until the conversation finishes.";
    }

    return displayString;
}

PlayBgmActionEditorDialogContents::PlayBgmActionEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::PlayBgmAction *pActionToEdit)
    : EditorDialogContents<Conversation::Action, Conversation::State>(stateBeforeObject, pActionToEdit)
{
    if (pActionToEdit != NULL)
    {
        pPlayBgmAction = static_cast<Conversation::PlayBgmAction *>(pActionToEdit->Clone());
    }
    else
    {
        pPlayBgmAction = new Conversation::PlayBgmAction();
    }

    pObject = pPlayBgmAction;

    QVBoxLayout *pLayout = new QVBoxLayout();

    QHBoxLayout *pBgmSelectionLayout = new QHBoxLayout();

    pBgmSelectionLayout->addWidget(new QLabel("Play the background music "));

    BackgroundMusicSelector *pBgmSelector = new BackgroundMusicSelector();
    pBgmSelectionLayout->addWidget(pBgmSelector);

    if (pActionToEdit != NULL)
    {
        pBgmSelector->SetToId(pActionToEdit->bgmId);
    }

    pLayout->addLayout(pBgmSelectionLayout);

    QCheckBox *pPreserveCheckBox = new QCheckBox();
    pPreserveCheckBox->setText("Continue this music after the conversation finishes");
    pLayout->addWidget(pPreserveCheckBox);

    if (pActionToEdit != NULL)
    {
        pPreserveCheckBox->setChecked(pActionToEdit->preserveBgm);
    }

    pLayout->addStretch(1);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(pLayout);

    if (pActionToEdit == NULL)
    {
        pPlayBgmAction->bgmId = pBgmSelector->GetId();
        pPlayBgmAction->preserveBgm = pPreserveCheckBox->isChecked();
    }

    QObject::connect(pBgmSelector, SIGNAL(BackgroundMusicSelected(QString)), this, SLOT(BackgroundMusicSelectorBackgroundMusicSelected(QString)));
    QObject::connect(pPreserveCheckBox, SIGNAL(toggled(bool)), this, SLOT(PreserveCheckBoxToggled(bool)));
}

void PlayBgmActionEditorDialogContents::BackgroundMusicSelectorBackgroundMusicSelected(const QString &bgmId)
{
    pPlayBgmAction->bgmId = bgmId;
}

void PlayBgmActionEditorDialogContents::PreserveCheckBoxToggled(bool isChecked)
{
    pPlayBgmAction->preserveBgm = isChecked;
}

bool PlayBgmActionEditorDialogContents::ValidateFields()
{
    return true;
}

EditorDialogContents<Conversation::Action, Conversation::State> * Conversation::PauseBgmAction::CreateEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::Action *pActionToEdit)
{
    return new PauseBgmActionEditorDialogContents(stateBeforeObject, static_cast<Conversation::PauseBgmAction *>(pActionToEdit));
}

void Conversation::PauseBgmAction::CopyPropertiesImpl(Action */*pOther*/, bool /*isForEdit*/)
{
}

QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> Conversation::PauseBgmAction::GetListItemsImpl(int indentLevel)
{
    QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> editorList;
    editorList.append(new PauseBgmActionListItem(this, indentLevel));
    return editorList;
}

QString PauseBgmActionListItem::GetDisplayString()
{
    QString displayString = "Pause the currently playing background music.";

    return displayString;
}

PauseBgmActionEditorDialogContents::PauseBgmActionEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::PauseBgmAction *pActionToEdit)
    : EditorDialogContents<Conversation::Action, Conversation::State>(stateBeforeObject, pActionToEdit)
{
    if (pActionToEdit != NULL)
    {
        pPauseBgmAction = static_cast<Conversation::PauseBgmAction *>(pActionToEdit->Clone());
    }
    else
    {
        pPauseBgmAction = new Conversation::PauseBgmAction();
    }

    pObject = pPauseBgmAction;

    QVBoxLayout *pLayout = new QVBoxLayout();

    pLayout->addWidget(new QLabel("Pause the currently playing background music."));

    pLayout->addStretch(1);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(pLayout);
}

bool PauseBgmActionEditorDialogContents::ValidateFields()
{
    return true;
}

EditorDialogContents<Conversation::Action, Conversation::State> * Conversation::ResumeBgmAction::CreateEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::Action *pActionToEdit)
{
    return new ResumeBgmActionEditorDialogContents(stateBeforeObject, static_cast<Conversation::ResumeBgmAction *>(pActionToEdit));
}

void Conversation::ResumeBgmAction::CopyPropertiesImpl(Action */*pOther*/, bool /*isForEdit*/)
{
}

QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> Conversation::ResumeBgmAction::GetListItemsImpl(int indentLevel)
{
    QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> editorList;
    editorList.append(new ResumeBgmActionListItem(this, indentLevel));
    return editorList;
}

QString ResumeBgmActionListItem::GetDisplayString()
{
    QString displayString = "Resume the paused background music.";

    return displayString;
}

ResumeBgmActionEditorDialogContents::ResumeBgmActionEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::ResumeBgmAction *pActionToEdit)
    : EditorDialogContents<Conversation::Action, Conversation::State>(stateBeforeObject, pActionToEdit)
{
    if (pActionToEdit != NULL)
    {
        pResumeBgmAction = static_cast<Conversation::ResumeBgmAction *>(pActionToEdit->Clone());
    }
    else
    {
        pResumeBgmAction = new Conversation::ResumeBgmAction();
    }

    pObject = pResumeBgmAction;

    QVBoxLayout *pLayout = new QVBoxLayout();

    pLayout->addWidget(new QLabel("Resume the paused background music."));

    pLayout->addStretch(1);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(pLayout);
}

bool ResumeBgmActionEditorDialogContents::ValidateFields()
{
    return true;
}

Conversation::StopBgmAction::StopBgmAction()
    : Action()
{
    isInstant = false;
    preserveBgm = false;
}

Conversation::StopBgmAction::StopBgmAction(Staging::Conversation::StopBgmAction *pStagingStopBgmAction)
    : StopBgmAction()
{
    isInstant = pStagingStopBgmAction->IsInstant;
    preserveBgm = pStagingStopBgmAction->PreserveBgm;
}

EditorDialogContents<Conversation::Action, Conversation::State> * Conversation::StopBgmAction::CreateEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::Action *pActionToEdit)
{
    return new StopBgmActionEditorDialogContents(stateBeforeObject, static_cast<Conversation::StopBgmAction *>(pActionToEdit));
}

void Conversation::StopBgmAction::CopyPropertiesImpl(Conversation::Action *pOther, bool /*isForEdit*/)
{
    Conversation::StopBgmAction *pAction = static_cast<Conversation::StopBgmAction *>(pOther);

    pAction->isInstant = isInstant;
    pAction->preserveBgm = preserveBgm;
}

QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> Conversation::StopBgmAction::GetListItemsImpl(int indentLevel)
{
    QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> editorList;
    editorList.append(new StopBgmActionListItem(this, indentLevel));
    return editorList;
}

QString StopBgmActionListItem::GetDisplayString()
{
    QString displayString = "Stop the currently playing background music ";

    if (pAction->isInstant)
    {
        displayString += " instantly";
    }
    else
    {
        displayString += " after a fade-out delay";
    }

    displayString += ",";

    if (pAction->preserveBgm)
    {
        displayString += " and keep it stopped after the conversation finishes.";
    }
    else
    {
        displayString += " until the conversation finishes.";
    }

    return displayString;
}

StopBgmActionEditorDialogContents::StopBgmActionEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::StopBgmAction *pActionToEdit)
    : EditorDialogContents<Conversation::Action, Conversation::State>(stateBeforeObject, pActionToEdit)
{
    if (pActionToEdit != NULL)
    {
        pStopBgmAction = static_cast<Conversation::StopBgmAction *>(pActionToEdit->Clone());
    }
    else
    {
        pStopBgmAction = new Conversation::StopBgmAction();
    }

    pObject = pStopBgmAction;

    QVBoxLayout *pLayout = new QVBoxLayout();

    pLayout->addWidget(new QLabel("Stop the currently playing background music"));

    QCheckBox *pIsInstantCheckBox = new QCheckBox();
    pIsInstantCheckBox->setText("Stop instantly, rather than allowing it to fade out");
    pLayout->addWidget(pIsInstantCheckBox);

    if (pActionToEdit != NULL)
    {
        pIsInstantCheckBox->setChecked(pActionToEdit->isInstant);
    }

    QCheckBox *pPreserveCheckBox = new QCheckBox();
    pPreserveCheckBox->setText("Keep the music stopped after the conversation finishes");
    pLayout->addWidget(pPreserveCheckBox);

    if (pActionToEdit != NULL)
    {
        pPreserveCheckBox->setChecked(pActionToEdit->preserveBgm);
    }

    pLayout->addStretch(1);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(pLayout);

    if (pActionToEdit == NULL)
    {
        pStopBgmAction->isInstant = pIsInstantCheckBox->isChecked();
        pStopBgmAction->preserveBgm = pPreserveCheckBox->isChecked();
    }

    QObject::connect(pIsInstantCheckBox, SIGNAL(toggled(bool)), this, SLOT(IsInstantCheckBoxToggled(bool)));
    QObject::connect(pPreserveCheckBox, SIGNAL(toggled(bool)), this, SLOT(PreserveCheckBoxToggled(bool)));
}

void StopBgmActionEditorDialogContents::IsInstantCheckBoxToggled(bool isChecked)
{
    pStopBgmAction->isInstant = isChecked;
}

void StopBgmActionEditorDialogContents::PreserveCheckBoxToggled(bool isChecked)
{
    pStopBgmAction->preserveBgm = isChecked;
}

bool StopBgmActionEditorDialogContents::ValidateFields()
{
    return true;
}

Conversation::PlayAmbianceAction::PlayAmbianceAction()
    : Action()
{
    preserveAmbiance = false;
}

Conversation::PlayAmbianceAction::PlayAmbianceAction(Staging::Conversation::PlayAmbianceAction *pStagingPlayAmbianceAction)
{
    ambianceSfxId = pStagingPlayAmbianceAction->AmbianceSfxId;
    preserveAmbiance = pStagingPlayAmbianceAction->PreserveAmbiance;
}

EditorDialogContents<Conversation::Action, Conversation::State> * Conversation::PlayAmbianceAction::CreateEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::Action *pActionToEdit)
{
    return new PlayAmbianceActionEditorDialogContents(stateBeforeObject, static_cast<Conversation::PlayAmbianceAction *>(pActionToEdit));
}

void Conversation::PlayAmbianceAction::CopyPropertiesImpl(Conversation::Action *pOther, bool /*isForEdit*/)
{
    Conversation::PlayAmbianceAction *pAction = static_cast<Conversation::PlayAmbianceAction *>(pOther);

    pAction->ambianceSfxId = ambianceSfxId;
    pAction->preserveAmbiance = preserveAmbiance;
}

QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> Conversation::PlayAmbianceAction::GetListItemsImpl(int indentLevel)
{
    QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> editorList;
    editorList.append(new PlayAmbianceActionListItem(this, indentLevel));
    return editorList;
}

QString PlayAmbianceActionListItem::GetDisplayString()
{
    QString displayString = "Play the ambiance ";
    displayString += UnderlineString(pAction->ambianceSfxId);

    if (pAction->preserveAmbiance)
    {
        displayString += " and let it continue after the conversation finishes.";
    }
    else
    {
        displayString += " until the conversation finishes.";
    }

    return displayString;
}

PlayAmbianceActionEditorDialogContents::PlayAmbianceActionEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::PlayAmbianceAction *pActionToEdit)
    : EditorDialogContents<Conversation::Action, Conversation::State>(stateBeforeObject, pActionToEdit)
{
    if (pActionToEdit != NULL)
    {
        pPlayAmbianceAction = static_cast<Conversation::PlayAmbianceAction *>(pActionToEdit->Clone());
    }
    else
    {
        pPlayAmbianceAction = new Conversation::PlayAmbianceAction();
    }

    pObject = pPlayAmbianceAction;

    QVBoxLayout *pLayout = new QVBoxLayout();

    QHBoxLayout *pSfxSelectionLayout = new QHBoxLayout();

    pSfxSelectionLayout->addWidget(new QLabel("Play the ambiance "));

    SoundEffectSelector *pSfxSelector = new SoundEffectSelector();
    pSfxSelectionLayout->addWidget(pSfxSelector);

    if (pActionToEdit != NULL)
    {
        pSfxSelector->SetToId(pActionToEdit->ambianceSfxId);
    }

    pLayout->addLayout(pSfxSelectionLayout);

    QCheckBox *pPreserveCheckBox = new QCheckBox();
    pPreserveCheckBox->setText("Continue this ambiance after the conversation finishes");
    pLayout->addWidget(pPreserveCheckBox);

    if (pActionToEdit != NULL)
    {
        pPreserveCheckBox->setChecked(pActionToEdit->preserveAmbiance);
    }

    pLayout->addStretch(1);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(pLayout);

    if (pActionToEdit == NULL)
    {
        pPlayAmbianceAction->ambianceSfxId = pSfxSelector->GetId();
        pPlayAmbianceAction->preserveAmbiance = pPreserveCheckBox->isChecked();
    }

    QObject::connect(pSfxSelector, SIGNAL(SoundEffectSelected(QString)), this, SLOT(SoundEffectSelectorSoundEffectSelected(QString)));
    QObject::connect(pPreserveCheckBox, SIGNAL(toggled(bool)), this, SLOT(PreserveCheckBoxToggled(bool)));
}

void PlayAmbianceActionEditorDialogContents::SoundEffectSelectorSoundEffectSelected(const QString &sfxId)
{
    pPlayAmbianceAction->ambianceSfxId = sfxId;
}

void PlayAmbianceActionEditorDialogContents::PreserveCheckBoxToggled(bool isChecked)
{
    pPlayAmbianceAction->preserveAmbiance = isChecked;
}

bool PlayAmbianceActionEditorDialogContents::ValidateFields()
{
    return true;
}

EditorDialogContents<Conversation::Action, Conversation::State> * Conversation::PauseAmbianceAction::CreateEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::Action *pActionToEdit)
{
    return new PauseAmbianceActionEditorDialogContents(stateBeforeObject, static_cast<Conversation::PauseAmbianceAction *>(pActionToEdit));
}

void Conversation::PauseAmbianceAction::CopyPropertiesImpl(Action */*pOther*/, bool /*isForEdit*/)
{
}

QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> Conversation::PauseAmbianceAction::GetListItemsImpl(int indentLevel)
{
    QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> editorList;
    editorList.append(new PauseAmbianceActionListItem(this, indentLevel));
    return editorList;
}

QString PauseAmbianceActionListItem::GetDisplayString()
{
    QString displayString = "Pause the currently playing ambiance.";

    return displayString;
}

PauseAmbianceActionEditorDialogContents::PauseAmbianceActionEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::PauseAmbianceAction *pActionToEdit)
    : EditorDialogContents<Conversation::Action, Conversation::State>(stateBeforeObject, pActionToEdit)
{
    if (pActionToEdit != NULL)
    {
        pPauseAmbianceAction = static_cast<Conversation::PauseAmbianceAction *>(pActionToEdit->Clone());
    }
    else
    {
        pPauseAmbianceAction = new Conversation::PauseAmbianceAction();
    }

    pObject = pPauseAmbianceAction;

    QVBoxLayout *pLayout = new QVBoxLayout();

    pLayout->addWidget(new QLabel("Pause the currently playing ambiance."));

    pLayout->addStretch(1);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(pLayout);
}

bool PauseAmbianceActionEditorDialogContents::ValidateFields()
{
    return true;
}

EditorDialogContents<Conversation::Action, Conversation::State> * Conversation::ResumeAmbianceAction::CreateEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::Action *pActionToEdit)
{
    return new ResumeAmbianceActionEditorDialogContents(stateBeforeObject, static_cast<Conversation::ResumeAmbianceAction *>(pActionToEdit));
}

void Conversation::ResumeAmbianceAction::CopyPropertiesImpl(Action */*pOther*/, bool /*isForEdit*/)
{
}

QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> Conversation::ResumeAmbianceAction::GetListItemsImpl(int indentLevel)
{
    QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> editorList;
    editorList.append(new ResumeAmbianceActionListItem(this, indentLevel));
    return editorList;
}

QString ResumeAmbianceActionListItem::GetDisplayString()
{
    QString displayString = "Resume the paused ambiance.";

    return displayString;
}

ResumeAmbianceActionEditorDialogContents::ResumeAmbianceActionEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::ResumeAmbianceAction *pActionToEdit)
    : EditorDialogContents<Conversation::Action, Conversation::State>(stateBeforeObject, pActionToEdit)
{
    if (pActionToEdit != NULL)
    {
        pResumeAmbianceAction = static_cast<Conversation::ResumeAmbianceAction *>(pActionToEdit->Clone());
    }
    else
    {
        pResumeAmbianceAction = new Conversation::ResumeAmbianceAction();
    }

    pObject = pResumeAmbianceAction;

    QVBoxLayout *pLayout = new QVBoxLayout();

    pLayout->addWidget(new QLabel("Resume the paused ambiance."));

    pLayout->addStretch(1);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(pLayout);
}

bool ResumeAmbianceActionEditorDialogContents::ValidateFields()
{
    return true;
}

Conversation::StopAmbianceAction::StopAmbianceAction()
    : Action()
{
    isInstant = false;
    preserveAmbiance = false;
}

Conversation::StopAmbianceAction::StopAmbianceAction(Staging::Conversation::StopAmbianceAction *pStagingStopAmbianceAction)
    : StopAmbianceAction()
{
    isInstant = pStagingStopAmbianceAction->IsInstant;
    preserveAmbiance = pStagingStopAmbianceAction->PreserveAmbiance;
}

EditorDialogContents<Conversation::Action, Conversation::State> * Conversation::StopAmbianceAction::CreateEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::Action *pActionToEdit)
{
    return new StopAmbianceActionEditorDialogContents(stateBeforeObject, static_cast<Conversation::StopAmbianceAction *>(pActionToEdit));
}

void Conversation::StopAmbianceAction::CopyPropertiesImpl(Conversation::Action *pOther, bool /*isForEdit*/)
{
    Conversation::StopAmbianceAction *pAction = static_cast<Conversation::StopAmbianceAction *>(pOther);

    pAction->isInstant = isInstant;
    pAction->preserveAmbiance = preserveAmbiance;
}

QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> Conversation::StopAmbianceAction::GetListItemsImpl(int indentLevel)
{
    QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> editorList;
    editorList.append(new StopAmbianceActionListItem(this, indentLevel));
    return editorList;
}

QString StopAmbianceActionListItem::GetDisplayString()
{
    QString displayString = "Stop the currently playing ambiance ";

    if (pAction->isInstant)
    {
        displayString += " instantly";
    }
    else
    {
        displayString += " after a fade-out delay";
    }

    displayString += ",";

    if (pAction->preserveAmbiance)
    {
        displayString += " and keep it stopped after the conversation finishes.";
    }
    else
    {
        displayString += " until the conversation finishes.";
    }

    return displayString;
}

StopAmbianceActionEditorDialogContents::StopAmbianceActionEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::StopAmbianceAction *pActionToEdit)
    : EditorDialogContents<Conversation::Action, Conversation::State>(stateBeforeObject, pActionToEdit)
{
    if (pActionToEdit != NULL)
    {
        pStopAmbianceAction = static_cast<Conversation::StopAmbianceAction *>(pActionToEdit->Clone());
    }
    else
    {
        pStopAmbianceAction = new Conversation::StopAmbianceAction();
    }

    pObject = pStopAmbianceAction;

    QVBoxLayout *pLayout = new QVBoxLayout();

    pLayout->addWidget(new QLabel("Stop the currently playing ambiance"));

    QCheckBox *pIsInstantCheckBox = new QCheckBox();
    pIsInstantCheckBox->setText("Stop instantly, rather than allowing it to fade out");
    pLayout->addWidget(pIsInstantCheckBox);

    if (pActionToEdit != NULL)
    {
        pIsInstantCheckBox->setChecked(pActionToEdit->isInstant);
    }

    QCheckBox *pPreserveCheckBox = new QCheckBox();
    pPreserveCheckBox->setText("Keep the ambiance stopped after the conversation finishes");
    pLayout->addWidget(pPreserveCheckBox);

    if (pActionToEdit != NULL)
    {
        pPreserveCheckBox->setChecked(pActionToEdit->preserveAmbiance);
    }

    pLayout->addStretch(1);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(pLayout);

    if (pActionToEdit == NULL)
    {
        pStopAmbianceAction->isInstant = pIsInstantCheckBox->isChecked();
        pStopAmbianceAction->preserveAmbiance = pPreserveCheckBox->isChecked();
    }

    QObject::connect(pIsInstantCheckBox, SIGNAL(toggled(bool)), this, SLOT(IsInstantCheckBoxToggled(bool)));
    QObject::connect(pPreserveCheckBox, SIGNAL(toggled(bool)), this, SLOT(PreserveCheckBoxToggled(bool)));
}

void StopAmbianceActionEditorDialogContents::IsInstantCheckBoxToggled(bool isChecked)
{
    pStopAmbianceAction->isInstant = isChecked;
}

void StopAmbianceActionEditorDialogContents::PreserveCheckBoxToggled(bool isChecked)
{
    pStopAmbianceAction->preserveAmbiance = isChecked;
}

bool StopAmbianceActionEditorDialogContents::ValidateFields()
{
    return true;
}

Conversation::StartAnimationAction::StartAnimationAction(Staging::Conversation::StartAnimationAction *pStartAnimationAction)
    : StartAnimationAction()
{
    animationId = pStartAnimationAction->AnimationId;
}

EditorDialogContents<Conversation::Action, Conversation::State> * Conversation::StartAnimationAction::CreateEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::Action *pActionToEdit)
{
    return new StartAnimationActionEditorDialogContents(stateBeforeObject, static_cast<Conversation::StartAnimationAction *>(pActionToEdit));
}

void Conversation::StartAnimationAction::CopyPropertiesImpl(Conversation::Action *pOther, bool /*isForEdit*/)
{
    Conversation::StartAnimationAction *pAction = static_cast<Conversation::StartAnimationAction *>(pOther);

    pAction->animationId = animationId;
}

QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> Conversation::StartAnimationAction::GetListItemsImpl(int indentLevel)
{
    QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> editorList;
    editorList.append(new StartAnimationActionListItem(this, indentLevel));
    return editorList;
}

QString StartAnimationActionListItem::GetDisplayString()
{
    QString displayString = "Start the animation " + UnderlineString(pAction->animationId) + ".";

    return displayString;
}

StartAnimationActionEditorDialogContents::StartAnimationActionEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::StartAnimationAction *pActionToEdit)
    : EditorDialogContents<Conversation::Action, Conversation::State>(stateBeforeObject, pActionToEdit)
{
    if (pActionToEdit != NULL)
    {
        pStartAnimationAction = static_cast<Conversation::StartAnimationAction *>(pActionToEdit->Clone());
    }
    else
    {
        pStartAnimationAction = new Conversation::StartAnimationAction();
    }

    pObject = pStartAnimationAction;

    QHBoxLayout *pLayout = new QHBoxLayout();

    pLayout->addWidget(new QLabel("Start the animation "));

    if (pActionToEdit != NULL)
    {
        pLayout->addWidget(new QLabel(pActionToEdit->animationId));
    }

    pLayout->addStretch(1);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(pLayout);
}

bool StartAnimationActionEditorDialogContents::ValidateFields()
{
    return true;
}

EditorDialogContents<Conversation::Action, Conversation::State> * Conversation::StopAnimationAction::CreateEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::Action *pActionToEdit)
{
    return new StopAnimationActionEditorDialogContents(stateBeforeObject, static_cast<Conversation::StopAnimationAction *>(pActionToEdit));
}

void Conversation::StopAnimationAction::CopyPropertiesImpl(Action */*pOther*/, bool /*isForEdit*/)
{
}

QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> Conversation::StopAnimationAction::GetListItemsImpl(int indentLevel)
{
    QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> editorList;
    editorList.append(new StopAnimationActionListItem(this, indentLevel));
    return editorList;
}

QString StopAnimationActionListItem::GetDisplayString()
{
    QString displayString = "Stop the currently running animation.";

    return displayString;
}

StopAnimationActionEditorDialogContents::StopAnimationActionEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::StopAnimationAction *pActionToEdit)
    : EditorDialogContents<Conversation::Action, Conversation::State>(stateBeforeObject, pActionToEdit)
{
    if (pActionToEdit != NULL)
    {
        pStopAnimationAction = static_cast<Conversation::StopAnimationAction *>(pActionToEdit->Clone());
    }
    else
    {
        pStopAnimationAction = new Conversation::StopAnimationAction();
    }

    pObject = pStopAnimationAction;

    QHBoxLayout *pLayout = new QHBoxLayout();

    pLayout->addWidget(new QLabel("Stop the currently running animation."));

    pLayout->addStretch(1);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(pLayout);
}

bool StopAnimationActionEditorDialogContents::ValidateFields()
{
    return true;
}

Conversation::SetPartnerAction::SetPartnerAction(Staging::Conversation::SetPartnerAction *pSetPartnerAction)
    : SetPartnerAction()
{
    partnerId = pSetPartnerAction->PartnerId;
}

EditorDialogContents<Conversation::Action, Conversation::State> * Conversation::SetPartnerAction::CreateEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::Action *pActionToEdit)
{
    return new SetPartnerActionEditorDialogContents(stateBeforeObject, static_cast<Conversation::SetPartnerAction *>(pActionToEdit));
}

void Conversation::SetPartnerAction::CopyPropertiesImpl(Conversation::Action *pOther, bool isForEdit)
{
    Conversation::SetPartnerAction *pAction = static_cast<Conversation::SetPartnerAction *>(pOther);

    Conversation::ActionWithNotification::CopyPropertiesImpl(pAction, isForEdit);
    pAction->partnerId = partnerId;
}

QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> Conversation::SetPartnerAction::GetListItemsImpl(int indentLevel)
{
    QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> editorList;
    editorList.append(new SetPartnerActionListItem(this, indentLevel));
    return editorList;
}

QString SetPartnerActionListItem::GetDisplayString()
{
    QString displayString = "Set the player's partner to " + UnderlineString(pAction->partnerId) + ".";

    return displayString;
}

SetPartnerActionEditorDialogContents::SetPartnerActionEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::SetPartnerAction *pActionToEdit)
    : EditorDialogContents<Conversation::Action, Conversation::State>(stateBeforeObject, pActionToEdit)
{
    if (pActionToEdit != NULL)
    {
        pSetPartnerAction = static_cast<Conversation::SetPartnerAction *>(pActionToEdit->Clone());
    }
    else
    {
        pSetPartnerAction = new Conversation::SetPartnerAction();
    }

    pObject = pSetPartnerAction;

    QHBoxLayout *pLayout = new QHBoxLayout();

    pLayout->addWidget(new QLabel("Set the player's partner to "));

    CharacterSelector *pPartnerSelector = new CharacterSelector(false /* allowNoCharacter */);
    pLayout->addWidget(pPartnerSelector);

    if (pActionToEdit != NULL)
    {
        pPartnerSelector->SetToCharacterById(pActionToEdit->partnerId);
    }

    pLayout->addStretch(1);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(pLayout);

    if (pActionToEdit == NULL)
    {
        pSetPartnerAction->partnerId = pPartnerSelector->GetSelectedCharacterId();
    }

    QObject::connect(pPartnerSelector, SIGNAL(CharacterSelected(QString)), this, SLOT(PartnerSelectorCharacterSelected(QString)));
}

void SetPartnerActionEditorDialogContents::PartnerSelectorCharacterSelected(const QString &partnerId)
{
    pSetPartnerAction->partnerId = partnerId;
}

bool SetPartnerActionEditorDialogContents::ValidateFields()
{
    return true;
}

EditorDialogContents<Conversation::Action, Conversation::State> * Conversation::GoToPresentWrongEvidenceAction::CreateEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::Action *pActionToEdit)
{
    return new GoToPresentWrongEvidenceActionEditorDialogContents(stateBeforeObject, static_cast<Conversation::GoToPresentWrongEvidenceAction *>(pActionToEdit));
}

void Conversation::GoToPresentWrongEvidenceAction::CopyPropertiesImpl(Action */*pOther*/, bool /*isForEdit*/)
{
}

QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> Conversation::GoToPresentWrongEvidenceAction::GetListItemsImpl(int indentLevel)
{
    QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> editorList;
    editorList.append(new GoToPresentWrongEvidenceActionListItem(this, indentLevel));
    return editorList;
}

QString GoToPresentWrongEvidenceActionListItem::GetDisplayString()
{
    QString displayString = "Go to the \"presented wrong evidence\" dialog section.";

    return displayString;
}

GoToPresentWrongEvidenceActionEditorDialogContents::GoToPresentWrongEvidenceActionEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::GoToPresentWrongEvidenceAction *pActionToEdit)
    : EditorDialogContents<Conversation::Action, Conversation::State>(stateBeforeObject, pActionToEdit)
{
    if (pActionToEdit != NULL)
    {
        pGoToPresentWrongEvidenceAction = static_cast<Conversation::GoToPresentWrongEvidenceAction *>(pActionToEdit->Clone());
    }
    else
    {
        pGoToPresentWrongEvidenceAction = new Conversation::GoToPresentWrongEvidenceAction();
    }

    pObject = pGoToPresentWrongEvidenceAction;

    QHBoxLayout *pLayout = new QHBoxLayout();

    pLayout->addWidget(new QLabel("Go to the \"presented wrong evidence\" dialog section."));

    pLayout->addStretch(1);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(pLayout);
}

bool GoToPresentWrongEvidenceActionEditorDialogContents::ValidateFields()
{
    return true;
}

Conversation::LockConversationAction::LockConversationAction()
{
    pUnlockCondition = NULL;
}

Conversation::LockConversationAction::LockConversationAction(Staging::Conversation::LockConversationAction *pLockConversationAction)
    : LockConversationAction()
{
    pUnlockCondition = UnlockCondition::LoadFromStagingObject(pLockConversationAction->pUnlockCondition);
}

Conversation::LockConversationAction::~LockConversationAction()
{
    delete pUnlockCondition;
    pUnlockCondition = NULL;
}

EditorDialogContents<Conversation::Action, Conversation::State> * Conversation::LockConversationAction::CreateEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::Action *pActionToEdit)
{
    return new LockConversationActionEditorDialogContents(stateBeforeObject, static_cast<Conversation::LockConversationAction *>(pActionToEdit));
}

void Conversation::LockConversationAction::CopyPropertiesImpl(Conversation::Action *pOther, bool /*isForEdit*/)
{
    Conversation::LockConversationAction *pAction = static_cast<Conversation::LockConversationAction *>(pOther);

    delete pAction->pUnlockCondition;
    pAction->pUnlockCondition = pUnlockCondition->Clone();
}

QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> Conversation::LockConversationAction::GetListItemsImpl(int indentLevel)
{
    QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> editorList;
    editorList.append(new LockConversationActionListItem(this, indentLevel));
    return editorList;
}

QString LockConversationActionListItem::GetDisplayString()
{
    return pAction->pUnlockCondition->GetDisplayString();
}

LockConversationActionEditorDialogContents::LockConversationActionEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::LockConversationAction *pActionToEdit)
    : EditorDialogContents<Conversation::Action, Conversation::State>(stateBeforeObject, pActionToEdit)
{
    if (pActionToEdit != NULL)
    {
        pLockConversationAction = static_cast<Conversation::LockConversationAction *>(pActionToEdit->Clone());
    }
    else
    {
        pLockConversationAction = new Conversation::LockConversationAction();
    }

    pObject = pLockConversationAction;

    QHBoxLayout *pLayout = new QHBoxLayout();

    pLayout->addWidget(new QLabel("Lock this conversation until "));

    QComboBox *pTypeSelectorComboBox = new QComboBox();
    pTypeSelectorComboBox->addItem("the following flag is set...");
    pTypeSelectorComboBox->addItem("the following partner is present...");
    pLayout->addWidget(pTypeSelectorComboBox);

    pFlagEditor = new FlagEditor();
    pLayout->addWidget(pFlagEditor);

    pPartnerSelector = new CharacterSelector(false /* allowNoCharacter */);
    pLayout->addWidget(pPartnerSelector);
    pPartnerSelector->hide();

    if (pActionToEdit != NULL)
    {
        pTypeSelectorComboBox->setCurrentIndex((int)pActionToEdit->pUnlockCondition->GetType());

        switch (pActionToEdit->pUnlockCondition->GetType())
        {
        case Conversation::UnlockCondition::Type::FlagSet:
            {
                Conversation::FlagSetUnlockCondition *pFlagSetUnlockCondition = static_cast<Conversation::FlagSetUnlockCondition *>(pActionToEdit->pUnlockCondition);
                pFlagEditor->SetToId(pFlagSetUnlockCondition->flagId);
                pFlagEditor->show();
                pPartnerSelector->hide();
            }
            break;

        case Conversation::UnlockCondition::Type::PartnerPresent:
            {
                Conversation::PartnerPresentUnlockCondition *pPartnerPresentUnlockCondition = static_cast<Conversation::PartnerPresentUnlockCondition *>(pActionToEdit->pUnlockCondition);
                pPartnerSelector->SetToCharacterById(pPartnerPresentUnlockCondition->partnerId);
                pFlagEditor->hide();
                pPartnerSelector->show();
            }
            break;

        default:
            throw new MLIException("Invalid unlock condition type.");
        }
    }

    pLayout->addStretch(1);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(pLayout);

    if (pActionToEdit == NULL)
    {
        switch ((Conversation::UnlockCondition::Type)pTypeSelectorComboBox->currentIndex())
        {
        case Conversation::UnlockCondition::Type::FlagSet:
            {
                delete pLockConversationAction->pUnlockCondition;
                pLockConversationAction->pUnlockCondition = new Conversation::FlagSetUnlockCondition(pFlagEditor->GetId());
            }
            break;

        case Conversation::UnlockCondition::Type::PartnerPresent:
            {
                delete pLockConversationAction->pUnlockCondition;
                pLockConversationAction->pUnlockCondition = new Conversation::PartnerPresentUnlockCondition(pPartnerSelector->GetSelectedCharacterId());
            }
            break;

        default:
            throw new MLIException("Invalid unlock condition type.");
        }
    }

    QObject::connect(pTypeSelectorComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(TypeSelectorComboBoxCurrentItemChanged(int)));
    QObject::connect(pFlagEditor, SIGNAL(FlagSelected(QString)), this, SLOT(FlagEditorFlagSelected(QString)));
    QObject::connect(pPartnerSelector, SIGNAL(CharacterSelected(QString)), this, SLOT(PartnerSelectorCharacterSelected(QString)));
}

void LockConversationActionEditorDialogContents::TypeSelectorComboBoxCurrentItemChanged(int selectedIndex)
{
    switch ((Conversation::UnlockCondition::Type)selectedIndex)
    {
    case Conversation::UnlockCondition::Type::FlagSet:
        {
            delete pLockConversationAction->pUnlockCondition;
            pLockConversationAction->pUnlockCondition = new Conversation::FlagSetUnlockCondition(pFlagEditor->GetId());
            pFlagEditor->show();
            pPartnerSelector->hide();
        }
        break;

    case Conversation::UnlockCondition::Type::PartnerPresent:
        {
            delete pLockConversationAction->pUnlockCondition;
            pLockConversationAction->pUnlockCondition = new Conversation::PartnerPresentUnlockCondition(pPartnerSelector->GetSelectedCharacterId());
            pFlagEditor->hide();
            pPartnerSelector->show();
        }
        break;

    default:
        throw new MLIException("Invalid unlock condition type.");
    }
}

void LockConversationActionEditorDialogContents::FlagEditorFlagSelected(const QString &flagId)
{
    if (pLockConversationAction->pUnlockCondition->GetType() != Conversation::UnlockCondition::Type::FlagSet)
    {
        throw new MLIException("The flag editor should only be visible when we're selecting which flag needs to be set.");
    }

    Conversation::FlagSetUnlockCondition *pFlagSetUnlockCondition = static_cast<Conversation::FlagSetUnlockCondition *>(pLockConversationAction->pUnlockCondition);
    pFlagSetUnlockCondition->flagId = flagId;
}

void LockConversationActionEditorDialogContents::PartnerSelectorCharacterSelected(const QString &partnerId)
{
    if (pLockConversationAction->pUnlockCondition->GetType() != Conversation::UnlockCondition::Type::PartnerPresent)
    {
        throw new MLIException("The partner selector should only be visible when we're selecting which partner needs to be present.");
    }

    Conversation::PartnerPresentUnlockCondition *pPartnerPresentUnlockCondition = static_cast<Conversation::PartnerPresentUnlockCondition *>(pLockConversationAction->pUnlockCondition);
    pPartnerPresentUnlockCondition->partnerId = partnerId;
}

bool LockConversationActionEditorDialogContents::ValidateFields()
{
    return true;
}

EditorDialogContents<Conversation::Action, Conversation::State> * Conversation::ExitEncounterAction::CreateEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::Action *pActionToEdit)
{
    return new ExitEncounterActionEditorDialogContents(stateBeforeObject, static_cast<Conversation::ExitEncounterAction *>(pActionToEdit));
}

void Conversation::ExitEncounterAction::CopyPropertiesImpl(Action */*pOther*/, bool /*isForEdit*/)
{
}

QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> Conversation::ExitEncounterAction::GetListItemsImpl(int indentLevel)
{
    QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> editorList;
    editorList.append(new ExitEncounterActionListItem(this, indentLevel));
    return editorList;
}

QString ExitEncounterActionListItem::GetDisplayString()
{
    QString displayString = "Exit the current encounter.";

    return displayString;
}

ExitEncounterActionEditorDialogContents::ExitEncounterActionEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::ExitEncounterAction *pActionToEdit)
    : EditorDialogContents<Conversation::Action, Conversation::State>(stateBeforeObject, pActionToEdit)
{
    if (pActionToEdit != NULL)
    {
        pExitEncounterAction = static_cast<Conversation::ExitEncounterAction *>(pActionToEdit->Clone());
    }
    else
    {
        pExitEncounterAction = new Conversation::ExitEncounterAction();
    }

    pObject = pExitEncounterAction;

    QHBoxLayout *pLayout = new QHBoxLayout();

    pLayout->addWidget(new QLabel("Exit the current encounter."));

    pLayout->addStretch(1);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(pLayout);
}

bool ExitEncounterActionEditorDialogContents::ValidateFields()
{
    return true;
}

Conversation::MoveToLocationAction::MoveToLocationAction(Staging::Conversation::MoveToLocationAction *pMoveToLocationAction)
    : MoveToLocationAction()
{
    newAreaId = pMoveToLocationAction->NewAreaId;
    newLocationId = pMoveToLocationAction->NewLocationId;
    transitionId = pMoveToLocationAction->TransitionId;
}

EditorDialogContents<Conversation::Action, Conversation::State> * Conversation::MoveToLocationAction::CreateEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::Action *pActionToEdit)
{
    return new MoveToLocationActionEditorDialogContents(stateBeforeObject, static_cast<Conversation::MoveToLocationAction *>(pActionToEdit));
}

void Conversation::MoveToLocationAction::CopyPropertiesImpl(Conversation::Action *pOther, bool /*isForEdit*/)
{
    Conversation::MoveToLocationAction *pAction = static_cast<Conversation::MoveToLocationAction *>(pOther);

    pAction->newAreaId = newAreaId;
    pAction->newLocationId = newLocationId;
    pAction->transitionId = transitionId;
}

QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> Conversation::MoveToLocationAction::GetListItemsImpl(int indentLevel)
{
    QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> editorList;
    editorList.append(new MoveToLocationActionListItem(this, indentLevel));
    return editorList;
}

QString MoveToLocationActionListItem::GetDisplayString()
{
    QString displayString = QString("Move the player to location ") + UnderlineString(pAction->newLocationId) + QString(", at transition ") + UnderlineString(pAction->transitionId) + ".";

    return displayString;
}

MoveToLocationActionEditorDialogContents::MoveToLocationActionEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::MoveToLocationAction *pActionToEdit)
    : EditorDialogContents<Conversation::Action, Conversation::State>(stateBeforeObject, pActionToEdit)
{
    if (pActionToEdit != NULL)
    {
        pMoveToLocationAction = static_cast<Conversation::MoveToLocationAction *>(pActionToEdit->Clone());
    }
    else
    {
        pMoveToLocationAction = new Conversation::MoveToLocationAction();
    }

    pObject = pMoveToLocationAction;

    QHBoxLayout *pLayout = new QHBoxLayout();

    pLayout->addWidget(new QLabel("Move the player to location "));

    LocationSelector *pLocationSelector = new LocationSelector();
    pLayout->addWidget(pLocationSelector);

    if (pActionToEdit != NULL)
    {
        pLocationSelector->SetToId(pActionToEdit->newLocationId);
    }

    pLayout->addStretch(1);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(pLayout);

    if (pActionToEdit == NULL)
    {
        pMoveToLocationAction->newLocationId = pLocationSelector->GetId();
    }

    QObject::connect(pLocationSelector, SIGNAL(LocationSelected(QString)), this, SLOT(LocationSelectorLocationSelected(QString)));
}

void MoveToLocationActionEditorDialogContents::LocationSelectorLocationSelected(const QString &locationId)
{
    pMoveToLocationAction->newLocationId = locationId;
}

void MoveToLocationActionEditorDialogContents::TransitionIdComboBoxCurrentItemChanged(const QString &transitionId)
{
    pMoveToLocationAction->transitionId = transitionId;
}

bool MoveToLocationActionEditorDialogContents::ValidateFields()
{
    return true;
}

Conversation::MoveToZoomedViewAction::MoveToZoomedViewAction(Staging::Conversation::MoveToZoomedViewAction *pMoveToZoomedViewAction)
    : MoveToZoomedViewAction()
{
    zoomedViewId = pMoveToZoomedViewAction->ZoomedViewId;
}

EditorDialogContents<Conversation::Action, Conversation::State> * Conversation::MoveToZoomedViewAction::CreateEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::Action *pActionToEdit)
{
    return new MoveToZoomedViewActionEditorDialogContents(stateBeforeObject, static_cast<Conversation::MoveToZoomedViewAction *>(pActionToEdit));
}

void Conversation::MoveToZoomedViewAction::CopyPropertiesImpl(Conversation::Action *pOther, bool /*isForEdit*/)
{
    Conversation::MoveToZoomedViewAction *pAction = static_cast<Conversation::MoveToZoomedViewAction *>(pOther);

    pAction->zoomedViewId = zoomedViewId;
}

QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> Conversation::MoveToZoomedViewAction::GetListItemsImpl(int indentLevel)
{
    QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> editorList;
    editorList.append(new MoveToZoomedViewActionListItem(this, indentLevel));
    return editorList;
}

QString MoveToZoomedViewActionListItem::GetDisplayString()
{
    QString displayString = QString("Move the player to zoomed view ") + UnderlineString(pAction->zoomedViewId) + ".";

    return displayString;
}

MoveToZoomedViewActionEditorDialogContents::MoveToZoomedViewActionEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::MoveToZoomedViewAction *pActionToEdit)
    : EditorDialogContents<Conversation::Action, Conversation::State>(stateBeforeObject, pActionToEdit)
{
    if (pActionToEdit != NULL)
    {
        pMoveToZoomedViewAction = static_cast<Conversation::MoveToZoomedViewAction *>(pActionToEdit->Clone());
    }
    else
    {
        pMoveToZoomedViewAction = new Conversation::MoveToZoomedViewAction();
    }

    pObject = pMoveToZoomedViewAction;

    QHBoxLayout *pLayout = new QHBoxLayout();

    pLayout->addWidget(new QLabel("Move the player to zoomed view "));

    /*QComboBox *pComboBoxId = new QComboBox();
    pLayout->addWidget(pComboBoxId);

    if (pActionToEdit != NULL)
    {
        pComboBoxId->SetToId(pActionToEdit->newLocationId);
    }*/

    pLayout->addStretch(1);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(pLayout);

    /*if (pActionToEdit == NULL)
    {
        pMoveToLocationAction->newLocationId = pLocationSelector->GetId();
    }*/

    //QObject::connect(pLocationSelector, SIGNAL(LocationSelected(QString)), this, SLOT(LocationSelectorLocationSelected(QString)));
}

void MoveToZoomedViewActionEditorDialogContents::ZoomedViewIdComboBoxCurrentItemChanged(const QString &zoomedViewId)
{
    pMoveToZoomedViewAction->zoomedViewId = zoomedViewId;
}

bool MoveToZoomedViewActionEditorDialogContents::ValidateFields()
{
    return true;
}

Conversation::EndCaseAction::EndCaseAction()
{
    completesCase = false;
}

Conversation::EndCaseAction::EndCaseAction(Staging::Conversation::EndCaseAction *pEndCaseAction)
    : EndCaseAction()
{
    completesCase = pEndCaseAction->CompletesCase;
}

EditorDialogContents<Conversation::Action, Conversation::State> * Conversation::EndCaseAction::CreateEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::Action *pActionToEdit)
{
    return new EndCaseActionEditorDialogContents(stateBeforeObject, static_cast<Conversation::EndCaseAction *>(pActionToEdit));
}

void Conversation::EndCaseAction::CopyPropertiesImpl(Conversation::Action *pOther, bool /*isForEdit*/)
{
    Conversation::EndCaseAction *pAction = static_cast<Conversation::EndCaseAction *>(pOther);

    pAction->completesCase = completesCase;
}

QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> Conversation::EndCaseAction::GetListItemsImpl(int indentLevel)
{
    QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> editorList;
    editorList.append(new EndCaseActionListItem(this, indentLevel));
    return editorList;
}

QString EndCaseActionListItem::GetDisplayString()
{
    QString displayString = "End the current case";

    if (pAction->completesCase)
    {
        displayString += ", and mark it as completed.";
    }
    else
    {
        displayString += ", but don't mark it as completed.";
    }

    return displayString;
}

EndCaseActionEditorDialogContents::EndCaseActionEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::EndCaseAction *pActionToEdit)
    : EditorDialogContents<Conversation::Action, Conversation::State>(stateBeforeObject, pActionToEdit)
{
    if (pActionToEdit != NULL)
    {
        pEndCaseAction = static_cast<Conversation::EndCaseAction *>(pActionToEdit->Clone());
    }
    else
    {
        pEndCaseAction = new Conversation::EndCaseAction();
    }

    pObject = pEndCaseAction;

    QVBoxLayout *pLayout = new QVBoxLayout();

    pLayout->addWidget(new QLabel("End the current case."));

    QCheckBox *pCompletesCaseCheckBox = new QCheckBox();
    pCompletesCaseCheckBox->setText("...and mark the case as completed");
    pLayout->addWidget(pCompletesCaseCheckBox);

    if (pActionToEdit != NULL)
    {
        pCompletesCaseCheckBox->setChecked(pActionToEdit->completesCase);
    }

    pLayout->addStretch(1);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(pLayout);

    if (pActionToEdit == NULL)
    {
        pEndCaseAction->completesCase = pCompletesCaseCheckBox->isChecked();
    }

    QObject::connect(pCompletesCaseCheckBox, SIGNAL(toggled(bool)), this, SLOT(CompletesCaseCheckBoxToggled(bool)));
}

void EndCaseActionEditorDialogContents::CompletesCaseCheckBoxToggled(bool isChecked)
{
    pEndCaseAction->completesCase = isChecked;
}

bool EndCaseActionEditorDialogContents::ValidateFields()
{
    return true;
}

Conversation::MultipleChoiceAction::~MultipleChoiceAction()
{
    for (Option *pOption : options)
    {
        delete pOption;
    }

    options.clear();
}

EditorDialogContents<Conversation::Action, Conversation::State> * Conversation::MultipleChoiceAction::CreateEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::Action *pActionToEdit)
{
    return new MultipleChoiceActionEditorDialogContents(stateBeforeObject, static_cast<Conversation::MultipleChoiceAction *>(pActionToEdit));
}

void Conversation::MultipleChoiceAction::CopyPropertiesImpl(Conversation::Action *pOther, bool /*isForEdit*/)
{
    Conversation::MultipleChoiceAction *pAction = static_cast<Conversation::MultipleChoiceAction *>(pOther);

    for (Conversation::MultipleChoiceAction::Option *pOption : pAction->options)
    {
        delete pOption;
    }

    pAction->options.clear();

    for (Conversation::MultipleChoiceAction::Option *pOption : options)
    {
        pAction->options.push_back(pOption->Clone());
    }
}

void Conversation::MultipleChoiceAction::ExchangeListItemBaseOwnershipImpl(Action *pOther)
{
    Conversation::MultipleChoiceAction *pAction = static_cast<Conversation::MultipleChoiceAction *>(pOther);

    for (Conversation::MultipleChoiceAction::Option *pOption : options)
    {
        Conversation::MultipleChoiceAction::Option *pOtherOption = NULL;

        for (Conversation::MultipleChoiceAction::Option *pOptionCandidate : pAction->options)
        {
            if (pOption->GetText() == pOptionCandidate->GetText())
            {
                pOtherOption = pOptionCandidate;
                break;
            }
        }

        if (pOtherOption != NULL)
        {
            pOption->ExchangeListItemBaseOwnership(pOtherOption);
        }
    }
}

void Conversation::MultipleChoiceAction::AddOption(const QString &text, const QList<Action *> &actions)
{
    options.append(new Option(text, actions));
}

QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> Conversation::MultipleChoiceAction::GetListItemsImpl(int indentLevel)
{
    QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> editorList;

    editorList.append(new MultipleChoiceActionListItem(this, indentLevel));

    for (Option *pOption : options)
    {
        editorList.append(new TextDisplay<Conversation::Action, Conversation::State, Conversation::ActionType>(QString("If the player selects \"") + pOption->GetText() + QString("\":"), indentLevel + 1));
        editorList.append(GetListItemsForActions(pOption->GetActions(), indentLevel + 2));
    }

    return editorList;
}

QString MultipleChoiceActionListItem::GetDisplayString()
{
    QString displayString = "<p>Have the player select from the following multiple choice options after showing the previous dialog line:</p>";
    displayString += "<p>";

    for (Conversation::MultipleChoiceAction::Option *pOption : pAction->options)
    {
        displayString += QString("- ") + pOption->GetText() + "<br />";
    }

    displayString += "<p>";
    return displayString;
}

MultipleChoiceActionEditorDialogContents::MultipleChoiceActionEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::MultipleChoiceAction *pActionToEdit)
    : EditorDialogContents<Conversation::Action, Conversation::State>(stateBeforeObject, pActionToEdit)
{
    if (pActionToEdit != NULL)
    {
        pMultipleChoiceAction = static_cast<Conversation::MultipleChoiceAction *>(pActionToEdit->Clone());
    }
    else
    {
        pMultipleChoiceAction = new Conversation::MultipleChoiceAction();
    }

    pObject = pMultipleChoiceAction;

    QVBoxLayout *pLayout = new QVBoxLayout();

    QLabel *pHeaderLabel = new QLabel("Have the player select from the following multiple choice options after showing the previous dialog line:");
    pHeaderLabel->setWordWrap(true);
    pLayout->addWidget(pHeaderLabel);

    MultipleSelectionWidget<LineEditMultipleSelectionSelectorWidget> *pOptionSelector = new MultipleSelectionWidget<LineEditMultipleSelectionSelectorWidget>();

    if (pActionToEdit != NULL && pActionToEdit->options.size() > 0)
    {
        QStringList strings;

        for (Conversation::MultipleChoiceAction::Option *pOption : pActionToEdit->options)
        {
            strings.append(pOption->GetText());
        }

        pOptionSelector->SetSelections(strings);
    }
    else
    {
        QStringList stringList;
        stringList.append("");

        pOptionSelector->SetSelections(stringList);
    }

    pLayout->addWidget(pOptionSelector);

    pLayout->addStretch(1);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(pLayout);

    QObject::connect(pOptionSelector, SIGNAL(StringChanged(int,QString)), this, SLOT(OptionSelectorStringChanged(int,QString)));
    QObject::connect(pOptionSelector, SIGNAL(StringAdded(int,QString)), this, SLOT(OptionSelectorStringAdded(int,QString)));
    QObject::connect(pOptionSelector, SIGNAL(StringRemoved(int)), this, SLOT(OptionSelectorStringRemoved(int)));
}

void MultipleChoiceActionEditorDialogContents::OptionSelectorStringChanged(int index, const QString &string)
{
    pMultipleChoiceAction->options[index]->SetText(string);
}

void MultipleChoiceActionEditorDialogContents::OptionSelectorStringAdded(int index, const QString &string)
{
    pMultipleChoiceAction->options.insert(index, new Conversation::MultipleChoiceAction::Option(string));
}

void MultipleChoiceActionEditorDialogContents::OptionSelectorStringRemoved(int index)
{
    Conversation::MultipleChoiceAction::Option *pRemovedOption = pMultipleChoiceAction->options[index];
    pMultipleChoiceAction->options.removeAt(index);
    delete pRemovedOption;
}

bool MultipleChoiceActionEditorDialogContents::ValidateFields()
{
    return true;
}

Conversation::MultipleChoiceAction::Option::Option(const QString &text)
{
    this->text = text;
}

Conversation::MultipleChoiceAction::Option::Option(const QString &text, const QList<Action *> &actions)
{
    this->text = text;
    this->actions = actions;
}

Conversation::MultipleChoiceAction::Option::~Option()
{
    for (Action *pAction : actions)
    {
        delete pAction;
    }

    actions.clear();
}

Conversation::MultipleChoiceAction::Option * Conversation::MultipleChoiceAction::Option::Clone()
{
    Conversation::MultipleChoiceAction::Option *pClonedOption = new Conversation::MultipleChoiceAction::Option();

    pClonedOption->text = text;

    for (Action *pAction : actions)
    {
        pClonedOption->actions.push_back(pAction->Clone());
    }

    return pClonedOption;
}

void Conversation::MultipleChoiceAction::Option::ReplaceAction(Action *pNewAction, Action *pOldAction)
{
    Conversation::ReplaceAction(pNewAction, pOldAction, actions);
}

void Conversation::MultipleChoiceAction::Option::ExchangeListItemBaseOwnership(Option *pOther)
{
    Conversation::Action::ExchangeListItemBaseOwnership(pOther->actions, actions);
}

EditorDialogContents<Conversation::Action, Conversation::State> * Conversation::ExitMultipleChoiceAction::CreateEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::Action *pActionToEdit)
{
    return new ExitMultipleChoiceActionEditorDialogContents(stateBeforeObject, static_cast<Conversation::ExitMultipleChoiceAction *>(pActionToEdit));
}

void Conversation::ExitMultipleChoiceAction::CopyPropertiesImpl(Action */*pOther*/, bool /*isForEdit*/)
{
}

QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> Conversation::ExitMultipleChoiceAction::GetListItemsImpl(int indentLevel)
{
    QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> editorList;
    editorList.append(new ExitMultipleChoiceActionListItem(this, indentLevel));
    return editorList;
}

QString ExitMultipleChoiceActionListItem::GetDisplayString()
{
    QString displayString = "Exit the current multiple-choice branches.";

    return displayString;
}

ExitMultipleChoiceActionEditorDialogContents::ExitMultipleChoiceActionEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::ExitMultipleChoiceAction *pActionToEdit)
    : EditorDialogContents<Conversation::Action, Conversation::State>(stateBeforeObject, pActionToEdit)
{
    if (pActionToEdit != NULL)
    {
        pExitMultipleChoiceAction = static_cast<Conversation::ExitMultipleChoiceAction *>(pActionToEdit->Clone());
    }
    else
    {
        pExitMultipleChoiceAction = new Conversation::ExitMultipleChoiceAction();
    }

    pObject = pExitMultipleChoiceAction;

    QVBoxLayout *pLayout = new QVBoxLayout();

    pLayout->addWidget(new QLabel("Exit the current multiple-choice branches."));

    pLayout->addStretch(1);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(pLayout);
}

bool ExitMultipleChoiceActionEditorDialogContents::ValidateFields()
{
    return true;
}

EditorDialogContents<Conversation::Action, Conversation::State> * Conversation::EnableFastForwardAction::CreateEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::Action *pActionToEdit)
{
    return new EnableFastForwardActionEditorDialogContents(stateBeforeObject, static_cast<Conversation::EnableFastForwardAction *>(pActionToEdit));
}

void Conversation::EnableFastForwardAction::CopyPropertiesImpl(Action */*pOther*/, bool /*isForEdit*/)
{
}

QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> Conversation::EnableFastForwardAction::GetListItemsImpl(int indentLevel)
{
    QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> editorList;
    editorList.append(new EnableFastForwardActionListItem(this, indentLevel));
    return editorList;
}

QString EnableFastForwardActionListItem::GetDisplayString()
{
    QString displayString = "Enable the player to fast-forward through this conversation.";

    return displayString;
}

EnableFastForwardActionEditorDialogContents::EnableFastForwardActionEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::EnableFastForwardAction *pActionToEdit)
    : EditorDialogContents<Conversation::Action, Conversation::State>(stateBeforeObject, pActionToEdit)
{
    if (pActionToEdit != NULL)
    {
        pEnableFastForwardAction = static_cast<Conversation::EnableFastForwardAction *>(pActionToEdit->Clone());
    }
    else
    {
        pEnableFastForwardAction = new Conversation::EnableFastForwardAction();
    }

    pObject = pEnableFastForwardAction;

    QVBoxLayout *pLayout = new QVBoxLayout();

    pLayout->addWidget(new QLabel("Enable the player to fast-forward through this conversation."));

    pLayout->addStretch(1);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(pLayout);
}

bool EnableFastForwardActionEditorDialogContents::ValidateFields()
{
    return true;
}

EditorDialogContents<Conversation::Action, Conversation::State> * Conversation::DisableFastForwardAction::CreateEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::Action *pActionToEdit)
{
    return new DisableFastForwardActionEditorDialogContents(stateBeforeObject, static_cast<Conversation::DisableFastForwardAction *>(pActionToEdit));
}

void Conversation::DisableFastForwardAction::CopyPropertiesImpl(Action */*pOther*/, bool /*isForEdit*/)
{
}

QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> Conversation::DisableFastForwardAction::GetListItemsImpl(int indentLevel)
{
    QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> editorList;
    editorList.append(new DisableFastForwardActionListItem(this, indentLevel));
    return editorList;
}

QString DisableFastForwardActionListItem::GetDisplayString()
{
    QString displayString = "Disable the player from fast-forwarding through this conversation.";

    return displayString;
}

DisableFastForwardActionEditorDialogContents::DisableFastForwardActionEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::DisableFastForwardAction *pActionToEdit)
    : EditorDialogContents<Conversation::Action, Conversation::State>(stateBeforeObject, pActionToEdit)
{
    if (pActionToEdit != NULL)
    {
        pDisableFastForwardAction = static_cast<Conversation::DisableFastForwardAction *>(pActionToEdit->Clone());
    }
    else
    {
        pDisableFastForwardAction = new Conversation::DisableFastForwardAction();
    }

    pObject = pDisableFastForwardAction;

    QVBoxLayout *pLayout = new QVBoxLayout();

    pLayout->addWidget(new QLabel("Disable the player from fast-forwarding through this conversation."));

    pLayout->addStretch(1);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(pLayout);
}

bool DisableFastForwardActionEditorDialogContents::ValidateFields()
{
    return true;
}

Conversation::BeginBreakdownAction::BeginBreakdownAction()
{
    characterPosition = CharacterPositionNone;
}

Conversation::BeginBreakdownAction::BeginBreakdownAction(Staging::Conversation::BeginBreakdownAction *pBeginBreakdownAction)
    : BeginBreakdownAction()
{
    characterPosition = (CharacterPosition)pBeginBreakdownAction->CharacterPositionValue;
}

EditorDialogContents<Conversation::Action, Conversation::State> * Conversation::BeginBreakdownAction::CreateEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::Action *pActionToEdit)
{
    return new BeginBreakdownActionEditorDialogContents(stateBeforeObject, static_cast<Conversation::BeginBreakdownAction *>(pActionToEdit));
}

void Conversation::BeginBreakdownAction::CopyPropertiesImpl(Conversation::Action *pOther, bool /*isForEdit*/)
{
    Conversation::BeginBreakdownAction *pAction = static_cast<Conversation::BeginBreakdownAction *>(pOther);

    pAction->characterPosition = characterPosition;
}

QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> Conversation::BeginBreakdownAction::GetListItemsImpl(int indentLevel)
{
    QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> editorList;
    editorList.append(new BeginBreakdownActionListItem(this, indentLevel));
    return editorList;
}

QString BeginBreakdownActionListItem::GetDisplayString()
{
    QString speakerName;

    Character *pLeftCharacter = CaseContent::GetInstance()->GetById<Character>(stateBeforeObject.GetLeftCharacterId());
    Character *pRightCharacter = CaseContent::GetInstance()->GetById<Character>(stateBeforeObject.GetRightCharacterId());

    switch (pAction->characterPosition)
    {
    case CharacterPositionLeft:
        speakerName = (pLeftCharacter != NULL ? pLeftCharacter->GetDisplayName() : "no one") + " (the left character)";
        break;
    case CharacterPositionRight:
        speakerName = (pRightCharacter != NULL ? pRightCharacter->GetDisplayName() : "no one") + " (the right character)";
        break;
    default:
        throw new MLIException("Invalid character position for breakdown.");
    }

    QString displayString = "Begin the breakdown animation for " + UnderlineString(speakerName);

    return displayString;
}

BeginBreakdownActionEditorDialogContents::BeginBreakdownActionEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::BeginBreakdownAction *pActionToEdit)
    : EditorDialogContents<Conversation::Action, Conversation::State>(stateBeforeObject, pActionToEdit)
{
    if (pActionToEdit != NULL)
    {
        pBeginBreakdownAction = static_cast<Conversation::BeginBreakdownAction *>(pActionToEdit->Clone());
    }
    else
    {
        pBeginBreakdownAction = new Conversation::BeginBreakdownAction();
    }

    pObject = pBeginBreakdownAction;

    QHBoxLayout *pLayout = new QHBoxLayout();

    pLayout->addWidget(new QLabel("Begin the breakdown animation for "));

    QComboBox *pCharacterComboBox = new QComboBox();

    Character *pLeftCharacter = CaseContent::GetInstance()->GetById<Character>(this->stateBeforeObject.GetLeftCharacterId());
    Character *pRightCharacter = CaseContent::GetInstance()->GetById<Character>(this->stateBeforeObject.GetRightCharacterId());

    pCharacterComboBox->addItem((pLeftCharacter != NULL ? pLeftCharacter->GetDisplayName() : "no one") + " (the left character)");
    pCharacterComboBox->addItem((pRightCharacter != NULL ? pRightCharacter->GetDisplayName() : "no one") + " (the right character)");
    pLayout->addWidget(pCharacterComboBox);

    if (pActionToEdit != NULL)
    {
        switch (pActionToEdit->characterPosition)
        {
        case CharacterPositionLeft:
            pCharacterComboBox->setCurrentIndex(0);
            break;
        case CharacterPositionRight:
            pCharacterComboBox->setCurrentIndex(1);
            break;
        default:
            throw new MLIException("Invalid character position for breakdown.");
        }
    }

    QObject::connect(pCharacterComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(SpeakerComboBoxCurrentIndexChanged(int)));

    CharacterComboBoxCurrentIndexChanged(pCharacterComboBox->currentIndex());

    pLayout->addStretch(1);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(pLayout);
}

void BeginBreakdownActionEditorDialogContents::CharacterComboBoxCurrentIndexChanged(int newIndex)
{
    switch (newIndex)
    {
    case 0:
        pBeginBreakdownAction->characterPosition = CharacterPositionLeft;
        break;
    case 1:
        pBeginBreakdownAction->characterPosition = CharacterPositionRight;
        break;
    default:
        throw new MLIException("Unexpected SpeakerComboBox index selected.");
    }
}

bool BeginBreakdownActionEditorDialogContents::ValidateFields()
{
    return true;
}

EditorDialogContents<Conversation::Action, Conversation::State> * Conversation::EndBreakdownAction::CreateEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::Action *pActionToEdit)
{
    return new EndBreakdownActionEditorDialogContents(stateBeforeObject, static_cast<Conversation::EndBreakdownAction *>(pActionToEdit));
}

void Conversation::EndBreakdownAction::CopyPropertiesImpl(Action */*pOther*/, bool /*isForEdit*/)
{
}

QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> Conversation::EndBreakdownAction::GetListItemsImpl(int indentLevel)
{
    QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> editorList;
    editorList.append(new EndBreakdownActionListItem(this, indentLevel));
    return editorList;
}

QString EndBreakdownActionListItem::GetDisplayString()
{
    QString displayString = "End the currently running breakdown animation.";

    return displayString;
}

EndBreakdownActionEditorDialogContents::EndBreakdownActionEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::EndBreakdownAction *pActionToEdit)
    : EditorDialogContents<Conversation::Action, Conversation::State>(stateBeforeObject, pActionToEdit)
{
    if (pActionToEdit != NULL)
    {
        pEndBreakdownAction = static_cast<Conversation::EndBreakdownAction *>(pActionToEdit->Clone());
    }
    else
    {
        pEndBreakdownAction = new Conversation::EndBreakdownAction();
    }

    pObject = pEndBreakdownAction;

    QVBoxLayout *pLayout = new QVBoxLayout();

    pLayout->addWidget(new QLabel("End the currently running breakdown animation."));

    pLayout->addStretch(1);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(pLayout);
}

bool EndBreakdownActionEditorDialogContents::ValidateFields()
{
    return true;
}

Interrogation::Interrogation(Staging::Interrogation *pStagingInterrogation) :
    Conversation(pStagingInterrogation)
{

}

void Interrogation::WriteToCaseXml(XmlWriter *pWriter)
{
    pWriter->StartElement("Interrogation");
    WriteToCaseXmlCore(pWriter);
    pWriter->EndElement();
}

void Interrogation::PopulateActionFromStaging(QList<Action *> &actions, const QList<Staging::Conversation::Action *> &stagingActions, int &indexInStagingActions, int initialIndexInFullList)
{
    Staging::Conversation::Action *pAction = stagingActions[indexInStagingActions];

    switch (pAction->GetType())
    {
    case Staging::ConversationActionType_BeginInterrogationRepeat:
        {
            Staging::Interrogation::BeginInterrogationRepeatAction *pStagingBeginInterrogationRepeatAction =
                    static_cast<Staging::Interrogation::BeginInterrogationRepeatAction *>(pAction);

            InterrogationRepeatAction *pInterrogationRepeatAction = new InterrogationRepeatAction(pStagingBeginInterrogationRepeatAction);

            int endIndex = -1;

            for (int j = indexInStagingActions + 1; j < stagingActions.count(); j++)
            {
                if (stagingActions[j]->GetType() == Staging::ConversationActionType_EndInterrogationRepeat)
                {
                    endIndex = j;
                    break;
                }
                else if (stagingActions[j]->GetType() == Staging::ConversationActionType_BeginInterrogationRepeat)
                {
                    throw new MLIException("We should never have nested InterrogationRepeatActions.");
                }
            }

            if (endIndex < 0)
            {
                throw new MLIException("Expected an EndInterrogationRepeatAction to follow a BeginInterrogationRepeatAction.");
            }

            int wrongEvidencePresentedIndex = -1;
            int wrongPartnerPresentedIndex = -1;
            int endRequestedIndex = -1;

            QMap<int, QList<Staging::Conversation::Action *> > partialStagingActionsToRepeatByStartIndex;
            QList<Staging::Conversation::Action *> currentPartialStagingActions;
            int currentPartialStagingActionsStartIndex = -1;

            QList<Staging::Conversation::Action *> wrongEvidencePresentedStagingActions;
            QList<Staging::Conversation::Action *> wrongPartnerUsedStagingActions;
            QList<Staging::Conversation::Action *> endRequestedStagingActions;

            for (int j = indexInStagingActions + 1; j < endIndex; j++)
            {
                Staging::Conversation::Action *pInnerStagingAction = stagingActions[j];

                if (pInnerStagingAction->GetType() == Staging::ConversationActionType_BeginWrongEvidencePresented)
                {
                    if (currentPartialStagingActionsStartIndex != -1)
                    {
                        partialStagingActionsToRepeatByStartIndex.insert(currentPartialStagingActionsStartIndex, currentPartialStagingActions);
                        currentPartialStagingActions.clear();
                        currentPartialStagingActionsStartIndex = -1;
                    }

                    wrongEvidencePresentedIndex = j;
                    int k = -1;

                    for (k = j; k < endIndex && stagingActions[k]->GetType() != Staging::ConversationActionType_EndWrongEvidencePresented; k++)
                    {
                        wrongEvidencePresentedStagingActions.append(stagingActions[k]);
                    }

                    wrongEvidencePresentedStagingActions.append(stagingActions[k]);
                    j = k;
                }
                else if (pInnerStagingAction->GetType() == Staging::ConversationActionType_BeginWrongPartnerUsed)
                {
                    if (currentPartialStagingActionsStartIndex != -1)
                    {
                        partialStagingActionsToRepeatByStartIndex.insert(currentPartialStagingActionsStartIndex, currentPartialStagingActions);
                        currentPartialStagingActions.clear();
                        currentPartialStagingActionsStartIndex = -1;
                    }

                    wrongPartnerPresentedIndex = j;
                    int k = -1;

                    for (k = j; k < endIndex && stagingActions[k]->GetType() != Staging::ConversationActionType_EndWrongPartnerUsed; k++)
                    {
                        wrongPartnerUsedStagingActions.append(stagingActions[k]);
                    }

                    wrongPartnerUsedStagingActions.append(stagingActions[k]);
                    j = k;
                }
                else if (pInnerStagingAction->GetType() == Staging::ConversationActionType_BeginEndRequested)
                {
                    if (currentPartialStagingActionsStartIndex != -1)
                    {
                        partialStagingActionsToRepeatByStartIndex.insert(currentPartialStagingActionsStartIndex, currentPartialStagingActions);
                        currentPartialStagingActions.clear();
                        currentPartialStagingActionsStartIndex = -1;
                    }

                    endRequestedIndex = j;
                    int k = -1;

                    for (k = j; k < endIndex && stagingActions[k]->GetType() != Staging::ConversationActionType_EndEndRequested; k++)
                    {
                        endRequestedStagingActions.append(stagingActions[k]);
                    }

                    endRequestedStagingActions.append(stagingActions[k]);
                    j = k;
                }
                else
                {
                    if (currentPartialStagingActionsStartIndex == -1)
                    {
                        currentPartialStagingActionsStartIndex = j;
                    }

                    currentPartialStagingActions.append(pInnerStagingAction);
                }
            }

            if (currentPartialStagingActionsStartIndex != -1)
            {
                partialStagingActionsToRepeatByStartIndex.insert(currentPartialStagingActionsStartIndex, currentPartialStagingActions);
                currentPartialStagingActions.clear();
                currentPartialStagingActionsStartIndex = -1;
            }

            for (int startIndex : partialStagingActionsToRepeatByStartIndex.keys())
            {
                QList<Action *> partialActionsToRepeat;
                PopulateActionsFromStaging(partialActionsToRepeat, partialStagingActionsToRepeatByStartIndex[startIndex], startIndex + initialIndexInFullList);

                pInterrogationRepeatAction->actionsToRepeat.append(partialActionsToRepeat);
            }

            PopulateActionsFromStaging(pInterrogationRepeatAction->wrongEvidencePresentedActions, wrongEvidencePresentedStagingActions, wrongEvidencePresentedIndex + initialIndexInFullList);
            PopulateActionsFromStaging(pInterrogationRepeatAction->wrongPartnerUsedActions, wrongPartnerUsedStagingActions, wrongPartnerPresentedIndex + initialIndexInFullList);
            PopulateActionsFromStaging(pInterrogationRepeatAction->endRequestedActions, endRequestedStagingActions, endRequestedIndex + initialIndexInFullList);

            actions.append(pInterrogationRepeatAction);
            indexInStagingActions = endIndex - 1;
        }
        break;

    case Staging::ConversationActionType_ShowInterrogation:
        {
            Staging::Interrogation::ShowInterrogationAction *pStagingShowInterrogationAction =
                    static_cast<Staging::Interrogation::ShowInterrogationAction *>(pAction);

            ShowInterrogationAction *pShowInterrogationAction = new ShowInterrogationAction(pStagingShowInterrogationAction);

            QList<Staging::Conversation::Action *> pressForInfoStagingActions;

            for (int j = pStagingShowInterrogationAction->PressForInfoIndex - initialIndexInFullList; j < stagingActions.count(); j++)
            {
                pressForInfoStagingActions.append(stagingActions[j]);

                if (stagingActions[j]->GetType() == Staging::ConversationActionType_EndPressForInfo)
                {
                    break;
                }
            }

            PopulateActionsFromStaging(pShowInterrogationAction->pressForInfoActions, pressForInfoStagingActions, pStagingShowInterrogationAction->PressForInfoIndex - initialIndexInFullList);

            for (QString evidenceId : pStagingShowInterrogationAction->EvidencePresentedIndexes.keys())
            {
                QList<Staging::Conversation::Action *> evidencePresentedStagingActions;
                QList<Action *> evidencePresentedActions;

                for (int j = pStagingShowInterrogationAction->EvidencePresentedIndexes[evidenceId] - initialIndexInFullList; j < stagingActions.count(); j++)
                {
                    evidencePresentedStagingActions.append(stagingActions[j]);

                    if (stagingActions[j]->GetType() == Staging::ConversationActionType_EndPresentEvidence)
                    {
                        break;
                    }
                }

                PopulateActionsFromStaging(evidencePresentedActions, evidencePresentedStagingActions, pStagingShowInterrogationAction->EvidencePresentedIndexes[evidenceId] - initialIndexInFullList);
                pShowInterrogationAction->evidencePresentedActionsByEvidenceId.insert(evidenceId, evidencePresentedActions);
            }

            for (QString partnerId : pStagingShowInterrogationAction->PartnerUsedIndexes.keys())
            {
                QList<Staging::Conversation::Action *> partnerUsedStagingActions;
                QList<Action *> partnerUsedActions;

                for (int j = pStagingShowInterrogationAction->PartnerUsedIndexes[partnerId] - initialIndexInFullList; j < stagingActions.count(); j++)
                {
                    partnerUsedStagingActions.append(stagingActions[j]);

                    if (stagingActions[j]->GetType() == Staging::ConversationActionType_EndUsePartner)
                    {
                        break;
                    }
                }

                PopulateActionsFromStaging(partnerUsedActions, partnerUsedStagingActions, pStagingShowInterrogationAction->PartnerUsedIndexes[partnerId] - initialIndexInFullList);
                pShowInterrogationAction->partnerUsedActionsByPartnerId.insert(partnerId, partnerUsedActions);
            }

            actions.append(pShowInterrogationAction);

            indexInStagingActions = (pStagingShowInterrogationAction->NextInterrogationIndex >= 0 ?
                     pStagingShowInterrogationAction->NextInterrogationIndex :
                     pStagingShowInterrogationAction->InterrogationFinishIndex) - initialIndexInFullList - 1;
        }
        break;

    case Staging::ConversationActionType_ExitInterrogationRepeat:
        actions.append(new ExitInterrogationRepeatAction(static_cast<Staging::Interrogation::ExitInterrogationRepeatAction *>(pAction)));
        break;

    default:
        Conversation::PopulateActionFromStaging(actions, stagingActions, indexInStagingActions, initialIndexInFullList);
    }
}

Interrogation::InterrogationRepeatAction::~InterrogationRepeatAction()
{
    for (Action *pAction : actionsToRepeat)
    {
        delete pAction;
    }

    actionsToRepeat.clear();

    for (Action *pAction : wrongEvidencePresentedActions)
    {
        delete pAction;
    }

    wrongEvidencePresentedActions.clear();

    for (Action *pAction : wrongPartnerUsedActions)
    {
        delete pAction;
    }

    wrongPartnerUsedActions.clear();

    for (Action *pAction : endRequestedActions)
    {
        delete pAction;
    }

    endRequestedActions.clear();
}

EditorDialogContents<Conversation::Action, Conversation::State> * Interrogation::InterrogationRepeatAction::CreateEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::Action *pActionToEdit)
{
    return new InterrogationRepeatActionEditorDialogContents(stateBeforeObject, static_cast<Interrogation::InterrogationRepeatAction *>(pActionToEdit));
}

void Interrogation::InterrogationRepeatAction::CopyPropertiesImpl(Conversation::Action *pOther, bool /*isForEdit*/)
{
    Interrogation::InterrogationRepeatAction *pAction = static_cast<Interrogation::InterrogationRepeatAction *>(pOther);

    Conversation::Action::CloneActionList(pAction->actionsToRepeat, actionsToRepeat);
    Conversation::Action::CloneActionList(pAction->wrongEvidencePresentedActions, wrongEvidencePresentedActions);
    Conversation::Action::CloneActionList(pAction->wrongPartnerUsedActions, wrongPartnerUsedActions);
    Conversation::Action::CloneActionList(pAction->endRequestedActions, endRequestedActions);
}

void Interrogation::InterrogationRepeatAction::ExchangeListItemBaseOwnershipImpl(Action *pOther)
{
    Interrogation::InterrogationRepeatAction *pAction = static_cast<Interrogation::InterrogationRepeatAction *>(pOther);

    Conversation::Action::ExchangeListItemBaseOwnership(pAction->actionsToRepeat, actionsToRepeat);
    Conversation::Action::ExchangeListItemBaseOwnership(pAction->wrongEvidencePresentedActions, wrongEvidencePresentedActions);
    Conversation::Action::ExchangeListItemBaseOwnership(pAction->wrongPartnerUsedActions, wrongPartnerUsedActions);
    Conversation::Action::ExchangeListItemBaseOwnership(pAction->endRequestedActions, endRequestedActions);
}

void Interrogation::InterrogationRepeatAction::ReplaceAction(Conversation::Action *pNewAction, Conversation::Action *pOldAction)
{
    Conversation::ReplaceAction(pNewAction, pOldAction, actionsToRepeat);
    Conversation::ReplaceAction(pNewAction, pOldAction, wrongEvidencePresentedActions);
    Conversation::ReplaceAction(pNewAction, pOldAction, wrongPartnerUsedActions);
    Conversation::ReplaceAction(pNewAction, pOldAction, endRequestedActions);
}

void Interrogation::InterrogationRepeatAction::UpdateAndCacheConversationState(State &currentState)
{
    Conversation::Action::UpdateAndCacheConversationState(currentState);

    State actionsToRepeatBranchState = currentState.Clone();
    State wrongEvidencePresentedActionsBranchState = currentState.Clone();
    State wrongPartnerUsedActionsBranchState = currentState.Clone();
    State endRequestedActionsBranchState = currentState.Clone();

    for (Action *pAction : actionsToRepeat)
    {
        pAction->UpdateAndCacheConversationState(actionsToRepeatBranchState);
    }

    for (Action *pAction : wrongEvidencePresentedActions)
    {
        pAction->UpdateAndCacheConversationState(wrongEvidencePresentedActionsBranchState);
    }

    for (Action *pAction : wrongPartnerUsedActions)
    {
        pAction->UpdateAndCacheConversationState(wrongPartnerUsedActionsBranchState);
    }

    for (Action *pAction : endRequestedActions)
    {
        pAction->UpdateAndCacheConversationState(endRequestedActionsBranchState);
    }

    currentState = actionsToRepeatBranchState.Clone();
}

void Interrogation::InterrogationRepeatAction::PreloadAudio()
{
    for (Action *pAction : actionsToRepeat)
    {
        pAction->PreloadAudio();
    }

    for (Action *pAction : wrongEvidencePresentedActions)
    {
        pAction->PreloadAudio();
    }

    for (Action *pAction : wrongPartnerUsedActions)
    {
        pAction->PreloadAudio();
    }

    for (Action *pAction : endRequestedActions)
    {
        pAction->PreloadAudio();
    }
}

QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> Interrogation::InterrogationRepeatAction::GetListItemsImpl(int indentLevel)
{
    QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> editorList;

    editorList.append(new InterrogationRepeatActionListItem(this, indentLevel));
    editorList.append(new TextDisplay<Conversation::Action, Conversation::State, Conversation::ActionType>("Loop until done:", indentLevel + 1));
    editorList.append(GetListItemsForActions(actionsToRepeat, indentLevel + 2));
    editorList.append(new TextDisplay<Conversation::Action, Conversation::State, Conversation::ActionType>("If the player presents the wrong evidence:", indentLevel + 1));
    editorList.append(GetListItemsForActions(wrongEvidencePresentedActions, indentLevel + 2));
    editorList.append(new TextDisplay<Conversation::Action, Conversation::State, Conversation::ActionType>("If the player uses the wrong partner:", indentLevel + 1));
    editorList.append(GetListItemsForActions(wrongPartnerUsedActions, indentLevel + 2));
    editorList.append(new TextDisplay<Conversation::Action, Conversation::State, Conversation::ActionType>("If the player gives up:", indentLevel + 1));
    editorList.append(GetListItemsForActions(endRequestedActions, indentLevel + 2));

    return editorList;
}

QString InterrogationRepeatActionListItem::GetDisplayString()
{
    QString displayString = "Repeat the following interrogation actions until the player reaches the exit condition or quits.";

    return displayString;
}

InterrogationRepeatActionEditorDialogContents::InterrogationRepeatActionEditorDialogContents(const Conversation::State &stateBeforeObject, Interrogation::InterrogationRepeatAction *pActionToEdit)
    : EditorDialogContents<Conversation::Action, Conversation::State>(stateBeforeObject, pActionToEdit)
{
    if (pActionToEdit != NULL)
    {
        pInterrogationRepeatAction = static_cast<Interrogation::InterrogationRepeatAction *>(pActionToEdit->Clone());
    }
    else
    {
        pInterrogationRepeatAction = new Interrogation::InterrogationRepeatAction();
    }

    pObject = pInterrogationRepeatAction;

    QVBoxLayout *pLayout = new QVBoxLayout();

    QLabel *pLabel = new QLabel("Repeat the following interrogation actions until the player reaches the exit condition or quits.");
    pLabel->setWordWrap(true);
    pLayout->addWidget(pLabel);

    pLayout->addStretch(1);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(pLayout);
}

bool InterrogationRepeatActionEditorDialogContents::ValidateFields()
{
    return true;
}

Interrogation::ShowInterrogationAction::ShowInterrogationAction(Staging::Interrogation::ShowInterrogationAction *pShowInterrogationAction)
    : ShowDialogAction(pShowInterrogationAction)
{
    conditionFlag = pShowInterrogationAction->ConditionFlag;
}

Interrogation::ShowInterrogationAction::~ShowInterrogationAction()
{
    for (Action *pAction : pressForInfoActions)
    {
        delete pAction;
    }

    pressForInfoActions.clear();

    for (QString evidenceId : evidencePresentedActionsByEvidenceId.keys())
    {
        for (Action *pAction : evidencePresentedActionsByEvidenceId[evidenceId])
        {
            delete pAction;
        }

        evidencePresentedActionsByEvidenceId[evidenceId].clear();
    }

    evidencePresentedActionsByEvidenceId.clear();

    for (QString partnerId : partnerUsedActionsByPartnerId.keys())
    {
        for (Action *pAction : partnerUsedActionsByPartnerId[partnerId])
        {
            delete pAction;
        }

        partnerUsedActionsByPartnerId[partnerId].clear();
    }

    partnerUsedActionsByPartnerId.clear();
}

EditorDialogContents<Conversation::Action, Conversation::State> * Interrogation::ShowInterrogationAction::CreateEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::Action *pActionToEdit)
{
    return new ShowDialogActionEditorDialogContents(stateBeforeObject, ShowDialogActionEditorDialogContents::Type::ShowInterrogation, dynamic_cast<Interrogation::ShowInterrogationAction *>(pActionToEdit));
}

void Interrogation::ShowInterrogationAction::CopyPropertiesImpl(Conversation::Action *pOther, bool isForEdit)
{
    Interrogation::ShowInterrogationAction *pAction = static_cast<Interrogation::ShowInterrogationAction *>(pOther);

    Conversation::ShowDialogAction::CopyPropertiesImpl(pAction, isForEdit);

    Conversation::Action::CloneActionList(pAction->pressForInfoActions, pressForInfoActions);

    pAction->evidencePresentedActionsByEvidenceId.clear();

    for (QString evidenceId : evidencePresentedActionsByEvidenceId.keys())
    {
        Conversation::Action::CloneActionList(
                    pAction->evidencePresentedActionsByEvidenceId[evidenceId],
                    evidencePresentedActionsByEvidenceId[evidenceId]);
    }

    pAction->partnerUsedActionsByPartnerId.clear();

    for (QString partnerId : partnerUsedActionsByPartnerId.keys())
    {
        Conversation::Action::CloneActionList(
                    pAction->partnerUsedActionsByPartnerId[partnerId],
                    partnerUsedActionsByPartnerId[partnerId]);
    }
}

void Interrogation::ShowInterrogationAction::ExchangeListItemBaseOwnershipImpl(Action *pOther)
{
    Interrogation::ShowInterrogationAction *pAction = static_cast<Interrogation::ShowInterrogationAction *>(pOther);

    Conversation::Action::ExchangeListItemBaseOwnership(pAction->pressForInfoActions, pressForInfoActions);

    for (QString evidenceId : evidencePresentedActionsByEvidenceId.keys())
    {
        QString otherEvidenceId = evidenceId;

        if (pAction->originalEvidenceIdToNewEvidenceIdMap.contains(evidenceId))
        {
            otherEvidenceId = pAction->originalEvidenceIdToNewEvidenceIdMap[evidenceId];
        }

        if (pAction->evidencePresentedActionsByEvidenceId.contains(otherEvidenceId) &&
                evidencePresentedActionsByEvidenceId[evidenceId].size() == pAction->evidencePresentedActionsByEvidenceId[otherEvidenceId].size())
        {
            Conversation::Action::ExchangeListItemBaseOwnership(
                        pAction->evidencePresentedActionsByEvidenceId[otherEvidenceId],
                        evidencePresentedActionsByEvidenceId[evidenceId]);
        }
    }

    for (QString partnerId : partnerUsedActionsByPartnerId.keys())
    {
        QString otherPartnerId = partnerId;

        if (pAction->originalPartnerIdToNewPartnerIdMap.contains(partnerId))
        {
            otherPartnerId = pAction->originalPartnerIdToNewPartnerIdMap[partnerId];
        }

        if (pAction->partnerUsedActionsByPartnerId.contains(otherPartnerId) &&
                partnerUsedActionsByPartnerId[partnerId].size() == pAction->partnerUsedActionsByPartnerId[otherPartnerId].size())
        {
            Conversation::Action::ExchangeListItemBaseOwnership(
                        pAction->partnerUsedActionsByPartnerId[otherPartnerId],
                        partnerUsedActionsByPartnerId[partnerId]);
        }
    }
}

void Interrogation::ShowInterrogationAction::ReplaceAction(Conversation::Action *pNewAction, Conversation::Action *pOldAction)
{
    Conversation::ReplaceAction(pNewAction, pOldAction, pressForInfoActions);

    for (QString evidenceId : evidencePresentedActionsByEvidenceId.keys())
    {
        Conversation::ReplaceAction(pNewAction, pOldAction, evidencePresentedActionsByEvidenceId[evidenceId]);
    }

    for (QString partnerId : partnerUsedActionsByPartnerId.keys())
    {
        Conversation::ReplaceAction(pNewAction, pOldAction, partnerUsedActionsByPartnerId[partnerId]);
    }
}

void Interrogation::ShowInterrogationAction::UpdateAndCacheConversationState(State &currentState)
{
    Conversation::ShowDialogAction::UpdateAndCacheConversationState(currentState);

    State tempBranchState = currentState.Clone();

    for (Action *pAction : pressForInfoActions)
    {
        pAction->UpdateAndCacheConversationState(tempBranchState);
    }

    for (QString evidenceId : evidencePresentedActionsByEvidenceId.keys())
    {
        tempBranchState = currentState.Clone();

        for (Action *pAction : evidencePresentedActionsByEvidenceId[evidenceId])
        {
            pAction->UpdateAndCacheConversationState(tempBranchState);
        }
    }

    for (QString partnerId : partnerUsedActionsByPartnerId.keys())
    {
        tempBranchState = currentState.Clone();

        for (Action *pAction : partnerUsedActionsByPartnerId[partnerId])
        {
            pAction->UpdateAndCacheConversationState(tempBranchState);
        }
    }
}

void Interrogation::ShowInterrogationAction::PreloadAudio()
{
    Conversation::ShowDialogAction::PreloadAudio();

    for (Action *pAction : pressForInfoActions)
    {
        pAction->PreloadAudio();
    }

    for (QString evidenceId : evidencePresentedActionsByEvidenceId.keys())
    {
        for (Action *pAction : evidencePresentedActionsByEvidenceId[evidenceId])
        {
            pAction->PreloadAudio();
        }
    }

    for (QString partnerId : partnerUsedActionsByPartnerId.keys())
    {
        for (Action *pAction : partnerUsedActionsByPartnerId[partnerId])
        {
            pAction->PreloadAudio();
        }
    }
}

void Interrogation::ShowInterrogationAction::SaveElementsToXml(XmlWriter *pWriter)
{
    ShowDialogAction::SaveElementsToXml(pWriter);

    if (evidencePresentedActionsByEvidenceId.keys().count() > 0)
    {
        pWriter->StartElement("EvidencePresentedActionsByEvidenceId");

        for (QString evidenceId : evidencePresentedActionsByEvidenceId.keys())
        {
            pWriter->StartElement("Entry");
            pWriter->WriteTextElement("EvidenceId", evidenceId);

            pWriter->StartElement("Actions");

            for (Action *pAction : evidencePresentedActionsByEvidenceId[evidenceId])
            {
                pWriter->StartElement("Action");
                pAction->SaveToXml(pWriter);
                pWriter->EndElement();
            }

            pWriter->EndElement();

            pWriter->EndElement();
        }

        pWriter->EndElement();
    }

    if (partnerUsedActionsByPartnerId.keys().count() > 0)
    {
        pWriter->StartElement("PartnerUsedActionsByPartnerId");

        for (QString partnerId : partnerUsedActionsByPartnerId.keys())
        {
            pWriter->StartElement("Entry");
            pWriter->WriteTextElement("PartnerId", partnerId);

            pWriter->StartElement("Actions");

            for (Action *pAction : partnerUsedActionsByPartnerId[partnerId])
            {
                pWriter->StartElement("Action");
                pAction->SaveToXml(pWriter);
                pWriter->EndElement();
            }

            pWriter->EndElement();

            pWriter->EndElement();
        }

        pWriter->EndElement();
    }
}

void Interrogation::ShowInterrogationAction::LoadElementsFromXml(XmlReader *pReader)
{
    ShowDialogAction::LoadElementsFromXml(pReader);

    if (pReader->ElementExists("EvidencePresentedActionsByEvidenceId"))
    {
        pReader->StartElement("EvidencePresentedActionsByEvidenceId");
        pReader->StartList("Entry");

        while (pReader->MoveToNextListItem())
        {
            QString evidenceId = pReader->ReadTextElement("EvidenceId");

            pReader->StartElement("Actions");
            pReader->StartList("Action");

            while (pReader->MoveToNextListItem())
            {
                evidencePresentedActionsByEvidenceId[evidenceId].push_back(Action::CreateFromXml(pReader));
            }

            pReader->EndElement();
        }

        pReader->EndElement();
    }

    if (pReader->ElementExists("PartnerUsedActionsByPartnerId"))
    {
        pReader->StartElement("PartnerUsedActionsByPartnerId");
        pReader->StartList("Entry");

        while (pReader->MoveToNextListItem())
        {
            QString partnerId = pReader->ReadTextElement("PartnerId");

            pReader->StartElement("Actions");
            pReader->StartList("Action");

            while (pReader->MoveToNextListItem())
            {
                partnerUsedActionsByPartnerId[partnerId].push_back(Action::CreateFromXml(pReader));
            }

            pReader->EndElement();
        }

        pReader->EndElement();
    }
}

QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> Interrogation::ShowInterrogationAction::GetListItemsImpl(int indentLevel)
{
    QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> editorList;

    editorList.append(new ShowInterrogationActionListItem(this, indentLevel));
    editorList.append(new TextDisplay<Conversation::Action, Conversation::State, Conversation::ActionType>("If the player presses for more information:", indentLevel + 1));
    editorList.append(GetListItemsForActions(pressForInfoActions, indentLevel + 2));

    for (QString evidenceId : evidencePresentedActionsByEvidenceId.keys())
    {
        editorList.append(new TextDisplay<Conversation::Action, Conversation::State, Conversation::ActionType>(QString("If the player presents \"") + evidenceId + QString("\":"), indentLevel + 1));
        editorList.append(GetListItemsForActions(evidencePresentedActionsByEvidenceId[evidenceId], indentLevel + 2));
    }

    for (QString partnerId : partnerUsedActionsByPartnerId.keys())
    {
        editorList.append(new TextDisplay<Conversation::Action, Conversation::State, Conversation::ActionType>(QString("If the player uses \"") + partnerId + QString("\":"), indentLevel + 1));
        editorList.append(GetListItemsForActions(partnerUsedActionsByPartnerId[partnerId], indentLevel + 2));
    }

    return editorList;
}

QString ShowInterrogationActionListItem::GetHeaderString(const QString &speakerName)
{
    QString headerString = "<p>Show ";
    headerString += UnderlineString(speakerName);
    headerString += " making the following interrogation statement:</p>";

    return headerString;
}

QString ShowInterrogationActionListItem::GetFooterString()
{
    Interrogation::ShowInterrogationAction *pShowInterrogationAction = GetShowInterrogationAction();
    QString footerString = "";

    if (pShowInterrogationAction->conditionFlag.length() > 0)
    {
        footerString += "<p>Only show this statement if the flag ";
        footerString += UnderlineString(pShowInterrogationAction->conditionFlag);
        footerString += " is set.</p>";
    }

    if (pShowInterrogationAction->evidencePresentedActionsByEvidenceId.size() > 0)
    {
        footerString += "<p>Accept the following as valid evidence to present against this statement: ";
        footerString += ConcatenateStringList(pShowInterrogationAction->evidencePresentedActionsByEvidenceId.keys());
        footerString += "</p>";
    }

    if (pShowInterrogationAction->partnerUsedActionsByPartnerId.size() > 0)
    {
        footerString += "<p>Accept the following as valid partners to use on this statement: ";
        footerString += ConcatenateStringList(pShowInterrogationAction->partnerUsedActionsByPartnerId.keys());
        footerString += "</p>";
    }

    return footerString;
}

EditorDialogContents<Conversation::Action, Conversation::State> * Interrogation::ExitInterrogationRepeatAction::CreateEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::Action *pActionToEdit)
{
    return new ExitInterrogationRepeatActionEditorDialogContents(stateBeforeObject, static_cast<Interrogation::ExitInterrogationRepeatAction *>(pActionToEdit));
}

void Interrogation::ExitInterrogationRepeatAction::CopyPropertiesImpl(Action */*pOther*/, bool /*isForEdit*/)
{
}

QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> Interrogation::ExitInterrogationRepeatAction::GetListItemsImpl(int indentLevel)
{
    QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> editorList;
    editorList.append(new ExitInterrogationRepeatActionListItem(this, indentLevel));
    return editorList;
}

QString ExitInterrogationRepeatActionListItem::GetDisplayString()
{
    QString displayString = "Exit the current interrogation repeat loop.";

    return displayString;
}

ExitInterrogationRepeatActionEditorDialogContents::ExitInterrogationRepeatActionEditorDialogContents(const Conversation::State &stateBeforeObject, Interrogation::ExitInterrogationRepeatAction *pActionToEdit)
    : EditorDialogContents<Conversation::Action, Conversation::State>(stateBeforeObject, pActionToEdit)
{
    if (pActionToEdit != NULL)
    {
        pExitInterrogationRepeatAction = static_cast<Interrogation::ExitInterrogationRepeatAction *>(pActionToEdit->Clone());
    }
    else
    {
        pExitInterrogationRepeatAction = new Interrogation::ExitInterrogationRepeatAction();
    }

    pObject = pExitInterrogationRepeatAction;

    QVBoxLayout *pLayout = new QVBoxLayout();

    pLayout->addWidget(new QLabel("Exit the current interrogation repeat loop."));

    pLayout->addStretch(1);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(pLayout);
}

bool ExitInterrogationRepeatActionEditorDialogContents::ValidateFields()
{
    return true;
}

Confrontation::Confrontation(Staging::Confrontation *pStagingConfrontation) :
    Interrogation(pStagingConfrontation)
{

}

void Confrontation::WriteToCaseXml(XmlWriter *pWriter)
{
    pWriter->StartElement("Confrontation");
    WriteToCaseXmlCore(pWriter);
    pWriter->EndElement();
}

void Confrontation::WriteToCaseXmlCore(XmlWriter *pWriter)
{
    Interrogation::WriteToCaseXmlCore(pWriter);
}

void Confrontation::PopulateActionFromStaging(QList<Action *> &actions, const QList<Staging::Conversation::Action *> &stagingActions, int &indexInStagingActions, int initialIndexInFullList)
{
    Staging::Conversation::Action *pAction = stagingActions[indexInStagingActions];

    switch (pAction->GetType())
    {
    case Staging::ConversationActionType_SetParticipants:
        {
            Staging::Confrontation::SetParticipantsAction *pStagingSetParticipantsAction = static_cast<Staging::Confrontation::SetParticipantsAction *>(pAction);

            playerCharacterId = pStagingSetParticipantsAction->PlayerCharacterId;
            opponentCharacterId = pStagingSetParticipantsAction->OpponentCharacterId;
        }
        break;

    case Staging::ConversationActionType_SetIconOffset:
        {
            Staging::Confrontation::SetIconOffsetAction *pStagingSetIconOffsetAction = static_cast<Staging::Confrontation::SetIconOffsetAction *>(pAction);

            if (pStagingSetIconOffsetAction->IsPlayer)
            {
                playerIconOffset = pStagingSetIconOffsetAction->Offset;
            }
            else
            {
                opponentIconOffset = pStagingSetIconOffsetAction->Offset;
            }
        }
        break;

    case Staging::ConversationActionType_SetHealth:
        {
            Staging::Confrontation::SetHealthAction *pStagingSetHealthAction = static_cast<Staging::Confrontation::SetHealthAction *>(pAction);

            if (pStagingSetHealthAction->IsPlayer)
            {
                playerInitialHealth = pStagingSetHealthAction->InitialHealth;
            }
            else
            {
                opponentInitialHealth = pStagingSetHealthAction->InitialHealth;
            }
        }
        break;

    case Staging::ConversationActionType_BeginConfrontationTopicSelection:
        {
            Staging::Confrontation::BeginConfrontationTopicSelectionAction *pBeginConfrontationTopicSelectionAction =
                    static_cast<Staging::Confrontation::BeginConfrontationTopicSelectionAction *>(pAction);

            ConfrontationTopicSelectionAction *pConfrontationTopicSelectionAction = new ConfrontationTopicSelectionAction(pBeginConfrontationTopicSelectionAction);

            QList<Staging::Confrontation::Topic *> topicList;
            topicList.append(pBeginConfrontationTopicSelectionAction->TopicList);

            if (pBeginConfrontationTopicSelectionAction->pInitialTopic != NULL)
            {
                topicList.append(pBeginConfrontationTopicSelectionAction->pInitialTopic);
            }

            for (Staging::Confrontation::Topic *pStagingTopic : topicList)
            {
                QList<Staging::Conversation::Action *> topicStagingActions;
                QList<Action *> topicActions;

                int endIndex = -1;

                for (int j = pStagingTopic->ActionIndex; j < stagingActions.count(); j++)
                {
                    Staging::Conversation::Action *pInnerStagingAction = stagingActions[j];

                    topicStagingActions.append(pInnerStagingAction);

                    if (pInnerStagingAction->GetType() == Staging::ConversationActionType_EndConfrontationTopic)
                    {
                        endIndex = j;
                        break;
                    }
                }

                if (endIndex < 0)
                {
                    throw new MLIException("Expected an EndConfrontationTopicAction to follow a BeginConfrontationTopicAction.");
                }

                PopulateActionsFromStaging(topicActions, topicStagingActions, pStagingTopic->ActionIndex - initialIndexInFullList);
                pConfrontationTopicSelectionAction->AddTopic(pStagingTopic, topicActions, pStagingTopic == pBeginConfrontationTopicSelectionAction->pInitialTopic);
            }

            if (pBeginConfrontationTopicSelectionAction->PlayerDefeatedIndex >= 0)
            {
                QList<Staging::Conversation::Action *> playerDefeatedStagingActions;

                int endIndex = -1;

                for (int j = pBeginConfrontationTopicSelectionAction->PlayerDefeatedIndex; j < stagingActions.count(); j++)
                {
                    Staging::Conversation::Action *pInnerStagingAction = stagingActions[j];

                    playerDefeatedStagingActions.append(pInnerStagingAction);

                    if (pInnerStagingAction->GetType() == Staging::ConversationActionType_EndPlayerDefeated)
                    {
                        endIndex = j;
                        break;
                    }
                }

                if (endIndex < 0)
                {
                    throw new MLIException("Expected an EndPlayerDefeatedAction to follow a BeginPlayerDefeatedAction.");
                }

                PopulateActionsFromStaging(pConfrontationTopicSelectionAction->playerDefeatedActions, playerDefeatedStagingActions, pBeginConfrontationTopicSelectionAction->PlayerDefeatedIndex - initialIndexInFullList);
            }

            actions.append(pConfrontationTopicSelectionAction);
            indexInStagingActions = pBeginConfrontationTopicSelectionAction->EndActionIndex - 1;
        }
        break;

    case Staging::ConversationActionType_EnableTopic:
        actions.append(new EnableTopicAction(static_cast<Staging::Confrontation::EnableTopicAction *>(pAction)));
        break;

    case Staging::ConversationActionType_BeginMultipleChoice:
        {
            Staging::Conversation::BeginMultipleChoiceAction *pStagingBeginMultipleChoiceAction =
                    static_cast<Staging::Conversation::BeginMultipleChoiceAction *>(pAction);

            QList<Action *> multipleChoiceActionHolder;
            MultipleChoiceAction *pMultipleChoiceAction = NULL;

            Interrogation::PopulateActionFromStaging(multipleChoiceActionHolder, stagingActions, indexInStagingActions, initialIndexInFullList);

            if (multipleChoiceActionHolder.count() != 1 || (pMultipleChoiceAction = dynamic_cast<MultipleChoiceAction *>(multipleChoiceActionHolder[0])) == NULL)
            {
                throw new MLIException("Expected to get a MultipleChoiceAction from a BeginMultipleChoiceAction in the stager.");
            }

            // In V1 of the case engine, we didn't actually have RestartDecisionActions, so we get at what we meant by those
            // by checking a MultipleChoiceAction for very specific options.
            bool isRestartDecisionAction =
                    pMultipleChoiceAction->options.count() == 2 &&
                    pMultipleChoiceAction->options[0]->text == QString("Try again - Restart confrontation") &&
                    pMultipleChoiceAction->options[1]->text == QString("Give up - Back to title screen");

            if (isRestartDecisionAction)
            {
                RestartDecisionAction *pRestartDecisionAction = new RestartDecisionAction(pStagingBeginMultipleChoiceAction);
                pRestartDecisionAction->restartActions = pMultipleChoiceAction->options[0]->actions;
                pRestartDecisionAction->quitActions = pMultipleChoiceAction->options[1]->actions;

                actions.append(pRestartDecisionAction);
            }
            else
            {
                actions.append(pMultipleChoiceAction);
            }
        }
        break;

    case Staging::ConversationActionType_RestartConfrontation:
        actions.append(new RestartConfrontationAction(static_cast<Staging::Confrontation::RestartConfrontationAction *>(pAction)));
        break;

    default:
        Interrogation::PopulateActionFromStaging(actions, stagingActions, indexInStagingActions, initialIndexInFullList);
    }
}

Confrontation::Topic::Topic()
{
    isEnabledAtStart = false;
}

Confrontation::Topic::Topic(QString id, QString name, bool isEnabledAtStart)
{
    this->id = id;
    this->name = name;
    this->isEnabledAtStart = isEnabledAtStart;
}

Confrontation::Topic::Topic(Staging::Confrontation::Topic *pStagingTopic)
    : Topic()
{
    id = pStagingTopic->Id;
    name = pStagingTopic->Name;
    isEnabledAtStart = pStagingTopic->IsEnabled;
}

Confrontation::Topic::~Topic()
{
    for (Action *pAction : actions)
    {
        delete pAction;
    }

    actions.clear();
}

Confrontation::Topic * Confrontation::Topic::Clone()
{
    Confrontation::Topic *pClonedTopic = new Confrontation::Topic();

    pClonedTopic->id = id;
    pClonedTopic->name = name;
    pClonedTopic->isEnabledAtStart = isEnabledAtStart;
    Conversation::Action::CloneActionList(pClonedTopic->actions, actions);

    return pClonedTopic;
}

void Confrontation::Topic::AddActions(const QList<Action *> &actions)
{
    this->actions.append(actions);
}

void Confrontation::Topic::ReplaceAction(Conversation::Action *pNewAction, Conversation::Action *pOldAction)
{
    Conversation::ReplaceAction(pNewAction, pOldAction, actions);
}

void Confrontation::Topic::ExchangeListItemBaseOwnership(Topic *pOther)
{
    Conversation::Action::ExchangeListItemBaseOwnership(pOther->actions, actions);
}

void Confrontation::Topic::UpdateAndCacheConversationState(State &currentState)
{
    for (Action *pAction : actions)
    {
        pAction->UpdateAndCacheConversationState(currentState);
    }
}

void Confrontation::Topic::PreloadAudio()
{
    for (Action *pAction : actions)
    {
        pAction->PreloadAudio();
    }
}

Confrontation::ConfrontationTopicSelectionAction::ConfrontationTopicSelectionAction()
{
    pInitialTopic = NULL;
}

Confrontation::ConfrontationTopicSelectionAction::~ConfrontationTopicSelectionAction()
{
    for (Topic *pTopic : topics)
    {
        delete pTopic;
    }

    topics.clear();

    delete pInitialTopic;
    pInitialTopic = NULL;

    for (Action *pAction : playerDefeatedActions)
    {
        delete pAction;
    }

    playerDefeatedActions.clear();
}

EditorDialogContents<Conversation::Action, Conversation::State> * Confrontation::ConfrontationTopicSelectionAction::CreateEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::Action *pActionToEdit)
{
    return new ConfrontationTopicSelectionActionEditorDialogContents(stateBeforeObject, static_cast<Confrontation::ConfrontationTopicSelectionAction *>(pActionToEdit));
}

void Confrontation::ConfrontationTopicSelectionAction::CopyPropertiesImpl(Conversation::Action *pOther, bool /*isForEdit*/)
{
    Confrontation::ConfrontationTopicSelectionAction *pAction = static_cast<Confrontation::ConfrontationTopicSelectionAction *>(pOther);

    delete pAction->pInitialTopic;
    pAction->pInitialTopic = pInitialTopic != NULL ? pInitialTopic->Clone() : NULL;

    for (Topic *pTopic : pAction->topics)
    {
        delete pTopic;
    }

    pAction->topics.clear();

    for (Topic *pTopic : topics)
    {
        pAction->topics.push_back(pTopic->Clone());
    }

    Conversation::Action::CloneActionList(pAction->playerDefeatedActions, playerDefeatedActions);
}

void Confrontation::ConfrontationTopicSelectionAction::ExchangeListItemBaseOwnershipImpl(Action *pOther)
{
    Confrontation::ConfrontationTopicSelectionAction *pAction = static_cast<Confrontation::ConfrontationTopicSelectionAction *>(pOther);

    for (Confrontation::Topic *pTopic : topics)
    {
        Confrontation::Topic *pOtherTopic = NULL;

        for (Confrontation::Topic *pTopicCandidate : pAction->topics)
        {
            if (pTopic->GetId() == pTopicCandidate->GetId())
            {
                pOtherTopic = pTopicCandidate;
                break;
            }
        }

        if (pOtherTopic != NULL)
        {
            pTopic->ExchangeListItemBaseOwnership(pOtherTopic);
        }
    }

    Conversation::Action::ExchangeListItemBaseOwnership(pAction->playerDefeatedActions, playerDefeatedActions);
}

void Confrontation::ConfrontationTopicSelectionAction::AddTopic(Staging::Confrontation::Topic *pStagingTopic, const QList<Action *> &actions, bool isInitialTopic)
{
    Topic *pTopic = new Topic(pStagingTopic);
    pTopic->AddActions(actions);

    if (isInitialTopic)
    {
        pInitialTopic = pTopic;
    }
    else
    {
        topics.append(pTopic);
    }
}

void Confrontation::ConfrontationTopicSelectionAction::ReplaceAction(Conversation::Action *pNewAction, Conversation::Action *pOldAction)
{
    for (Topic *pTopic : topics)
    {
        pTopic->ReplaceAction(pNewAction, pOldAction);
    }

    if (pInitialTopic != NULL)
    {
        pInitialTopic->ReplaceAction(pNewAction, pOldAction);
    }

    Conversation::ReplaceAction(pNewAction, pOldAction, playerDefeatedActions);
}

void Confrontation::ConfrontationTopicSelectionAction::UpdateAndCacheConversationState(State &currentState)
{
    Conversation::Action::UpdateAndCacheConversationState(currentState);

    for (Topic *pTopic : topics)
    {
        State tempState = currentState.Clone();
        pTopic->UpdateAndCacheConversationState(tempState);
    }

    if (pInitialTopic != NULL)
    {
        State tempState = currentState.Clone();
        pInitialTopic->UpdateAndCacheConversationState(tempState);
    }

    State tempState = currentState.Clone();

    for (Action *pAction : playerDefeatedActions)
    {
        pAction->UpdateAndCacheConversationState(tempState);
    }
}

void Confrontation::ConfrontationTopicSelectionAction::PreloadAudio()
{
    Conversation::Action::PreloadAudio();

    for (Topic *pTopic : topics)
    {
        pTopic->PreloadAudio();
    }

    if (pInitialTopic != NULL)
    {
        pInitialTopic->PreloadAudio();
    }

    for (Action *pAction : playerDefeatedActions)
    {
        pAction->PreloadAudio();
    }
}

QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> Confrontation::ConfrontationTopicSelectionAction::GetListItemsImpl(int indentLevel)
{
    QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> editorList;

    editorList.append(new ConfrontationTopicSelectionActionListItem(this, indentLevel));

    if (pInitialTopic != NULL)
    {
        editorList.append(new TextDisplay<Conversation::Action, Conversation::State, Conversation::ActionType>("When the player first enters the confrontation:", indentLevel + 1));
        editorList.append(GetListItemsForActions(pInitialTopic->GetActions(), indentLevel + 2));
    }

    for (Topic *pTopic : topics)
    {
        editorList.append(new TextDisplay<Conversation::Action, Conversation::State, Conversation::ActionType>(QString("When the player selects \"") + pTopic->GetName() + "\":", indentLevel + 1));
        editorList.append(GetListItemsForActions(pTopic->GetActions(), indentLevel + 2));
    }

    editorList.append(new TextDisplay<Conversation::Action, Conversation::State, Conversation::ActionType>("If the player is defeated:", indentLevel + 1));
    editorList.append(GetListItemsForActions(playerDefeatedActions, indentLevel + 2));

    return editorList;
}

QString ConfrontationTopicSelectionActionListItem::GetDisplayString()
{
    QString displayString = "<p>Have the player select from the following confrontation topics:</p>";
    displayString += "<p>";

    for (Confrontation::Topic *pTopic : pAction->topics)
    {
        displayString += QString("- ") + pTopic->GetName() + (pTopic->GetIsEnabledAtStart() ? " (enabled at the start)" : "") + "<br />";
    }

    displayString += "</p>";

    if (pAction->pInitialTopic != NULL)
    {
        displayString += "<p>Also include an initial topic to play immediately as soon as the player enters the confrontation.</p>";
    }

    return displayString;
}

ConfrontationTopicSelectionActionEditorDialogContents::ConfrontationTopicSelectionActionEditorDialogContents(const Conversation::State &stateBeforeObject, Confrontation::ConfrontationTopicSelectionAction *pActionToEdit)
    : EditorDialogContents<Conversation::Action, Conversation::State>(stateBeforeObject, pActionToEdit)
{
    if (pActionToEdit != NULL)
    {
        pConfrontationTopicSelectionAction = static_cast<Confrontation::ConfrontationTopicSelectionAction *>(pActionToEdit->Clone());
    }
    else
    {
        pConfrontationTopicSelectionAction = new Confrontation::ConfrontationTopicSelectionAction();
    }

    pObject = pConfrontationTopicSelectionAction;

    QVBoxLayout *pLayout = new QVBoxLayout();

    QLabel *pHeaderLabel = new QLabel("Have the player select from the following confrontation topics:");
    pHeaderLabel->setWordWrap(true);
    pLayout->addWidget(pHeaderLabel);

    MultipleSelectionWidget<ConfrontationTopicMultipleSelectionSelectorWidget> *pTopicSelector = new MultipleSelectionWidget<ConfrontationTopicMultipleSelectionSelectorWidget>();

    if (pActionToEdit != NULL && pActionToEdit->topics.size() > 0)
    {
        QStringList strings;

        for (Confrontation::Topic *pTopic : pActionToEdit->topics)
        {
            strings.append(ConfrontationTopicMultipleSelectionSelectorWidget::CreateString(pTopic->GetId(), pTopic->GetName(), pTopic->GetIsEnabledAtStart()));
        }

        pTopicSelector->SetSelections(strings);
    }
    else
    {
        QStringList stringList;
        stringList.append("");

        pTopicSelector->SetSelections(stringList);
    }

    pLayout->addWidget(pTopicSelector);

    QCheckBox *pInitialTopicCheckBox = new QCheckBox("Include an initial topic");
    pLayout->addWidget(pInitialTopicCheckBox);

    if (pActionToEdit != NULL)
    {
        pInitialTopicCheckBox->setChecked(pActionToEdit->pInitialTopic != NULL);
    }

    pLayout->addStretch(1);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(pLayout);

    QObject::connect(pTopicSelector, SIGNAL(StringChanged(int,QString)), this, SLOT(TopicSelectorStringChanged(int,QString)));
    QObject::connect(pTopicSelector, SIGNAL(StringAdded(int,QString)), this, SLOT(TopicSelectorStringAdded(int,QString)));
    QObject::connect(pTopicSelector, SIGNAL(StringRemoved(int)), this, SLOT(TopicSelectorStringRemoved(int)));
    QObject::connect(pInitialTopicCheckBox, SIGNAL(toggled(bool)), this, SLOT(InitialTopicCheckBoxToggled(bool)));
}

bool ConfrontationTopicSelectionActionEditorDialogContents::ValidateFields()
{
    return true;
}

void ConfrontationTopicSelectionActionEditorDialogContents::TopicSelectorStringChanged(int index, const QString &string)
{
    QString id;
    QString name;
    bool isEnabledAtStart;

    ConfrontationTopicMultipleSelectionSelectorWidget::ParseString(string, &id, &name, &isEnabledAtStart);

    pConfrontationTopicSelectionAction->topics[index]->SetId(id);
    pConfrontationTopicSelectionAction->topics[index]->SetName(name);
    pConfrontationTopicSelectionAction->topics[index]->SetIsEnabledAtStart(isEnabledAtStart);
}

void ConfrontationTopicSelectionActionEditorDialogContents::TopicSelectorStringAdded(int index, const QString &string)
{
    QString id;
    QString name;
    bool isEnabledAtStart;

    ConfrontationTopicMultipleSelectionSelectorWidget::ParseString(string, &id, &name, &isEnabledAtStart);
    pConfrontationTopicSelectionAction->topics.insert(index, new Confrontation::Topic(id, name, isEnabledAtStart));
}

void ConfrontationTopicSelectionActionEditorDialogContents::TopicSelectorStringRemoved(int index)
{
    Confrontation::Topic *pRemovedTopic = pConfrontationTopicSelectionAction->topics[index];
    pConfrontationTopicSelectionAction->topics.removeAt(index);
    delete pRemovedTopic;
}

void ConfrontationTopicSelectionActionEditorDialogContents::InitialTopicCheckBoxToggled(bool checked)
{
    if (checked && pConfrontationTopicSelectionAction->pInitialTopic == NULL)
    {
        pConfrontationTopicSelectionAction->pInitialTopic = new Confrontation::Topic();
    }
    else if (!checked && pConfrontationTopicSelectionAction->pInitialTopic != NULL)
    {
        delete pConfrontationTopicSelectionAction->pInitialTopic;
        pConfrontationTopicSelectionAction->pInitialTopic = NULL;
    }
}

Confrontation::EnableTopicAction::EnableTopicAction(Staging::Confrontation::EnableTopicAction *pEnableTopicAction)
{
    topicId = pEnableTopicAction->TopicId;
}

EditorDialogContents<Conversation::Action, Conversation::State> * Confrontation::EnableTopicAction::CreateEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::Action *pActionToEdit)
{
    return new EnableTopicActionEditorDialogContents(stateBeforeObject, static_cast<Confrontation::EnableTopicAction *>(pActionToEdit));
}

void Confrontation::EnableTopicAction::CopyPropertiesImpl(Conversation::Action *pOther, bool /*isForEdit*/)
{
    Confrontation::EnableTopicAction *pAction = static_cast<Confrontation::EnableTopicAction *>(pOther);

    pAction->topicId = topicId;
}

QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> Confrontation::EnableTopicAction::GetListItemsImpl(int indentLevel)
{
    QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> editorList;
    editorList.append(new EnableTopicActionListItem(this, indentLevel));
    return editorList;
}

QString EnableTopicActionListItem::GetDisplayString()
{
    QString displayString = "Enable the confrontation topic ";
    displayString += UnderlineString(pAction->topicId);
    displayString += ".";

    return displayString;
}

EnableTopicActionEditorDialogContents::EnableTopicActionEditorDialogContents(const Conversation::State &stateBeforeObject, Confrontation::EnableTopicAction *pActionToEdit)
    : EditorDialogContents<Conversation::Action, Conversation::State>(stateBeforeObject, pActionToEdit)
{
    if (pActionToEdit != NULL)
    {
        pEnableTopicAction = static_cast<Confrontation::EnableTopicAction *>(pActionToEdit->Clone());
    }
    else
    {
        pEnableTopicAction = new Confrontation::EnableTopicAction();
    }

    pObject = pEnableTopicAction;

    QHBoxLayout *pLayout = new QHBoxLayout();

    pLayout->addWidget(new QLabel("Enable the confrontation topic "));

    QComboBox *pConfrontationTopicComboBox = new QComboBox();
    pLayout->addWidget(pConfrontationTopicComboBox);

    if (pActionToEdit != NULL)
    {
        pConfrontationTopicComboBox->addItem(pEnableTopicAction->topicId);
    }

    pLayout->addStretch(1);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(pLayout);

    if (pActionToEdit == NULL)
    {
        pEnableTopicAction->topicId = pConfrontationTopicComboBox->currentText();
    }

    QObject::connect(pConfrontationTopicComboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(ConfrontationTopicComboBoxCurrentTextChanged(QString)));
}

void EnableTopicActionEditorDialogContents::ConfrontationTopicComboBoxCurrentTextChanged(const QString &topicId)
{
    pEnableTopicAction->topicId = topicId;
}

bool EnableTopicActionEditorDialogContents::ValidateFields()
{
    return true;
}

Confrontation::RestartDecisionAction::~RestartDecisionAction()
{
    for (Action *pAction : restartActions)
    {
        delete pAction;
    }

    restartActions.clear();

    for (Action *pAction : quitActions)
    {
        delete pAction;
    }

    quitActions.clear();
}

EditorDialogContents<Conversation::Action, Conversation::State> * Confrontation::RestartDecisionAction::CreateEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::Action *pActionToEdit)
{
    return new RestartDecisionActionEditorDialogContents(stateBeforeObject, static_cast<Confrontation::RestartDecisionAction *>(pActionToEdit));
}

void Confrontation::RestartDecisionAction::CopyPropertiesImpl(Conversation::Action *pOther, bool /*isForEdit*/)
{
    Confrontation::RestartDecisionAction *pAction = static_cast<Confrontation::RestartDecisionAction *>(pOther);

    Conversation::Action::CloneActionList(pAction->restartActions, restartActions);
    Conversation::Action::CloneActionList(pAction->quitActions, quitActions);
}

void Confrontation::RestartDecisionAction::ExchangeListItemBaseOwnershipImpl(Action *pOther)
{
    Confrontation::RestartDecisionAction *pAction = static_cast<Confrontation::RestartDecisionAction *>(pOther);

    Conversation::Action::ExchangeListItemBaseOwnership(pAction->restartActions, restartActions);
    Conversation::Action::ExchangeListItemBaseOwnership(pAction->quitActions, quitActions);
}

void Confrontation::RestartDecisionAction::ReplaceAction(Conversation::Action *pNewAction, Conversation::Action *pOldAction)
{
    Conversation::ReplaceAction(pNewAction, pOldAction, restartActions);
    Conversation::ReplaceAction(pNewAction, pOldAction, quitActions);
}

void Confrontation::RestartDecisionAction::UpdateAndCacheConversationState(State &currentState)
{
    Conversation::Action::UpdateAndCacheConversationState(currentState);

    State tempState = currentState.Clone();

    for (Action *pAction : restartActions)
    {
        pAction->UpdateAndCacheConversationState(tempState);
    }

    tempState = currentState.Clone();

    for (Action *pAction : quitActions)
    {
        pAction->UpdateAndCacheConversationState(tempState);
    }
}

void Confrontation::RestartDecisionAction::PreloadAudio()
{
    Conversation::Action::PreloadAudio();

    for (Action *pAction : restartActions)
    {
        pAction->PreloadAudio();
    }

    for (Action *pAction : quitActions)
    {
        pAction->PreloadAudio();
    }
}

QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> Confrontation::RestartDecisionAction::GetListItemsImpl(int indentLevel)
{
    QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> editorList;

    editorList.append(new RestartDecisionActionListItem(this, indentLevel));
    editorList.append(new TextDisplay<Conversation::Action, Conversation::State, Conversation::ActionType>("If the player restarts the confrontation:", indentLevel + 1));
    editorList.append(GetListItemsForActions(restartActions, indentLevel + 2));
    editorList.append(new TextDisplay<Conversation::Action, Conversation::State, Conversation::ActionType>("If the player quits the game:", indentLevel + 1));
    editorList.append(GetListItemsForActions(quitActions, indentLevel + 2));

    return editorList;
}

QString RestartDecisionActionListItem::GetDisplayString()
{
    QString displayString = "Prompt the player to either restart the confrontation or quit the game.";

    return displayString;
}

RestartDecisionActionEditorDialogContents::RestartDecisionActionEditorDialogContents(const Conversation::State &stateBeforeObject, Confrontation::RestartDecisionAction *pActionToEdit)
    : EditorDialogContents<Conversation::Action, Conversation::State>(stateBeforeObject, pActionToEdit)
{
    if (pActionToEdit != NULL)
    {
        pRestartDecisionAction = static_cast<Confrontation::RestartDecisionAction *>(pActionToEdit->Clone());
    }
    else
    {
        pRestartDecisionAction = new Confrontation::RestartDecisionAction();
    }

    pObject = pRestartDecisionAction;

    QHBoxLayout *pLayout = new QHBoxLayout();

    pLayout->addWidget(new QLabel("Prompt the player to either restart the confrontation or quit the game."));

    pLayout->addStretch(1);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(pLayout);
}

bool RestartDecisionActionEditorDialogContents::ValidateFields()
{
    return true;
}

EditorDialogContents<Conversation::Action, Conversation::State> * Confrontation::RestartConfrontationAction::CreateEditorDialogContents(const Conversation::State &stateBeforeObject, Conversation::Action *pActionToEdit)
{
    return new RestartConfrontationActionEditorDialogContents(stateBeforeObject, static_cast<Confrontation::RestartConfrontationAction *>(pActionToEdit));
}

void Confrontation::RestartConfrontationAction::CopyPropertiesImpl(Action */*pOther*/, bool /*isForEdit*/)
{
}

QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> Confrontation::RestartConfrontationAction::GetListItemsImpl(int indentLevel)
{
    QList<ListItemBase<Conversation::Action, Conversation::State, Conversation::ActionType> *> editorList;
    editorList.append(new RestartConfrontationActionListItem(this, indentLevel));
    return editorList;
}

QString RestartConfrontationActionListItem::GetDisplayString()
{
    QString displayString = "Restart the current confrontation.";

    return displayString;
}

RestartConfrontationActionEditorDialogContents::RestartConfrontationActionEditorDialogContents(const Conversation::State &stateBeforeObject, Confrontation::RestartConfrontationAction *pActionToEdit)
    : EditorDialogContents<Conversation::Action, Conversation::State>(stateBeforeObject, pActionToEdit)
{
    if (pActionToEdit != NULL)
    {
        pRestartConfrontationAction = static_cast<Confrontation::RestartConfrontationAction *>(pActionToEdit->Clone());
    }
    else
    {
        pRestartConfrontationAction = new Confrontation::RestartConfrontationAction();
    }

    pObject = pRestartConfrontationAction;

    QHBoxLayout *pLayout = new QHBoxLayout();

    pLayout->addWidget(new QLabel("Restart the current confrontation."));

    pLayout->addStretch(1);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(pLayout);
}

bool RestartConfrontationActionEditorDialogContents::ValidateFields()
{
    return true;
}

ConversationSelector::ConversationSelector(QWidget *parent)
    : QComboBox(parent)
{
    QObject::connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(CurrentIndexChanged(int)));
}

QString ConversationSelector::GetId()
{
    return currentText();
}

void ConversationSelector::SetToId(const QString &id)
{
    int indexOfCurrentConversation = conversationIds.indexOf(QRegExp(id, Qt::CaseInsensitive));

    if (indexOfCurrentConversation >= 0)
    {
        setCurrentIndex(indexOfCurrentConversation);
    }
    else
    {
        setCurrentIndex(0);
    }
}

void ConversationSelector::Reset(Encounter *pParentEncounter)
{
    conversationIds = CaseContent::GetInstance()->GetIds<Conversation>(pParentEncounter);
    previousIndex = -1;

    clear();
    addItems(conversationIds);
}

void ConversationSelector::CurrentIndexChanged(int currentIndex)
{
    previousIndex = currentIndex;

    if (currentIndex >= 0)
    {
        emit ConversationSelected(conversationIds[currentIndex]);
    }
    else
    {
        emit ConversationSelected("");
    }
}

namespace ConversationPrivate
{
    QMap<QString, Conversation::ActionType> ActionTypeByComboBoxStringMap
    {
        { "Change character...", Conversation::ActionType::CharacterChange },
        { "Show dialog...", Conversation::ActionType::ShowDialog },
        { "Require the player to present evidence...", Conversation::ActionType::MustPresentEvidence },
        { "Set flag...", Conversation::ActionType::SetFlag },
        { "Branch on condition...", Conversation::ActionType::BranchOnCondition },
        { "Enable conversation...", Conversation::ActionType::EnableConversation },
        { "Enable evidence...", Conversation::ActionType::EnableEvidence },
        { "Update evidence...", Conversation::ActionType::UpdateEvidence },
        { "Disable evidence...", Conversation::ActionType::DisableEvidence },
        { "Enable cutscene...", Conversation::ActionType::EnableCutscene },
        { "Play background music...", Conversation::ActionType::PlayBgm },
        { "Pause background music...", Conversation::ActionType::PauseBgm },
        { "Resume background music...", Conversation::ActionType::ResumeBgm },
        { "Stop background music...", Conversation::ActionType::StopBgm },
        { "Play ambiance...", Conversation::ActionType::PlayAmbiance },
        { "Pause ambiance...", Conversation::ActionType::PauseAmbiance },
        { "Resume ambiance...", Conversation::ActionType::ResumeAmbiance },
        { "Stop ambiance...", Conversation::ActionType::StopAmbiance },
        { "Start animation...", Conversation::ActionType::StartAnimation },
        { "Stop animation...", Conversation::ActionType::StopAnimation },
        { "Set partner...", Conversation::ActionType::SetPartner },
        { "Go to the present-wrong-evidence branch...", Conversation::ActionType::GoToPresentWrongEvidence },
        { "Lock this conversation...", Conversation::ActionType::LockConversation },
        { "Exit this encounter...", Conversation::ActionType::ExitEncounter },
        { "Move to location...", Conversation::ActionType::MoveToLocation },
        { "Move to zoomed view...", Conversation::ActionType::MoveToZoomedView },
        { "End the current case...", Conversation::ActionType::EndCase },
        { "Display multiple choice options...", Conversation::ActionType::MultipleChoice },
        { "Exit multiple choice loop...", Conversation::ActionType::ExitMultipleChoice },
        { "Enable fast-forwarding...", Conversation::ActionType::EnableFastForward },
        { "Disable fast-forwarding...", Conversation::ActionType::DisableFastForward },
        { "Begin opponent's breakdown...", Conversation::ActionType::BeginBreakdown },
        { "End opponent's breakdown...", Conversation::ActionType::EndBreakdown },
        { "Repeat the following interrogation lines...", Conversation::ActionType::InterrogationRepeat },
        { "Show interrogation line...", Conversation::ActionType::ShowInterrogation },
        { "Exit the interrogation loop...", Conversation::ActionType::ExitInterrogationRepeat },
        { "Display the following confrontation topics...", Conversation::ActionType::ConfrontationTopicSelection },
        { "Enable confrontation topic...", Conversation::ActionType::EnableTopic },
        { "Ask the player whether they want to restart the confrontation...", Conversation::ActionType::RestartDecision },
        { "Restart the confrontation...", Conversation::ActionType::RestartConfrontation },
    };

    typedef EditorDialogContents<Conversation::Action, Conversation::State> * (*PFNEDITORDIALOGCONTENTSCREATE)(const Conversation::State &, Conversation::Action *);

    QMap<Conversation::ActionType, PFNEDITORDIALOGCONTENTSCREATE> EditorDialogContentsCreationMethodsByTypeMap
    {
        { Conversation::ActionType::CharacterChange, Conversation::CharacterChangeAction::CreateEditorDialogContents },
        { Conversation::ActionType::SetFlag, Conversation::SetFlagAction::CreateEditorDialogContents },
        { Conversation::ActionType::BranchOnCondition, Conversation::BranchOnConditionAction::CreateEditorDialogContents },
        { Conversation::ActionType::ShowDialog, Conversation::ShowDialogAction::CreateEditorDialogContents },
        { Conversation::ActionType::MustPresentEvidence, Conversation::MustPresentEvidenceAction::CreateEditorDialogContents },
        { Conversation::ActionType::EnableConversation, Conversation::EnableConversationAction::CreateEditorDialogContents },
        { Conversation::ActionType::EnableEvidence, Conversation::EnableEvidenceAction::CreateEditorDialogContents },
        { Conversation::ActionType::UpdateEvidence, Conversation::UpdateEvidenceAction::CreateEditorDialogContents },
        { Conversation::ActionType::DisableEvidence, Conversation::DisableEvidenceAction::CreateEditorDialogContents },
        { Conversation::ActionType::EnableCutscene, Conversation::EnableCutsceneAction::CreateEditorDialogContents },
        { Conversation::ActionType::PlayBgm, Conversation::PlayBgmAction::CreateEditorDialogContents },
        { Conversation::ActionType::PauseBgm, Conversation::PauseBgmAction::CreateEditorDialogContents },
        { Conversation::ActionType::ResumeBgm, Conversation::ResumeBgmAction::CreateEditorDialogContents },
        { Conversation::ActionType::StopBgm, Conversation::StopBgmAction::CreateEditorDialogContents },
        { Conversation::ActionType::PlayAmbiance, Conversation::PlayAmbianceAction::CreateEditorDialogContents },
        { Conversation::ActionType::PauseAmbiance, Conversation::PauseAmbianceAction::CreateEditorDialogContents },
        { Conversation::ActionType::ResumeAmbiance, Conversation::ResumeAmbianceAction::CreateEditorDialogContents },
        { Conversation::ActionType::StopAmbiance, Conversation::StopAmbianceAction::CreateEditorDialogContents },
        { Conversation::ActionType::StartAnimation, Conversation::StartAnimationAction::CreateEditorDialogContents },
        { Conversation::ActionType::StopAnimation, Conversation::StopAnimationAction::CreateEditorDialogContents },
        { Conversation::ActionType::SetPartner, Conversation::SetPartnerAction::CreateEditorDialogContents },
        { Conversation::ActionType::GoToPresentWrongEvidence, Conversation::GoToPresentWrongEvidenceAction::CreateEditorDialogContents },
        { Conversation::ActionType::LockConversation, Conversation::LockConversationAction::CreateEditorDialogContents },
        { Conversation::ActionType::ExitEncounter, Conversation::ExitEncounterAction::CreateEditorDialogContents },
        { Conversation::ActionType::MoveToLocation, Conversation::MoveToLocationAction::CreateEditorDialogContents },
        { Conversation::ActionType::MoveToZoomedView, Conversation::MoveToZoomedViewAction::CreateEditorDialogContents },
        { Conversation::ActionType::EndCase, Conversation::EndCaseAction::CreateEditorDialogContents },
        { Conversation::ActionType::MultipleChoice, Conversation::MultipleChoiceAction::CreateEditorDialogContents },
        { Conversation::ActionType::ExitMultipleChoice, Conversation::ExitMultipleChoiceAction::CreateEditorDialogContents },
        { Conversation::ActionType::EnableFastForward, Conversation::EnableFastForwardAction::CreateEditorDialogContents },
        { Conversation::ActionType::DisableFastForward, Conversation::DisableFastForwardAction::CreateEditorDialogContents },
        { Conversation::ActionType::BeginBreakdown, Conversation::BeginBreakdownAction::CreateEditorDialogContents },
        { Conversation::ActionType::EndBreakdown, Conversation::EndBreakdownAction::CreateEditorDialogContents },
        { Conversation::ActionType::InterrogationRepeat, Interrogation::InterrogationRepeatAction::CreateEditorDialogContents },
        { Conversation::ActionType::ShowInterrogation, Interrogation::ShowInterrogationAction::CreateEditorDialogContents },
        { Conversation::ActionType::ExitInterrogationRepeat, Interrogation::ExitInterrogationRepeatAction::CreateEditorDialogContents },
        { Conversation::ActionType::ConfrontationTopicSelection, Confrontation::ConfrontationTopicSelectionAction::CreateEditorDialogContents },
        { Conversation::ActionType::EnableTopic, Confrontation::EnableTopicAction::CreateEditorDialogContents },
        { Conversation::ActionType::RestartDecision, Confrontation::RestartDecisionAction::CreateEditorDialogContents },
        { Conversation::ActionType::RestartConfrontation, Confrontation::RestartConfrontationAction::CreateEditorDialogContents },
    };
}

template <>
QString GetObjectDisplayName<Conversation::Action>()
{
    return "conversation action";
}

template <>
QMap<QString, Conversation::ActionType> GetTypeByComboBoxStringMap<Conversation::Action, Conversation::ActionType>()
{
    return ConversationPrivate::ActionTypeByComboBoxStringMap;
}

template <>
QMap<Conversation::ActionType, ConversationPrivate::PFNEDITORDIALOGCONTENTSCREATE> GetEditorDialogContentsCreationMethodsByTypeMap<Conversation::Action, Conversation::State, Conversation::ActionType>()
{
    return ConversationPrivate::EditorDialogContentsCreationMethodsByTypeMap;
}
