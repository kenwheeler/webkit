/*
 * Copyright (C) 2017 Apple Inc. All rights reserved.
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

function(strategy, ancestorElement, query, firstResultOnly, timeoutDuration, callback) {
    ancestorElement = ancestorElement || document;

    switch (strategy) {
    case "id":
        strategy = "css selector";
        query = "[id=\"" + escape(query) + "\"]";
        break;
    case "name":
        strategy = "css selector";
        query = "[name=\"" + escape(query) + "\"]";
        break;
    case "link text":
        strategy = "xpath";
        query = ".//a[@href][normalize-space(descendant-or-self::text()) = \"" + escape(query) + "\"]";
        break;
    case "partial link text":
        strategy = "xpath";
        query = ".//a[@href][contains(normalize-space(descendant-or-self::text()), \"" + escape(query) + "\")]";
        break;
    }

    switch (strategy) {
    case "css selector":
    case "tag name":
    case "class name":
    case "xpath":
        break;
    default:
        // §12.2 Find Element and §12.3 Find Elements, step 4: If location strategy is not present as a keyword
        // in the table of location strategies, return error with error code invalid argument.
        // https://www.w3.org/TR/webdriver/#find-element
        throw { name: "InvalidParameter", message: ("Unsupported locator strategy: " + strategy + ".") };
    }

    function escape(string) {
        return string.replace(/\\/g, "\\\\").replace(/"/g, "\\\"");
    }

    function tryToFindNode() {
        try {
            switch (strategy) {
            case "css selector":
                if (firstResultOnly)
                    return ancestorElement.querySelector(query) || null;
                return Array.from(ancestorElement.querySelectorAll(query));

            case "tag name":
                let tagNameResult = ancestorElement.getElementsByTagName(query);
                if (firstResultOnly)
                    return tagNameResult[0] || null;
                return Array.from(tagNameResult);

            case "class name":
                let classNameResult = ancestorElement.getElementsByClassName(query);
                if (firstResultOnly)
                    return classNameResult[0] || null;
                return Array.from(classNameResult);

            case "xpath":
                if (firstResultOnly) {
                    let xpathResult = document.evaluate(query, ancestorElement, null, XPathResult.FIRST_ORDERED_NODE_TYPE, null);
                    if (!xpathResult)
                        return null;
                    return xpathResult.singleNodeValue;
                }

                let xpathResult = document.evaluate(query, ancestorElement, null, XPathResult.ORDERED_NODE_SNAPSHOT_TYPE, null);
                if (!xpathResult || !xpathResult.snapshotLength)
                    return [];

                let arrayResult = [];
                for (let i = 0; i < xpathResult.snapshotLength; ++i)
                    arrayResult.push(xpathResult.snapshotItem(i));
                return arrayResult;
            }
        } catch (error) {
            if (error instanceof XPathException && error.code === XPathException.INVALID_EXPRESSION_ERR)
                return "InvalidXPathExpression";
            // FIXME: Bad CSS can throw an error that we should report back to the endpoint. There is no
            // special error code for that though, so we just return an empty match.
            return firstResultOnly ? null : [];
        }
    }

    const pollInterval = 50;
    let pollUntil = performance.now() + timeoutDuration;

    function pollForNode() {
        let result = tryToFindNode();

        // Report any valid results.
        if (typeof result === "string" || result instanceof Node || (result instanceof Array && result.length)) {
            callback(result);
            return;
        }

        // Schedule another attempt if we have time remaining.
        let durationRemaining = pollUntil - performance.now();
        if (durationRemaining < pollInterval) {
            callback(firstResultOnly ? null : []);
            return;
        }

        setTimeout(pollForNode, pollInterval);
    }

    pollForNode();
}
