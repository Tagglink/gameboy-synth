/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Synth.h"

Synth Synth::INSTANCE;

//==============================================================================
GameBoySynthAudioProcessor::GameBoySynthAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
#else
    : AudioProcessor(),
#endif
      logger_(juce::FileLogger::createDateStampedLogger("gameboysynth", "Processor_", ".log", "GameBoySynth Processor log")),
      // The identifier can't contain spaces or most special characters, since it is
      // serialized as an xml tag name when saving the state.
      parameters_(*this, nullptr, juce::Identifier("GameBoySynth"), parameterLayout())
{
    juce::Logger::setCurrentLogger(logger_);
    addParameterListeners();
    Synth::INSTANCE.loadFromParams(parameters_);
}

GameBoySynthAudioProcessor::~GameBoySynthAudioProcessor()
{
    juce::Logger::setCurrentLogger(nullptr);
    delete logger_;
}

//==============================================================================
const juce::String GameBoySynthAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GameBoySynthAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool GameBoySynthAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool GameBoySynthAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double GameBoySynthAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GameBoySynthAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int GameBoySynthAudioProcessor::getCurrentProgram()
{
    return 0;
}

void GameBoySynthAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String GameBoySynthAudioProcessor::getProgramName(int index)
{
    return {};
}

void GameBoySynthAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void GameBoySynthAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    Synth::INSTANCE.configure(sampleRate, getTotalNumOutputChannels());
    midiCollector_.reset(sampleRate);
}

void GameBoySynthAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    Synth::INSTANCE.stop();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GameBoySynthAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void GameBoySynthAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    if (!buffer.hasBeenCleared()) buffer.clear();

    // also append any events from the collector
    midiCollector_.removeNextBlockOfMessages(midiMessages, (int) buffer.getNumSamples());
    Synth::INSTANCE.handleMIDI(midiMessages);
    Synth::INSTANCE.readSamples(&buffer);
    midiMessages.clear();
}

//==============================================================================
bool GameBoySynthAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* GameBoySynthAudioProcessor::createEditor()
{
    return new GameBoySynthAudioProcessorEditor(*this, parameters_);
}

//==============================================================================
void GameBoySynthAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    juce::ValueTree state = parameters_.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void GameBoySynthAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr) {
        if (xmlState->hasTagName(parameters_.state.getType())) {
            parameters_.replaceState(juce::ValueTree::fromXml(*xmlState));
            parametersReplaced();
        }
    }
}

//==============================================================================
class PWMRange : public juce::NormalisableRange<float> {
public:
    static float normalize(float rangeStart, float rangeEnd, float valueToRemap)
    {
        return valueToRemap / 100.0f;
    }
    static float denormalize(float rangeStart, float rangeEnd, float valueToRemap)
    {
        return valueToRemap * 100.0f;
    }
    static float snap(float rangeStart, float rangeEnd, float valueToRemap)
    {
        return SquareOscilator::normalizeDutyCycle(valueToRemap);
    }
    PWMRange() : juce::NormalisableRange<float>(0, 100, denormalize, normalize, snap) {}
};

void GameBoySynthAudioProcessor::parameterChanged(const juce::String& parameterId, float newValue)
{
    if (parameterId.contains("enable") || parameterId.contains("channel") || parameterId.contains("voice")
        || parameterId.contains("transpose")) {
        Synth::INSTANCE.reconfigure(0);
    } else if (parameterId.compare("osc0pwm") == 0) {
        Synth::INSTANCE.setDutyCycle(0, newValue);
    } else if (parameterId.compare("osc1pwm") == 0) {
        Synth::INSTANCE.setDutyCycle(1, newValue);
    } else if (parameterId.compare("osc0envstep") == 0) {
        Synth::INSTANCE.setEnvelopeStep(0, (uint8_t) newValue);
    } else if (parameterId.compare("osc1envstep") == 0) {
        Synth::INSTANCE.setEnvelopeStep(1, (uint8_t) newValue);
    } else if (parameterId.compare("osc3envstep") == 0) {
        Synth::INSTANCE.setEnvelopeStep(3, (uint8_t) newValue);
    } else if (parameterId.compare("osc0envdir") == 0) {
        Synth::INSTANCE.setEnvelopeDirection(0, (EnvelopeDirection) newValue);
    } else if (parameterId.compare("osc1envdir") == 0) {
        Synth::INSTANCE.setEnvelopeDirection(1, (EnvelopeDirection) newValue);
    } else if (parameterId.compare("osc3envdir") == 0) {
        Synth::INSTANCE.setEnvelopeDirection(3, (EnvelopeDirection) newValue);
    } else if (parameterId.compare("osc3shiftwidth") == 0) {
        Synth::INSTANCE.setShiftWidth((NoiseShiftWidth) newValue);
    }
}

juce::StringArray GameBoySynthAudioProcessor::sequence(int from, int to)
{
   jassert(to > from);
   int len = std::max(to - from + 1, 0);
   juce::StringArray ret;
   for (int i = 0; i < len; i++) {
       ret.add(juce::String(from + i));
   }
   return ret;
}

juce::AudioProcessorValueTreeState::ParameterLayout GameBoySynthAudioProcessor::parameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout params;
    params.add(std::make_unique<juce::AudioParameterBool>("osc0enable", "OSC0 Enable", true));
    params.add(std::make_unique<juce::AudioParameterBool>("osc1enable", "OSC1 Enable", true));
    params.add(std::make_unique<juce::AudioParameterBool>("osc2enable", "OSC2 Enable", false));
    params.add(std::make_unique<juce::AudioParameterBool>("osc3enable", "OSC3 Enable", false));
    params.add(std::make_unique<juce::AudioParameterChoice>("osc0channel", "OSC0 Channel", sequence(1, 16), 0));
    params.add(std::make_unique<juce::AudioParameterChoice>("osc1channel", "OSC1 Channel", sequence(1, 16), 0));
    params.add(std::make_unique<juce::AudioParameterChoice>("osc2channel", "OSC2 Channel", sequence(1, 16), 0));
    params.add(std::make_unique<juce::AudioParameterChoice>("osc3channel", "OSC3 Channel", sequence(1, 16), 0));
    params.add(std::make_unique<juce::AudioParameterChoice>("osc0voice", "OSC0 Voice", sequence(1, 4), 0));
    params.add(std::make_unique<juce::AudioParameterChoice>("osc1voice", "OSC1 Voice", sequence(1, 4), 1));
    params.add(std::make_unique<juce::AudioParameterChoice>("osc2voice", "OSC2 Voice", sequence(1, 4), 0));
    params.add(std::make_unique<juce::AudioParameterChoice>("osc3voice", "OSC3 Voice", sequence(1, 4), 0));
    params.add(std::make_unique<juce::AudioParameterChoice>("osc0transpose", "OSC0 Transpose", sequence(-48, 48), 48));
    params.add(std::make_unique<juce::AudioParameterChoice>("osc1transpose", "OSC1 Transpose", sequence(-48, 48), 48));
    params.add(std::make_unique<juce::AudioParameterChoice>("osc2transpose", "OSC2 Transpose", sequence(-48, 48), 48));
    params.add(std::make_unique<juce::AudioParameterChoice>("osc3transpose", "OSC3 Transpose", sequence(-48, 48), 48));
    params.add(std::make_unique<juce::AudioParameterFloat>("osc0volume", "OSC0 Volume", juce::NormalisableRange<float>(0.0f, 15.0f, 1.0f, 1.0f), 15.0f, juce::AudioParameterFloatAttributes().withStringFromValueFunction(sliderStringFromValueFn(0))));
    params.add(std::make_unique<juce::AudioParameterFloat>("osc1volume", "OSC1 Volume", juce::NormalisableRange<float>(0.0f, 15.0f, 1.0f, 1.0f), 15.0f, juce::AudioParameterFloatAttributes().withStringFromValueFunction(sliderStringFromValueFn(0))));
    params.add(std::make_unique<juce::AudioParameterFloat>("osc3volume", "OSC3 Volume", juce::NormalisableRange<float>(0.0f, 15.0f, 1.0f, 1.0f), 15.0f, juce::AudioParameterFloatAttributes().withStringFromValueFunction(sliderStringFromValueFn(0))));
    params.add(std::make_unique<juce::AudioParameterFloat>("osc0pwm", "OSC0 PWM", PWMRange(), 50.0f, juce::AudioParameterFloatAttributes().withStringFromValueFunction(sliderStringFromValueFn(1))));
    params.add(std::make_unique<juce::AudioParameterFloat>("osc1pwm", "OSC1 PWM", PWMRange(), 50.0f, juce::AudioParameterFloatAttributes().withStringFromValueFunction(sliderStringFromValueFn(1))));
    params.add(std::make_unique<juce::AudioParameterFloat>("osc0envstep", "OSC0 Envelope step", juce::NormalisableRange<float>(0.0f, 7.0f, 1.0f, 1.0f), 0.0f, juce::AudioParameterFloatAttributes().withStringFromValueFunction(sliderStringFromValueFn(0))));
    params.add(std::make_unique<juce::AudioParameterFloat>("osc1envstep", "OSC1 Envelope step", juce::NormalisableRange<float>(0.0f, 7.0f, 1.0f, 1.0f), 0.0f, juce::AudioParameterFloatAttributes().withStringFromValueFunction(sliderStringFromValueFn(0))));
    params.add(std::make_unique<juce::AudioParameterFloat>("osc3envstep", "OSC3 Envelope step", juce::NormalisableRange<float>(0.0f, 7.0f, 1.0f, 1.0f), 0.0f, juce::AudioParameterFloatAttributes().withStringFromValueFunction(sliderStringFromValueFn(0))));
    params.add(std::make_unique<juce::AudioParameterChoice>("osc0envdir", "OSC0 Envelope direction", juce::StringArray{"-", "+"}, 0));
    params.add(std::make_unique<juce::AudioParameterChoice>("osc1envdir", "OSC1 Envelope direction", juce::StringArray{"-", "+"}, 0));
    params.add(std::make_unique<juce::AudioParameterChoice>("osc3envdir", "OSC3 Envelope direction", juce::StringArray{"-", "+"}, 0));
    params.add(std::make_unique<juce::AudioParameterChoice>("osc3shiftwidth", "OSC3 Shift width", juce::StringArray{"15", "7"}, 0));
    return params;
}


std::function<juce::String(float, int)> GameBoySynthAudioProcessor::sliderStringFromValueFn(int numDecimalsToShow)
{
    return [numDecimalsToShow](float v, int maxLength) {
        juce::String str(v, numDecimalsToShow);
        return maxLength > 0 ? str.substring(0, maxLength) : str;
    };
}

void GameBoySynthAudioProcessor::addParameterListeners()
{
    parameters_.addParameterListener("osc0enable", this);
    parameters_.addParameterListener("osc1enable", this);
    parameters_.addParameterListener("osc2enable", this);
    parameters_.addParameterListener("osc3enable", this);
    parameters_.addParameterListener("osc0channel", this);
    parameters_.addParameterListener("osc1channel", this);
    parameters_.addParameterListener("osc2channel", this);
    parameters_.addParameterListener("osc3channel", this);
    parameters_.addParameterListener("osc0voice", this);
    parameters_.addParameterListener("osc1voice", this);
    parameters_.addParameterListener("osc2voice", this);
    parameters_.addParameterListener("osc3voice", this);
    parameters_.addParameterListener("osc0transpose", this);
    parameters_.addParameterListener("osc1transpose", this);
    parameters_.addParameterListener("osc2transpose", this);
    parameters_.addParameterListener("osc3transpose", this);
    parameters_.addParameterListener("osc0volume", this);
    parameters_.addParameterListener("osc1volume", this);
    parameters_.addParameterListener("osc3volume", this);
    parameters_.addParameterListener("osc0pwm", this);
    parameters_.addParameterListener("osc1pwm", this);
    parameters_.addParameterListener("osc0envstep", this);
    parameters_.addParameterListener("osc1envstep", this);
    parameters_.addParameterListener("osc3envstep", this);
    parameters_.addParameterListener("osc0envdir", this);
    parameters_.addParameterListener("osc1envdir", this);
    parameters_.addParameterListener("osc3envdir", this);
    parameters_.addParameterListener("osc3shiftwidth", this);
}

void GameBoySynthAudioProcessor::parametersReplaced()
{
    Synth::INSTANCE.reconfigure(0);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GameBoySynthAudioProcessor();
}
