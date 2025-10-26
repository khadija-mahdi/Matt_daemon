#include "RemoteShell.hpp"
#include "../server.hpp"
#include "../tintin_reporter.hpp"
#include <iostream>

int main() {
    try {
        // Create dummy reporter (not used for client)
        Tintin_reporter reporter;
        Server server(reporter);
        
        RemoteShell shell(server);
        shell.createRemoteShellSession();
        
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}