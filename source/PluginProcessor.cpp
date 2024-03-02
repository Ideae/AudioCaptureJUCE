#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <juce_audio_formats/juce_audio_formats.h>
#include <memory>
// #include <juce_core/juce_core.h>
// #include <juce_data_structures/juce_data_structures.h>
// #include <juce_audio_processors/juce_audio_processors.h>

// #include "LAMEEncoderAudioFormat.h"
// #include <juce_LAMEEncoderAudioFormat.h>

using namespace juce;
using namespace std;

//==============================================================================
PluginProcessor::PluginProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
                       //, storedBuffer(AudioBuffer<float>(2, 48000 * (60 * 2))) // start at 2 minutes for now
{
    // juce::ignoreUnused (storedBuffer);
    // storedBuffer = AudioBuffer<float>(2, 48000);
}

PluginProcessor::~PluginProcessor()
{
}

//==============================================================================
const juce::String PluginProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PluginProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PluginProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PluginProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PluginProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PluginProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PluginProcessor::getCurrentProgram()
{
    return 0;
}

void PluginProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String PluginProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void PluginProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void PluginProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    std::cout << "Sample Rate: " << sampleRate << std::endl;
    sampRate = (int)sampleRate;
    // Rest of the code...
    //storedBuffer(AudioBuffer<float>(2, 48000 * (60 * 2))) // start at 2 minutes for now
    int maxBufferLengthInMinutes = 3;
    storedBuffer.reset(new AudioBuffer<float>(2, sampRate * (60 * maxBufferLengthInMinutes)));
}

void PluginProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool PluginProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
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

void PluginProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // std::cout << "Total number of input channels: " << totalNumInputChannels << std::endl;
    // std::cout << "Total number of output channels: " << totalNumOutputChannels << std::endl;

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        // juce::ignoreUnused (channelData);
        // ..do something to the data...
        auto numSamples = buffer.getNumSamples();
        auto numSamplesStoredBuffer = storedBuffer->getNumSamples();
        //storedBuffer.copyFrom(channel, bufferPos, buffer, channel, 0, numSamples);
        int bufferPosTemp = bufferPos;
        for (int i = 0; i < numSamples; i++)
        {
            storedBuffer->setSample(channel, bufferPosTemp, channelData[i]);
            bufferPosTemp++;
            if (bufferPosTemp >= numSamplesStoredBuffer)
            {
                bufferPosTemp = 0;
            }
        }
        if (channel == totalNumInputChannels - 1) {
            bufferPos = bufferPosTemp;
        }
    }
}

// int PluginProcessor::floor_div(int a, int b)
// {
//     //assert(b != 0);
//     if (b == 0) {
//         return 0;
//     }
//     div_t r = div(a, b);
//     if (r.rem != 0 && ((a < 0) ^ (b < 0)))
//         r.quot--;
//     return r.quot;
// }

int PluginProcessor::floor_div(int a, int b) {
    int d = a / b;
    if (a < 0 != b < 0) {  /* negative output (check inputs since 'd' isn't floored) */
        if (d * a != b) {  /* avoid modulo, use multiply instead */
            d -= 1;        /* floor */
        }
    }
    return d;
}


void PluginProcessor::cacheBufferPosWhenClicked() {
    bufferPosWhenClicked = bufferPos;
}

void PluginProcessor::WriteToMP3(File fileToSaveTo) {
    auto TEMP = File::getSpecialLocation(File::SpecialLocationType::tempDirectory).getFullPathName() + File::getSeparatorString();
        // auto LAME_EXE = File(TEMP + "lame.exe");
        auto LAME_EXE = File(TEMP + "lame"); //todo: it should instead use a named resource instead with the lame exec in the binary data, and in the current directory, I think.

        if (!LAME_EXE.existsAsFile())
        {
            cout << "LAME Executable does not exist as file at location: " << TEMP << endl;
            int size(0);
            auto data = BinaryData::getNamedResource(juce::String("lame").replace(".", "_").replace("-", "").toRawUTF8(), size);
            if (size > 0)
                if (LAME_EXE.create().ok())
                    LAME_EXE.appendData(data, size);
        }
        else {
            cout << "LAME execubatble did exist as file." << endl;
        }

                //juce::File outFile(TEMP + "output.mp3"); //AI.

                //auto buffer = storedBuffer; // todo: start from bufferPos - duration, and only go for as long as the requested duration


                // std::unique_ptr<juce::StringPairArray> WavMetadata; // AI.
                // WavMetadata->set("id3title", String());
                // WavMetadata->set("id3artist", String());
                // WavMetadata->set("id3album", String());

                // StringPairArray WavMetadata; // AI.
                // WavMetadata.set("id3title", String());
                // WavMetadata.set("id3artist", String());
                // WavMetadata.set("id3album", String());

                int samplesToWrite = sampRate * (int)(minutesToRecord * 60.0f);

                int finalBufferPos = bufferPosWhenClicked;
                int initialBufferPos = negativeAwareModulo(finalBufferPos - samplesToWrite, storedBuffer->getNumSamples());

                fileToSaveTo.deleteFile();

                std::unique_ptr<LAMEEncoderAudioFormat> format;
                format.reset(new LAMEEncoderAudioFormat(LAME_EXE));
                // auto qualityOptions = format->getQualityOptions();
                // for (auto i = 0; i < qualityOptions.size(); i++) {
                //     cout << "qualityOptions[" << i << "] = " << qualityOptions[i] << endl;
                // }
                std::unique_ptr<AudioFormatWriter> writer;
                // writer.reset(format->createWriterFor(new FileOutputStream(fileToSaveTo), sampleRate, 2, 16, WavMetadata, 23)); // CBR 320 Kbps
                writer.reset(format->createWriterFor(new FileOutputStream(fileToSaveTo), sampRate, 2, 16, {}, 23)); // CBR 320 Kbps
                if (writer != nullptr) {
                    writer->writeFromAudioSampleBuffer(*storedBuffer, initialBufferPos, samplesToWrite);
                    cout << "Done writing mp3 file." << endl;
                }
                else {
                    cout << "Writer was null." << endl;
                }
        //return true;
}

//==============================================================================
bool PluginProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PluginProcessor::createEditor()
{
    return new PluginEditor (*this);
}

//==============================================================================
void PluginProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
}

void PluginProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}
