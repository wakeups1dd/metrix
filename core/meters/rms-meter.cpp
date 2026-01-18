#include "rms-meter.h"
#include <algorithm>
#include <cmath>

namespace openmeters::core::meters {

common::RmsValue RmsMeter::process(
    const float* buffer,
    std::size_t frameCount,
    const common::AudioFormat& format
) const noexcept {
    common::RmsValue result{0.0f, 0.0f};
    
    if (!buffer || frameCount == 0 || !format.isValid()) {
        return result;
    }
    
    const std::size_t samplesPerFrame = format.samplesPerFrame();
    
    // Accumulate sum of squares
    double leftSumSquares = 0.0;
    double rightSumSquares = 0.0;
    
    // Process each frame
    for (std::size_t frame = 0; frame < frameCount; ++frame) {
        const std::size_t offset = frame * samplesPerFrame;
        
        // Left channel (channel 0)
        const float leftSample = buffer[offset];
        leftSumSquares += static_cast<double>(leftSample * leftSample);
        
        // Right channel (channel 1) - if stereo
        if (format.channelCount >= 2) {
            const float rightSample = buffer[offset + 1];
            rightSumSquares += static_cast<double>(rightSample * rightSample);
        } else {
            // Mono: use left value for right
            rightSumSquares = leftSumSquares;
        }
    }
    
    // Compute RMS: sqrt(sum of squares / count)
    const double frameCountDouble = static_cast<double>(frameCount);
    result.left = static_cast<float>(std::sqrt(leftSumSquares / frameCountDouble));
    result.right = static_cast<float>(std::sqrt(rightSumSquares / frameCountDouble));
    
    // Clamp to [0.0, 1.0]
    result.left = std::clamp(result.left, 0.0f, 1.0f);
    result.right = std::clamp(result.right, 0.0f, 1.0f);
    
    return result;
}

void RmsMeter::reset() noexcept {
    // No internal state to reset currently
}

} // namespace openmeters::core::meters

