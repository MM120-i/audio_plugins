
/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

constexpr int NUMBER_OF_OCTAVES = 4;

class A1AudioProcessorEditor : public juce::AudioProcessorEditor, private juce::Slider::Listener, private juce::Button::Listener, private juce::Timer
{
public:

    A1AudioProcessorEditor(A1AudioProcessor&);
    ~A1AudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    
    void A1AudioProcessorEditor::setupAdditionalModesToggle();
    void A1AudioProcessorEditor::setupNoteDisplayLabel();
    void A1AudioProcessorEditor::setupArpSlider();
    void A1AudioProcessorEditor::setupAscendingButton(juce::Colour buttonColour);
    void A1AudioProcessorEditor::setupDescendingButton(juce::Colour buttonColour);
    void A1AudioProcessorEditor::setupRandomButton(juce::Colour buttonColour);
    void A1AudioProcessorEditor::setupAlternativeButton(juce::Colour buttonColour);
    void A1AudioProcessorEditor::setupRotarySlider();
    void A1AudioProcessorEditor::setupOctaveComboBox();

private:

    void sliderValueChanged(juce::Slider* slider) override;
    void buttonClicked(juce::Button* button) override;

    A1AudioProcessor& audioProcessor;

    juce::Slider arpSlider;
    juce::TextButton ascendingButton{ "Ascending" };
    juce::TextButton descendingButton{ "Descending" };
    juce::TextButton random{ "Randomizer" };
    juce::TextButton alternative{ "Alternative" };
    juce::Label noteDisplayLabel;
    juce::ComboBox octaveComboBox;
    juce::Slider rotarySlider;
    juce::Slider rotarySlider2;
    juce::Slider rotarySlider3;
    juce::Label rotaryValueLabel;
    juce::Label rotaryValueLabel2;
    juce::Label rotaryValueLabel3;
    juce::ToggleButton additionalModesToggle;

    bool isAscendingSelected = false;
    bool isDescendingSelected = false;
    bool isRandomSelected = false;
    bool isAlternativeSelected = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(A1AudioProcessorEditor)
};
