#ifndef ANIMATION_H
#define ANIMATION_H

#include "CaseCreator/Utilities/Interfaces.h"
#include "XmlStorableObject.h"

#include "Staging/Animation.Staging.h"

#include <QObject>
#include <QString>
#include <QList>
#include <QPixmap>

class XmlReader;
class XmlWriter;

class Animation : public XmlStorableObject
{
    BEGIN_XML_STORABLE_OBJECT(Animation)
        XML_STORABLE_CUSTOM_OBJECT_LIST(frameList, Animation::Frame::CreateFromXml)
    END_XML_STORABLE_OBJECT()

public:
    Animation() { }
    Animation(Staging::Animation *pStagingAnimation);
    virtual ~Animation();

    static Animation * CreateFromXml(XmlReader *pReader)
    {
        return new Animation(pReader);
    }

    void AddFrame(int msDuration, const QString &spriteFilePath);
    void SetFrame(int index, QString filePath);
    void SetFrames(QStringList filePaths);
    void InsertFrame(int index, QString filePath);
    void RemoveFrame(int index);

    QStringList GetFramePaths();

    class Frame : public XmlStorableObject
    {
        friend class Animation;

        BEGIN_XML_STORABLE_OBJECT(Frame)
            XML_STORABLE_INT(msDuration)
            XML_STORABLE_FILE_PATH(spriteFilePath)
        END_XML_STORABLE_OBJECT()

    public:
        Frame() { }

        Frame(int msDuration, const QString &spriteFilePath)
        {
            this->msDuration = msDuration;
            this->spriteFilePath = spriteFilePath;
        }

        Frame(Staging::Animation::Frame *pStagingAnimationFrame);
        virtual ~Frame();

        static Frame * CreateFromXml(XmlReader *pReader)
        {
            return new Frame(pReader);
        }

        bool GetIsForever() const { return msDuration == 0; }

        QString GetSpriteFilePath() { return spriteFilePath; }
        void SetSpriteFilePath(const QString &filePath) { spriteFilePath = filePath; }

        class DrawingView : public IDrawingView<Animation::Frame>
        {
            friend class Animation::Frame;

        public:
            DrawingView(Animation::Frame *pFrame, IDrawable *pParent);
            virtual ~DrawingView();

            void DrawCore(QGraphicsScene *, QList<QGraphicsItem *> &)
            {
                // We should not be drawing anything in the frame -
                // instead, the animation should be handling that.
                throw new MLIException("Animation::Frame::DrawingView::Draw() should never be called.  Instead, GetSpritePixmap() should be called in Animation::DrawingView::Draw().");
            }

            QPixmap & GetSpritePixmap() { return spritePixmap; }
            int GetTimeoutDuration() { return pObject->msDuration; }

        private:
            QPixmap spritePixmap;
        };

        GET_DRAWING_VIEW_DEFINITION(Animation::Frame)

    private:
        int msDuration;
        QString spriteFilePath;
    };

    class DrawingView : public IDrawingView<Animation>
    {
        friend class Animation;

    public:
        DrawingView(Animation *pAnimation, IDrawable *pParent);
        ~DrawingView();

        void DrawCore(QGraphicsScene *pScene, QList<QGraphicsItem *> &addedItems);
        void UpdateCore();
        void UpdateOnTimeout();

        Vector2 GetPosition() override;
        void SetPosition(Vector2 position);

        qreal GetOpacity() override;
        void SetOpacity(qreal opacity);

        bool GetFlipHorizontal() override;
        void SetFlipHorizontal(bool flipHorizontal);

        RectangleWH GetBoundingRect()
        {
            return
                (frameList.empty() ||
                frameList[0]->GetSpritePixmap().width() <= 0 ||
                frameList[0]->GetSpritePixmap().height() <= 0) ?
                    IDrawable::GetBoundingRect() :
                    RectangleWH(0, 0, frameList[0]->GetSpritePixmap().width(), frameList[0]->GetSpritePixmap().height());
        }

    private:
        QList<Animation::Frame::DrawingView *> frameList;
        QList<QGraphicsItem *> frameGraphicsItemList;

        int currentIndex;
        bool positionIsSet;
        Vector2 position;
        qreal opacity;
        bool flipHorizontalIsSet;
        bool flipHorizontal;
    };

    GET_DRAWING_VIEW_DEFINITION(Animation)

    class CompositeDrawingView : public IDrawable
    {
        friend class Animation;

    public:
        CompositeDrawingView(IDrawable *pParent);
        ~CompositeDrawingView();

        void AddAnimation(Animation *pAnimation, bool flipHorizontal);
        void ReplaceAnimation(int animationIndex, Animation *pAnimation, bool flipHorizontal);

        void DrawCore(QGraphicsScene *pScene, QList<QGraphicsItem *> &addedItems);
        void UpdateCore();
        void UpdateOnTimeout();

        Vector2 GetPosition() override;
        void SetPosition(Vector2 position);

        qreal GetOpacity() override;
        void SetOpacity(qreal opacity);

        bool GetFlipHorizontal() override;

        RectangleWH GetBoundingRect()
        {
            return
                (frameList.empty() ||
                frameList[0]->GetSpritePixmap().width() <= 0 ||
                frameList[0]->GetSpritePixmap().height() <= 0) ?
                    IDrawable::GetBoundingRect() :
                    RectangleWH(0, 0, frameList[0]->GetSpritePixmap().width(), frameList[0]->GetSpritePixmap().height());
        }

    private:
        QList<Animation::Frame::DrawingView *> frameList;
        QList<QGraphicsItem *> frameGraphicsItemList;

        int currentIndex;
        bool positionIsSet;
        Vector2 position;
        qreal opacity;

        QMap<int, bool> flipHorizontalByAnimationIndexMap;
        QList<int> animationStartIndexes;
    };

private:
    QList<Animation::Frame *> frameList;
};

#endif // ANIMATION_H
