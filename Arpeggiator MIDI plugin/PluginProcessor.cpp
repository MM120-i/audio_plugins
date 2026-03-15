/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <JuceHeader.h>
#include <algorithm> 
#include <random> 

A1AudioProcessor::A1AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ),
    arpSpeed(0.5f),
    isAscending(true),
    currentNote(0),
    lastNoteValue(-1),
    rate(44100.0f),
    time(0),
    numOctaves(1),
    duration3(1.0f),
    longDuration(1.0f),
    shortDuration(1.0f)
#endif
{
}

const juce::String A1AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool A1AudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool A1AudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool A1AudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double A1AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int A1AudioProcessor::getNumPrograms()
{
    return 1;   
}

int A1AudioProcessor::getCurrentProgram()
{
    return 0;
}

void A1AudioProcessor::setCurrentProgram(int index)
{
}

const juce::String A1AudioProcessor::getProgramName(int index)
{
    return {};
}

void A1AudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

void A1AudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    notes.clear();
    currentNote = 0;
    lastNoteValue = -1;
    time = 0;
    rate = static_cast<float> (sampleRate);
    arpSpeed = 0.5;
    std::cout << arpSpeed;
    availableNotes.clear();
    longDuration = 1.0;
    shortDuration = 1.0;
    duration3 = 1.0;
}

void A1AudioProcessor::releaseResources()
{
    return;
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool A1AudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else

    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

// for debugging purposes
juce::String A1AudioProcessor::noteNumberToString(int noteNumber) {

    int octave = (noteNumber / 12) - 1;
    int noteIndex = noteNumber % 12;
    const char *noteNames[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    juce::String noteLetter = noteNames[noteIndex];

    return noteLetter + juce::String(octave);
}

void A1AudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    auto numSamples = buffer.getNumSamples();
    auto standardNoteDuration = static_cast<int>(std::ceil(rate * 0.25f * (0.1f + (1.0f - (arpSpeed)))));
    standardNoteDuration = static_cast<int>(standardNoteDuration * longDuration);
    standardNoteDuration = static_cast<int>(standardNoteDuration / shortDuration);
    standardNoteDuration = juce::jmax(1, standardNoteDuration);

    auto noteDuration = (currentNote == 0)
        ? static_cast<int>(standardNoteDuration * duration3)
        : standardNoteDuration;

    for (const auto metadata : midi) {
        const auto msg = metadata.getMessage();

        if (msg.isNoteOn()) {
            if (!notes.contains(msg.getNoteNumber())) 
                notes.add(msg.getNoteNumber());

        else if (msg.isNoteOff()) 
            notes.removeValue(msg.getNoteNumber());
    }

    midi.clear();

    if ((time + numSamples) >= noteDuration) {
        auto offset = juce::jmax(0, juce::jmin((int)(noteDuration - time), numSamples - 1));

        if (lastNoteValue > 0) {
            midi.addEvent(juce::MidiMessage::noteOff(1, lastNoteValue), offset);
            lastNoteValue = -1;
        }

        if (notes.size() == 0) {
            currentNote = 0;
            time = 0;
            return;
        }

        if (isRandom) {
            bool flag2 = isAscending;
            handleRandom(midi, offset, flag2);
        }
        else if (isAlternative) {
            bool flag = isAscending;
            handleAlternative(midi, offset, flag);
        }
        else if (isAscending) {
            handleAscending(midi, offset);
        }
        else if (isDescending) {
            handleDescending(midi, offset);
        }

        if (longDuration > 1.0) {
            longDuration += 0.6f;
            longDuration = juce::jlimit(1.0f, 10.0f, longDuration);
        }

        if (shortDuration > 1.0) {
            shortDuration -= 0.6f;
            shortDuration = juce::jlimit(1.0f, 10.0f, shortDuration);
        }
    }

    time = (time + numSamples) % noteDuration;
}

void A1AudioProcessor::handleAlternative(juce::MidiBuffer& midi, int offset, bool flag)
{
    if (notes.size() > 0) {

        std::vector<int> arpeggioOrder;

        for (int note : notes) 
            arpeggioOrder.push_back(note);

        for (int i = notes.size() - 1; i >= 0; --i) 
            arpeggioOrder.push_back(notes[i]);

        int totalNotes = arpeggioOrder.size();
        int noteIndex = currentNote % totalNotes;
        int currentOctave = currentNote / totalNotes;

        if (!flag) 
            currentOctave = -currentOctave;

        lastNoteValue = arpeggioOrder[noteIndex] + (currentOctave * 12);

        if (lastNoteValue <= MAX_MIDI_VALUE)
            midi.addEvent(juce::MidiMessage::noteOn(1, lastNoteValue, (juce::uint8)MAX_MIDI_VALUE), offset);

        currentNote = (currentNote + 1) % (totalNotes * numOctaves);
    }
}

void A1AudioProcessor::handleRandom(juce::MidiBuffer& midi, int offset, bool flag)
{
    if (availableNotes.empty() || availableNotes.size() != notes.size())
    {
        availableNotes.clear();

        for (auto note : notes) 
            availableNotes.push_back(note);

        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(availableNotes.begin(), availableNotes.end(), g);
        currentNote = 0;
    }

    if (availableNotes.size() > 0)
    {
        int noteIndexInNotes = currentNote % availableNotes.size();
        int currentOctave = currentNote / availableNotes.size();
        lastNoteValue = availableNotes[noteIndexInNotes];

        if (!flag) 
            currentOctave = -currentOctave;

        lastNoteValue += (currentOctave * 12);

        if (lastNoteValue <= MAX_MIDI_VALUE)
            midi.addEvent(juce::MidiMessage::noteOn(1, lastNoteValue, (juce::uint8)MAX_MIDI_VALUE), offset);

        currentNote = (static_cast<std::size_t>(currentNote) + 1) % (availableNotes.size() * numOctaves);

        if (currentNote == 0)
        {
            std::random_device rd;
            std::mt19937 g(rd());
            std::shuffle(availableNotes.begin(), availableNotes.end(), g);
        }
    }
}

void A1AudioProcessor::handleAscending(juce::MidiBuffer& midi, int offset)
{
    if (notes.size() > 0) {

        int noteIndexInNotes = currentNote % notes.size();
        int currentOctave = currentNote / notes.size();
        lastNoteValue = (notes[noteIndexInNotes] + currentOctave * 12);

        if (lastNoteValue <= MAX_MIDI_VALUE) 
            midi.addEvent(juce::MidiMessage::noteOn(1, lastNoteValue, (juce::uint8)MAX_MIDI_VALUE), offset);

        currentNote = (currentNote + 1) % (notes.size() * numOctaves);
    }
}

void A1AudioProcessor::handleDescending(juce::MidiBuffer& midi, int offset)
{
    if (notes.size() > 0) {

        int totalNotes = notes.size() * numOctaves;
        int currentOctave = currentNote / notes.size();
        int noteIndexInNotes = (notes.size() - 1 - (currentNote % notes.size()));
        lastNoteValue = notes[noteIndexInNotes] - currentOctave * 12;

        if (lastNoteValue >= 0 && lastNoteValue <= MAX_MIDI_VALUE) 
            midi.addEvent(juce::MidiMessage::noteOn(1, lastNoteValue, (juce::uint8)MAX_MIDI_VALUE), offset);

        currentNote = (currentNote + 1) % totalNotes;
    }
}

bool A1AudioProcessor::hasEditor() const
{
    return true; 
}

juce::AudioProcessorEditor* A1AudioProcessor::createEditor()
{
    return new A1AudioProcessorEditor(*this);
}

void A1AudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
}

void A1AudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new A1AudioProcessor();
}
