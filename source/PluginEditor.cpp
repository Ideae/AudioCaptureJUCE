#include "PluginEditor.h"

// todo: Create an option that will record all the way back to the last time you saved a recording (as long as it was within the time equal to the max buffer size)

PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);

    //addAndMakeVisible (inspectButton);
    addAndMakeVisible (textField);
    addAndMakeVisible (saveFileButton);

    textField.setText("3", juce::dontSendNotification);
    textField.setTextToShowWhenEmpty("minutes", juce::Colours::grey);
    textField.setJustification(juce::Justification::centred);
    
    inspectButton.onClick = [&] {
        if (!inspector)
        {
            inspector = std::make_unique<melatonin::Inspector> (*this);
            inspector->onClose = [this]() { inspector.reset(); };
        }

        inspector->setVisible (true);
    };

    saveFileButton.onClick = [&] {
        processorRef.cacheBufferPosWhenClicked();
        juce::FileChooser fileChooser ("Save a file", {}, "*.mp3");

        if (fileChooser.browseForFileToSave(true)) {
            auto file = fileChooser.getResult();

            float minutes = textField.getText().getFloatValue();
            //std::cout << "Writing MP3..." << std::endl;
            processorRef.WriteToMP3(file, minutes);
        }
    };

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
}

PluginEditor::~PluginEditor()
{
}

void PluginEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    auto area = getLocalBounds();
    g.setColour (juce::Colours::white);
    g.setFont (16.0f);
    // auto helloWorld = juce::String ("Hello from ") + PRODUCT_NAME_WITHOUT_VERSION + " v" VERSION + " running in " + CMAKE_BUILD_TYPE;
    // g.drawText (helloWorld, area.removeFromTop (150), juce::Justification::centred, false);
}

void PluginEditor::resized()
{
    // Add an input filter to the text field to accept only numeric input
    textField.setInputRestrictions(4, "0123456789.");
    // layout the positions of your child components here
    auto area = getLocalBounds();
    area.removeFromBottom(50);
    //inspectButton.setBounds (getLocalBounds().withSizeKeepingCentre(100, 50));
    textField.setBounds (getLocalBounds().removeFromTop(getLocalBounds().getHeight() / 2).withSizeKeepingCentre(60, 30));
    saveFileButton.setBounds (getLocalBounds().removeFromBottom(getLocalBounds().getHeight() / 2).withSizeKeepingCentre(100, 50));
}
