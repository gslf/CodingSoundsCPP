#include "PluginProcessor.h"
#include "PluginEditor.h"

CodingSoundsSynthAudioProcessorEditor::CodingSoundsSynthAudioProcessorEditor(CodingSoundsSynthAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p) {
    setSize(400, 300);
    startTimer(60);
}

CodingSoundsSynthAudioProcessorEditor::~CodingSoundsSynthAudioProcessorEditor() {
    stopTimer();
}

void CodingSoundsSynthAudioProcessorEditor::timerCallback() {
    repaint();
}

void CodingSoundsSynthAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
    
    int note = audioProcessor.lastNoteNumber.load();
    int count = audioProcessor.noteOnCount.load();

    g.drawText("MIDI DEBUGGER", getLocalBounds().removeFromTop(50), juce::Justification::centred);

    g.setFont(15.0f);
    juce::String text;

    if (note == -1) 
        text << "Waiting for MIDI ...";
    else 
        text << "Last Note: " << juce::MidiMessage::getMidiNoteName(note, true, true, 3) << " (" << note << ")";
 
    text << "\nTotal Note On Events: " << count;

    g.drawFittedText(text, getLocalBounds(), juce::Justification::centred, 10);
}

void CodingSoundsSynthAudioProcessorEditor::resized() {}
