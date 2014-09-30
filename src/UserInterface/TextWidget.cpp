/**
 * Class for text drawing. Handle text alignment, newlines.
 * Can wrap text with given width. Does not interact directly with the user.
 *
 * @author mad-mix
 * @since 1.0.7
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
#include "TextWidget.h"

#include "../utf8cpp/utf8.h"

TextWidget::TextWidget(const string &text, MLIFont *pFont, Color textColor, HAlignment hAlignment, VAlignment vAlignment)
    : text(text),
      pFont(pFont),
      textColor(textColor),
      hAlignment(hAlignment),
      vAlignment(vAlignment),
      x(0),
      y(0),
      width(0),
      height(0)
{
    SplitText(false, 0);
}

TextWidget::~TextWidget()
{

}

void TextWidget::Draw(double x, double y) const
{
    if (pFont == NULL)
    {
        return;
    }

    Vector2 drawingPoint(x, y);

    if (vAlignment == VAlignmentTop)
    {

    }
    else if (vAlignment == VAlignmentCenter)
    {
        drawingPoint.SetY(y + (height - GetTextHeight()) / 2);
    }
    else if (vAlignment == VAlignmentBottom)
    {
        drawingPoint.SetY(y + (height - GetTextHeight()));
    }

    double lineHeight = pFont->GetLineHeight();
    for (vector<pair<string::const_iterator, string::const_iterator> >::const_iterator it = lines.begin(); it != lines.end(); it++)
    {
        // TODO: add to MLIFont functions working with iterators to avoid string copying here
        string line(it->first, it->second);
        if (hAlignment == HAlignmentLeft)
        {
            drawingPoint.SetX(x);
        }
        else if (hAlignment == HAlignmentCenter)
        {
            drawingPoint.SetX(x + (width - pFont->GetWidth(line)) / 2);
        }
        else if (hAlignment == HAlignmentRight)
        {
            drawingPoint.SetX(x + (width - pFont->GetWidth(line)));
        }
        pFont->Draw(line, drawingPoint, textColor);
        drawingPoint.SetY(drawingPoint.GetY() + lineHeight);
    }
}

void TextWidget::SetText(const string &text)
{
    this->text = text;
    SplitText(false, 0);
}

void TextWidget::WrapText(double width)
{
    SplitText(true, width);
}

void TextWidget::FitSizeToContent()
{
    width = GetTextWidth();
    height = GetTextHeight();
}

double TextWidget::GetTextHeight() const
{
    return pFont == NULL ? 0 : pFont->GetLineHeight() * lines.size();
}

double TextWidget::GetTextWidth() const
{
    if (pFont == NULL)
    {
        return 0;
    }

    double w = 0;
    for (vector<pair<string::const_iterator, string::const_iterator> >::const_iterator it = lines.begin(); it != lines.end(); it++)
    {
        string line(it->first, it->second);
        w = max(w, pFont->GetWidth(line));
    }

    return w;
}

void TextWidget::SplitText(bool wrap, double maxWidth)
{
    lines.clear();

    const string &s = GetText();

    string::const_iterator lineStart = s.begin();
    string::const_iterator lineEnd = s.begin();
    for (string::const_iterator it = s.begin(); it != s.end();)
    {
        uint32_t c = utf8::next(it, s.end());

        if (c == ' ' || c == '\n') // we got full word
        {
            string line(lineStart, it - 1);
            if (wrap && pFont->GetWidth(line) > maxWidth)
            {
                lines.push_back(make_pair(lineStart, lineEnd));
                lineStart = lineEnd;
            }

            lineEnd = it;
            utf8::previous(lineEnd, s.begin()); // skip delimiter

            if (c == '\n') // force newline
            {
                lines.push_back(make_pair(lineStart, lineEnd));
                lineStart = it;
            }
        }

        if (it == s.end()) // at least one line exist if we are inside this cycle
        {
            lines.push_back(make_pair(lineStart, it));
        }
    }
}
