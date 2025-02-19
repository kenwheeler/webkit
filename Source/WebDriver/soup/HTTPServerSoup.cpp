/*
 * Copyright (C) 2017 Igalia S.L.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "HTTPServer.h"

#include <libsoup/soup.h>
#include <wtf/Function.h>
#include <wtf/glib/GUniquePtr.h>

namespace WebDriver {

bool HTTPServer::listen(unsigned port)
{
    m_soupServer = adoptGRef(soup_server_new(SOUP_SERVER_SERVER_HEADER, "WebKitWebDriver", nullptr));
    GUniqueOutPtr<GError> error;
    if (!soup_server_listen_local(m_soupServer.get(), port, static_cast<SoupServerListenOptions>(0), &error.outPtr())) {
        WTFLogAlways("Failed to start HTTP server at port %u: %s", port, error->message);
        return false;
    }

    soup_server_add_handler(m_soupServer.get(), nullptr, [](SoupServer* server, SoupMessage* message, const char* path, GHashTable*, SoupClientContext*, gpointer userData) {
        auto* httpServer = static_cast<HTTPServer*>(userData);
        GRefPtr<SoupMessage> protectedMessage = message;
        soup_server_pause_message(server, message);
        httpServer->m_requestHandler.handleRequest({ String::fromUTF8(message->method), String::fromUTF8(path), message->request_body->data, static_cast<size_t>(message->request_body->length) },
            [server, message = WTFMove(protectedMessage)](HTTPRequestHandler::Response&& response) {
                soup_message_set_status(message.get(), response.statusCode);
                if (!response.data.isNull()) {
                    soup_message_headers_append(message->response_headers, "Content-Type", response.contentType.utf8().data());
                    soup_message_body_append(message->response_body, SOUP_MEMORY_COPY, response.data.data(), response.data.length());
                }
                soup_server_unpause_message(server, message.get());
        });
    }, this, nullptr);

    return true;
}

void HTTPServer::disconnect()
{
    soup_server_disconnect(m_soupServer.get());
    m_soupServer = nullptr;
}

} // namespace WebDriver
