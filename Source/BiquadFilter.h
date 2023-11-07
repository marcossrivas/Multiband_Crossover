#pragma once

#include <JuceHeader.h>
#include <cmath>

class BiquadFilter
{
private:

	//-- Coefficients --//

	// LPF
	float b0;
	float b1;
	float b2;
	float a0;
	float a1;
	float a2;

	// HPF

	float _b0;
	float _b1;
	float _b2;
	float _a1;
	float _a2;
	float _a0;

	//-- Aux. variables --//

	float wo;
	float coswo;
	float sinwo;
	float alpha;
	float Q = 0.707106781;
	float pi = juce::MathConstants<float>::pi;

	float woh;
	float coswoh;
	float sinwoh;
	float alphah;

	//-- Buffers --//

	float x2{ 0.0 };
	float x1{ 0.0 };
	float x0{ 0.0 };
	float y2{ 0.0 };
	float y1{ 0.0 };
	float y0{ 0.0 };

	float x2_{ 0.0 };
	float x1_{ 0.0 };
	float x0_{ 0.0 };
	float y2_{ 0.0 };
	float y1_{ 0.0 };
	float y0_{ 0.0 };

	float _x2_{ 0.0 };
	float _x1_{ 0.0 };
	float _x0_{ 0.0 };
	float _y2_{ 0.0 };
	float _y1_{ 0.0 };
	float _y0_{ 0.0 };

	float _x2{ 0.0 };
	float _x1{ 0.0 };
	float _x0{ 0.0 };
	float _y2{ 0.0 };
	float _y1{ 0.0 };
	float _y0{ 0.0 };

	float sampleRate;

public:

	BiquadFilter();

	void samplerate(double samplerate);
	void processBiquadLP(float* InAudio, float* OutAudio, float samplestoRender, double fs, float f1, float gain);
	void processBiquadBP(float* InAudio, float* OutAudio, float samplestoRender, double fs, float f1, float f2, float gain);
	void processBiquadHP(float* InAudio, float* OutAudio, float samplestoRender, double fs, float f2, float gain);

	~BiquadFilter();
};
