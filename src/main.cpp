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
    (void)signum;
    Monitor::stop();
    Profile::restore_balanced();
    Utils::update_module_desc("Balanced (Stopped)");
    exit(0);
}

int main(int argc, char* argv[]) {
    if (argc > 1 && std::string(argv[1]) == "daemon") {
        signal(SIGTERM, cleanup_and_exit);
        signal(SIGINT, cleanup_and_exit);

        Utils::update_module_desc("Inisialisasi Lumina...");
        Utils::send_notif("Lumina Tweaks", "Inisialisasi Daemon");

        Profile::init();
        Profile::apply_boot_tweaks();

        std::this_thread::sleep_for(std::chrono::milliseconds(800));

        ConfigManager::start_watcher();

        Profile::restore_balanced();
        Utils::update_module_desc("Balanced");
        Utils::send_notif("Lumina Tweaks", "Profile Balance");

        Monitor::start();
        return 0;
    }

    if (argc > 1 && std::string(argv[1]) == "restore") {
        Profile::init();
        Profile::restore_balanced();
        Utils::update_module_desc("Balanced (Manual Restore)");
        std::cout << "[LUMINA] System restored to Stock Balanced." << std::endl;
        return 0;
    }

    std::cout << "Usage: luminad [daemon|restore]" << std::endl;
    return 1;
}
