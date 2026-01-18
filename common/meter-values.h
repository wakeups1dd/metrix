#pragma once

#include "types.h"

namespace openmeters::common {

/**
 * Peak meter value (linear scale, 0.0 to 1.0).
 * Represents the maximum absolute sample value in a buffer.
 */
struct PeakValue {
    float left = 0.0f;
    float right = 0.0f;
    
    /**
     * Get peak value for a specific channel.
     */
    [[nodiscard]] float getChannel(ChannelIndex channel) const noexcept {
        return (channel == 0) ? left : right;
    }
    
    /**
     * Get maximum peak across all channels.
     */
    [[nodiscard]] float getMax() const noexcept {
        return (left > right) ? left : right;
    }
};

/**
 * RMS meter value (linear scale, 0.0 to 1.0).
 * Represents the root mean square of samples in a buffer.
 */
struct RmsValue {
    float left = 0.0f;
    float right = 0.0f;
    
    /**
     * Get RMS value for a specific channel.
     */
    [[nodiscard]] float getChannel(ChannelIndex channel) const noexcept {
        return (channel == 0) ? left : right;
    }
    
    /**
     * Get maximum RMS across all channels.
     */
    [[nodiscard]] float getMax() const noexcept {
        return (left > right) ? left : right;
    }
};

/**
 * Combined meter values snapshot.
 * Contains peak and RMS values for the current audio buffer.
 */
struct MeterSnapshot {
    PeakValue peak;
    RmsValue rms;
    
    /**
     * Timestamp in milliseconds (relative to engine start).
     * TODO: Implement proper timing system.
     */
    std::uint64_t timestampMs = 0;
};

} // namespace openmeters::common

