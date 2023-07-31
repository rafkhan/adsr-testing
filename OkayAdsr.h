typedef enum eAdsrRunSegment
{
    IDLE    = 0,
    ATTACK  = 1,
    DECAY   = 2,
    SUSTAIN = 4,
    RELEASE = 5,
} AdsrRunSegment;

class OkayAdsr {
  public:
    void init(float sampleRate, float blockSize);
    float tick(int isGateHigh);

    void setAttack(float timeMs);
    void setDecay(float timeMs);
    void setSustain(float level);
    void setRelease(float timeMs);

  private:
    float sampleRate;
    float blockSize;
    float output = 0;

    // float attack = 0;
    // float decay = 0;
    // float release = 0;

    float sustain = 0;
    float attackTimeSamples = 0;
    float decayTimeSamples = 0;
    float releaseTimeSamples = 0;

    float runDeltaTime = 0;
    int runState = 0;
    // AdsrRunSegment runState = AdsrRunSegment::IDLE;
};