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

#pragma once

#include <wtf/Forward.h>
#include <wtf/Function.h>
#include <wtf/OptionSet.h>
#include <wtf/RefCounted.h>
#include <wtf/Seconds.h>
#include <wtf/Vector.h>
#include <wtf/text/WTFString.h>

namespace Inspector {
class InspectorArray;
class InspectorObject;
class InspectorValue;
}

namespace WebDriver {

class Capabilities;
class CommandResult;
class SessionHost;

class Session : public RefCounted<Session> {
public:
    static Ref<Session> create(std::unique_ptr<SessionHost>&& host)
    {
        return adoptRef(*new Session(WTFMove(host)));
    }
    ~Session();

    const String& id() const { return m_id; }
    const Capabilities& capabilities() const;

    enum class FindElementsMode { Single, Multiple };
    enum class ExecuteScriptMode { Sync, Async };
    enum class Timeout { Script, PageLoad, Implicit };

    struct Timeouts {
        std::optional<Seconds> script;
        std::optional<Seconds> pageLoad;
        std::optional<Seconds> implicit;
    };

    void createTopLevelBrowsingContext(Function<void (CommandResult&&)>&&);
    void close(Function<void (CommandResult&&)>&&);
    void getTimeouts(Function<void (CommandResult&&)>&&);
    void setTimeouts(const Timeouts&, Function<void (CommandResult&&)>&&);

    void go(const String& url, Function<void (CommandResult&&)>&&);
    void getCurrentURL(Function<void (CommandResult&&)>&&);
    void back(Function<void (CommandResult&&)>&&);
    void forward(Function<void (CommandResult&&)>&&);
    void refresh(Function<void (CommandResult&&)>&&);
    void getTitle(Function<void (CommandResult&&)>&&);
    void getWindowHandle(Function<void (CommandResult&&)>&&);
    void closeWindow(Function<void (CommandResult&&)>&&);
    void switchToWindow(const String& windowHandle, Function<void (CommandResult&&)>&&);
    void getWindowHandles(Function<void (CommandResult&&)>&&);
    void switchToFrame(RefPtr<Inspector::InspectorValue>&&, Function<void (CommandResult&&)>&&);
    void switchToParentFrame(Function<void (CommandResult&&)>&&);
    void getWindowPosition(Function<void (CommandResult&&)>&&);
    void setWindowPosition(int windowX, int windowY, Function<void (CommandResult&&)>&&);
    void getWindowSize(Function<void (CommandResult&&)>&&);
    void setWindowSize(int windowWidth, int windowHeight, Function<void (CommandResult&&)>&&);
    void findElements(const String& strategy, const String& selector, FindElementsMode, const String& rootElementID, Function<void (CommandResult&&)>&&);
    void isElementSelected(const String& elementID, Function<void (CommandResult&&)>&&);
    void getElementText(const String& elementID, Function<void (CommandResult&&)>&&);
    void getElementTagName(const String& elementID, Function<void (CommandResult&&)>&&);
    void getElementRect(const String& elementID, Function<void (CommandResult&&)>&&);
    void isElementEnabled(const String& elementID, Function<void (CommandResult&&)>&&);
    void isElementDisplayed(const String& elementID, Function<void (CommandResult&&)>&&);
    void getElementAttribute(const String& elementID, const String& attribute, Function<void (CommandResult&&)>&&);
    void elementClick(const String& elementID, Function<void (CommandResult&&)>&&);
    void elementClear(const String& elementID, Function<void (CommandResult&&)>&&);
    void elementSendKeys(const String& elementID, Vector<String>&& keys, Function<void (CommandResult&&)>&&);
    void elementSubmit(const String& elementID, Function<void (CommandResult&&)>&&);
    void executeScript(const String& script, RefPtr<Inspector::InspectorArray>&& arguments, ExecuteScriptMode, Function<void (CommandResult&&)>&&);

private:
    Session(std::unique_ptr<SessionHost>&&);

    RefPtr<Inspector::InspectorObject> createElement(RefPtr<Inspector::InspectorValue>&&);
    RefPtr<Inspector::InspectorObject> createElement(const String& elementID);
    RefPtr<Inspector::InspectorObject> extractElement(Inspector::InspectorValue&);
    String extractElementID(Inspector::InspectorValue&);
    RefPtr<Inspector::InspectorValue> handleScriptResult(RefPtr<Inspector::InspectorValue>&&);

    struct Point {
        int x { 0 };
        int y { 0 };
    };

    struct Size {
        int width { 0 };
        int height { 0 };
    };

    struct Rect {
        Point origin;
        Size size;
    };

    enum class ElementLayoutOption {
        ScrollIntoViewIfNeeded = 1 << 0,
        UseViewportCoordinates = 1 << 1,
    };
    void computeElementLayout(const String& elementID, OptionSet<ElementLayoutOption>, Function<void (std::optional<Rect>&&, RefPtr<Inspector::InspectorObject>&&)>&&);

    enum class MouseButton { None, Left, Middle, Right };
    enum class MouseInteraction { Move, Down, Up, SingleClick, DoubleClick };
    void performMouseInteraction(int x, int y, MouseButton, MouseInteraction, Function<void (CommandResult&&)>&&);

    enum class KeyboardInteractionType { KeyPress, KeyRelease, InsertByKey };
    struct KeyboardInteraction {
        KeyboardInteractionType type { KeyboardInteractionType::InsertByKey };
        std::optional<String> text;
        std::optional<String> key;
    };
    enum KeyModifier {
        None = 0,
        Shift = 1 << 0,
        Control = 1 << 1,
        Alternate = 1 << 2,
        Meta = 1 << 3,
    };
    String virtualKeyForKeySequence(const String& keySequence, KeyModifier&);
    void performKeyboardInteractions(Vector<KeyboardInteraction>&&, Function<void (CommandResult&&)>&&);

    std::unique_ptr<SessionHost> m_host;
    Timeouts m_timeouts;
    String m_id;
    std::optional<String> m_toplevelBrowsingContext;
    std::optional<String> m_browsingContext;
};

} // WebDriver
