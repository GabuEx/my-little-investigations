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

#include "../SharedUtils.h"

#include <limits>

TextWidget::TextWidget(const string &textId, MLIFont *pFont, Color textColor, HAlignment hAlignment, VAlignment vAlignment)
    : textId(textId),
      wrapTextWidth(0),
      pFont(pFont),
      textColor(textColor),
      hAlignment(hAlignment),
      vAlignment(vAlignment),
      x(0),
      y(0),
      width(0),
      height(0)
{
    ReloadLocalizableText();
    gpLocalizableContent->AddLocalizableTextOwner(this);
}

TextWidget::~TextWidget()
{
    gpLocalizableContent->RemoveLocalizableTextOwner(this);
}

void TextWidget::Draw() const
{
    Draw(x, y, hAlignment, vAlignment);
}

void TextWidget::Draw(double x, double y, HAlignment hAlignment) const
{
    Draw(x, y, hAlignment, vAlignment);
}

void TextWidget::Draw(double x, double y, HAlignment hAlignment, VAlignment vAlignment) const
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
        drawingPoint.SetY(y - GetTextHeight() / 2);
    }
    else if (vAlignment == VAlignmentBottom)
    {
        drawingPoint.SetY(y - GetTextHeight());
    }

    double lineHeight = pFont->GetLineHeight();
    for (string line : lines)
    {
        if (hAlignment == HAlignmentLeft)
        {
            drawingPoint.SetX(x);
        }
        else if (hAlignment == HAlignmentCenter)
        {
            drawingPoint.SetX(x - pFont->GetWidth(line) / 2);
        }
        else if (hAlignment == HAlignmentRight)
        {
            drawingPoint.SetX(x - pFont->GetWidth(line));
        }
        pFont->Draw(line, drawingPoint, textColor);
        drawingPoint.SetY(drawingPoint.GetY() + lineHeight);
    }
}

void TextWidget::SetTextId(const string &textId)
{
    this->textId = textId;
    ReloadLocalizableText();
}

void TextWidget::SetText(const string &text)
{
    this->text = text;
    ReloadLocalizableText();
}

void TextWidget::WrapText(double width)
{
    SplitText(width);
}

void TextWidget::FitSizeToContent()
{
    width = GetTextWidth();
    height = GetTextHeight();
}

void TextWidget::ReloadLocalizableText()
{
    if (textId.length() > 0)
    {
        text = gpLocalizableContent->GetText(textId);
    }

    SplitText(wrapTextWidth);
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
    for (string line : lines)
    {
        w = max(w, pFont->GetWidth(line));
    }

    return w;
}

void TextWidget::SplitText(double maxWidth)
{
    wrapTextWidth = maxWidth;

    if (maxWidth > 0)
    {
        lines = split(ParseRawDialog(NULL, GetText(), RectangleWH(0, 0, maxWidth, std::numeric_limits<double>::infinity()), 0, pFont), '\n');
    }
    else
    {
        lines = split(GetText(), '\n');
    }
}
