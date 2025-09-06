/*
  ==============================================================================

    ToneStack.cpp
    Created: 31 Aug 2025 5:52:44pm
    Author:  bertrand GUILLAUME

  ==============================================================================
*/

#include "ToneStack.h"
#include <complex>

static constexpr double kPI = 3.14159265358979323846;

// ---------------- Biquad ----------------
inline float ToneStack::Biquad::process(float x) {
    double y = b0 * x + z1;
    z1 = b1 * x - a1 * y + z2;
    z2 = b2 * x - a2 * y;
    return static_cast<float>(y);
}

void ToneStack::Biquad::setCoeffs(double nb0,double nb1,double nb2,double na0,double na1,double na2){
    const double invA0 = 1.0 / ((std::abs(na0) < 1e-20) ? 1e-20 : na0);
    b0 = nb0 * invA0; b1 = nb1 * invA0; b2 = nb2 * invA0;
    a1 = na1 * invA0; a2 = na2 * invA0;
}

// ---------------- Paramètres ----------------
void ToneStack::setPots(float bass, float mid, float treble) {
    potB_ = std::clamp(bass, 0.0f, 1.0f);
    potM_ = std::clamp(mid, 0.0f, 1.0f);
    potT_ = std::clamp(treble, 0.0f, 1.0f);
    updateFilters();
}

void ToneStack::setOutputTrimDb(double dB) {
    outTrimDb_ = dB;
    makeupLinear_ = std::pow(10.0, outTrimDb_ / 20.0);
}

// ---------------- Traitement ----------------
void ToneStack::processBlock(float* data, size_t numSamples) {
    if (!data) return;
    for (size_t i = 0; i < numSamples; ++i) {
        float x = data[i];
        float y = low_.process(x);
        y = mid_.process(y);
        y = high_.process(y);
        data[i] = static_cast<float>(y * makeupLinear_);
    }
}

// ---------------- Helpers ----------------
double ToneStack::clampFreq(double f, double fs) {
    const double nyq = 0.5 * fs;
    return std::clamp(f, 10.0, 0.45 * nyq);
}

double ToneStack::potToDb(double pos, double maxBoostCut) {
    return (pos * 2.0 - 1.0) * maxBoostCut;
}

// ---------------- Filtres ----------------
void ToneStack::makeLowShelf(Biquad& b, double fs, double f0, double gainDB, double S) {
    f0 = clampFreq(f0, fs);
    const double A     = std::sqrt(std::pow(10.0, gainDB / 20.0));
    const double w0    = 2.0 * kPI * f0 / fs;
    const double cw    = std::cos(w0);
    const double sw    = std::sin(w0);
    const double alpha = sw * 0.5 * std::sqrt((A + 1.0/A) * (1.0/S - 1.0) + 2.0);
    const double beta  = 2.0 * std::sqrt(A) * alpha;

    const double b0 =    A * ((A + 1.0) - (A - 1.0) * cw + beta);
    const double b1 =  2*A * ((A - 1.0) - (A + 1.0) * cw);
    const double b2 =    A * ((A + 1.0) - (A - 1.0) * cw - beta);
    const double a0 =         (A + 1.0) + (A - 1.0) * cw + beta;
    const double a1 =    -2 * ((A - 1.0) + (A + 1.0) * cw);
    const double a2 =         (A + 1.0) + (A - 1.0) * cw - beta;
    b.setCoeffs(b0,b1,b2,a0,a1,a2);
}

void ToneStack::makeHighShelf(Biquad& b, double fs, double f0, double gainDB, double S) {
    f0 = clampFreq(f0, fs);
    const double A     = std::sqrt(std::pow(10.0, gainDB / 20.0));
    const double w0    = 2.0 * kPI * f0 / fs;
    const double cw    = std::cos(w0);
    const double sw    = std::sin(w0);
    const double alpha = sw * 0.5 * std::sqrt((A + 1.0/A) * (1.0/S - 1.0) + 2.0);
    const double beta  = 2.0 * std::sqrt(A) * alpha;

    const double b0 =    A * ((A + 1.0) + (A - 1.0) * cw + beta);
    const double b1 = -2*A * ((A - 1.0) + (A + 1.0) * cw);
    const double b2 =    A * ((A + 1.0) + (A - 1.0) * cw - beta);
    const double a0 =         (A + 1.0) - (A - 1.0) * cw + beta;
    const double a1 =     2 * ((A - 1.0) - (A + 1.0) * cw);
    const double a2 =         (A + 1.0) - (A - 1.0) * cw - beta;

    b.setCoeffs(b0,b1,b2,a0,a1,a2);
}

void ToneStack::makePeaking(Biquad& b, double fs, double f0, double Q, double gainDB) {
    f0 = clampFreq(f0, fs);
    Q = std::max(0.1, Q);
    const double A   = std::pow(10.0, gainDB / 40.0);
    const double w0  = 2.0 * kPI * f0 / fs;
    const double cw  = std::cos(w0);
    const double sw  = std::sin(w0);
    const double alpha = sw / (2.0 * Q);

    const double b0 = 1.0 + alpha * A;
    const double b1 = -2.0 * cw;
    const double b2 = 1.0 - alpha * A;
    const double a0 = 1.0 + alpha / A;
    const double a1 = -2.0 * cw;
    const double a2 = 1.0 - alpha / A;

    b.setCoeffs(b0,b1,b2,a0,a1,a2);
}

void ToneStack::updateFilters() {
    const double fB = 1.0 / (2.0 * kPI * std::max(1.0, comp_.Rb) * std::max(1e-12, comp_.Cb));
    const double fM = 1.0 / (2.0 * kPI * std::max(1.0, comp_.Rm) * std::max(1e-12, comp_.Cm));
    const double fT = 1.0 / (2.0 * kPI * std::max(1.0, comp_.Rt) * std::max(1e-12, comp_.Ct));

    const double gB = potToDb(potB_);
    const double gM = potToDb(potM_);
    const double gT = potToDb(potT_);

    makeLowShelf (low_,  fs_, fB, gB, comp_.shelfSlope);
    makePeaking  (mid_,  fs_, fM, comp_.Qm, gM);
    makeHighShelf(high_, fs_, fT, gT, comp_.shelfSlope);
}

double ToneStack::magnitudeAt(double fHz) const {
    auto mag = [&](const Biquad& b)->double{
        const double w = 2.0 * kPI * fHz / fs_;
        const double cw = std::cos(w);
        const double sw = std::sin(w);
        const std::complex<double> z1(cw, -sw);
        const std::complex<double> z2 = z1 * z1;
        std::complex<double> num = b.b0 + b.b1 * z1 + b.b2 * z2;
        std::complex<double> den = 1.0 + b.a1 * z1 + b.a2 * z2;
        return std::abs(num / den);
    };
    return mag(low_) * mag(mid_) * mag(high_) * makeupLinear_;
}
