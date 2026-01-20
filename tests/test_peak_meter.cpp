#include <catch2/catch_test_macros.hpp>
#include "../../core/meters/peak-meter.h"
#include "../../common/audio-format.h"

using namespace openmeters;

TEST_CASE("Peak meter - basic functionality", "[meters]") {
    core::meters::PeakMeter meter;
    common::AudioFormat format;
    format.sampleRate = 48000;
    format.channelCount = 2;
    
    SECTION("Zero input produces zero output") {
        float buffer[] = {0.0f, 0.0f, 0.0f, 0.0f};
        auto result = meter.process(buffer, 2, format);
        
        REQUIRE(result.left == 0.0f);
        REQUIRE(result.right == 0.0f);
    }
    
    SECTION("Positive values") {
        float buffer[] = {0.5f, 0.3f, 0.8f, 0.2f};
        auto result = meter.process(buffer, 2, format);
        
        REQUIRE(result.left == Approx(0.8f));
        REQUIRE(result.right == Approx(0.3f));
    }
    
    SECTION("Negative values") {
        float buffer[] = {-0.5f, -0.3f, -0.8f, -0.2f};
        auto result = meter.process(buffer, 2, format);
        
        REQUIRE(result.left == Approx(0.8f));
        REQUIRE(result.right == Approx(0.3f));
    }
    
    SECTION("Mixed positive and negative") {
        float buffer[] = {0.5f, -0.7f, -0.3f, 0.9f};
        auto result = meter.process(buffer, 2, format);
        
        REQUIRE(result.left == Approx(0.5f));
        REQUIRE(result.right == Approx(0.9f));
    }
    
    SECTION("Mono input") {
        format.channelCount = 1;
        float buffer[] = {0.5f, 0.8f, 0.3f};
        auto result = meter.process(buffer, 3, format);
        
        REQUIRE(result.left == Approx(0.8f));
        REQUIRE(result.right == Approx(0.8f)); // Mono uses left for both
    }
    
    SECTION("Clamping to 1.0") {
        float buffer[] = {1.5f, 2.0f};
        auto result = meter.process(buffer, 1, format);
        
        REQUIRE(result.left <= 1.0f);
        REQUIRE(result.right <= 1.0f);
    }
}

TEST_CASE("Peak meter - edge cases", "[meters]") {
    core::meters::PeakMeter meter;
    common::AudioFormat format;
    format.sampleRate = 48000;
    format.channelCount = 2;
    
    SECTION("Empty buffer") {
        float buffer[] = {0.0f};
        auto result = meter.process(buffer, 0, format);
        
        REQUIRE(result.left == 0.0f);
        REQUIRE(result.right == 0.0f);
    }
    
    SECTION("Single sample") {
        float buffer[] = {0.7f, 0.3f};
        auto result = meter.process(buffer, 1, format);
        
        REQUIRE(result.left == Approx(0.7f));
        REQUIRE(result.right == Approx(0.3f));
    }
}

