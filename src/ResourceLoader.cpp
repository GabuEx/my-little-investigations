/**
 * Classes and functions having to do with collision detection.
 *
 * @author GabuEx, dawnmew
 * @since 1.0
 *
 * Licensed under the MIT License.
 *
 * Copyright (c) 2014 Equestrian Dreamers
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "XmlReader.h"
#include "MLIException.h"
#include "ResourceLoader.h"
#include "mli_audio.h"
#include "CaseInformation/Case.h"
#include <cryptopp/sha.h>

ResourceLoader * ResourceLoader::pInstance = NULL;

void ResourceLoader::LoadImageStep::Execute()
{
    Case::GetInstance()->GetSpriteManager()->LoadImageFromFilePath(spriteId);
}

void ResourceLoader::DeleteImageStep::Execute()
{
    Case::GetInstance()->GetSpriteManager()->DeleteImage(spriteId);
}

void ResourceLoader::LoadVideoStep::Execute()
{
    pVideo->LoadFile();
}

void ResourceLoader::DeleteVideoStep::Execute()
{
    pVideo->UnloadFile();
}

void ResourceLoader::Close()
{
    delete pInstance;
    pInstance = NULL;
}

bool ResourceLoader::Init(const string &commonResourcesFilePath)
{
    ArchiveSource *pCommonResourcesSource = NULL;

    if (!ArchiveSource::CreateAndInit(commonResourcesFilePath, &pCommonResourcesSource))
    {
        return false;
    }

    this->pCommonResourcesSource = pCommonResourcesSource;
    return true;
}

bool ResourceLoader::LoadCase(const string &caseFilePath)
{
    bool retVal;

    SDL_SemWait(pLoadingSemaphore);
    delete pCaseResourcesSource;
    retVal = ArchiveSource::CreateAndInit(caseFilePath, &pCaseResourcesSource);
    SDL_SemPost(pLoadingSemaphore);

    return retVal;
}

bool ResourceLoader::LoadTemporaryCase(const string &caseFilePath)
{
    pCachedCaseResourcesSource = pCaseResourcesSource;
    pCaseResourcesSource = NULL;

    bool retVal = LoadCase(caseFilePath);

    if (!retVal)
    {
        UnloadTemporaryCase();
    }

    return retVal;
}

void ResourceLoader::UnloadTemporaryCase()
{
    delete pCaseResourcesSource;
    pCaseResourcesSource = pCachedCaseResourcesSource;
    pCachedCaseResourcesSource = NULL;
}

void ResourceLoader::UnloadCase()
{
    SDL_SemWait(pLoadingSemaphore);
    delete pCaseResourcesSource;
    pCaseResourcesSource = NULL;
    SDL_SemPost(pLoadingSemaphore);
}

SDL_Surface * ResourceLoader::LoadRawSurface(const string &relativeFilePath)
{
    void *pMemToFree = NULL;
    SDL_RWops * pRW = NULL;

    SDL_SemWait(pLoadingSemaphore);
    pRW = pCommonResourcesSource->LoadFile(relativeFilePath,&pMemToFree);
    SDL_SemPost(pLoadingSemaphore);

    if(pRW==NULL) return NULL;
    SDL_Surface * pSurface = IMG_Load_RW(pRW,1);
    free(pMemToFree);
    return pSurface;
}

Image * ResourceLoader::LoadImage(const string &relativeFilePath)
{
    SDL_RWops *pRW = NULL;
    void *pMemToFree = NULL;

    SDL_SemWait(pLoadingSemaphore);
    pRW = pCommonResourcesSource->LoadFile(relativeFilePath, &pMemToFree);

    if (pRW == NULL && pCaseResourcesSource != NULL)
    {
        pRW = pCaseResourcesSource->LoadFile(relativeFilePath, &pMemToFree);
    }
    SDL_SemPost(pLoadingSemaphore);

    if (pRW == NULL)
    {
        return NULL;
    }

    Image *pSprite = Image::Load(pRW);
    pSprite->FlagResourceLoaderSource(relativeFilePath);
    free(pMemToFree);
    return pSprite;
}

void ResourceLoader::ReloadImage(Image *pSprite, const string &originFilePath)
{
    SDL_RWops *pRW = NULL;
    void *pMemToFree = NULL;

    SDL_SemWait(pLoadingSemaphore);
    pRW = pCommonResourcesSource->LoadFile(originFilePath, &pMemToFree);

    if (pRW == NULL && pCaseResourcesSource != NULL)
    {
        pRW = pCaseResourcesSource->LoadFile(originFilePath, &pMemToFree);
    }
    SDL_SemPost(pLoadingSemaphore);

    if (pRW == NULL)
    {
        pSprite->UnloadTextures();
    }
    else
    {
        pSprite->Reload(pRW, false /* loadImmediately */);
    }

    free(pMemToFree);
}

tinyxml2::XMLDocument * ResourceLoader::LoadDocument(const string &relativeFilePath)
{
    void *pMemToFree = NULL;
    SDL_RWops * pRW = NULL;

    if (pLoadingSemaphore != NULL)
    {
        SDL_SemWait(pLoadingSemaphore);
        pRW = pCommonResourcesSource->LoadFile(relativeFilePath, &pMemToFree);

        if (pRW == NULL && pCaseResourcesSource != NULL)
        {
            pRW = pCaseResourcesSource->LoadFile(relativeFilePath, &pMemToFree);
        }
        SDL_SemPost(pLoadingSemaphore);
    }

    if (pRW == NULL) return NULL;
    tinyxml2::XMLDocument * pDocument = new tinyxml2::XMLDocument();
    pDocument->LoadFile(pRW);
    SDL_RWclose(pRW);
    free(pMemToFree);
    return pDocument;
}

TTF_Font * ResourceLoader::LoadFont(const string &relativeFilePath, int ptSize, void **ppMemToFree)
{
    *ppMemToFree = NULL;
    SDL_RWops * pRW = NULL;

    SDL_SemWait(pLoadingSemaphore);
    pRW = pCommonResourcesSource->LoadFile(relativeFilePath, ppMemToFree);

    if (pRW == NULL && pCaseResourcesSource != NULL)
    {
        pRW = pCaseResourcesSource->LoadFile(relativeFilePath, ppMemToFree);
    }
    SDL_SemPost(pLoadingSemaphore);
    if (pRW == NULL) return NULL;
    TTF_Font *pFont = TTF_OpenFontRW(pRW, 1, ptSize);
    return pFont;
}

void ResourceLoader::LoadVideo(
    const string &relativeFilePath,
    RWOpsIOContext **ppRWOpsIOContext,
    AVFormatContext **ppFormatContext,
    int *pVideoStream,
    AVCodecContext **ppCodecContext,
    AVCodec **ppCodec,
    void **ppMemToFree)
{
    *ppRWOpsIOContext = NULL;
    *ppFormatContext = NULL;
    *pVideoStream = -1;
    *ppCodecContext = NULL;
    *ppCodec = NULL;
    *ppMemToFree = NULL;

    void *pMemToFree = NULL;
    SDL_RWops *pRW = NULL;

    SDL_SemWait(pLoadingSemaphore);
    pRW = pCommonResourcesSource->LoadFile(relativeFilePath, &pMemToFree);

    if (pRW == NULL && pCaseResourcesSource != NULL)
    {
        pRW = pCaseResourcesSource->LoadFile(relativeFilePath, &pMemToFree);
    }
    SDL_SemPost(pLoadingSemaphore);

    if (pRW == NULL) return;

    RWOpsIOContext *pRWOpsIOContext = new RWOpsIOContext(pRW);
    AVFormatContext *pFormatContext = avformat_alloc_context();
    pFormatContext->pb = pRWOpsIOContext->GetAVIOContext();
    pFormatContext->flags = AVFMT_FLAG_CUSTOM_IO;

    // At this point we should determine the input format.
    unsigned char *pBuffer = reinterpret_cast<unsigned char *>(av_malloc(IOContextBufferSize + AVPROBE_PADDING_SIZE));
    pRWOpsIOContext->Read(pRWOpsIOContext, pBuffer, IOContextBufferSize);
    pRWOpsIOContext->Seek(pRWOpsIOContext, 0, RW_SEEK_SET);
    memset(pBuffer + IOContextBufferSize, 0, AVPROBE_PADDING_SIZE);

    AVProbeData probeData;
    probeData.buf = pBuffer;
    probeData.buf_size = IOContextBufferSize;
    probeData.filename = NULL;
    probeData.mime_type = NULL;

    pFormatContext->iformat = av_probe_input_format(&probeData, 1);
    av_free(pBuffer);

    if (avformat_open_input(&pFormatContext, NULL, NULL, NULL) < 0)
    {
        throw MLIException("Couldn't open video file!");
    }

    if (avformat_find_stream_info(pFormatContext, NULL) < 0)
    {
        throw MLIException("Couldn't find video stream info!");
    }

    int videoStream = -1;

    for (unsigned int i = 0; i < pFormatContext->nb_streams; i++)
    {
        if (pFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            videoStream = i;
            break;
        }
    }

    AVCodecContext *pCodecContext = pFormatContext->streams[videoStream]->codec;
    AVCodec *pCodec = avcodec_find_decoder(pCodecContext->codec_id);

    if (avcodec_open2(pCodecContext, pCodec, NULL) < 0)
    {
        throw MLIException("Couldn't open codec!");
    }

    *ppRWOpsIOContext = pRWOpsIOContext;
    *ppFormatContext = pFormatContext;
    *pVideoStream = videoStream;
    *ppCodecContext = pCodecContext;
    *ppCodec = pCodec;
    *ppMemToFree = pMemToFree;
}

void ResourceLoader::PreloadMusic(const string &id, const string &relativeFilePath)
{
    SDL_RWops *pRWA = NULL;
    SDL_RWops *pRWB = NULL;
    void *pMemToFreeA = NULL;
    void *pMemToFreeB = NULL;

    SDL_SemWait(pLoadingSemaphore);
    pRWA = pCommonResourcesSource->LoadFile(relativeFilePath + "A.ogg", &pMemToFreeA);

    if (pRWA == NULL && pCaseResourcesSource != NULL)
    {
        pRWA = pCaseResourcesSource->LoadFile(relativeFilePath + "A.ogg", &pMemToFreeA);
        pRWB = pCaseResourcesSource->LoadFile(relativeFilePath + "B.ogg", &pMemToFreeB);
    }
    else
    {
        pRWB = pCommonResourcesSource->LoadFile(relativeFilePath + "B.ogg", &pMemToFreeB);
    }
    SDL_SemPost(pLoadingSemaphore);

    if (pRWA == NULL || pRWB == NULL || !preloadMusic(id, pRWA, pRWB))
    {
        free(pMemToFreeA);
        free(pMemToFreeB);
        return;
    }

    musicIdToMemToFreeMap[id + "_A"] = pMemToFreeA;
    musicIdToMemToFreeMap[id + "_B"] = pMemToFreeB;
}

void ResourceLoader::UnloadMusic(const string &id)
{
    unloadMusic(id);

    free(musicIdToMemToFreeMap[id + "_A"]);
    free(musicIdToMemToFreeMap[id + "_B"]);

    musicIdToMemToFreeMap.erase(id + "_A");
    musicIdToMemToFreeMap.erase(id + "_B");
}

void ResourceLoader::PreloadSound(const string &id, const string &relativeFilePath)
{
    SDL_RWops *pRW = NULL;
    void *pMemToFree = NULL;

    SDL_SemWait(pLoadingSemaphore);
    pRW = pCommonResourcesSource->LoadFile(relativeFilePath + ".ogg", &pMemToFree);

    if (pRW == NULL && pCaseResourcesSource != NULL)
    {
        pRW = pCaseResourcesSource->LoadFile(relativeFilePath + ".ogg", &pMemToFree);
    }
    SDL_SemPost(pLoadingSemaphore);

    if (pRW == NULL)
    {
        return;
    }

    preloadSound(id, pRW);
    free(pMemToFree);
}

void ResourceLoader::UnloadSound(const string &id)
{
    unloadSound(id);
}

void ResourceLoader::PreloadDialog(const string &id, const string &relativeFilePath)
{
    SDL_RWops *pRW = NULL;
    void *pMemToFree = NULL;

    SDL_SemWait(pLoadingSemaphore);
    pRW = pCommonResourcesSource->LoadFile(relativeFilePath + ".ogg", &pMemToFree);

    if (pRW == NULL && pCaseResourcesSource != NULL)
    {
        pRW = pCaseResourcesSource->LoadFile(relativeFilePath + ".ogg", &pMemToFree);
    }
    SDL_SemPost(pLoadingSemaphore);

    if (pRW == NULL)
    {
        return;
    }

    preloadDialog(id, pRW);
    free(pMemToFree);
}

void ResourceLoader::UnloadDialog(const string &id)
{
    unloadDialog(id);
}

void * ResourceLoader::LoadFileToMemory(const string &relativeFilePath, unsigned int *pFileSize)
{
    void *p = NULL;
    unsigned int fileSize = 0;

    SDL_SemWait(pLoadingSemaphore);
    p = pCommonResourcesSource->LoadFileToMemory(relativeFilePath, &fileSize);

    if (p == NULL && pCaseResourcesSource != NULL)
    {
        p = pCaseResourcesSource->LoadFileToMemory(relativeFilePath, &fileSize);
    }
    SDL_SemPost(pLoadingSemaphore);

    *pFileSize = fileSize;
    return p;
}

void ResourceLoader::HashFile(const string &relativeFilePath, byte hash[CryptoPP::SHA256::DIGESTSIZE])
{
    void *p = NULL;
    unsigned int fileSize = 0;

    SDL_SemWait(pLoadingSemaphore);
    p = pCommonResourcesSource->LoadFileToMemory(relativeFilePath, &fileSize);

    if (p == NULL && pCaseResourcesSource != NULL)
    {
        p = pCaseResourcesSource->LoadFileToMemory(relativeFilePath, &fileSize);
    }
    SDL_SemPost(pLoadingSemaphore);

    if (p != NULL)
    {
        CryptoPP::SHA256 sha256;
        sha256.CalculateDigest(hash, reinterpret_cast<const byte *>(p), fileSize);
    }
    else
    {
        for (int i = 0; i < CryptoPP::SHA256::DIGESTSIZE; i++)
        {
            hash[i] = 0;
        }
    }
}

void ResourceLoader::AddImage(Image *pImage)
{
    SDL_SemWait(pQueueSemaphore);
    smartSpriteQueue.push_back(pImage);
    SDL_SemPost(pQueueSemaphore);
}

void ResourceLoader::RemoveImage(Image *pImage)
{
    SDL_SemWait(pQueueSemaphore);

    for (int i = smartSpriteQueue.size() - 1; i >= 0; i--)
    {
        if (smartSpriteQueue[i] == pImage)
        {
            smartSpriteQueue.erase(smartSpriteQueue.begin() + i);
        }
    }

    SDL_SemPost(pQueueSemaphore);
}

void ResourceLoader::TryLoadOneImageTexture()
{
    SDL_SemWait(pQueueSemaphore);

    if (!smartSpriteQueue.empty())
    {
        Image *pImage = smartSpriteQueue.front();
        smartSpriteQueue.pop_front();

        pImage->LoadTextures();
    }

    SDL_SemPost(pQueueSemaphore);
}

bool ResourceLoader::HasImageTexturesToLoad()
{
    bool hasImageTexturesToLoad;

    SDL_SemWait(pQueueSemaphore);
    hasImageTexturesToLoad = !smartSpriteQueue.empty();
    SDL_SemPost(pQueueSemaphore);

    return hasImageTexturesToLoad;
}

void ResourceLoader::FlushImages()
{
    SDL_SemWait(pQueueSemaphore);
    smartSpriteQueue.clear();
    SDL_SemPost(pQueueSemaphore);
}

void ResourceLoader::AddImageIdToLoadList(const string &id)
{
    if (id.length() == 0)
    {
        return;
    }

    SDL_SemWait(pLoadQueueSemaphore);
    bool alreadyExists = false;

    for (unsigned int i = 0; i < loadResourceStepList.size(); i++)
    {
        LoadImageStep *pStep = dynamic_cast<LoadImageStep *>(loadResourceStepList[i]);

        if (pStep != NULL && pStep->GetSpriteId() == id)
        {
            alreadyExists = true;
            break;
        }
    }

    if (!alreadyExists)
    {
        loadResourceStepList.push_back(new LoadImageStep(id));
    }
    SDL_SemPost(pLoadQueueSemaphore);
}

void ResourceLoader::AddImageIdToDeleteList(const string &id)
{
    if (id.length() == 0)
    {
        return;
    }

    SDL_SemWait(pLoadQueueSemaphore);
    bool alreadyExists = false;

    for (unsigned int i = 0; i < loadResourceStepList.size(); i++)
    {
        DeleteImageStep *pStep = dynamic_cast<DeleteImageStep *>(loadResourceStepList[i]);

        if (pStep != NULL && pStep->GetSpriteId() == id)
        {
            alreadyExists = true;
            break;
        }
    }

    if (!alreadyExists)
    {
        loadResourceStepList.push_back(new DeleteImageStep(id));
    }
    SDL_SemPost(pLoadQueueSemaphore);
}

void ResourceLoader::AddVideoToLoadList(Video *pVideo)
{
    if (pVideo == NULL)
    {
        return;
    }

    SDL_SemWait(pLoadQueueSemaphore);
    bool alreadyExists = false;

    for (unsigned int i = 0; i < loadResourceStepList.size(); i++)
    {
        LoadVideoStep *pStep = dynamic_cast<LoadVideoStep *>(loadResourceStepList[i]);

        if (pStep != NULL && pStep->GetVideo() == pVideo)
        {
            alreadyExists = true;
            break;
        }
    }

    if (!alreadyExists)
    {
        loadResourceStepList.push_back(new LoadVideoStep(pVideo));
    }
    SDL_SemPost(pLoadQueueSemaphore);
}

void ResourceLoader::AddVideoToDeleteList(Video *pVideo)
{
    if (pVideo == NULL)
    {
        return;
    }

    SDL_SemWait(pLoadQueueSemaphore);
    bool alreadyExists = false;

    for (unsigned int i = 0; i < loadResourceStepList.size(); i++)
    {
        DeleteVideoStep *pStep = dynamic_cast<DeleteVideoStep *>(loadResourceStepList[i]);

        if (pStep != NULL && pStep->GetVideo() == pVideo)
        {
            alreadyExists = true;
            break;
        }
    }

    if (!alreadyExists)
    {
        loadResourceStepList.push_back(new DeleteVideoStep(pVideo));
    }
    SDL_SemPost(pLoadQueueSemaphore);
}

void ResourceLoader::SnapLoadStepQueue()
{
    SDL_SemWait(pLoadQueueSemaphore);
    for (unsigned int i = 0; i < loadResourceStepList.size(); i++)
    {
        cachedLoadResourceStepList.push_back(loadResourceStepList[i]);
    }
    loadResourceStepList.clear();
    SDL_SemPost(pLoadQueueSemaphore);
}

bool ResourceLoader::HasLoadStep()
{
    return !cachedLoadResourceStepList.empty();
}

void ResourceLoader::TryRunOneLoadStep()
{
    if (HasLoadStep())
    {
        LoadResourceStep *pStep = cachedLoadResourceStepList.front();
        pStep->Execute();
        cachedLoadResourceStepList.pop_front();
        delete pStep;
    }
}

ResourceLoader::ResourceLoader()
{
    pCommonResourcesSource = NULL;
    pCaseResourcesSource = NULL;
    pCachedCaseResourcesSource = NULL;

    pLoadingSemaphore = SDL_CreateSemaphore(1);
    pQueueSemaphore = SDL_CreateSemaphore(1);
    pLoadQueueSemaphore = SDL_CreateSemaphore(1);
}

ResourceLoader::~ResourceLoader()
{
    delete pCommonResourcesSource;
    pCommonResourcesSource = NULL;
    delete pCaseResourcesSource;
    pCaseResourcesSource = NULL;

    SDL_DestroySemaphore(pLoadingSemaphore);
    pLoadingSemaphore = NULL;
    SDL_DestroySemaphore(pQueueSemaphore);
    pQueueSemaphore = NULL;
    SDL_DestroySemaphore(pLoadQueueSemaphore);
    pLoadQueueSemaphore = NULL;

    smartSpriteQueue.clear();
    deleteTextureQueue.clear();
}

ResourceLoader::ArchiveSource::~ArchiveSource()
{
    mz_zip_reader_end(&zip_archive);
}

bool ResourceLoader::ArchiveSource::CreateAndInit(const string &archiveFilePath, ArchiveSource **ppSource)
{
    ArchiveSource *pSource = new ArchiveSource();

    if (!pSource->Init(archiveFilePath))
    {
        delete pSource;
        return false;
    }

    *ppSource = pSource;
    return true;
}

SDL_RWops * ResourceLoader::ArchiveSource::LoadFile(const string &relativeFilePath, void **ppMemToFree)
{
    size_t uncomp_size = 0;
    void *p = mz_zip_reader_extract_file_to_heap(&zip_archive, relativeFilePath.c_str(), &uncomp_size, 0);

    if (p == NULL)
    {
        return NULL;
    }

    *ppMemToFree = p;
    return SDL_RWFromMem(p, (unsigned int)uncomp_size);
}

void * ResourceLoader::ArchiveSource::LoadFileToMemory(const string &relativeFilePath, unsigned int *pSize)
{
    size_t uncomp_size = 0;
    void *p = mz_zip_reader_extract_file_to_heap(&zip_archive, relativeFilePath.c_str(), &uncomp_size, 0);

    if (p == NULL)
    {
        return NULL;
    }

    *pSize = (unsigned int)uncomp_size;
    return p;
}

bool ResourceLoader::ArchiveSource::Init(const string &archiveFilePath)
{
    return mz_zip_reader_init_file(&zip_archive, archiveFilePath.c_str(), 0) > 0;
}
