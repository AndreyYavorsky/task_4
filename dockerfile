FROM debian

WORKDIR /laba_5

COPY main.cpp /laba_5/

RUN apt update;                             \
	apt install nano -y;                    \
	apt install g++ -y;                     \
	apt install gdb -y

CMD g++ main.cpp -lpthread -o main;         \
	/bin/bash