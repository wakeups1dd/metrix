#pragma once

#include "../../common/types.h"
#include "../../common/audio-format.h"
#include "../../common/meter-values.h"

namespace openmeters::core::meters {

/**
 * Peak meter implementation.
 * Computes peak (maximum absolute) values per channel from audio buffers.
 * 
 * Thread safety: Not thread-safe. Must be called from a single thread.
 */
class PeakMeter {
public:
    /**
     * Process an audio buffer and compute peak values.
     * 
     * @param buffer Audio buffer (interleaved samples)
     * @param frameCount Number of frames
     * @param format Audio format descriptor
     * @return Peak values per channel
     */
    [[nodiscard]] common::PeakValue process(
        const float* buffer,
        std::size_t frameCount,
        const common::AudioFormat& format
    ) const noexcept;
    
    /**
     * Reset the meter (clears any internal state).
     * Currently a no-op, but included for future extensibility.
     */
    void reset() noexcept;
};

} // namespace openmeters::core::meters

