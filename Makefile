INCS := ResourceBundle.hpp \
	ResourceBundleManifest.hpp \
	FileEntry.hpp

SRCS := ResourceBundleManifest.cpp \
	ResourceBundle.cpp

kbundle: main.cpp $(SRCS) $(INCS)
	g++ $(shell pkg-config --cflags --libs Qt5Xml quazip1-qt5) -fPIC -o $@ $(SRCS) $<

.PHONY: clean
clean:
	rm -f kbundle
