/*
  ==============================================================================

    BasicControlsComponent.h
    Created: 13 Mar 2021 10:16:57am
    Author:  Charles Julian Knight

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Synth.h"
#include "EditorHost.h"

//==============================================================================
/*
*/
class BasicControlsComponent  : public juce::Component
{
public:
    BasicControlsComponent(OSCID id, EditorHost* host);
    ~BasicControlsComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    OSCID id_;
    juce::ToggleButton enableButton;
    juce::Slider volSlider;
    juce::Slider pwmSlider;
    juce::ComboBox voicePicker;
    juce::ComboBox channelPicker;
    juce::ComboBox transposePicker;
    juce::Slider envelopeStepSlider;
    juce::ComboBox envelopeDirPicker;

    std::unique_ptr<EditorHost::ButtonAttachment> enableButtonAttach;
    std::unique_ptr<EditorHost::SliderAttachment> volSliderAttach;
    std::unique_ptr<EditorHost::SliderAttachment> pwmSliderAttach;
    std::unique_ptr<EditorHost::ComboBoxAttachment> channelPickerAttach;
    std::unique_ptr<EditorHost::ComboBoxAttachment> voicePickerAttach;
    std::unique_ptr<EditorHost::ComboBoxAttachment> transposePickerAttach;
    std::unique_ptr<EditorHost::SliderAttachment> envelopeStepSliderAttach;
    std::unique_ptr<EditorHost::ComboBoxAttachment> envelopeDirPickerAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BasicControlsComponent)
};
