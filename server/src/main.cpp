#include "server/DataPaths.h"
#include "server/Server.h"

int main(int argc, char* argv[]) {
    DataPaths::init(argv[0]);   

    Server server(9000);
    server.run();
    return 0;
}
