/*
  ==============================================================================

    EditorHost.h
    Created: 1 Sep 2025 7:35:56pm
    Author:  Tomas Granlund

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

/**
 * Plugin editor's interface towards the plugin processor.
 */
class EditorHost
{
public:
    typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
    typedef juce::AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;

    virtual ButtonAttachment* attachButtonToParameter(const juce::String& parameterID,
                                                      juce::Button& button) = 0;

    virtual ComboBoxAttachment* attachComboBoxToParameter(const juce::String& parameterID,
                                                          juce::ComboBox& comboBox) = 0;

    virtual SliderAttachment* attachSliderToParameter(const juce::String& parameterID,
                                                      juce::Slider& slider) = 0;
};
