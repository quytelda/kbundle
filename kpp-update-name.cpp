/*
 * This program updates the metadata of a Krita brush preset (a `.kpp`
 * file) so the metadata 'name' attribute matches the file base
 * name. However, underscores will be replaced with spaces since this is
 * the default behavior in Krita's brush editor.
 *
 * For example, `kpp-update-name My_Preset.kpp` will update the preset's
 * metadata name to `My Preset`.
 *
 * To set the preset name to something else (or avoid replacing
 * underscores), specify the new name as a second argument:
 *
 * `kpp-update-name My_Preset.kpp 'Some_Other_Name'`
 */

#include <iostream>
#include <QImageReader>
#include <QImageWriter>
#include <QtXml>

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "Missing filename argument." << std::endl;
        return 1;
    }

    char *path = argv[1];

    QImageReader reader(path, "PNG");
    QString version = reader.text("version");
    QString preset  = reader.text("preset");

    QImage img;
    if (!reader.read(&img)) {
        std::cerr << "Failed to read image data." << std::endl;
        return 1;
    }

    QDomDocument doc;
    if (!doc.setContent(preset)) {
        std::cerr << "Failed to parse preset metadata." << std::endl;
        return 1;
    }

    QDomElement root = doc.documentElement();
    if (root.tagName() != "Preset" || !root.hasAttribute("name")) {
        std::cerr << "Invalid preset data." << std::endl;
        return 1;
    }

    QString oldName = root.attribute("name");
    QString newName;
    if (argc < 3) {
        newName = QFileInfo(path).baseName().replace('_', ' ');
    } else {
        newName = QString(argv[2]);
    }

    root.setAttribute("name", newName);
    std::cout << "Changed name: \""
              << qPrintable(oldName)
              << "\" -> \""
              << qPrintable(newName)
              << "\"" << std::endl;

    QImageWriter writer(path, "PNG");
    writer.setText("version", version);
    writer.setText("preset", doc.toString());

    if (!writer.write(img)) {
        std::cerr << "Failed to write image." << std::endl;
        return 1;
    }

    return 0;
}
