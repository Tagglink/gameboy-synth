/*
  ==============================================================================

    NoiseOscComponent.h
    Created: 13 Mar 2021 2:09:59pm
    Author:  Charles Julian Knight

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "BasicControlsComponent.h"
#include "EditorHost.h"

//==============================================================================
/*
*/
class NoiseOscComponent  : public juce::Component
{
public:
    NoiseOscComponent(EditorHost* host);
    ~NoiseOscComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    BasicControlsComponent controls;
    juce::ComboBox shiftWidthPicker;

    std::unique_ptr<EditorHost::ComboBoxAttachment> shiftWidthPickerAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NoiseOscComponent)
};
