/**
 * Represents a version, both for checking for higher or lower versions and
 * for creating a string representation of a version.
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

#include "Version.h"

#include "XmlReader.h"
#include "XmlWriter.h"

#include <cstdio>

Version::Version(XmlReader *pReader)
{
    pReader->StartElement("Version");
    major = pReader->ReadIntElement("Major");
    minor = pReader->ReadIntElement("Minor");
    revision = pReader->ReadIntElement("Revision");
    pReader->EndElement();
}

Version::Version(const string &versionString)
{
    sscanf(versionString.c_str(), "%d.%d.%d", &major, &minor, &revision);
}

bool Version::operator==(const Version &other) const
{
    return
        major == other.major &&
        minor == other.minor &&
        revision == other.revision;
}

bool Version::operator!=(const Version &other) const
{
    return !operator==(other);
}

bool Version::operator<(const Version &other) const
{
    if (major > other.major)
    {
        return false;
    }
    else if (major < other.major)
    {
        return true;
    }

    if (minor > other.minor)
    {
        return false;
    }
    else if (minor < other.minor)
    {
        return true;
    }

    return revision < other.revision;
}

bool Version::operator<=(const Version &other) const
{
    return operator<(other) || operator==(other);
}

bool Version::operator>(const Version &other) const
{
    if (major < other.major)
    {
        return false;
    }
    else if (major > other.major)
    {
        return true;
    }

    if (minor < other.minor)
    {
        return false;
    }
    else if (minor > other.minor)
    {
        return true;
    }

    return revision > other.revision;
}

bool Version::operator>=(const Version &other) const
{
    return operator>(other) || operator==(other);
}

Version::operator string()
{
    char buf[16];
    sprintf(buf, "%d.%d.%d", major, minor, revision);
    return string(buf);
}

void Version::SaveToXml(XmlWriter *pWriter)
{
    pWriter->StartElement("Version");
    pWriter->WriteIntElement("Major", major);
    pWriter->WriteIntElement("Minor", minor);
    pWriter->WriteIntElement("Revision", revision);
    pWriter->EndElement();
}
