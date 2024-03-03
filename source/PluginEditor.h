#pragma once

#include "PluginProcessor.h"
#include "BinaryData.h"
#include "melatonin_inspector/melatonin_inspector.h"

using namespace juce;
using namespace std;

//==============================================================================
class PluginEditor : public juce::AudioProcessorEditor
{
public:
    explicit PluginEditor (PluginProcessor&);
    ~PluginEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    bool renameFile (const juce::File& f, const juce::String& newName);

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PluginProcessor& processorRef;
    unique_ptr<melatonin::Inspector> inspector;
    TextButton inspectButton { "Inspect the UI!" };
    TextButton saveFileButton { "Save MP3 File" };
    TextEditor textField { "Minutes" };
    ToggleButton isMutedCheckbox { "Mute Output" };
    ToggleButton includeTimestampCheckbox { "Include Timestamp" };
    Label minutesLabel { "Minutes" };
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
