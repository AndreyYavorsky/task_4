FROM debian

WORKDIR /laba_4

COPY my_copy.cpp main.cpp /laba_4/

RUN apt update;                             \
	apt install nano -y;                    \
	apt install g++ -y;                     \
	apt install gdb -y

CMD g++ my_copy.cpp -o my_copy;             \
	g++ main.cpp -o main;                   \
	/bin/bash