#include "peak-meter.h"
#include <algorithm>
#include <cmath>

namespace openmeters::core::meters {

common::PeakValue PeakMeter::process(
    const float* buffer,
    std::size_t frameCount,
    const common::AudioFormat& format
) const noexcept {
    common::PeakValue result{0.0f, 0.0f};
    
    if (!buffer || frameCount == 0 || !format.isValid()) {
        return result;
    }
    
    const std::size_t samplesPerFrame = format.samplesPerFrame();
    
    // Process each frame
    for (std::size_t frame = 0; frame < frameCount; ++frame) {
        const std::size_t offset = frame * samplesPerFrame;
        
        // Left channel (channel 0)
        const float leftSample = std::abs(buffer[offset]);
        if (leftSample > result.left) {
            result.left = leftSample;
        }
        
        // Right channel (channel 1) - if stereo
        if (format.channelCount >= 2) {
            const float rightSample = std::abs(buffer[offset + 1]);
            if (rightSample > result.right) {
                result.right = rightSample;
            }
        } else {
            // Mono: use left value for right
            result.right = result.left;
        }
    }
    
    // Clamp to [0.0, 1.0] (should already be in range, but defensive)
    result.left = std::clamp(result.left, 0.0f, 1.0f);
    result.right = std::clamp(result.right, 0.0f, 1.0f);
    
    return result;
}

void PeakMeter::reset() noexcept {
    // No internal state to reset currently
}

} // namespace openmeters::core::meters

