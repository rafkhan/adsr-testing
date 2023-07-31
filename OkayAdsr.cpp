#include "OkayAdsr.h"

void OkayAdsr::init(float sampleRate, float blockSize) {
  this->sampleRate = sampleRate;
  this->blockSize = blockSize;
  this->runState = 0;
}

// Take milliseconds as input and convert to sample length
float convertMsToSamples(float sampleRate, float timeMs){
  if(timeMs == 0) {
    return 0;
  }

  return sampleRate / (1/(timeMs/1000));
}

void OkayAdsr::setAttack(float timeMs) {
  this->attackTimeSamples = convertMsToSamples(this->sampleRate, timeMs);
}

void OkayAdsr::setDecay(float timeMs) {
  this->decayTimeSamples = convertMsToSamples(this->sampleRate, timeMs);
}

void OkayAdsr::setRelease(float timeMs) {
  this->releaseTimeSamples = convertMsToSamples(this->sampleRate, timeMs);
}

float OkayAdsr::tick(int isGateHigh) {
  // isGateHigh = 1;

  // Not running, low gate. Do nothing.
  if(!isGateHigh && this->runState == 0) {
    this->output = 0;
    return this->output;
  }

  // Not running, high gate. Start attack.
  if(isGateHigh && this->runState == 0) {
    this->runState = 1;
    runDeltaTime = 0;
  }

  // Attack can disregard the gate state, as long it was
  // triggered earlier.
  if(this->runState == 1) {
    if(attackTimeSamples == 0) {
      this->output = 1;
    } else {
      this->output = this->output + (this->blockSize * (1 / attackTimeSamples));
    }

    // Do not exceed 1, or sample run time and if we hit that, switch to next segment
    if(this->output >= 1) {
      this->output = 1;
      this->runState = 2;
    }

    return this->output;
  }

  if(this->runState == 2) {
    if(decayTimeSamples == 0) {
      this->output = 0;
    } else {
      this->output = this->output - (this->blockSize * (1 / decayTimeSamples));
    }

    // Do not exceed 0 or sample runtime, and if we hit that, switch to next segment
    if(this->output <= 0) {
      this->output = 0;
      this->runState = 0;
    }

    return this->output;
  }

  return this->output;
}