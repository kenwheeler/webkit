/*
    Copyright (C) 1998 Lars Knoll (knoll@mpi-hd.mpg.de)
    Copyright (C) 2001 Dirk Mueller <mueller@kde.org>
    Copyright (C) 2006 Samuel Weinig (sam.weinig@gmail.com)
    Copyright (C) 2004, 2005, 2006, 2007 Apple Inc. All rights reserved.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#pragma once

#include "CachedResource.h"
#include "Image.h"
#include "ImageObserver.h"
#include "IntRect.h"
#include "IntSizeHash.h"
#include "LayoutSize.h"
#include "SVGImageCache.h"
#include <wtf/HashMap.h>

namespace WebCore {

class CachedImageClient;
class CachedResourceLoader;
class FloatSize;
class MemoryCache;
class RenderElement;
class RenderObject;
class SecurityOrigin;

struct Length;

class CachedImage final : public CachedResource {
    friend class MemoryCache;

public:
    CachedImage(CachedResourceRequest&&, SessionID);
    CachedImage(Image*, SessionID);
    // Constructor to use for manually cached images.
    CachedImage(const URL&, Image*, SessionID);
    virtual ~CachedImage();

    WEBCORE_EXPORT Image* image(); // Returns the nullImage() if the image is not available yet.
    WEBCORE_EXPORT Image* imageForRenderer(const RenderObject*); // Returns the nullImage() if the image is not available yet.
    bool hasImage() const { return m_image.get(); }
    bool currentFrameKnownToBeOpaque(const RenderElement*);

    std::pair<Image*, float> brokenImage(float deviceScaleFactor) const; // Returns an image and the image's resolution scale factor.
    bool willPaintBrokenImage() const;

    bool canRender(const RenderElement* renderer, float multiplier) { return !errorOccurred() && !imageSizeForRenderer(renderer, multiplier).isEmpty(); }

    void setContainerSizeForRenderer(const CachedImageClient*, const LayoutSize&, float);
    bool usesImageContainerSize() const { return m_image && m_image->usesContainerSize(); }
    bool imageHasRelativeWidth() const { return m_image && m_image->hasRelativeWidth(); }
    bool imageHasRelativeHeight() const { return m_image && m_image->hasRelativeHeight(); }

    void addDataBuffer(SharedBuffer&) override;
    void finishLoading(SharedBuffer*) override;

    enum SizeType {
        UsedSize,
        IntrinsicSize
    };
    // This method takes a zoom multiplier that can be used to increase the natural size of the image by the zoom.
    LayoutSize imageSizeForRenderer(const RenderElement*, float multiplier, SizeType = UsedSize); // returns the size of the complete image.
    void computeIntrinsicDimensions(Length& intrinsicWidth, Length& intrinsicHeight, FloatSize& intrinsicRatio);

    bool isManuallyCached() const { return m_isManuallyCached; }
    RevalidationDecision makeRevalidationDecision(CachePolicy) const override;
    void load(CachedResourceLoader&) override;

    bool isOriginClean(SecurityOrigin*);

    void addPendingImageDrawingClient(CachedImageClient&);

private:
    void clear();

    CachedImage(CachedImage&, const ResourceRequest&, SessionID);

    void setBodyDataFrom(const CachedResource&) final;

    void createImage();
    void clearImage();
    // If not null, changeRect is the changed part of the image.
    void notifyObservers(const IntRect* changeRect = nullptr);
    void checkShouldPaintBrokenImage();

    void switchClientsToRevalidatedResource() final;
    bool mayTryReplaceEncodedData() const final { return true; }

    void didAddClient(CachedResourceClient&) final;
    void didRemoveClient(CachedResourceClient&) final;

    void allClientsRemoved() override;
    void destroyDecodedData() override;

    EncodedDataStatus setImageDataBuffer(SharedBuffer*, bool allDataReceived);
    void addData(const char* data, unsigned length) override;
    void error(CachedResource::Status) override;
    void responseReceived(const ResourceResponse&) override;

    // For compatibility, images keep loading even if there are HTTP errors.
    bool shouldIgnoreHTTPStatusCodeErrors() const override { return true; }

    bool stillNeedsLoad() const override { return !errorOccurred() && status() == Unknown && !isLoading(); }

    class CachedImageObserver final : public RefCounted<CachedImageObserver>, public ImageObserver {
    public:
        static Ref<CachedImageObserver> create(CachedImage& image) { return adoptRef(*new CachedImageObserver(image)); }
        HashSet<CachedImage*>& cachedImages() { return m_cachedImages; }
        const HashSet<CachedImage*>& cachedImages() const { return m_cachedImages; }

    private:
        explicit CachedImageObserver(CachedImage&);

        // ImageObserver API
        URL sourceUrl() const override { return !m_cachedImages.isEmpty() ? (*m_cachedImages.begin())->url() : URL(); }
        void decodedSizeChanged(const Image&, long long delta) final;
        void didDraw(const Image&) final;

        bool canDestroyDecodedData(const Image&) final;
        void imageFrameAvailable(const Image&, ImageAnimatingState, const IntRect* changeRect = nullptr) final;
        void changedInRect(const Image&, const IntRect*) final;

        HashSet<CachedImage*> m_cachedImages;
    };

    void decodedSizeChanged(const Image&, long long delta);
    void didDraw(const Image&);
    bool canDestroyDecodedData(const Image&);
    void imageFrameAvailable(const Image&, ImageAnimatingState, const IntRect* changeRect = nullptr);
    void changedInRect(const Image&, const IntRect*);

    void addIncrementalDataBuffer(SharedBuffer&);

    void didReplaceSharedBufferContents() override;

    typedef std::pair<LayoutSize, float> SizeAndZoom;
    typedef HashMap<const CachedImageClient*, SizeAndZoom> ContainerSizeRequests;
    ContainerSizeRequests m_pendingContainerSizeRequests;

    HashSet<CachedImageClient*> m_pendingImageDrawingClients;

    RefPtr<CachedImageObserver> m_imageObserver;
    RefPtr<Image> m_image;
    std::unique_ptr<SVGImageCache> m_svgImageCache;
    bool m_isManuallyCached { false };
    bool m_shouldPaintBrokenImage { true };
};

} // namespace WebCore

SPECIALIZE_TYPE_TRAITS_CACHED_RESOURCE(CachedImage, CachedResource::ImageResource)
