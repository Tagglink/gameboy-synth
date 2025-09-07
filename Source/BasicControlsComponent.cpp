/*
  ==============================================================================

    BasicControlsComponent.cpp
    Created: 13 Mar 2021 10:16:57am
    Author:  Charles Julian Knight

  ==============================================================================
*/

#include <JuceHeader.h>
#include "BasicControlsComponent.h"
#include "Synth.h"
#include "EditorHost.h"

//==============================================================================

BasicControlsComponent::BasicControlsComponent(OSCID id, EditorHost* host) :
    enableButton("Enable"),
    volSlider("Volume"),
    pwmSlider("PWM"),
    voicePicker("Voice"),
    channelPicker("Channel"),
    transposePicker("Transpose"),
    envelopeStepSlider("Envelope rate"),
    envelopeDirPicker("Envelope direction")
{
    id_ = id;

    // enable
    juce::String enableButtonParamId = juce::String("osc{}enable").replace("{}", juce::String((int) id));
    enableButtonAttach.reset(host->attachButtonToParameter(enableButtonParamId, enableButton));
    addAndMakeVisible(enableButton);
    // volume
    if (id != 2) {
        juce::String volSliderParamId = juce::String("osc{}volume").replace("{}", juce::String((int) id));
        volSliderAttach.reset(host->attachSliderToParameter(volSliderParamId, volSlider));
        volSlider.setSliderStyle(juce::Slider::Rotary);
        volSlider.setTextBoxStyle(pwmSlider.TextBoxBelow, true, 0, 0);
        addAndMakeVisible(volSlider);
    }
    // pwm
    if (id == 0 || id == 1) {
        juce::String pwmSliderParamId = juce::String("osc{}pwm").replace("{}", juce::String((int) id));
        pwmSliderAttach.reset(host->attachSliderToParameter(pwmSliderParamId, pwmSlider));
        pwmSlider.setSliderStyle(juce::Slider::Rotary);
        pwmSlider.setTextBoxStyle(pwmSlider.TextBoxBelow, true, 0, 0);
        addAndMakeVisible(pwmSlider);
    }
    // voice
    juce::String voicePickerParamId = juce::String("osc{}voice").replace("{}", juce::String((int) id));
    voicePickerAttach.reset(host->attachComboBoxToParameter(voicePickerParamId, voicePicker));
    addAndMakeVisible(voicePicker);
    // channel
    juce::String channelPickerParamId = juce::String("osc{}channel").replace("{}", juce::String((int) id));
    channelPickerAttach.reset(host->attachComboBoxToParameter(channelPickerParamId, channelPicker));
    addAndMakeVisible(channelPicker);
    // transpose
    juce::String transposePickerParamId = juce::String("osc{}transpose").replace("{}", juce::String((int) id));
    transposePickerAttach.reset(host->attachComboBoxToParameter(transposePickerParamId, transposePicker));
    addAndMakeVisible(transposePicker);
    // envelope
    if (id != 2) {
        juce::String envelopeStepSliderParamId = juce::String("osc{}envstep").replace("{}", juce::String((int) id));
        envelopeStepSliderAttach.reset(host->attachSliderToParameter(envelopeStepSliderParamId, envelopeStepSlider));
        envelopeStepSlider.setSliderStyle(juce::Slider::Rotary);
        envelopeStepSlider.setTextBoxStyle(envelopeStepSlider.TextBoxBelow, true, 0, 0);
        addAndMakeVisible(envelopeStepSlider);

        juce::String envelopeDirPickerParamId = juce::String("osc{}envdir").replace("{}", juce::String((int) id));
        envelopeDirPickerAttach.reset(host->attachComboBoxToParameter(envelopeDirPickerParamId, envelopeDirPicker));
        addAndMakeVisible(envelopeDirPicker);
    }
}

BasicControlsComponent::~BasicControlsComponent() {}

void BasicControlsComponent::paint (juce::Graphics& g) {}

void BasicControlsComponent::resized()
{
    juce::Rectangle<int> bounds = getLocalBounds();
    int left = 0;
    int height = bounds.getHeight();
    // enable
    enableButton.setBounds(left, 0, height, height);
    left = enableButton.getBounds().getRight();
    // volume
    volSlider.setBounds(left, 0, height, height);
    volSlider.setTextBoxStyle(pwmSlider.TextBoxBelow, true, volSlider.getBounds().getWidth(), volSlider.getBounds().getHeight()/4);
    left = volSlider.getBounds().getRight();
    // pwm
    if (id_ == 0 || id_ == 1) {
        pwmSlider.setBounds(left, 0, height, height);
        pwmSlider.setTextBoxStyle(pwmSlider.TextBoxBelow, true, pwmSlider.getBounds().getWidth(), pwmSlider.getBounds().getHeight()/4);
        left = pwmSlider.getBounds().getRight();
    }
    // pickers
    static int pickerHeight = 25;
    int pickerPad = std::max((height - (3 * pickerHeight)) / 2, 0);
    voicePicker.setBounds(left, pickerPad, height, pickerHeight);
    channelPicker.setBounds(left, voicePicker.getBounds().getBottom(), height, pickerHeight);
    transposePicker.setBounds(left, channelPicker.getBounds().getBottom(), height, pickerHeight);
    left = transposePicker.getBounds().getRight();
    // envelope step slider
    envelopeStepSlider.setBounds(left, 0, height, height);
    envelopeStepSlider.setTextBoxStyle(envelopeStepSlider.TextBoxBelow, true, envelopeStepSlider.getBounds().getWidth(), envelopeStepSlider.getBounds().getHeight() / 4);
    left = envelopeStepSlider.getBounds().getRight();
    // envelope direction picker
    envelopeDirPicker.setBounds(left, height - pickerHeight, height, pickerHeight);
}

