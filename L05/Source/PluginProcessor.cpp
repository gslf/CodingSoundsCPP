#include "PluginProcessor.h"
#include "PluginEditor.h"

CodingSoundsSynthAudioProcessor::CodingSoundsSynthAudioProcessor()
	: AudioProcessor(
		BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)){
}

CodingSoundsSynthAudioProcessor::~CodingSoundsSynthAudioProcessor() {
}

void CodingSoundsSynthAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
	gainSmoothed.reset(sampleRate, 0.02);
	gainSmoothed.setCurrentAndTargetValue(0.8f);
}

void CodingSoundsSynthAudioProcessor::releaseResources() {
	// EMPTY
}

void CodingSoundsSynthAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
	juce::ScopedNoDenormals noDenormals;
	auto totalNumInputChannels = getTotalNumInputChannels();
	auto totalNumOutputChannels = getTotalNumOutputChannels();

	for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
		buffer.clear(i, 0, buffer.getNumSamples());


	// MIDI PROCESSING
	for (const auto metadata : midiMessages)
	{
		auto message = metadata.getMessage();

		if (message.isNoteOn())
		{
			lastNoteNumber.store(message.getNoteNumber());
			noteOnCount++;
		}
	}

	// SOUND GENERATION 
	for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
	{
		float noise = random.nextFloat() * 2.0f - 1.0f;
		for (int channel = 0; channel < totalNumOutputChannels; ++channel)
			buffer.getWritePointer(channel)[sample] = noise;
	}

	// GAIN PROECSSING
	gainSmoothed.setTargetValue(0.1f);

	for (int sample = 0; sample < buffer.getNumSamples(); ++sample){
		float currentGain = gainSmoothed.getNextValue();
		for (int channel = 0; channel < totalNumOutputChannels; ++channel){
			buffer.getWritePointer(channel)[sample] *= currentGain;
		}
	}
	currentGainForDisplay.store(gainSmoothed.getTargetValue());
}

juce::AudioProcessorEditor* CodingSoundsSynthAudioProcessor::createEditor() {
	return new CodingSoundsSynthAudioProcessorEditor(*this);
}

bool CodingSoundsSynthAudioProcessor::hasEditor() const {
	return true;
}

const juce::String CodingSoundsSynthAudioProcessor::getName() const { return JucePlugin_Name; }
bool CodingSoundsSynthAudioProcessor::acceptsMidi() const { return true; }
bool CodingSoundsSynthAudioProcessor::producesMidi() const { return false; }
bool CodingSoundsSynthAudioProcessor::isMidiEffect() const { return false; }
double CodingSoundsSynthAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int CodingSoundsSynthAudioProcessor::getNumPrograms() { return 1; }
int CodingSoundsSynthAudioProcessor::getCurrentProgram() { return 0; }
void CodingSoundsSynthAudioProcessor::setCurrentProgram(int index) {}
const juce::String CodingSoundsSynthAudioProcessor::getProgramName(int index) { return {}; }
void CodingSoundsSynthAudioProcessor::changeProgramName(int index, const juce::String& newName) {}
void CodingSoundsSynthAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {}
void CodingSoundsSynthAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
	return new CodingSoundsSynthAudioProcessor();
}
