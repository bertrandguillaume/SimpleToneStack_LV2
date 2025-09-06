/*
  ==============================================================================

    ToneStack.h
    Created: 31 Aug 2025 5:52:35pm
    Author:  bertrand GUILLAUME

  ==============================================================================
*/

#pragma once
#include <cstddef>
#include <cmath>
#include <algorithm>

class ToneStack
{
public:
    struct Components {
        double Rb = 100e3;    // basse (Ohms)
        double Cb = 8e-9;   // basse (Farads)
        double Rm = 22e3;     // medium
        double Cm = 10e-9;
        double Rt = 250e3;    // aigucal
        double Ct = 220e-12;

        double shelfSlope = 1.0;   // pente shelf
        double Qm = 0.707;         // facteur de qualité du médium
    };

    ToneStack() = default;

    // --- Gestion du sample rate ---
    void setSampleRate(double fs) {
        fs_ = std::max(8000.0, fs);
        updateFilters();
    }
    double getSampleRate() const { return fs_; }

    // --- Cycle de vie ---
    void prepare(double sampleRate) { setSampleRate(sampleRate); reset(); }
    void reset() { low_.reset(); mid_.reset(); high_.reset(); }

    // --- Paramétrage ---
    void setComponents(const Components& c) { comp_ = c; updateFilters(); }
    void setPots(float bass, float mid, float treble);
    void setOutputTrimDb(double dB);

    // --- Traitement ---
    void processBlock(float* data, size_t numSamples);

    // --- Outils ---
    double magnitudeAt(double fHz) const;

private:
    struct Biquad {
        double b0=1, b1=0, b2=0, a1=0, a2=0; 
        double z1=0, z2=0;

        inline float process(float x);
        void setCoeffs(double nb0,double nb1,double nb2,double na0,double na1,double na2);
        void reset() { z1 = z2 = 0.0; }
    };

    void updateFilters();

    // Design de filtres
    static void makeLowShelf (Biquad& b, double fs, double f0, double gainDB, double S);
    static void makeHighShelf(Biquad& b, double fs, double f0, double gainDB, double S);
    static void makePeaking  (Biquad& b, double fs, double f0, double Q,   double gainDB);

    static double clampFreq(double f, double fs);
    static double potToDb(double pos, double maxBoostCut = 18.0);

    double fs_ = 48000.0;
    Components comp_;

    double potB_ = 0.5, potM_ = 0.5, potT_ = 0.5;
    double outTrimDb_ = 0.0, makeupLinear_ = 1.0;

    Biquad low_, mid_, high_;
};
