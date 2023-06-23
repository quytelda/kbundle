#include <iostream>

#include "ResourceBundle.hpp"

int main(int argc, char** argv)
{
    if (argc < 3) {
        std::cerr << "Not enough arguments!\n"
                  << "Usage: kbundle <COMMAND> [ARGS...] <PATH>"
                  << std::endl;
        return 1;
    }

    QString command(argv[1]);
    QString path(argv[argc - 1]);

    ResourceBundle bundle(path);
    if (!bundle.init()) {
        std::cerr << "Unable to load bundle from path: "
                  << qPrintable(path)
                  << std::endl;
        return 2;
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

    return ok ? 0 : 3;
}
