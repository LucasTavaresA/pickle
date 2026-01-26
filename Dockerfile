FROM debian:latest

RUN apt-get update && apt-get install -y build-essential libx11-dev \
	libxcursor-dev libxrandr-dev libxinerama-dev libxi-dev

WORKDIR /build

COPY . .

RUN chmod +x build.sh && ./build.sh linux -R

CMD ["sh"]
