/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Nathan Osman
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <QByteArray>

#include "qhttpparser.h"

QList<QByteArray> QHttpParser::split(const QByteArray &data, const QByteArray &delim, int maxSplit)
{
    QList<QByteArray> parts;
    int index = 0;

    for(int i = 0; !maxSplit || i < maxSplit; ++i) {
        int nextIndex = data.indexOf(delim, index);

        if(nextIndex == -1) {
            break;
        }

        parts.append(data.mid(index, nextIndex - index));
        index = nextIndex + delim.length();
    }

    // Append whatever remains to the list
    parts.append(data.mid(index));

    return parts;
}

bool QHttpParser::parseRequest(const QByteArray &data, QByteArray &method, QByteArray &path, QList<QHttpHeader> &headers)
{
    // Split the data into individual lines
    QList<QByteArray> lines = split(data, "\r\n");

    // Separate the status line into three components
    QList<QByteArray> parts = split(lines.takeFirst(), " ");
    if(parts.count() != 3) {
        return false;
    }

    // Check the HTTP version
    if(parts[2] != "HTTP/1.0" && parts[2] != "HTTP/1.1") {
        return false;
    }

    method = parts[0];
    path = parts[1];

    // Parse each of the headers
    foreach(QByteArray line, lines) {

        // Split each header by ":" and ignore any invalid ones
        parts = split(line, ":", 1);
        if(parts.count() != 2) {
            continue;
        }

        headers.append(QHttpHeader(parts[0].trimmed(), parts[1].trimmed()));
    }

    return true;
}