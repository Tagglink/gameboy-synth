/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Synth.h"

//==============================================================================
GameBoySynthAudioProcessorEditor::GameBoySynthAudioProcessorEditor(
        GameBoySynthAudioProcessor& p,
        juce::AudioProcessorValueTreeState& parameterState)
    : AudioProcessorEditor (&p),
        audioProcessor (p),
        parameters(parameterState),
        osc0(0, this),
        osc1(1, this),
        osc2(this),
        osc3(this),
        keyboard(keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    theme(getLookAndFeel());
    setSize(WindowWidth, WindowHeight);

    addAndMakeVisible(osc0);
    addAndMakeVisible(osc1);
    addAndMakeVisible(osc2);
    addAndMakeVisible(osc3);
    // keyboard
    keyboardState.addListener(audioProcessor.getMidiCollector());
    addAndMakeVisible(keyboard);
}

GameBoySynthAudioProcessorEditor::~GameBoySynthAudioProcessorEditor() {}

//==============================================================================
void GameBoySynthAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void GameBoySynthAudioProcessorEditor::resized()
{
    osc0.setBounds(0, 0, OscBoxWidth, OscBoxHeight);
    osc1.setBounds(OscBoxWidth, 0, OscBoxWidth, OscBoxHeight);
    osc2.setBounds(0, OscBoxHeight, OscBoxWidth, OscBoxHeight);
    osc3.setBounds(OscBoxWidth, OscBoxHeight, OscBoxWidth, OscBoxHeight);
    keyboard.setBounds(0, WindowHeight-KeyboardHeight, WindowWidth, KeyboardHeight);
}

//==============================================================================
EditorHost::ButtonAttachment* GameBoySynthAudioProcessorEditor::attachButtonToParameter(
        const juce::String& parameterId,
        juce::Button& button)
{
    return new juce::AudioProcessorValueTreeState::ButtonAttachment(parameters, parameterId, button);
}

EditorHost::ComboBoxAttachment* GameBoySynthAudioProcessorEditor::attachComboBoxToParameter(
        const juce::String& parameterId,
        juce::ComboBox& comboBox)
{
    juce::RangedAudioParameter* parameter = parameters.getParameter(parameterId);
    comboBox.addItemList(parameter->getAllValueStrings(), 1);
    return new juce::AudioProcessorValueTreeState::ComboBoxAttachment(parameters, parameterId, comboBox);
}

EditorHost::SliderAttachment* GameBoySynthAudioProcessorEditor::attachSliderToParameter(
        const juce::String& parameterId,
        juce::Slider& slider)
{
    return new juce::AudioProcessorValueTreeState::SliderAttachment(parameters, parameterId, slider);
}
