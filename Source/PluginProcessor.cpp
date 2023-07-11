/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MultibandCrossoverAudioProcessor::MultibandCrossoverAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), parameters(*this, nullptr, "PARAMETERS", initializeGUI())
#endif
{

    initializeDSP();

}

MultibandCrossoverAudioProcessor::~MultibandCrossoverAudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout MultibandCrossoverAudioProcessor::initializeGUI()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;


    params.push_back(std::make_unique<juce::AudioParameterFloat>("BAND1_gain_ID", "Band1_gain", 0, 1, 1));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("BAND2_gain_ID", "Band2_gain", 0, 1, 1));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("BAND3_gain_ID", "Band3_gain", 0, 1, 1));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("f1", "f1", 20, 20000, 500));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("f2", "f2", 20, 20000, 3000));



    return{ params.begin() , params.end() };


}

void MultibandCrossoverAudioProcessor::initializeDSP()
{
    for (int x = 0; x < getTotalNumOutputChannels(); x++)
    {
        ptrBiquad1[x] = std::unique_ptr<BiquadFilter>(new BiquadFilter());
        ptrBiquad2[x] = std::unique_ptr<BiquadFilter>(new BiquadFilter());
        ptrBiquad3[x] = std::unique_ptr<BiquadFilter>(new BiquadFilter());
    }
}

//==============================================================================
const juce::String MultibandCrossoverAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MultibandCrossoverAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MultibandCrossoverAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MultibandCrossoverAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MultibandCrossoverAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MultibandCrossoverAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MultibandCrossoverAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MultibandCrossoverAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String MultibandCrossoverAudioProcessor::getProgramName (int index)
{
    return {};
}

void MultibandCrossoverAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void MultibandCrossoverAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    data.samplerate(sampleRate);
}

void MultibandCrossoverAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MultibandCrossoverAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void MultibandCrossoverAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();


    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    //create a new buffer for each filter

    juce::AudioBuffer<float> band1Buffer(totalNumInputChannels, buffer.getNumSamples());
    juce::AudioBuffer<float> band2Buffer(totalNumInputChannels, buffer.getNumSamples());
    juce::AudioBuffer<float> band3Buffer(totalNumInputChannels, buffer.getNumSamples());
    band1Buffer.clear();
    band2Buffer.clear();
    band3Buffer.clear();


    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);

        //band 1 
        ptrBiquad1[channel]->processBiquadLP(channelData, band1Buffer.getWritePointer(channel), buffer.getNumSamples(), getSampleRate(), *parameters.getRawParameterValue("f1"), *parameters.getRawParameterValue("BAND1_gain_ID"));

        //band 2 
        ptrBiquad2[channel]->processBiquadBP(channelData, band2Buffer.getWritePointer(channel), buffer.getNumSamples(), getSampleRate(), *parameters.getRawParameterValue("f1"), *parameters.getRawParameterValue("f2"), *parameters.getRawParameterValue("BAND2_gain_ID"));

        //band 3 
        ptrBiquad3[channel]->processBiquadHP(channelData, band3Buffer.getWritePointer(channel), buffer.getNumSamples(), getSampleRate(), *parameters.getRawParameterValue("f2"), *parameters.getRawParameterValue("BAND3_gain_ID"));
    }

    // sum of three filters

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);

        auto* band1Data = band1Buffer.getReadPointer(channel);
        auto* band2Data = band2Buffer.getReadPointer(channel);
        auto* band3Data = band3Buffer.getReadPointer(channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            channelData[sample] = band1Data[sample] + band2Data[sample] + band3Data[sample];
        }
    }
}

//==============================================================================
bool MultibandCrossoverAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MultibandCrossoverAudioProcessor::createEditor()
{
    return new MultibandCrossoverAudioProcessorEditor (*this);
}

//==============================================================================
void MultibandCrossoverAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void MultibandCrossoverAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MultibandCrossoverAudioProcessor();
}
