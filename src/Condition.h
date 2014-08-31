/**
 * Basic header/include file for Condition.cpp.
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

#ifndef CONDITION_H
#define CONDITION_H

#include <string>

using namespace std;

class XmlReader;

class Condition
{
public:
    Condition()
    {
        pCriterion = NULL;
    }

    Condition(XmlReader *pReader);
    ~Condition();

    bool IsTrue();
    Condition * Clone();

    class ConditionCriterion
    {
    public:
        virtual ~ConditionCriterion() {}

        virtual bool IsTrue() = 0;
        virtual ConditionCriterion * Clone() = 0;

        static ConditionCriterion * LoadFromXml(XmlReader *pReader);
    };

    class FlagSetCondition : public ConditionCriterion
    {
    public:
        FlagSetCondition(const string &flagId);
        FlagSetCondition(XmlReader *pReader);
        ~FlagSetCondition();

        bool IsTrue();
        ConditionCriterion * Clone();

    private:
        string flagId;
    };

    class EvidencePresentCondition : public ConditionCriterion
    {
    public:
        EvidencePresentCondition(const string &partnerId);
        EvidencePresentCondition(XmlReader *pReader);
        ~EvidencePresentCondition();

        bool IsTrue();
        ConditionCriterion * Clone();

    private:
        string evidenceId;
    };

    class PartnerPresentCondition : public ConditionCriterion
    {
    public:
        PartnerPresentCondition(const string &partnerId);
        PartnerPresentCondition(XmlReader *pReader);
        ~PartnerPresentCondition();

        bool IsTrue();
        ConditionCriterion * Clone();

    private:
        string partnerId;
    };

    class ConversationLockedCondition : public ConditionCriterion
    {
    public:
        ConversationLockedCondition(const string &conversationId);
        ConversationLockedCondition(XmlReader *pReader);
        ~ConversationLockedCondition();

        bool IsTrue();
        ConditionCriterion * Clone();

    private:
        string conversationId;
    };

    class TutorialsEnabledCondition : public ConditionCriterion
    {
    public:
        TutorialsEnabledCondition();
        TutorialsEnabledCondition(XmlReader *pReader);
        ~TutorialsEnabledCondition();

        bool IsTrue();
        ConditionCriterion * Clone();
    };

    class AndCondition : public ConditionCriterion
    {
    public:
        AndCondition(ConditionCriterion *pFirstCriterion, ConditionCriterion *pSecondCriterion);
        AndCondition(XmlReader *pReader);
        ~AndCondition();

        bool IsTrue();
        ConditionCriterion * Clone();

    private:
        ConditionCriterion *pFirstCriterion;
        ConditionCriterion *pSecondCriterion;
    };

    class OrCondition : public ConditionCriterion
    {
    public:
        OrCondition(ConditionCriterion *pFirstCriterion, ConditionCriterion *pSecondCriterion);
        OrCondition(XmlReader *pReader);
        ~OrCondition();

        bool IsTrue();
        ConditionCriterion * Clone();

    private:
        ConditionCriterion *pFirstCriterion;
        ConditionCriterion *pSecondCriterion;
    };

    class NotCondition : public ConditionCriterion
    {
    public:
        NotCondition(ConditionCriterion *pCriterion);
        NotCondition(XmlReader *pReader);
        ~NotCondition();

        bool IsTrue();
        ConditionCriterion * Clone();

    private:
        ConditionCriterion *pCriterion;
    };

private:
    ConditionCriterion *pCriterion;
};

#endif
