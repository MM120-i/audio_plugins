
/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

A2AudioProcessor::A2AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations: AudioProcessor(BusesProperties()
    #if !JucePlugin_IsMidiEffect
    #if !JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
    #endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
    #endif
    ),
    rate(0.0f)
#endif
{
}

A2AudioProcessor::~A2AudioProcessor()
{
}

const juce::String A2AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool A2AudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool A2AudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool A2AudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double A2AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int A2AudioProcessor::getNumPrograms()
{
    return 1; 
}

int A2AudioProcessor::getCurrentProgram()
{
    return 0;
}

void A2AudioProcessor::setCurrentProgram(int index)
{
}

const juce::String A2AudioProcessor::getProgramName(int index)
{
    return {};
}

void A2AudioProcessor::changeProgramName(int index, const juce::String &newName)
{
}

void A2AudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    rate = static_cast<float>(sampleRate);
    int delayBufferLengthInSamples = static_cast<int>(timeInterval * sampleRate);
    delayBufferLength = delayBufferLengthInSamples > 0 ? delayBufferLengthInSamples : 1;
    delayBuffer.setSize(2, delayBufferLength);
    delayBuffer.clear();
    delayBufferWritePos = 0;
}

void A2AudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool A2AudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void A2AudioProcessor::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    auto numSamples = buffer.getNumSamples();
    float fixedValue = 0.5f;

    if (timeInterval != previousTimeInterval)
    {
        counter = 0;
        previousTimeInterval = timeInterval;
    }

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        float *channelData = buffer.getWritePointer(channel);
        float *delayData = delayBuffer.getWritePointer(channel);
        float feedback = zenoEnabled ? 1.0f : feedbackLevel;
        int baseDelay = static_cast<int>(timeInterval * rate);
        float currentDelay = baseDelay;

        for (size_t sample = 0; sample < numSamples; ++sample)
        {
            if (zenoEnabled && currentDelay > 1)
            {
                if (counter >= 50 && counter < 200)
                    fixedValue = 0.38f;
                else if (counter >= 300 && counter < 500)
                    fixedValue = 0.34f;
                else if (counter >= 500 && counter < 700)
                    fixedValue = 0.28f;
                else if (counter >= 700 && counter < 900)
                    fixedValue = 0.24f;
                else if (counter >= 900)
                    fixedValue = 0.20f;

                currentDelay = (baseDelay * std::pow(fixedValue, (sample / baseDelay)) * fixedValue);
                currentDelay = std::max(currentDelay, 1.0f);
            }

            int delayReadPos = (delayBufferWritePos + delayBufferLength - static_cast<int>(currentDelay)) % delayBufferLength;
            float delayedSample = delayData[delayReadPos];
            delayData[delayBufferWritePos] = channelData[sample] + (delayedSample * feedback);
            channelData[sample] = (channelData[sample] * dryLevel) + (delayedSample * wetLevel);

            if (++delayBufferWritePos >= delayBufferLength)
                delayBufferWritePos = 0;
        }
    }

    counter++;
}

void A2AudioProcessor::setTimeInterval(float newTimeInterval)
{
    timeInterval = newTimeInterval;

    if (rate > 0)
    {
        int delayBufferLengthInSamples = static_cast<int>(timeInterval * rate);
        delayBufferLength = delayBufferLengthInSamples > 0 ? delayBufferLengthInSamples : 1;
        delayBuffer.setSize(2, delayBufferLength);
        delayBuffer.clear();
    }
}

void A2AudioProcessor::setZenoEnabled(bool isEnabled)
{
    zenoEnabled = isEnabled;
}

bool A2AudioProcessor::hasEditor() const
{
    return true; 
}

juce::AudioProcessorEditor *A2AudioProcessor::createEditor()
{
    return new A2AudioProcessorEditor(*this);
}

void A2AudioProcessor::getStateInformation(juce::MemoryBlock &destData)
{
}

void A2AudioProcessor::setStateInformation(const void *data, int sizeInBytes)
{
}

juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new A2AudioProcessor();
}
