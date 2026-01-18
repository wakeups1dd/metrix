#pragma once

#include "../../common/audio-format.h"
#include "../../common/meter-values.h"

namespace openmeters::core::audio {

/**
 * Interface for audio data callbacks.
 * Implementations receive real-time audio buffers and meter values.
 */
class IAudioDataCallback {
public:
    virtual ~IAudioDataCallback() = default;
    
    /**
     * Called when new audio data is available.
     * 
     * @param buffer Audio buffer (interleaved samples: L, R, L, R, ...)
     * @param frameCount Number of frames (samples per channel)
     * @param format Audio format descriptor
     * 
     * Thread: Audio capture thread (real-time priority)
     * Ownership: Buffer is valid only during this call
     */
    virtual void onAudioData(
        const float* buffer,
        std::size_t frameCount,
        const common::AudioFormat& format
    ) = 0;
    
    /**
     * Called when new meter values are available.
     * 
     * @param snapshot Current meter snapshot (peak, RMS)
     * 
     * Thread: Audio capture thread (real-time priority)
     */
    virtual void onMeterData(const common::MeterSnapshot& snapshot) = 0;
};

/**
 * Audio engine interface.
 * Manages WASAPI capture and exposes audio data via callbacks.
 */
class IAudioEngine {
public:
    virtual ~IAudioEngine() = default;
    
    /**
     * Initialize the audio engine.
     * Sets up WASAPI loopback capture.
     * 
     * @return true if initialization succeeded, false otherwise
     */
    virtual bool initialize() = 0;
    
    /**
     * Start audio capture.
     * Begins streaming audio data to registered callbacks.
     * 
     * @return true if start succeeded, false otherwise
     */
    virtual bool start() = 0;
    
    /**
     * Stop audio capture.
     * Stops streaming and releases audio resources.
     */
    virtual void stop() = 0;
    
    /**
     * Shutdown the audio engine.
     * Releases all resources.
     */
    virtual void shutdown() = 0;
    
    /**
     * Register a callback for audio data.
     * Multiple callbacks can be registered.
     * 
     * @param callback Callback interface (must remain valid until unregistered)
     */
    virtual void registerCallback(IAudioDataCallback* callback) = 0;
    
    /**
     * Unregister a callback.
     * 
     * @param callback Callback to remove
     */
    virtual void unregisterCallback(IAudioDataCallback* callback) = 0;
    
    /**
     * Get the current audio format.
     * 
     * @return Audio format descriptor
     */
    [[nodiscard]] virtual common::AudioFormat getFormat() const = 0;
    
    /**
     * Check if the engine is currently capturing.
     * 
     * @return true if capturing, false otherwise
     */
    [[nodiscard]] virtual bool isCapturing() const = 0;
};

} // namespace openmeters::core::audio

