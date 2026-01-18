#pragma once

#include <cstdint>
#include <cstddef>

namespace openmeters::common {

/**
 * Audio sample format.
 * All audio processing uses float32 samples in the range [-1.0, 1.0].
 */
using Sample = float;

/**
 * Audio buffer type.
 * Represents a contiguous buffer of audio samples.
 */
using AudioBuffer = Sample*;

/**
 * Channel index (0 = left, 1 = right for stereo).
 */
using ChannelIndex = std::size_t;

/**
 * Frame count (number of samples per channel).
 */
using FrameCount = std::size_t;

/**
 * Sample rate in Hz.
 */
using SampleRate = std::uint32_t;

/**
 * Channel count (1 = mono, 2 = stereo).
 */
using ChannelCount = std::uint8_t;

} // namespace openmeters::common

