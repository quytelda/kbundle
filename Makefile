INCS := ResourceBundle.hpp \
	ResourceBundleManifest.hpp \
	FileEntry.hpp

SRCS := ResourceBundleManifest.cpp \
	ResourceBundle.cpp

all: kbundle kpp-update-name

kbundle: main.cpp $(SRCS) $(INCS)
	g++ $(shell pkg-config --cflags --libs Qt5Xml quazip1-qt5) -fPIC -o $@ $(SRCS) $<

kpp-update-name: kpp-update-name.cpp
	g++ $(shell pkg-config --cflags --libs Qt5Gui Qt5Xml) -fPIC -o $@ $<

.PHONY: clean all
clean:
	rm -f kbundle kpp-update-name
