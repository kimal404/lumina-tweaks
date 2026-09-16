#include "utils.hpp"
#include "config.hpp"
#include "profile.hpp"
#include "monitor.hpp"
#include <iostream>
#include <string>
#include <csignal>
#include <cstdlib>
#include <thread>
#include <chrono>

static void cleanup_and_exit(int signum) {
    Profile::restore_balanced();
    Monitor::stop();
    exit(signum);
}

int main(int argc, char* argv[]) {
    if (argc > 1 && std::string(argv[1]) == "daemon") {
        signal(SIGTERM, cleanup_and_exit);
        signal(SIGINT, cleanup_and_exit);

        Utils::update_module_desc("Instalasi Lumina");
        Utils::send_notif("Lumina Tweaks", "Instalasi Lumina");

        Profile::init();
        Profile::apply_boot_tweaks();

        std::this_thread::sleep_for(std::chrono::milliseconds(1200));

        ConfigManager::start_watcher();

        Utils::update_module_desc("Balanced");
        Utils::send_notif("Lumina Tweaks", "Profile Balance");

        Monitor::start();
        return 0;
    }

    std::cout << "Usage: luminad daemon" << std::endl;
    return 1;
}
