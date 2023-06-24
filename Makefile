SRCS := ResourceBundleManifest.cpp \
	ResourceBundle.cpp

kbundle: main.cpp $(SRCS)
	g++ $(shell pkg-config --cflags --libs Qt5Xml quazip1-qt5) -fPIC -o $@ $^

.PHONY: clean
clean:
	rm -f kbundle
