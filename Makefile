kbundle:
	g++ $(shell pkg-config --cflags --libs Qt5Xml) -fPIC -o $@ \
		main.cpp \
		ResourceBundleManifest.cpp \
		ResourceBundle.cpp

.PHONY: clean
clean:
	rm -f kbundle
