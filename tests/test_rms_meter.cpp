#include <catch2/catch_test_macros.hpp>
#include "../../core/meters/rms-meter.h"
#include "../../common/audio-format.h"
#include <cmath>

using namespace openmeters;

TEST_CASE("RMS meter - basic functionality", "[meters]") {
    core::meters::RmsMeter meter;
    common::AudioFormat format;
    format.sampleRate = 48000;
    format.channelCount = 2;
    
    SECTION("Zero input produces zero output") {
        float buffer[] = {0.0f, 0.0f, 0.0f, 0.0f};
        auto result = meter.process(buffer, 2, format);
        
        REQUIRE(result.left == 0.0f);
        REQUIRE(result.right == 0.0f);
    }
    
    SECTION("Constant positive values") {
        float buffer[] = {0.5f, 0.5f, 0.5f, 0.5f};
        auto result = meter.process(buffer, 2, format);
        
        REQUIRE(result.left == Approx(0.5f));
        REQUIRE(result.right == Approx(0.5f));
    }
    
    SECTION("Constant negative values") {
        float buffer[] = {-0.5f, -0.5f, -0.5f, -0.5f};
        auto result = meter.process(buffer, 2, format);
        
        REQUIRE(result.left == Approx(0.5f));
        REQUIRE(result.right == Approx(0.5f));
    }
    
    SECTION("Mixed values") {
        // RMS of [0.5, -0.5, 0.5, -0.5] = sqrt((0.25+0.25+0.25+0.25)/4) = sqrt(0.25) = 0.5
        float buffer[] = {0.5f, -0.5f, 0.5f, -0.5f};
        auto result = meter.process(buffer, 2, format);
        
        REQUIRE(result.left == Approx(0.5f));
        REQUIRE(result.right == Approx(0.5f));
    }
    
    SECTION("Mono input") {
        format.channelCount = 1;
        float buffer[] = {0.5f, 0.3f, 0.7f};
        auto result = meter.process(buffer, 3, format);
        
        float expected = std::sqrt((0.25f + 0.09f + 0.49f) / 3.0f);
        REQUIRE(result.left == Approx(expected));
        REQUIRE(result.right == Approx(expected)); // Mono uses left for both
    }
    
    SECTION("Clamping to 1.0") {
        float buffer[] = {1.5f, 2.0f};
        auto result = meter.process(buffer, 1, format);
        
        REQUIRE(result.left <= 1.0f);
        REQUIRE(result.right <= 1.0f);
    }
}

TEST_CASE("RMS meter - mathematical correctness", "[meters]") {
    core::meters::RmsMeter meter;
    common::AudioFormat format;
    format.sampleRate = 48000;
    format.channelCount = 1;
    
    SECTION("RMS calculation") {
        // RMS of [1.0, 0.0, 1.0, 0.0] = sqrt((1+0+1+0)/4) = sqrt(0.5) ≈ 0.707
        float buffer[] = {1.0f, 0.0f, 1.0f, 0.0f};
        auto result = meter.process(buffer, 4, format);
        
        float expected = std::sqrt(0.5f);
        REQUIRE(result.left == Approx(expected).margin(0.001f));
    }
}

