/* Copyright 2023 Quytelda Kahja
 *
 * This file is part of kbundle.
 *
 * kbundle is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * kbundle is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with kbundle. If not, see <https://www.gnu.org/licenses/>.
 */

#include <iostream>

#include "ResourceBundle.hpp"

#define REQUIRE_ARGS(n, args)                              \
    if ((args).size() < (n)) {                             \
        std::cerr << "Not enough arguments." << std::endl; \
        return 1;                                          \
    }


int main(int argc, char** argv)
{
    QString path = QDir::currentPath();

    // Parse the argument list.
    QStringList args;
    for (int i = 1; i < argc; i++) {
        QString arg(argv[i]);
        if (arg == "-r" && (i+1) < argc) {
            path = QString(argv[++i]);
            continue;
        }

        args.append(arg);
    }

    if (args.isEmpty()) {
        std::cerr << "Not enough arguments!\n"
                  << "Usage: kbundle <COMMAND> [ARGS...] <PATH>"
                  << std::endl;
        return 1;
    }
    QString command = args.takeFirst();

    ResourceBundle bundle(path);
    if (!bundle.load()) {
        std::cerr << "Unable to load bundle."
                  << std::endl;
        return 2;
    }

    bool ok;
    if (command == "update") {
        ok = bundle.updateManifest();
    } else if (command == "build") {
        REQUIRE_ARGS(1, args);
        ok = bundle.build(args[0]);
    } else if (command == "add-tag") {
        REQUIRE_ARGS(2, args);
        ok = bundle.addTag(args[1], args[0]);
    } else if (command == "remove-tag") {
        REQUIRE_ARGS(2, args);
        ok = bundle.removeTag(args[1], args[0]);
    } else {
        ok = false;
        std::cerr << "Unknown command: "
                  << qPrintable(command)
                  << std::endl;
    }

    return ok ? 0 : 3;
}
