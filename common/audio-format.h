#pragma once

#include "types.h"

namespace openmeters::common {

/**
 * Audio format descriptor.
 * Describes the format of audio data (sample rate, channels, etc.).
 */
struct AudioFormat {
    SampleRate sampleRate = 48000;
    ChannelCount channelCount = 2;
    
    /**
     * Number of samples per frame (equals channelCount).
     */
    [[nodiscard]] constexpr std::size_t samplesPerFrame() const noexcept {
        return static_cast<std::size_t>(channelCount);
    }
    
    /**
     * Number of bytes per frame (samplesPerFrame * sizeof(Sample)).
     */
    [[nodiscard]] constexpr std::size_t bytesPerFrame() const noexcept {
        return samplesPerFrame() * sizeof(Sample);
    }
    
    /**
     * Number of bytes for a given number of frames.
     */
    [[nodiscard]] constexpr std::size_t bytesForFrames(FrameCount frames) const noexcept {
        return frames * bytesPerFrame();
    }
    
    /**
     * Check if format is valid (non-zero sample rate, 1-2 channels).
     */
    [[nodiscard]] constexpr bool isValid() const noexcept {
        return sampleRate > 0 && channelCount >= 1 && channelCount <= 2;
    }
};

} // namespace openmeters::common

