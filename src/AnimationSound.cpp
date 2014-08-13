/**
 * Represents a sound to be played during an animation.
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

#include "AnimationSound.h"
#include "CaseInformation/Case.h"
#include "XmlReader.h"

AnimationSound * AnimationSound::LoadSoundFromXml(XmlReader *pReader)
{
    if (pReader->ElementExists("HoofstepSound"))
    {
        return new HoofstepSound(pReader);
    }
    else if (pReader->ElementExists("SpecifiedSound"))
    {
        return new SpecifiedSound(pReader);
    }
    else
    {
        throw Exception("Invalid sound type.");
    }
}

HoofstepSound::HoofstepSound()
{
}

HoofstepSound::HoofstepSound(XmlReader *pReader)
{
    pReader->StartElement("HoofstepSound");
    pReader->EndElement();
}

void HoofstepSound::Play(double volume)
{
    Case::GetInstance()->GetAudioManager()->PlayRandomHoofstepSound("Gravel", volume);
}

AnimationSound * HoofstepSound::Clone()
{
    AnimationSound *pSound = new HoofstepSound();
    return pSound;
}

SpecifiedSound::SpecifiedSound(const string &sfxId)
{
    this->sfxId = sfxId;
}

SpecifiedSound::SpecifiedSound(XmlReader *pReader)
{
    pReader->StartElement("SpecifiedSound");
    sfxId = pReader->ReadTextElement("SfxId");
    pReader->EndElement();
}

void SpecifiedSound::Play(double volume)
{
    playSound(sfxId, volume);
}

AnimationSound * SpecifiedSound::Clone()
{
    AnimationSound *pSound = new SpecifiedSound(sfxId);
    return pSound;
}
