/*
 * Copyright 2017 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Oscillator.h"
#include <math.h>

#define TWO_PI (3.14159 * 2)
#define AMPLITUDE 0.3
#define FREQUENCY 440.0

void Oscillator::setSampleRate(int32_t sampleRate) {
    phaseIncrement_ = (TWO_PI * FREQUENCY) / (double) sampleRate;
}

void Oscillator::setWaveOn(bool isWaveOn) {
    isWaveOn_.store(isWaveOn);
    if (isWaveOn) {
        // Sets the initial phase for a click-free start.
        phase_ = 0.0;
        isRunning_.store(true);
    }
}

void Oscillator::render(float *audioData, int32_t numFrames) {

    // Stages an off-request for implementation at the next zero crossing with a positive slope.
    bool isOffStaged = isRunning_.load() && !isWaveOn_.load();

    for (int i = 0; i < numFrames; i++) {

        if (isRunning_.load()) {
            // Calculates the next sample value for the sine wave.
            audioData[i] = (float) (AMPLITUDE * sin(phase_));
            phase_ += phaseIncrement_;
            // Handles phase wrap around.
            if (phase_ > TWO_PI) {
                phase_ -= TWO_PI;
                // Switches off at a zero crossing for a click-free stop, if staged.
                if (isOffStaged) {
                    isRunning_.store(false);
                }
            }
        } else {
            // Outputs silence by setting sample value to zero.
            audioData[i] = 0.0;
        }
    }
}
