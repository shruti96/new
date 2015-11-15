

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
//#include "newp.h"

//==============================================================================

class MainContentComponent : public AudioAppComponent, public ChangeListener ,
								public Button::Listener
{
public:
    //==============================================================================
	MainContentComponent() : state(Stopped)
    {
		vol.setSliderStyle(Slider::LinearHorizontal);
		vol.setRange(0.0, 127.0, 1.0);
		
		vol.setTextBoxStyle(Slider::NoTextBox, false, 90, 0);
    	vol.setPopupDisplayEnabled(true,this);
		vol.setTextValueSuffix(" Volume");
		vol.setValue(1.0);
		//vol.addListener()

		addAndMakeVisible(&vol);
		

		addAndMakeVisible(&openButton);
		openButton.setButtonText("Open...");
		openButton.addListener(this);
		openButton.setColour(TextButton::buttonColourId, Colours::lavender);

		addAndMakeVisible(&playButton);
		playButton.setButtonText("Play");
		playButton.addListener(this);
		playButton.setColour(TextButton::buttonColourId, Colours::yellow);
		playButton.setEnabled(false);

		addAndMakeVisible(&stopButton);
		stopButton.setButtonText("Stop");
		stopButton.addListener(this);
		stopButton.setColour(TextButton::buttonColourId, Colours::red);
		stopButton.setEnabled(false);

        setSize (600, 430);
		formatManager.registerBasicFormats();     
		transportSource.addChangeListener(this);  
        // specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
    }

    ~MainContentComponent()
    {
        shutdownAudio();
    }

    //=======================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override
    {
		transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);

    }

    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override
    {

		if (readerSource == nullptr)
		{
			bufferToFill.clearActiveBufferRegion();
			return;
		}

		transportSource.getNextAudioBlock(bufferToFill);
       
    }

    void releaseResources() override
    {
		transportSource.releaseResources();
        
    }

	void sliderValueChanged(Slider* slider)
	{
//		processor.noteOnVel = vol.getValue();
	}
    //=======================================================================
    void paint (Graphics& g) override
    {
		
        // (Our component is opaque, so we must completely fill the background with a solid colour)
        g.fillAll (Colours::orchid);


		String text = "AudioAttempt : Welcome  ";
		
		g.setFont(Font("Times New Roman",30.0f,Font::bold));
		g.setColour(Colours::gold);

		g.drawMultiLineText(text,150, 50, 800);
	//	g.drawText(text, 10, 10, 300, 10, Justification::centred);
		//ColourGradient s=setColour(Colours::darkgrey, (10.0f), (100.0f), Colours::lavender, getWidth() - 20, 20));
	
		//g.setGradientFill();
		g.fillRect(10, 130, getWidth()-20, 20);
        // You can add your drawing code here!
		g.setColour(Colours::darkblue);
		g.drawRoundedRectangle(10, 300,80 , 20, 2, 2);
		g.setFont(20);
		g.drawFittedText("Volume", 10, 300, 70, Justification::centredTop, 2, 2);


		
    }
	

	void resized() override
	{
		vol.setBounds(100, 300, getWidth() - 200, 20);
	
		//newpage.setBounds(10, 390, getWidth() - 200, 20);
		openButton.setBounds(100, 200, getWidth() - 200, 20);
		playButton.setBounds(100, 230, getWidth() - 200, 20);
		stopButton.setBounds(100, 260, getWidth() - 200, 20);
		//n.setBounds(getLocalBounds());
	}

	void changeListenerCallback(ChangeBroadcaster* source) override
	{
		if (source == &transportSource)
		{
			if (transportSource.isPlaying())
				changeState(Playing);
			else
				changeState(Stopped);
		}
	}

	void buttonClicked(Button* button) override
	{
		//if (button == &newpage) newpagebuttonclicked();
		if (button == &openButton)  openButtonClicked();
		if (button == &playButton)  playButtonClicked();
		if (button == &stopButton)  stopButtonClicked();
	}
	

private:

	enum TransportState
	{
		Stopped,
		Starting,
		Playing,
		Stopping
	};

	void changeState(TransportState newState)
	{
		if (state != newState)
		{
			state = newState;

			switch (state)
			{
			case Stopped:                           // [3]
				stopButton.setEnabled(false);
				playButton.setEnabled(true);
				transportSource.setPosition(0.0);
				break;

			case Starting:                          // [4]
				playButton.setEnabled(false);
				transportSource.start();
				break;

			case Playing:                           // [5]
				stopButton.setEnabled(true);
				break;

			case Stopping:                          // [6]
				transportSource.stop();
				break;
			}
		}
	}

	void openButtonClicked()
	{
		FileChooser chooser("Select a Wave file to play...",
			File::nonexistent,
			"*.wav");                                        // [7]

		if (chooser.browseForFileToOpen())                                    // [8]
		{
			File file(chooser.getResult());                                  // [9]
			AudioFormatReader* reader = formatManager.createReaderFor(file); // [10]

			if (reader != nullptr)
			{
				ScopedPointer<AudioFormatReaderSource> newSource = new AudioFormatReaderSource(reader, true); // [11]
				transportSource.setSource(newSource, 0, nullptr, reader->sampleRate);                         // [12]
				playButton.setEnabled(true);                                                                  // [13]
				readerSource = newSource.release();                                                            // [14]
			}
		}
	}

	void playButtonClicked()
	{
		changeState(Starting);
	}

	void stopButtonClicked()
	{
		changeState(Stopping);
	}
	/*void newpagebuttonclicked()
	{
		addAndMakeVisible(n);
	}
*/
	//==========================================================================
	TextButton openButton;
	TextButton playButton;
	TextButton stopButton;
	Slider vol;
	//TextButton newpage;

	AudioFormatManager formatManager;
	ScopedPointer<AudioFormatReaderSource> readerSource;
	AudioTransportSource transportSource;
	TransportState state;
	
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};


// (This function is called by the app startup code to create our main component)
Component* createMainContentComponent()     { return new MainContentComponent(); }


#endif  // MAINCOMPONENT_H_INCLUDED
