#include "Animation.h"

#include "XmlReader.h"
#include "XmlWriter.h"

#include "Staging/CaseContentLoadingStager.h"

Animation::Animation(Staging::Animation *pStagingAnimation)
{
    for (Staging::Animation::Frame *pFrame : pStagingAnimation->FrameList)
    {
        frameList.push_back(new Animation::Frame(pFrame));
    }
}

Animation::~Animation()
{
    for (Animation::Frame *pFrame : frameList)
    {
        delete pFrame;
    }

    frameList.clear();
}

void Animation::AddFrame(int msDuration, const QString &spriteFilePath)
{
    frameList.push_back(new Animation::Frame(msDuration, spriteFilePath));
}

void Animation::SetFrame(int index, QString filePath)
{
    frameList[index]->SetSpriteFilePath(filePath);
}

void Animation::SetFrames(QStringList filePaths)
{
    for (Animation::Frame *pFrame : frameList)
    {
        delete pFrame;
    }

    frameList.clear();

    for (QString filePath : filePaths)
    {
        frameList.push_back(new Animation::Frame(30, filePath));
    }
}

void Animation::InsertFrame(int index, QString filePath)
{
    frameList.insert(index, new Animation::Frame(30, filePath));
}

void Animation::RemoveFrame(int index)
{
    Animation::Frame *pRemovedFrame = frameList[index];
    frameList.removeAt(index);
    delete pRemovedFrame;
}

QStringList Animation::GetFramePaths()
{
    QStringList framePaths;

    for (Animation::Frame *pFrame : frameList)
    {
        framePaths.append(pFrame->GetSpriteFilePath());
    }

    return framePaths;
}

Animation::Frame::Frame(Staging::Animation::Frame *pStagingAnimationFrame)
{
    msDuration = pStagingAnimationFrame->MsDuration;
    spriteFilePath = CaseContentLoadingStager::GetCurrent()->GetSpriteFilePathFromId(pStagingAnimationFrame->SpriteId);
}

Animation::Frame::~Frame()
{

}

Animation::Frame::DrawingView::DrawingView(Animation::Frame *pFrame, IDrawable *pParent)
    : IDrawable(pParent)
    , IDrawingView<Animation::Frame>(pFrame, pParent)
{
    spritePixmap = QPixmap(pObject->spriteFilePath);
}

Animation::Frame::DrawingView::~DrawingView()
{
    spritePixmap = QPixmap();
}

Animation::DrawingView::DrawingView(Animation *pAnimation, IDrawable *pParent)
    : IDrawable(pParent)
    , IDrawingView<Animation>(pAnimation, pParent)
{
    for (Animation::Frame *pFrame : pObject->frameList)
    {
        frameList.push_back(pFrame->GetDrawingView(this));
    }

    currentIndex = 0;
    msTimeoutDuration = frameList[currentIndex]->GetTimeoutDuration();
    positionIsSet = false;
    opacity = 1.0;
    flipHorizontalIsSet = false;
    flipHorizontal = false;
}

Animation::DrawingView::~DrawingView()
{
    for (Animation::Frame::DrawingView *pFrame : frameList)
    {
        delete pFrame;
    }

    frameList.clear();
}

void Animation::DrawingView::DrawCore(QGraphicsScene *pScene, QList<QGraphicsItem *> &addedItems)
{
    frameGraphicsItemList.clear();

    for (Animation::Frame::DrawingView *pFrame : frameList)
    {
        QGraphicsItem *pFrameGraphicsItem = pScene->addPixmap(pFrame->GetSpritePixmap());

        frameGraphicsItemList.push_back(pFrameGraphicsItem);
        addedItems.push_back(pFrameGraphicsItem);
    }
}

void Animation::DrawingView::UpdateCore()
{
    // The way animations work is that we add graphics items for every frame
    // and just hide all of the frames that aren't the current frame.
    // This makes it so we don't need to keep adding and removing graphics items
    // from the scene every time we need to move to a new frame.
    for (int i = 0; i < min(frameList.count(), frameGraphicsItemList.count()); i++)
    {
        if (i != currentIndex)
        {
            frameGraphicsItemList[i]->setOpacity(0.0);
        }
    }
}

void Animation::DrawingView::UpdateOnTimeout()
{
    currentIndex = (currentIndex + 1) % frameList.count();
    msTimeoutDuration = frameList[currentIndex]->GetTimeoutDuration();
}

Vector2 Animation::DrawingView::GetPosition()
{
    if (positionIsSet)
    {
        return position;
    }
    else
    {
        return IDrawingView<Animation>::GetPosition();
    }
}

void Animation::DrawingView::SetPosition(Vector2 position)
{
    positionIsSet = true;
    this->position = position;
}

qreal Animation::DrawingView::GetOpacity()
{
    qreal netOpacity = opacity;

    if (pParent != NULL)
    {
        netOpacity *= pParent->GetOpacity();
    }

    return netOpacity;
}

void Animation::DrawingView::SetOpacity(qreal opacity)
{
    this->opacity = opacity;
    Update();
}

bool Animation::DrawingView::GetFlipHorizontal()
{
    if (flipHorizontalIsSet)
    {
        return flipHorizontal;
    }
    else
    {
        return IDrawingView<Animation>::GetFlipHorizontal();
    }
}

void Animation::DrawingView::SetFlipHorizontal(bool flipHorizontal)
{
    flipHorizontalIsSet = true;
    this->flipHorizontal = flipHorizontal;
}

Animation::CompositeDrawingView::CompositeDrawingView(IDrawable *pParent)
    : IDrawable(pParent)
{
    currentIndex = 0;
    msTimeoutDuration = 0;
    positionIsSet = false;
    opacity = 1.0;

    flipHorizontalByAnimationIndexMap.clear();
    animationStartIndexes.clear();
}

Animation::CompositeDrawingView::~CompositeDrawingView()
{
    for (Animation::Frame::DrawingView *pFrame : frameList)
    {
        delete pFrame;
    }

    frameList.clear();
}

void Animation::CompositeDrawingView::AddAnimation(Animation *pAnimation, bool flipHorizontal)
{
    flipHorizontalByAnimationIndexMap.insert(animationStartIndexes.size(), flipHorizontal);
    animationStartIndexes.append(frameList.size());

    for (Animation::Frame *pFrame : pAnimation->frameList)
    {
        Animation::Frame::DrawingView *pFrameDrawingView = pFrame->GetDrawingView(this);
        frameList.push_back(pFrameDrawingView);
    }

    if (frameList.size() > 0 && msTimeoutDuration == 0)
    {
        msTimeoutDuration = frameList[0]->GetTimeoutDuration();
    }
}

void Animation::CompositeDrawingView::ReplaceAnimation(int animationIndex, Animation *pAnimation, bool flipHorizontal)
{
    if (animationIndex >= animationStartIndexes.size())
    {
        throw new MLIException("Trying to replace an animation that doesn't exist.");
    }

    int startIndex = animationStartIndexes[animationIndex];
    int endIndex = animationIndex + 1 == animationStartIndexes.size() ? frameList.size() : animationStartIndexes[animationIndex + 1];

    for (int i = endIndex - 1; i >= startIndex; i--)
    {
        Animation::Frame::DrawingView *pFrameDrawingView = frameList[i];
        frameList.removeAt(i);
        delete pFrameDrawingView;
    }

    flipHorizontalByAnimationIndexMap.remove(animationIndex);

    int currentIndex = 0;

    for (Animation::Frame *pFrame : pAnimation->frameList)
    {
        Animation::Frame::DrawingView *pFrameDrawingView = pFrame->GetDrawingView(this);
        frameList.push_back(pFrameDrawingView);

        currentIndex++;
    }

    flipHorizontalByAnimationIndexMap.insert(animationIndex, flipHorizontal);

    int indexDelta = currentIndex - (endIndex - startIndex);

    for (int i = animationIndex + 1; i < animationStartIndexes.size(); i++)
    {
        animationStartIndexes[i] += indexDelta;
    }

    if (frameList.size() > 0 && msTimeoutDuration == 0)
    {
        msTimeoutDuration = frameList[0]->GetTimeoutDuration();
    }

    Redraw();
}

void Animation::CompositeDrawingView::DrawCore(QGraphicsScene *pScene, QList<QGraphicsItem *> &addedItems)
{
    frameGraphicsItemList.clear();

    for (Animation::Frame::DrawingView *pFrame : frameList)
    {
        QGraphicsItem *pFrameGraphicsItem = pScene->addPixmap(pFrame->GetSpritePixmap());

        frameGraphicsItemList.push_back(pFrameGraphicsItem);
        addedItems.push_back(pFrameGraphicsItem);
    }
}

void Animation::CompositeDrawingView::UpdateCore()
{
    // The way animations work is that we add graphics items for every frame
    // and just hide all of the frames that aren't the current frame.
    // This makes it so we don't need to keep adding and removing graphics items
    // from the scene every time we need to move to a new frame.
    for (int i = 0; i < min(frameList.count(), frameGraphicsItemList.count()); i++)
    {
        if (i != currentIndex)
        {
            frameGraphicsItemList[i]->setOpacity(0.0);
        }
    }
}

void Animation::CompositeDrawingView::UpdateOnTimeout()
{
    currentIndex = (currentIndex + 1) % frameList.count();
    msTimeoutDuration = frameList[currentIndex]->GetTimeoutDuration();
}

Vector2 Animation::CompositeDrawingView::GetPosition()
{
    if (positionIsSet)
    {
        return position;
    }
    else
    {
        return IDrawable::GetPosition();
    }
}

void Animation::CompositeDrawingView::SetPosition(Vector2 position)
{
    positionIsSet = true;
    this->position = position;
}

qreal Animation::CompositeDrawingView::GetOpacity()
{
    qreal netOpacity = opacity;

    if (pParent != NULL)
    {
        netOpacity *= pParent->GetOpacity();
    }

    return netOpacity;
}

void Animation::CompositeDrawingView::SetOpacity(qreal opacity)
{
    this->opacity = opacity;
    Update();
}

bool Animation::CompositeDrawingView::GetFlipHorizontal()
{
    int animationIndex = 0;

    while (animationIndex + 1 < animationStartIndexes.size() &&
           currentIndex >= animationStartIndexes[animationIndex + 1])
    {
        animationIndex++;
    }

    return flipHorizontalByAnimationIndexMap[animationIndex];
}
