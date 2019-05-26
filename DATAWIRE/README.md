## Building

Be in the `DATAWIRE` directory, then

```
sh run_builder_bash.sh
```

This will fire up a Docker container with the build environment and give you a shell. If you exit out of the build container, you can restart it with state preserved using

```
docker start -i -a envoy-build
```

Inside the container, the whole `datawire/envoy` tree from the host is mounted on `/xfer` -- however, on a Mac at least, just running the build in `/xfer` is glacially slow. Instead, we have scripts to rsync the sources from the host into the container's disk at `~/envoy`, and build there:

```
sh /xfer/DATAWIRE/go.sh
```

## Formatting

Envoy is draconian about formatting. Once you're feeling good about the build, from inide the container you can run

```
cd ~/envoy
BUILDIFIER_BIN=/usr/local/bin/buildifier python tools/check_format.py fix
```

to make sure the source formatting matches Envoy's draconian rules. You'll have to copy any changed files out to `/xfer` by hand, sadly.

## Dockerizing

Back on the host, after you have a good build, in the `DATAWIRE` directory:

```
sh build-docker-images.sh
```

This will build and push images for Ubuntu unstripped, Alpine unstripped, and Alpine stripped, e.g.:

```
datawire/ambassador-envoy-ubuntu-unstripped:v1.5.0-230-g79cefbed8
datawire/ambassador-envoy-alpine-unstripped:v1.5.0-230-g79cefbed8
datawire/ambassador-envoy-alpine-stripped:v1.5.0-230-g79cefbed8
```

The pushes will fail if you're not from Datawire -- you can use

```
sh build-docker-images.sh $registry
```

to supply a different Docker registry for this. Use a registry of '-' to skip the push entirely.
