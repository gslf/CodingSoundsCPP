#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class CodingSoundsSynthAudioProcessorEditor : public juce::AudioProcessorEditor, 
                                              public juce::Timer{
public:
	CodingSoundsSynthAudioProcessorEditor(CodingSoundsSynthAudioProcessor&);
	~CodingSoundsSynthAudioProcessorEditor() override;
	void paint(juce::Graphics&) override;
	void resized() override;

	void timerCallback() override;
private:
	CodingSoundsSynthAudioProcessor& audioProcessor;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CodingSoundsSynthAudioProcessorEditor)
};
