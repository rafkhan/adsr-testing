#include "daisy_patch_sm.h"
#include "daisysp.h"
#include "OkayAdsr.h"

using namespace daisy;
using namespace patch_sm;
using namespace daisysp;


DaisyPatchSM hw;
OkayAdsr adsr;

/** Create a button to control the ADSR activation */
Switch button;
int env_state = 0;
float cvoutput;

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
	hw.ProcessAllControls();
	for (size_t i = 0; i < size; i++)
	{
		OUT_L[i] = IN_L[i];
		OUT_R[i] = IN_R[i];
	}
}

/** Similar to the audio callback, you can generate audio rate CV signals out of the CV outputs.
 *  These signals are 12-bit DC signals that range from 0-5V out of the Patch SM
 */
void DacCallback(uint16_t **output, size_t size)
{
	/** Process the controls */
	hw.ProcessAnalogControls();
	button.Debounce();

	/** Set the input value of the ADSR */
	if (button.Pressed() || hw.gate_in_1.State())
	{
		env_state = 1;
	}
	else
	{
		env_state = 0;
	}

	float knob_attack = hw.GetAdcValue(CV_1);
	float knob_decay = hw.GetAdcValue(CV_2);
	adsr.setAttack(knob_attack * 2000);
	adsr.setDecay(knob_decay * 2000);

	for (size_t i = 0; i < size; i++)
	{
		cvoutput = adsr.tick(env_state);
		// convert to 12-bit integer (0-4095)
		uint16_t value = cvoutput * 4095.0;
		output[0][i] = value; /**< To CV OUT 1 - Jack */
		output[1][i] = value; /**< To CV OUT 2 - LED */
	}
}

int main(void)
{
	hw.Init();
	hw.StartLog(false);
	hw.SetAudioBlockSize(1); // number of samples handled per callback
	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
	adsr.init(48000);
	adsr.setAttack(1000);
	adsr.setDecay(3000);
	button.Init(hw.B7, 1000);
	// hw.StartAudio(AudioCallback);
	hw.StartDac(DacCallback);
	while (1)
	{
		// hw.PrintLine("%d", env_state);
		hw.PrintLine("%f", cvoutput);
	}
}
