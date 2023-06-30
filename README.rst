===============
kbundle Utility
===============

``kbundle`` is a small utility program for packing Krita bundles
manually and keeping the manifest up to date. This is useful if you
want to manage the files in a bundle outside Krita. For example, you
could keep the bundle contents in a Git repository and quickly
rebuild the ``.bundle`` file whenever you change something.

.. note:: This is a first draft! It works for me, but there's still
	  some quirks to smooth out. Pull requests are welcome :)

Usage
=====

The program accepts a subcommand and some number of arguments. If the
root directory of the bundle tree isn't the current directory, an
alternate path can be specified using ``-r <DIR>``::

  kbundle [-r <DIR>] <COMMAND> [ARG]...

Recognized commands are:

- ``kbundle update`` scans for resource files and updates the manifest
  file (``META-INF/manifest.xml``) accordingly.
- ``kbundle build <FILE>`` builds a Krita bundle file and writes it to
  ``<FILE>``.
- ``kbundle add-tag <TAG> <FILE>`` adds a tag ``<TAG>`` to
  ``<FILE>``. The file must be listed in the manifest and not already
  have the given tag.
- ``kbundle remove-tag <TAG> <FILE>`` removes a tag ``<TAG>`` from
  ``<FILE>``. The file must be listed in the manifest and have the
  given tag.

Example
=======

Suppose ``example.bundle`` is an existing bundle file created in
Krita. Since the bundle is actually a ZIP archive, we can unzip the
contents into a local directory::

  $ mkdir example && cd example
  $ unzip ../example.bundle
  $ ls
  brushes  META-INF  meta.xml  mimetype  paintoppresets  preview.png

The unzipped files are read-only, so we need to change the permissions
before making any changes: ``chmod -R u+w *``

Now we can manage the contents of the bundle as files on our
filesystem. For example, we could add a new preset file and quickly
rebuild the bundle::

  $ cp ~/.local/share/krita/paintoppresets/my_preset.kpp ./paintoppresets/
  $ kbundle update # Update the manifest file to include the new preset.
  $ kbundle build example_v2.bundle

Now we have a new bundle file ``example_v2.bundle`` that includes the
new preset.

We can also add and remove tags for resources in the bundle::

  $ kbundle add-tag Digital paintoppresets/my_preset.kpp
  $ kbundle remove-tag Sketch paintoppresets/my_pen_preset.kpp

.. note:: Currently, resources need to be added to the manifest
	  before they can be tagged. Make sure to run ``kbundle
	  update`` after adding new resources.

Building
========

The program depends on Qt 5 (QtCore and QtXml) and QuaZip, which are
also build dependencies for Krita. On ArchLinux, the relevant packages
are ``qt5-base`` and ``quazip-qt5``.

You could build the program directly with ``g++`` and ``pkg-config``::

  g++ $(pkg-config --cflags --libs Qt5Xml quazip1-qt5) -fPIC -o kbundle \
      main.cpp \
      ResourceBundle.cpp \
      ResourceBundleManifest.cpp

However, there's also very basic Makefile to do this, so you could
just run ``make`` instead.

After building, you can copy the ``kbundle`` executable somewhere in
your path.
