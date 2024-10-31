#ifndef OKAY_ADSR_H
#define OKAY_ADSR_H

class OkayAdsr {
  public:
    void setAttack(float timeMs);
    void setDecay(float timeMs);
    void setSustain(float level);
    void setRelease(float timeMs);

    void init(float sampleRate);
    float tick(int isGateHigh);

  private:
    float tickInternal(int isGateHigh);
    void startRelease(float level);

    float sampleRate;
    float blockSize;
    float output = 0;
    float runDeltaTime = 0;
    int runState = 0;
    bool lastGate = false;

    float sustain = 0;
    float attackTimeSamples = 0;
    float attackDeltaTime = 0;

    float decayStartY = 0;
    float decayTimeSamples = 0;
    float decayDeltaTime = 0;

    float releaseStartY = 0;
    float releaseTimeSamples = 0;
    float releaseDeltaTime = 0;
};

#endif