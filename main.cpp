#include <iostream>

#include "ResourceBundle.hpp"

int main(int argc, char** argv)
{
    if (argc < 3) {
        std::cerr << "Missing arguments: <COMMAND> [ARGS...] <PATH>" << std::endl;
        return 1;
    }

    QString command(argv[1]);
    QString path(argv[argc - 1]);

    ResourceBundle bundle(path);
    if (!bundle.init()) {
        return false;
    }

    bool ok;
    if (command == "update") {
        ok = bundle.updateManifest();
    } else if (command == "add-tag") {
        ok = bundle.addTag(argv[2], argv[3]);
    } else if (command == "remove-tag") {
        ok = bundle.removeTag(argv[2], argv[3]);
    } else {
        ok = false;
        std::cerr << "Unknown command: "
                  << qPrintable(command)
                  << std::endl;
    }

    return ok ? 0 : 1;
}
