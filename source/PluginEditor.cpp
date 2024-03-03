#include "PluginEditor.h"

// todo: Create an option that will record all the way back to the last time you saved a recording (as long as it was within the time equal to the max buffer size)

PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);

    //addAndMakeVisible (inspectButton);
    addAndMakeVisible (textField);
    addAndMakeVisible (saveFileButton);
    addAndMakeVisible(includeTimestampCheckbox);
    addAndMakeVisible(isMutedCheckbox);
    addAndMakeVisible(minutesLabel);

    minutesLabel.setText("Minutes:", juce::dontSendNotification);
    minutesLabel.attachToComponent(&textField, true);

    isMutedCheckbox.onClick = [this] { *processorRef.isMutedParam = isMutedCheckbox.getToggleState(); };
    isMutedCheckbox.setToggleState(*processorRef.isMutedParam, juce::dontSendNotification);

    includeTimestampCheckbox.onClick = [this] { *processorRef.includeTimestampParam = includeTimestampCheckbox.getToggleState(); };
    includeTimestampCheckbox.setToggleState(*processorRef.includeTimestampParam, juce::dontSendNotification);


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

        // Create a datestamp for the filename
        juce::Time time = juce::Time::getCurrentTime();
        juce::String datestamp = time.formatted("%Y-%m-%d_%H-%M-%S");

        //juce::FileChooser dummyFileChooser ("this shouldnt show", {}, "*.mp3");
        //dummyFileChooser.getInitialFileOrDirectory();

        //auto initFile = File::getCurrentWorkingDirectory().getChildFile(datestamp + ".mp3");

        //auto filePattern = *processorRef.includeTimestampParam ? "_AudCap_" + datestamp + ".mp3" : "*.mp3";
        auto filePattern = "*.mp3";

        juce::FileChooser fileChooser ("Save a file", {}, filePattern);

        if (fileChooser.browseForFileToSave(true)) {
            auto file = fileChooser.getResult();


            float minutes = textField.getText().getFloatValue();
            //std::cout << "Writing MP3..." << std::endl;
            processorRef.WriteToMP3(file, minutes);

            if (*processorRef.includeTimestampParam) {
                auto newFileName = "AudCap_" + datestamp + "__" + file.getFileName();
                if (renameFile(file, newFileName)) {
                    cout << "Renamed file to " << newFileName << endl;
                    file = file.getSiblingFile(newFileName);
                }
                else {
                    cout << "Failed to rename file to " << newFileName << endl;
                }
            }
        }
    };

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
}

PluginEditor::~PluginEditor()
{
}

bool PluginEditor::renameFile (const juce::File& f, const juce::String& newName)
{
    return f.moveFileTo (f.getSiblingFile (newName));
}

void PluginEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    //auto area = getLocalBounds();
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
    int elementHeight = 30;
    int numberOfElements = 4;
    int padding = (area.getHeight() - (numberOfElements * elementHeight)) / (numberOfElements + 1);
    
    // auto bottomArea = area.removeFromBottom(area.getHeight() / 2);
    // auto topArea = getLocalBounds().removeFromTop(getLocalBounds().getHeight() / 2);
    // textField.setBounds (topArea.removeFromTop(topArea.getHeight() / 2).withSizeKeepingCentre(60, 30));
    // saveFileButton.setBounds(topArea.removeFromBottom(topArea.getHeight() / 2).withSizeKeepingCentre(100, 30));
    // isMutedCheckbox.setBounds (bottomArea.withSizeKeepingCentre(100, 50));

    area.removeFromTop(padding);
    textField.setBounds (area.removeFromTop(elementHeight).withSizeKeepingCentre(60, 30));
    area.removeFromTop(padding);
    saveFileButton.setBounds(area.removeFromTop(elementHeight).withSizeKeepingCentre(100, 30));
    area.removeFromTop(padding);
    includeTimestampCheckbox.setBounds (area.removeFromTop(elementHeight).withSizeKeepingCentre(160, 30));
    area.removeFromTop(padding);
    isMutedCheckbox.setBounds (area.removeFromTop(elementHeight).withSizeKeepingCentre(100, 30));
}
