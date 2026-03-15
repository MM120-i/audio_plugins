/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

constexpr int MAX_MIDI_VALUE = 127;

class A1AudioProcessor : public juce::AudioProcessor
{

public:

    float arpSpeed;
    float longDuration;
    float shortDuration;
    float duration3;
    bool isAscending = true;
    bool isDescending = false;
    juce::String currentNoteDisplay;
    int numOctaves = 1;
    juce::String noteNumberToString(int noteNumber);
    bool isRandom = false;
    bool isAlternative = false;

    A1AudioProcessor();

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;
    void handleAscending(juce::MidiBuffer& midi, int offset);
    void handleDescending(juce::MidiBuffer& midi, int offset);
    void A1AudioProcessor::handleRandom(juce::MidiBuffer& midi, int offset, bool flag);
    void A1AudioProcessor::handleAlternative(juce::MidiBuffer& midi, int offset, bool flag);
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

private:

    int currentNote, lastNoteValue;
    int time;
    float rate;
    juce::SortedSet<int> notes;
    int currentOctaveIndex = 0;
    std::vector<int> availableNotes;
    std::vector<int> shuffledNotes;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(A1AudioProcessor)
};
