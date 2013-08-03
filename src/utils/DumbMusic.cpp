#include <cstring>
#include "DumbMusic.hpp"


void DumbMusic::initDumb()
{
	// Must be called before trying to open a file
	dumb_register_stdfiles();
}


void DumbMusic::exitDumb()
{
	dumb_exit();
}


DumbMusic::DumbMusic():
	m_module(NULL),
	m_player(NULL)
{
}


DumbMusic::~DumbMusic()
{
	close();
}


bool DumbMusic::openFromFile(const std::string& filename)
{
	close();

	// Guess file type from extension
	const char* ext = strrchr(filename.c_str(), '.');

	if (strcmp(ext, ".mod") == 0) // Amiga module
		m_module = dumb_load_mod_quick(filename.c_str());

	else if (strcmp(ext, ".xm") == 0) // Fast Tracker II
		m_module = dumb_load_xm_quick(filename.c_str());

	else if (strcmp(ext, ".s3m") == 0) // Scream Tracker 3
		m_module = dumb_load_s3m_quick(filename.c_str());

	else if (strcmp(ext, ".it") == 0) // Impulse Tracker
		m_module = dumb_load_it_quick(filename.c_str());

	if (m_module != NULL)
	{
		initialize(NB_CHANNELS, SAMPLING_RATE);
		return true;
	}
	return false;
}


sf::Time DumbMusic::getDuration() const
{
	// 65536 represents one second
	return sf::seconds(static_cast<float>(duh_get_length(m_module) / 65536));
}


void DumbMusic::close()
{
	if (m_player != NULL)
	{
		duh_end_sigrenderer(m_player);
		m_player = NULL;
	}
	if (m_module != NULL)
	{
		unload_duh(m_module);
		m_module = NULL;
	}
}


void DumbMusic::onSeek(sf::Time timeOffset)
{
	// When specifying the position, 0 represents the start of the DUH, and 65536 represents one second.
	long pos = static_cast<long>(timeOffset.asSeconds() * 65536);
	m_player = duh_start_sigrenderer(m_module, 0, NB_CHANNELS, pos);
}


bool DumbMusic::onGetData(Chunk& data)
{
	// Use delta to control the speed of the output signal. If you pass 1.0f, the resultant signal
	// will be suitable for a 65536-Hz sampling rate (which isn't a commonly used rate).
	float delta =  65536.0f / SAMPLING_RATE;

	// le /2 est magique (sans lui, skipping)
	duh_render(m_player, 16, 0, 1.0f, delta, BUFFER_SIZE / 2, m_samples);

	data.samples = m_samples;
	data.sampleCount = BUFFER_SIZE;
	return true;
}
