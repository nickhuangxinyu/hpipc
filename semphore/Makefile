cc = g++ -std=c++11
link = -lpthread -lnick -lzmq

all:
	$(cc) -o sr shot_reader.cpp $(link)
	$(cc) -o sw shot_writer.cpp $(link)

clean:
	rm -rf sr sw
