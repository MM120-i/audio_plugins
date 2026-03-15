

/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

A2AudioProcessorEditor::A2AudioProcessorEditor(A2AudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{

    setSize(560, 550);
    setupTimeInterval();
    setupDrySlider();
    setupWetSlider();
    setupFeedbackSlider();
    setupZenoButton();
    setupBottomLabel();
}

void A2AudioProcessorEditor::setupTimeInterval()
{
    timeIntervalLabel.setText("Time Interval", juce::dontSendNotification);
    timeIntervalLabel.setJustificationType(juce::Justification::centred);
    timeIntervalLabel.setColour(juce::Label::textColourId, juce::Colour::fromRGB(57, 255, 20));
    
    addAndMakeVisible(&timeIntervalLabel);

    for (float i = 0.0f; i <= 3.0f; i += 0.1f)
        timeIntervalComboBox.addItem(juce::String(i, 1) + " sec", static_cast<int>(i * 10) + 1);

    timeIntervalComboBox.setSelectedId(1);
    timeIntervalComboBox.setColour(juce::ComboBox::backgroundColourId, juce::Colour::fromRGB(0, 30, 30));
    timeIntervalComboBox.setColour(juce::ComboBox::textColourId, juce::Colours::white);
    timeIntervalComboBox.setColour(juce::ComboBox::outlineColourId, juce::Colour::fromRGB(0, 200, 200));

    addAndMakeVisible(&timeIntervalComboBox);
    timeIntervalComboBox.addListener(this);
}

void A2AudioProcessorEditor::setupDrySlider()
{
    dryLabel.setText("Dry", juce::dontSendNotification);
    dryLabel.setJustificationType(juce::Justification::centred);
    dryLabel.setColour(juce::Label::textColourId, juce::Colour::fromRGB(57, 255, 20));
    addAndMakeVisible(&dryLabel);

    drySlider.setSliderStyle(juce::Slider::LinearBarVertical);
    drySlider.setRange(0.0, 1.0, 0.01);
    drySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    drySlider.setPopupDisplayEnabled(true, false, this);
    drySlider.setValue(1.0);

    drySlider.setColour(juce::Slider::thumbColourId, juce::Colour::fromRGB(0, 255, 255));
    drySlider.setColour(juce::Slider::trackColourId, juce::Colour::fromRGB(0, 200, 255));
    drySlider.setColour(juce::Slider::backgroundColourId, juce::Colour::fromRGB(50, 50, 50));
    drySlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    drySlider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour::fromRGB(30, 30, 30));

    addAndMakeVisible(&drySlider);
    drySlider.addListener(this);
}

void A2AudioProcessorEditor::setupWetSlider()
{
    wetLabel.setText("Wet", juce::dontSendNotification);
    wetLabel.setJustificationType(juce::Justification::centred);
    wetLabel.setColour(juce::Label::textColourId, juce::Colour::fromRGB(57, 255, 20));
    addAndMakeVisible(&wetLabel);

    wetSlider.setSliderStyle(juce::Slider::LinearBarVertical);
    wetSlider.setRange(0.0, 1.0, 0.01);
    wetSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    wetSlider.setPopupDisplayEnabled(true, false, this);
    wetSlider.setValue(0.5);

    wetSlider.setColour(juce::Slider::thumbColourId, juce::Colour::fromRGB(0, 255, 255));
    wetSlider.setColour(juce::Slider::trackColourId, juce::Colour::fromRGB(0, 200, 255));
    wetSlider.setColour(juce::Slider::backgroundColourId, juce::Colour::fromRGB(50, 50, 50));
    wetSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    wetSlider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour::fromRGB(30, 30, 30));

    addAndMakeVisible(&wetSlider);
    wetSlider.addListener(this);
}

void A2AudioProcessorEditor::setupFeedbackSlider()
{
    feedbackLabel.setText("Feedback", juce::dontSendNotification);
    feedbackLabel.setJustificationType(juce::Justification::centred);
    feedbackLabel.setColour(juce::Label::textColourId, juce::Colour::fromRGB(57, 255, 20));
    addAndMakeVisible(&feedbackLabel);

    feedbackSlider.setSliderStyle(juce::Slider::LinearBarVertical);
    feedbackSlider.setRange(0.0, 1.0, 0.01);
    feedbackSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    feedbackSlider.setPopupDisplayEnabled(true, false, this);
    feedbackSlider.setValue(0.5);

    feedbackSlider.setColour(juce::Slider::thumbColourId, juce::Colour::fromRGB(0, 255, 255));
    feedbackSlider.setColour(juce::Slider::trackColourId, juce::Colour::fromRGB(0, 200, 255));
    feedbackSlider.setColour(juce::Slider::backgroundColourId, juce::Colour::fromRGB(50, 50, 50));
    feedbackSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    feedbackSlider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour::fromRGB(30, 30, 30));

    addAndMakeVisible(&feedbackSlider);
    feedbackSlider.addListener(this);
}

void A2AudioProcessorEditor::setupZenoButton()
{
    zenoLabel.setText("Zeno", juce::dontSendNotification);
    zenoLabel.setJustificationType(juce::Justification::centred);
    zenoLabel.setColour(juce::Label::textColourId, juce::Colour::fromRGB(57, 255, 20));
    addAndMakeVisible(&zenoLabel);

    zenoButton.setToggleState(false, juce::NotificationType::dontSendNotification);
    zenoButton.setColour(juce::ToggleButton::textColourId, juce::Colours::white);
    zenoButton.setColour(juce::ToggleButton::tickColourId, juce::Colours::green);
    zenoButton.setColour(juce::ToggleButton::tickDisabledColourId, juce::Colours::darkgreen);
    addAndMakeVisible(&zenoButton);
    zenoButton.addListener(this);
}

void A2AudioProcessorEditor::setupBottomLabel()
{
    bottomLabel.setText("Note: To stop audio, disable Zeno and/or set feedback to 0.00", juce::dontSendNotification);
    bottomLabel.setJustificationType(juce::Justification::centredLeft);
    bottomLabel.setColour(juce::Label::textColourId, juce::Colour::fromRGB(57, 225, 20));
    addAndMakeVisible(&bottomLabel);

    additionalLabel.setText("Note: Zeno is buggy, works for time interval 1.0s, 2.0s, and 3.0s", juce::dontSendNotification);
    additionalLabel.setJustificationType(juce::Justification::centredLeft);
    additionalLabel.setColour(juce::Label::textColourId, juce::Colour::fromRGB(57, 225, 20)); // Same color as the bottom label
    addAndMakeVisible(&additionalLabel);
}

A2AudioProcessorEditor::~A2AudioProcessorEditor()
{
}

void A2AudioProcessorEditor::paint(juce::Graphics &g)
{
    juce::ColourGradient gradient(juce::Colours::transparentBlack, 0, 0, juce::Colours::darkcyan, getWidth(), getHeight(), false);
    g.setGradientFill(gradient);
    g.fillAll();
}

void A2AudioProcessorEditor::resized()
{
    timeIntervalLabel.setBounds(40, 10, 100, 20);
    timeIntervalComboBox.setBounds(40, 30, 100, 30);

    dryLabel.setBounds(190, 10, 40, 20);
    drySlider.setBounds(200, 30, 20, getHeight() - 60);

    wetLabel.setBounds(290, 10, 40, 20);
    wetSlider.setBounds(300, 30, 20, getHeight() - 60);

    feedbackLabel.setBounds(350, 10, 80, 20);
    feedbackSlider.setBounds(380, 30, 20, getHeight() - 60);

    zenoLabel.setBounds(470, 10, 50, 20);
    zenoButton.setBounds(480, 26, 100, 40);

    bottomLabel.setBounds(0, getHeight() - 33, getWidth(), 20);
    additionalLabel.setBounds(0, getHeight() - 20, getWidth(), 20);
}

void A2AudioProcessorEditor::sliderValueChanged(juce::Slider *slider)
{
    audioProcessor.dryLevel = drySlider.getValue();
    audioProcessor.wetLevel = wetSlider.getValue();
    audioProcessor.feedbackLevel = feedbackSlider.getValue();
}

void A2AudioProcessorEditor::comboBoxChanged(juce::ComboBox *comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == &timeIntervalComboBox)
    {
        auto selectedInterval = timeIntervalComboBox.getText().getFloatValue();
        audioProcessor.setTimeInterval(selectedInterval);
    }
}

void A2AudioProcessorEditor::buttonClicked(juce::Button *button)
{
    if (button == &zenoButton)
    {
        bool zenoEnabled = zenoButton.getToggleState();
        audioProcessor.setZenoEnabled(zenoEnabled);
    }
}
