#include "XmlStorableObject.h"

#include "Utils.h"

XmlString ToProperCase(XmlString s)
{
#ifndef CASE_CREATOR
    s = ToProperCaseUnicode(s);
#else
    if (s.length() > 0)
    {
        s[0] = s[0].toUpper();
    }
#endif

    return s;
}

XmlString RemovePointerPrefix(XmlString s)
{
#ifndef CASE_CREATOR
    if (s.length() > 1 && s[0] == 'p' && !islower(s[1]))
    {
        s = s.erase(0, 1);
    }
#else
    if (s.length() > 1 && s[0] == 'p' && !s[1].isLower())
    {
        s = s.right(s.length() - 1);
    }
#endif

    return s;
}

XmlString RemoveScopeOperator(XmlString s)
{
#ifndef CASE_CREATOR
    while (s.find(':') != string::npos)
    {
        s = s.replace(s.find(':'), 1, "");
    }
#else
    s = s.replace(":", "");
#endif

    return s;
}
