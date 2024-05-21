#include "main.hpp"
#include "window.hpp"
#include "config.hpp"

#ifdef PULSEAUDIO
#include "pulse.hpp"
PulseAudio pa;
#else
#include "wireplumber.hpp"
sysvol_wireplumber *sysvol_wp;
#endif

#include <glibmm.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <string>
#include <stdio.h>
#include <signal.h>

std::thread thread_audio;

void audio_server() {
#ifdef PULSEAUDIO
	pa = PulseAudio();
	pa.initialize();
	pa.run();
#else
	sysvol_wp = new sysvol_wireplumber(win);
#endif
}

void quit(int signum) {
	#ifdef PULSEAUDIO
	// Disconnect pulseaudio
	pa.quit(0);
	#endif

	thread_audio.join();

	// Remove window
	app->release();
	app->remove_window(*win);
	delete win;
	app->quit();
}

int main(int argc, char* argv[]) {
	// Read launch arguments
	while (true) {
		switch(getopt(argc, argv, "p:dW:dH:di:dPm:dt:dT:dh")) {
			case 'p':
				position = std::stoi(optarg);
				continue;

			case 'W':
				width = std::stoi(optarg);
				continue;

			case 'H':
				height = std::stoi(optarg);
				continue;

			case 'i':
				icon_size = std::stoi(optarg);
				continue;

			case 'P':
				show_percentage = false;
				continue;

			case 'm':
				margin = std::stoi(optarg);
				continue;

			case 't':
				desired_timeout = std::stoi(optarg);
				continue;

			case 'T':
				transition_time = std::stoi(optarg);
				continue;

			case 'h':
			default :
				std::cout << "usage:" << std::endl;
				std::cout << "  sysvol [argument...]:\n" << std::endl;
				std::cout << "arguments:" << std::endl;
				std::cout << "  -p	Set position" << std::endl;
				std::cout << "  -W	Set window width" << std::endl;
				std::cout << "  -H	Set window Height" << std::endl;
				std::cout << "  -i	Set icon size" << std::endl;
				std::cout << "  -P	Hide percentage" << std::endl;
				std::cout << "  -m	Set margins" << std::endl;
				std::cout << "  -t	Set timeout" << std::endl;
				std::cout << "  -T	Set transition time" << std::endl;
				std::cout << "  -h	Show this help message" << std::endl;
				return 0;

			case -1:
				break;
			}

			break;
	}

	signal(SIGINT, quit);

	app = Gtk::Application::create("funky.sys64.sysvol");
	app->hold();
	win = new sysvol();

	thread_audio = std::thread(audio_server);

	return app->run();
}