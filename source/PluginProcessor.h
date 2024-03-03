#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

using namespace juce;

#if (MSVC)
#include "ipps.h"
#endif

class PluginProcessor : public juce::AudioProcessor
{
public:
    PluginProcessor();
    ~PluginProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void WriteToMP3(File fileToSaveTo, float minutes);
    int floor_div(int a, int b);

    void cacheBufferPosWhenClicked();
    void setMinutesToRecord(float minutes);

private:
    std::unique_ptr<AudioBuffer<float>> storedBuffer;
    int bufferPos = 0, bufferPosWhenClicked = 0;
    float minutesToRecord = 0.1f;  // this gets overwritten by the textField in the editor
    int sampRate = 44100; // this gets overwritten by the prepareToPlay function
    int maxBufferLengthInMinutes = 31; // the higher this is, the more memory it will take up
    // 44100 * 60 * 31 = 82,008,000 which is 82MB
    bool hasWrappedAroundBuffer = false; // if the app has been running long enough that the maxBufferLengthInMinutes is reached, this will be set to true
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginProcessor)
};
