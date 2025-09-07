/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "SquareOscComponent.h"
#include "WaveOscComponent.h"
#include "NoiseOscComponent.h"
#include "Theme.h"
#include "EditorHost.h"

//==============================================================================
/**
*/
class GameBoySynthAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                          public EditorHost
{
public:
    GameBoySynthAudioProcessorEditor(GameBoySynthAudioProcessor&,
                                     juce::AudioProcessorValueTreeState&);
    ~GameBoySynthAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

    //==============================================================================
    juce::AudioProcessorValueTreeState::ButtonAttachment* attachButtonToParameter(
            const juce::String& parameterId, juce::Button& button) override;
    juce::AudioProcessorValueTreeState::ComboBoxAttachment* attachComboBoxToParameter(
            const juce::String& parameterId, juce::ComboBox& comboBox) override;
    juce::AudioProcessorValueTreeState::SliderAttachment* attachSliderToParameter(
            const juce::String& parameterId, juce::Slider& slider) override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    GameBoySynthAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& parameters;
    SquareOscComponent osc0;
    SquareOscComponent osc1;
    WaveOscComponent osc2;
    NoiseOscComponent osc3;
    juce::MidiKeyboardState keyboardState;
    juce::MidiKeyboardComponent keyboard;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GameBoySynthAudioProcessorEditor)
};
