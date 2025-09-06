/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ToneStackAudioProcessorEditor::ToneStackAudioProcessorEditor (ToneStackAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    
    trebleSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    trebleSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(trebleSlider);
    trebleSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "treble", trebleSlider);

    midSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    midSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(midSlider);
    midSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "mid", midSlider);

    bassSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    bassSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(bassSlider);
    bassSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "bass", bassSlider);

    setSize (300, 100);
}

ToneStackAudioProcessorEditor::~ToneStackAudioProcessorEditor()
{
}

//==============================================================================
void ToneStackAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
}

void ToneStackAudioProcessorEditor::resized()
{
    bassSlider.setBounds({5, 5, 80, 80}); // x, y, width, height
    midSlider.setBounds({105, 5, 80, 80}); // x, y, width, height
    trebleSlider.setBounds({210, 5, 80, 80}); // x, y, width, height
}
