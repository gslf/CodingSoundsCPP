#include "PluginProcessor.h"
#include "ParameterIDs.h"

static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout() {
	juce::AudioProcessorValueTreeState::ParameterLayout layout;

	// MASTER GAIN
	layout.add(std::make_unique<juce::AudioParameterFloat>(
		juce::ParameterID{ ParameterIDs::masterGain, 1 },
		"Master Gain",
		juce::NormalisableRange<float>(-60.0f, 6.0f, 0.1f),
		0.0f
	));

	// OSCILLATOR
	juce::StringArray waveChoices{ "Saw", "Square", "Sine" };

	layout.add(std::make_unique<juce::AudioParameterChoice>(
		juce::ParameterID{ ParameterIDs::osc1Wave, 1 },
		"OSC 1 Wave",
		waveChoices,
		0       // default: Saw
	));

	layout.add(std::make_unique<juce::AudioParameterChoice>(
		juce::ParameterID{ ParameterIDs::osc2Wave, 1 },
		"OSC 2 Wave",
		waveChoices,
		0
	));

	// OSCILLATOR MIX
	layout.add(std::make_unique<juce::AudioParameterFloat>(
		juce::ParameterID{ ParameterIDs::oscMix, 1 },
		"OSC Mix",
		juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
		0.5f
	));

	// FILTER
	layout.add(std::make_unique<juce::AudioParameterFloat>(
		juce::ParameterID{ ParameterIDs::filterCutoff, 1 },
		"Filter Cutoff",
		juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.25f),
		1000.0f
	));

	layout.add(std::make_unique<juce::AudioParameterFloat>(
		juce::ParameterID{ ParameterIDs::filterReso, 1 },
		"Filter Resonance",
		juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
		0.0f
	));

	// AMP ENVELOPE
	layout.add(std::make_unique<juce::AudioParameterFloat>(
		juce::ParameterID{ ParameterIDs::ampAttack, 1 },
		"Amp Attack",
		juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.4f),
		0.01f
	));

	layout.add(std::make_unique<juce::AudioParameterFloat>(
		juce::ParameterID{ ParameterIDs::ampDecay, 1 },
		"Amp Decay",
		juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.4f),
		0.1f
	));

	layout.add(std::make_unique<juce::AudioParameterFloat>(
		juce::ParameterID{ ParameterIDs::ampSustain, 1 },
		"Amp Sustain",
		juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
		0.8f
	));

	layout.add(std::make_unique<juce::AudioParameterFloat>(
		juce::ParameterID{ ParameterIDs::ampRelease, 1 },
		"Amp Release",
		juce::NormalisableRange<float>(0.001f, 10.0f, 0.001f, 0.4f),
		0.3f
	));

	// FILTER ENVELOPE
	layout.add(std::make_unique<juce::AudioParameterFloat>(
		juce::ParameterID{ ParameterIDs::fltAttack, 1 },
		"Filter Attack",
		juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.4f),
		0.01f
	));

	layout.add(std::make_unique<juce::AudioParameterFloat>(
		juce::ParameterID{ ParameterIDs::fltDecay, 1 },
		"Filter Decay",
		juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.4f),
		0.3f
	));

	layout.add(std::make_unique<juce::AudioParameterFloat>(
		juce::ParameterID{ ParameterIDs::fltSustain, 1 },
		"Filter Sustain",
		juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
		0.5f
	));

	layout.add(std::make_unique<juce::AudioParameterFloat>(
		juce::ParameterID{ ParameterIDs::fltRelease, 1 },
		"Filter Release",
		juce::NormalisableRange<float>(0.001f, 10.0f, 0.001f, 0.4f),
		0.5f
	));

	// Envelope Amount: -1.0 (inverted) to +1.0 (full)
	layout.add(std::make_unique<juce::AudioParameterFloat>(
		juce::ParameterID{ ParameterIDs::fltEnvAmount, 1 },
		"Filter Env Amount",
		juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01f),
		0.5f
	));

	return layout;
}

CodingSoundsSynthAudioProcessor::CodingSoundsSynthAudioProcessor()
	: AudioProcessor(
		BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
		apvts(*this, nullptr, "PARAMETERS", createParameterLayout()) {

	masterGainParam = apvts.getRawParameterValue(ParameterIDs::masterGain);

}

CodingSoundsSynthAudioProcessor::~CodingSoundsSynthAudioProcessor() {
}

void CodingSoundsSynthAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
	gainSmoothed.reset(sampleRate, 0.02); // 20ms
	gainSmoothed.setCurrentAndTargetValue(
		juce::Decibels::decibelsToGain(masterGainParam->load())
	);
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
	for (const auto metadata : midiMessages) {
		auto message = metadata.getMessage();

		if (message.isNoteOn()) {
			lastNoteNumber.store(message.getNoteNumber());
			noteOnCount++;
		}
	}

	// SOUND GENERATION 
	for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
		float noise = random.nextFloat() * 2.0f - 1.0f;
		for (int channel = 0; channel < totalNumOutputChannels; ++channel){
			buffer.getWritePointer(channel)[sample] = noise;
		}
	}

	// GAIN PROCESSING
	float targetGainDB = masterGainParam->load();
	float targetGainLinear = juce::Decibels::decibelsToGain(targetGainDB);

	gainSmoothed.setTargetValue(targetGainLinear);

	for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
		float currentGain = gainSmoothed.getNextValue();
		for (int channel = 0; channel < totalNumOutputChannels; ++channel) {
			buffer.getWritePointer(channel)[sample] *= currentGain;
		}
	}


}

juce::AudioProcessorEditor* CodingSoundsSynthAudioProcessor::createEditor() {
	return new juce::GenericAudioProcessorEditor(*this);
}

bool CodingSoundsSynthAudioProcessor::hasEditor() const {
	return true;
}

// BOILERPLATE METADATA
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

void CodingSoundsSynthAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
	auto state = apvts.copyState();
	std::unique_ptr<juce::XmlElement> xml(state.createXml());
	copyXmlToBinary(*xml, destData);
}

void CodingSoundsSynthAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {
	std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data,sizeInBytes));
	if (xml != nullptr && xml->hasTagName(apvts.state.getType()))
		apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
	return new CodingSoundsSynthAudioProcessor();
}
