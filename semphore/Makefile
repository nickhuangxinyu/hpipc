cc = g++ -std=c++11
link = -lpthread -L /home/xhuang/locallib/lib -lzmq -ltimecontroller -lz -lconfig++

all:
	$(cc) -O3 -o sr shot_reader.cpp $(link)
	$(cc) -O3 -o sw shot_writer.cpp $(link)

clean:
	rm -rf sr sw sender *.csv *.txt
