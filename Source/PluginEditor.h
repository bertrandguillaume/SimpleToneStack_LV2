/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class ToneStackAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    ToneStackAudioProcessorEditor (ToneStackAudioProcessor&);
    ~ToneStackAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ToneStackAudioProcessor& audioProcessor;

    juce::Slider trebleSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> trebleSliderAttachment;
    juce::Label trebleLabel;

    juce::Slider midSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> midSliderAttachment;
    juce::Label midLabel;

    juce::Slider bassSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> bassSliderAttachment;
    juce::Label bassLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ToneStackAudioProcessorEditor)
};
