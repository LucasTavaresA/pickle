# Pickle

Crossplatform spinning wheel picker so you can make important life decisions

https://github.com/LucasTavaresA/pickle/assets/80704612/f061aa5f-06e5-4496-8af1-e89e2a6efe89

## Build

Don't forget to clone recursively to get the used raylib version

Building for windows requires [w64devkit](https://github.com/skeeto/w64devkit)

```sh
git clone --recurse-submodules https://github.com/LucasTavaresA/pickle.git
cd pickle
./build.sh
```

### Build with docker/podman

**Needed to work with old ass versions of glibc if you are on arch or other cutting edge distros**

This will build pickle, get you the executable and delete everything

First, install docker/podman then run

```sh
docker build -t pickle .
docker run --name pickle-container pickle
docker cp pickle-container:/build/pickle ./pickle
docker rm pickle-container
docker rmi pickle
```

## Credits

Made with [raylib](https://www.raylib.com/)

Font Used: [Iosevka](https://typeof.net/Iosevka/)
