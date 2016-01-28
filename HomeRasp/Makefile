all: test

test: HomeEasyOnOff.o HomeEasyTransmitter.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $+ -o $@ -lwiringPi

clean:
	$(RM) *.o sensor
