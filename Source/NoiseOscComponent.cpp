/*
  ==============================================================================

    NoiseOscComponent.cpp
    Created: 13 Mar 2021 2:09:59pm
    Author:  Charles Julian Knight

  ==============================================================================
*/

#include <JuceHeader.h>
#include "NoiseOscComponent.h"
#include "Theme.h"

//==============================================================================
NoiseOscComponent::NoiseOscComponent(EditorHost* host) :
    controls(3, host),
    shiftWidthPicker("Width")
{
    addAndMakeVisible(controls);

    shiftWidthPickerAttach.reset(host->attachComboBoxToParameter("osc3shiftwidth", shiftWidthPicker));
    addAndMakeVisible(shiftWidthPicker);
}

NoiseOscComponent::~NoiseOscComponent() {}

void NoiseOscComponent::paint(juce::Graphics& g)
{
    g.setColour(getLookAndFeel().findColour(GameBoyColorIds::OscOutlineColorId));
    g.drawRect(getLocalBounds());
}

void NoiseOscComponent::resized()
{
    juce::Rectangle<int> bounds = getLocalBounds();
    int rowUnit = bounds.proportionOfHeight(0.25);
    controls.setBounds(0, 0, bounds.getWidth(), rowUnit);

    int left = rowUnit;
    static int pickerHeight = 25;
    shiftWidthPicker.setBounds(left, rowUnit + rowUnit / 2 - pickerHeight / 2, rowUnit, pickerHeight);
}

