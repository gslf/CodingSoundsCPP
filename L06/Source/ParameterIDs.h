#pragma once
#include <JuceHeader.h>

namespace ParameterIDs {
	// MASTER
	inline constexpr auto masterGain = "masterGain";

	// Oscillator section
	inline constexpr auto osc1Wave = "osc1Wave";
	inline constexpr auto osc2Wave = "osc2Wave";
	inline constexpr auto oscMix = "oscMix";

	// Filter section
	inline constexpr auto filterType = "filterType";   // LP / BP / HP
	inline constexpr auto filterCutoff = "filterCutoff";
	inline constexpr auto filterReso = "filterReso";

	// Amp Envelope
	inline constexpr auto ampAttack = "ampAttack";
	inline constexpr auto ampDecay = "ampDecay";
	inline constexpr auto ampSustain = "ampSustain";
	inline constexpr auto ampRelease = "ampRelease";

	// Filter Envelope
	inline constexpr auto fltAttack = "fltAttack";
	inline constexpr auto fltDecay = "fltDecay";
	inline constexpr auto fltSustain = "fltSustain";
	inline constexpr auto fltRelease = "fltRelease";
	inline constexpr auto fltEnvAmount = "fltEnvAmount";
}