/*
 * Copyright (C) 2015-2017 Apple Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#import "config.h"
#import "NetworkStorageSession.h"

#import "CFNetworkSPI.h"
#import "Cookie.h"
#import "CookieStorageObserver.h"
#import "URL.h"
#import <wtf/BlockObjCExceptions.h>

namespace WebCore {

void NetworkStorageSession::setCookie(const Cookie& cookie)
{
    BEGIN_BLOCK_OBJC_EXCEPTIONS;
    [nsCookieStorage() setCookie:(NSHTTPCookie *)cookie];
    END_BLOCK_OBJC_EXCEPTIONS;
}

void NetworkStorageSession::setCookies(const Vector<Cookie>& cookies, const URL& url, const URL& mainDocumentURL)
{
    RetainPtr<NSMutableArray> nsCookies = adoptNS([[NSMutableArray alloc] initWithCapacity:cookies.size()]);
    for (const auto& cookie : cookies)
        [nsCookies addObject:(NSHTTPCookie *)cookie];

    BEGIN_BLOCK_OBJC_EXCEPTIONS;
    [nsCookieStorage() setCookies:nsCookies.get() forURL:(NSURL *)url mainDocumentURL:(NSURL *)mainDocumentURL];
    END_BLOCK_OBJC_EXCEPTIONS;
}

void NetworkStorageSession::deleteCookie(const Cookie& cookie)
{
    [nsCookieStorage() deleteCookie:(NSHTTPCookie *)cookie];
}

static Vector<Cookie> nsCookiesToCookieVector(NSArray<NSHTTPCookie *> *nsCookies)
{
    Vector<Cookie> cookies;
    cookies.reserveInitialCapacity(nsCookies.count);
    for (NSHTTPCookie *nsCookie in nsCookies)
        cookies.uncheckedAppend(nsCookie);

    return cookies;
}

Vector<Cookie> NetworkStorageSession::getAllCookies()
{
    return nsCookiesToCookieVector(nsCookieStorage().cookies);
}

Vector<Cookie> NetworkStorageSession::getCookies(const URL& url)
{
    return nsCookiesToCookieVector([nsCookieStorage() cookiesForURL:(NSURL *)url]);
}

void NetworkStorageSession::flushCookieStore()
{
    [nsCookieStorage() _saveCookies];
}

NSHTTPCookieStorage *NetworkStorageSession::nsCookieStorage() const
{
    auto cfCookieStorage = cookieStorage();
    if (!cfCookieStorage || [NSHTTPCookieStorage sharedHTTPCookieStorage]._cookieStorage == cfCookieStorage)
        return [NSHTTPCookieStorage sharedHTTPCookieStorage];

    return [[[NSHTTPCookieStorage alloc] _initWithCFHTTPCookieStorage:cfCookieStorage.get()] autorelease];
}

CookieStorageObserver& NetworkStorageSession::cookieStorageObserver() const
{
    if (!m_cookieStorageObserver)
        m_cookieStorageObserver = CookieStorageObserver::create([nsCookieStorage() _cookieStorage]);

    return *m_cookieStorageObserver;
}

} // namespace WebCore
