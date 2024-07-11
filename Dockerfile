FROM debian:latest

RUN apt-get update && apt-get install -y git build-essential libx11-dev \
	libxcursor-dev libxrandr-dev libxinerama-dev libxi-dev

WORKDIR /build

RUN git clone --depth=1 --recurse-submodules https://github.com/lucastavaresa/pickle.git .

RUN chmod +x build.sh && ./build.sh linux -R

CMD ["sh"]
