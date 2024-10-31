#include "OkayAdsr.h"

template <typename T> 
T clamp(T n, T lower, T upper )
{
    n = ( n > lower ) * n + !( n > lower ) * lower;
    return ( n < upper ) * n + !( n < upper ) * upper;
}

enum EnvelopeState
{
  STOPPED,
  ATTACK,
  DECAY,
  SUSTAIN,
  RELEASE
};

void OkayAdsr::init(float sampleRate)
{
  this->sampleRate = sampleRate;
  this->runState = 0;
}

// Take milliseconds as input and convert to sample length
float convertMsToSamples(float sampleRate, float timeMs)
{
  if (timeMs == 0)
  {
    return 0;
  }

  return sampleRate / (1 / (timeMs / 1000));
}

void OkayAdsr::setAttack(float timeMs)
{
  this->attackTimeSamples = convertMsToSamples(this->sampleRate, timeMs);
}

void OkayAdsr::setDecay(float timeMs)
{
  this->decayTimeSamples = convertMsToSamples(this->sampleRate, timeMs);
}

void OkayAdsr::setRelease(float timeMs)
{
  this->releaseTimeSamples = convertMsToSamples(this->sampleRate, timeMs);
}

void OkayAdsr::setSustain(float sustain)
{
  this->sustain = clamp<float>(sustain, 0.f, 1.f);
}

void OkayAdsr::startRelease(float level) {
  this->runState = RELEASE;
  this->releaseStartY = level;
}

float OkayAdsr::tickInternal(int isGateHigh)
{
  // Start at last output
  float _output = this->output;

  // Not running, low gate. Do nothing.
  if (!isGateHigh && this->runState == STOPPED)
  {
    return 0;
  }

  // THINK THIS THROUGH!!!
  if ((!lastGate && isGateHigh) && this->runState == STOPPED)
  {
    this->runState = ATTACK;
  }

  if(lastGate && !isGateHigh) {
    this->startRelease(_output);
  }

  // // THIS IS QUESTIONABLE CODE
  // // Is running and gate is now low, skip to release segment
  // if(!isGateHigh && this->runState != STOPPED) {
  //   this->runState = RELEASE;
  // }

  if (this->runState == ATTACK)
  {
    if (attackTimeSamples == 0)
    {
      _output = 1;
    }
    else
    {
      _output = clamp<float>(_output + (1 / attackTimeSamples), 0, 1);
    }

    // Do not exceed 1, or sample run time and if we hit that, switch to next segment
    if (_output >= 1 || isGateHigh)
    {
      this->decayStartY = _output;
      this->runState = DECAY;
    }

    if(!isGateHigh) {
      this->releaseStartY = _output;
      this->runState = RELEASE;
    }

    return _output;

    // TEST FOR RETRIGGER ????
  }

  if (this->runState == DECAY)
  {
    if(decayTimeSamples == 0) {
      this->runState = SUSTAIN;
      return this->output;
    }

    float decayIncrement = (this->decayStartY - sustain) / decayTimeSamples;
    _output = clamp<float>(_output - decayIncrement, 0, 1);

    if (_output <= 0)
    {
      // this->releaseStartY = _output;
      this->runState = STOPPED;
    }

    return _output;
  }

  if (this->runState == SUSTAIN)
  {
    return _output; // hold
  }

  if(this->runState == RELEASE) {
    float releaseIncrement = this->releaseStartY / releaseTimeSamples; 
    return clamp<float>(this->output - releaseIncrement, 0, 1);
  }

  return _output;
}

float OkayAdsr::tick(int isGateHigh)
{
  this->output = this->tickInternal(isGateHigh);
  this->lastGate = isGateHigh;
  return this->output;
}