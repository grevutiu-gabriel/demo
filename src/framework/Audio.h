//
// root of the demo - provides audio playback
//
#pragma once
#include <iostream>
#include <memory>
#include <util/fs.h>
//#include <portaudio/portaudio.h>
#include <stblib/stb_vorbis.h>




class Audio
{
public:
	typedef std::shared_ptr<Audio> Ptr;

	struct StreamData
	{
		short *data;
		int len;
		int current;
	};


	Audio()
	{
	}
	~Audio()
	{
	}

	void load(  const base::Path &streamPath  )
	{
	}


	void startPlayback()
	{
	}

	void stopPlayback()
	{
	}


private:

};

/*
class Audio
{
public:
	typedef std::shared_ptr<Audio> Ptr;

	struct StreamData
	{
		short *data;
		int len;
		int current;
	};


	Audio() : m_streamPath(""), m_stream(0)
	{
		// initialize pa
		PaError err = Pa_Initialize();
		if (Pa_GetDefaultOutputDevice() == paNoDevice)
		{
			std::cerr << "Audio::startAudio - error no default output device\n";
			throw std::runtime_error("audio problems");
		}
	}
	~Audio()
	{
		stopPlayback();

		// deinitialize pa
		Pa_Terminate();
	}

	void load(  const base::Path &streamPath  )
	{
		m_streamPath = streamPath.str();
		if(m_streamPath.empty())
		{
			std::cerr << "Audio::startAudio - empty streampath";
			return;
		}

		// try to load stream ============
		if( base::fs::exists( m_streamPath ) )
		{
			base::fs::File *f = base::fs::open( m_streamPath );

			// read file content
			unsigned int size = (unsigned int)base::fs::size(f);

			unsigned char *data = (unsigned char *)malloc( size*sizeof(unsigned char) );
			base::fs::read(f, data, size, sizeof(char));

			// close file
			base::fs::close(f);

			// load from file content
			int channels;
			StreamData *streamData = new StreamData();
			streamData->current = 0;
			std::cout << "Audio::startAudio - loading stream " << m_streamPath << std::endl;
			streamData->len = stb_vorbis_decode_memory( data, size, &channels, &streamData->data);

			// dispose file content
			free(data);

			if(!streamData->len)
			{
				std::cerr << "Audio::startAudio - error loading ogg file\n";
			}else
			{
				std::cout << "Audio::startAudio - number of channels " << channels << std::endl;
				std::cout << "Audio::startAudio - number of samples " << streamData->len << std::endl;
				m_streamData = streamData;
			}
		}

		PaError err;

		// setup audio ===============
		PaStreamParameters outputParameters;
		outputParameters.device = Pa_GetDefaultOutputDevice(); // default output device
		outputParameters.channelCount = 2;       // stereo output
		outputParameters.sampleFormat = paInt16;
		outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
		outputParameters.hostApiSpecificStreamInfo = NULL;

		err = Pa_OpenStream(
				  &m_stream,
				  NULL, // no input
				  &outputParameters,
				  44100,
				  64,
				  paClipOff,      // we won't output out of range samples so don't bother clipping them
				  streamDataCallback,
				  m_streamData );

		err = Pa_SetStreamFinishedCallback( m_stream, &Audio::streamFinishedCallback );
	}


	void startPlayback()
	{
		if( m_stream )
		{
			// play..
			PaError err = Pa_StartStream( m_stream );
		}
	}

	void stopPlayback()
	{
		if( m_stream )
		{
			PaError err;
			// stop
			err = Pa_StopStream( m_stream );
			err = Pa_CloseStream( m_stream );
			m_stream = 0;
		}
	}


private:


	// Audio =============================

	// This routine will be called by the PortAudio engine when audio is needed.
	// It may called at interrupt level on some machines so don't do anything
	// that could mess up the system like calling malloc() or free().
	//
	static int streamDataCallback( const void *inputBuffer, void *outputBuffer,
								unsigned long framesPerBuffer,
								const PaStreamCallbackTimeInfo* timeInfo,
								PaStreamCallbackFlags statusFlags,
								void *userData )
	{
		StreamData *streamData = (StreamData *)userData;
		short *out = (short*)outputBuffer;
		unsigned long i;

		(void) timeInfo; // Prevent unused variable warnings.
		(void) statusFlags;
		(void) inputBuffer;

		for( i=0; i<framesPerBuffer; i++ )
		{
			*out++ = streamData->data[streamData->current];  // left
			*out++ = streamData->data[streamData->current+1];  // right
			streamData->current += 2;
		}

		return paContinue;
	}

	//
	// This routine is called by portaudio when playback is done.
	//
	static void streamFinishedCallback( void* userData )
	{
		StreamData *streamData = (StreamData *) userData;
		if(streamData->data)
			free(streamData->data);
		delete streamData;

	}


	PaStream               *m_stream;
	StreamData         *m_streamData;
	std::string         m_streamPath;

};


*/
