# PS Vita 2d game

## Prerequesites 

1. jailbreak the psvita with [henkaku](https://henkaku.xyz)
2. install [vita shell](https://www.logic-sunrise.com/news-1087328-vita-vitashell-v202-disponible.html)

## Development environment

Build and start ps vita development environment:
```
docker compose up --build
```

Find the container id:
```
docker ps
```

Bash commands into the docker container
```
docker exec --it <docker_id> bash
```

## Build the app and automatically export it

Open the ps vita ftp port with vita shell and get the ps vita ip address.

Set the PSVITAIP variable to the actual ps vita ip address:
```
export PSVITAIP=<ip_address>
```

Build the application and transfer the file to the psvita:
```
cd ./src
make vpksend
```

On the psvita, in vita shell find the new file and install it by `pressing x` on the filename.

## Build the app and manually export it

Build the application
```
cd ./src
make
```

At this step the app binary should be available in `./src/out`. 

Connect the psvita to computer with cable.

Allow psvita to connect to computer with vita shell.

On the computer, drag and drop the vpk to `ux0:/downloads` to the psvita peripherals.

On the psvita, in vita shell find the new file and install it by `pressing x` on the filename.
