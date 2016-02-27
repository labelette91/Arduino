all: rfm

rfm: rfm.o RFM69.o HomeEasyTransmitter.o
	$(CXX) $(CXXFLAGS)  -I./ $(LDFLAGS) $+ -o $@ -lwiringPi

clean:
	$(RM) *.o rfm
