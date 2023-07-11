#include "BiquadFilter.h"


BiquadFilter::BiquadFilter()
{}

BiquadFilter::~BiquadFilter()
{}

void BiquadFilter::samplerate(double samplerate)
{
	sampleRate = samplerate;
}

// 2 Biquad Band pass filter ( hp + lp , -6dB at fo ) for Linkwitz-Riley response.


void BiquadFilter::processBiquadBP(float* InAudio, float* OutAudio, float samplestoRender, double fs, float f1, float f2, float gain) //total 4 filters 
{

	// Cascade Butterworth 

	for (int i = 0; i < samplestoRender; i++)


	{
		//math coef.

		wo = 2 * pi * (f2 / fs);
		coswo = std::cos(wo);
		sinwo = std::sin(wo);
		alpha = sinwo / (2 * Q);

		woh = 2 * pi * (f1 / fs);
		coswoh = std::cos(woh);
		sinwoh = std::sin(woh);
		alphah = sinwoh / (2 * Q);

		// Low Pass coef
		a0 = 1 + alpha;
		b0 = ((1 - coswo) / 2) / a0;
		b1 = (1 - coswo) / a0;
		b2 = b0;
		a1 = (-2 * coswo) / a0;
		a2 = (1 - alpha) / a0;

		// High Pass
		_a0 = 1 + alphah;
		_b0 = ((1 + coswoh) / 2) / _a0;
		_b1 = (-1 - coswoh) / _a0;
		_b2 = _b0;
		_a1 = (-2 * coswoh) / _a0;
		_a2 = (1 - alphah) / _a0;


		// LOW PASS


		//	First 2nd order Butterworth

		x0 = InAudio[i];

		y0 = (b0 * x0) + (b1 * x1) + (b2 * x2) - (a1 * y1) - (a2 * y2);

		// Buffer
		x2 = x1;
		x1 = x0;
		y2 = y1;
		y1 = y0;

		//	Second 2nd order Butterworth 

		x0_ = y0;

		y0_ = (b0 * x0_) + (b1 * x1_) + (b2 * x2_) - (a1 * y1_) - (a2 * y2_);

		// Buffer
		x2_ = x1_;
		x1_ = x0_;
		y2_ = y1_;
		y1_ = y0_;

		////////////////////////////////////////////

		//HIGH PASS


		//	First 2nd order Butterworth

		_x0 = y0_;

		_y0 = (_b0 * _x0) + (_b1 * _x1) + (_b2 * _x2) - (_a1 * _y1) - (_a2 * _y2);

		// Buffer
		_x2 = _x1;
		_x1 = _x0;
		_y2 = _y1;
		_y1 = _y0;

		//	Second 2nd order Butterworth 

		_x0_ = _y0;

		_y0_ = (_b0 * _x0_) + (_b1 * _x1_) + (_b2 * _x2_) - (_a1 * _y1_) - (_a2 * _y2_);

		// Buffer
		_x2_ = _x1_;
		_x1_ = _x0_;
		_y2_ = _y1_;
		_y1_ = _y0_;

		OutAudio[i] = gain * _y0_;
	}


}


void BiquadFilter::processBiquadLP(float* InAudio, float* OutAudio, float samplestoRender, double fs, float f1, float gain) //total 4 filters 
{

	// Cascade Butterworth 

	for (int i = 0; i < samplestoRender; i++)


	{
		//math coef.

		wo = 2 * pi * (f1 / fs);
		coswo = std::cos(wo);
		sinwo = std::sin(wo);
		alpha = sinwo / (2 * Q);


		// Low Pass coef
		a0 = 1 + alpha;
		b0 = ((1 - coswo) / 2) / a0;
		b1 = (1 - coswo) / a0;
		b2 = b0;
		a1 = (-2 * coswo) / a0;
		a2 = (1 - alpha) / a0;


		// LOW PASS

		//	First 2nd order Butterworth

		x0 = InAudio[i];

		y0 = (b0 * x0) + (b1 * x1) + (b2 * x2) - (a1 * y1) - (a2 * y2);

		// Buffer
		x2 = x1;
		x1 = x0;
		y2 = y1;
		y1 = y0;

		//	Second 2nd order Butterworth 

		x0_ = y0;

		y0_ = (b0 * x0_) + (b1 * x1_) + (b2 * x2_) - (a1 * y1_) - (a2 * y2_);

		// Buffer
		x2_ = x1_;
		x1_ = x0_;
		y2_ = y1_;
		y1_ = y0_;

		OutAudio[i] = gain * y0_;
	}


}

void BiquadFilter::processBiquadHP(float* InAudio, float* OutAudio, float samplestoRender, double fs, float f2, float gain) //total 4 filters 
{

	// Cascade Butterworth 

	for (int i = 0; i < samplestoRender; i++)


	{
		//math coef.

		woh = 2 * pi * (f2 / fs);
		coswoh = std::cos(woh);
		sinwoh = std::sin(woh);
		alphah = sinwoh / (2 * Q);


		// High Pass
		_a0 = 1 + alphah;
		_b0 = ((1 + coswoh) / 2) / _a0;
		_b1 = (-1 - coswoh) / _a0;
		_b2 = _b0;
		_a1 = (-2 * coswoh) / _a0;
		_a2 = (1 - alphah) / _a0;

		//HIGH PASS


		//	First 2nd order Butterworth

		_x0 = InAudio[i];

		_y0 = (_b0 * _x0) + (_b1 * _x1) + (_b2 * _x2) - (_a1 * _y1) - (_a2 * _y2);

		// Buffer
		_x2 = _x1;
		_x1 = _x0;
		_y2 = _y1;
		_y1 = _y0;

		//	Second 2nd order Butterworth 

		_x0_ = _y0;

		_y0_ = (_b0 * _x0_) + (_b1 * _x1_) + (_b2 * _x2_) - (_a1 * _y1_) - (_a2 * _y2_);

		// Buffer
		_x2_ = _x1_;
		_x1_ = _x0_;
		_y2_ = _y1_;
		_y1_ = _y0_;

		OutAudio[i] = gain * _y0_;
	}


}