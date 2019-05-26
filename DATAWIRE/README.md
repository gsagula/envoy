# Datawire notes -- Flynn <flynn@datawire.io>

**THIS WILL ALL GO AWAY** after the upstream merge. But it's handy for now.

## Overview

`datawire/envoy` is the GitHub repo for the Datawire fork of Envoy. Within it, there are two branches:

- `datawire/extauth` has the code changes to support our `extauth` work.
- `datawire/extauth-build-automation` has the supporting docs and scripting to build and test our `extauth` work.

**At all times, `extauth-build-automation` must contain all of the changes in `extauth`.** Everything will break if you don't do this.

## Initial Setup

- After cloning, set up your upstream with

    ```
    git remote add -t master upstream git@github.com:envoyproxy/envoy.git
    ```

- When you get to the testing part of the world, you'll need Python 3 with Flask and Requests installed.

- Everything we use for Datawire's build is in the DATAWIRE directory, so get used to `cd`'ing there:

    ```
    cd DATAWIRE
    ```

## Updating With Changes From Upstream

Note that part of this procedure has you on branches other than `datawire/extauth-build-automation`, which means that these instructions will disappear. Keeping it open in an editor or browser is one solution. Two clones is another. None of these are great.

- Start on your local `master` branch:

    ```
    git checkout master
    ```

- Pull any upstream changes in:

    ```
    git fetch upstream
    git merge upstream/master
    git push
    ```

- Review changes here if it seems relevant.

- Next, merge into the `datawire/extauth` branch:

    ```
    git checkout datawire/extauth
    git merge master
    git push
    ```

- Next, wrangle `datawire/extauth-build-automation`:

    ```
    git checkout datawire/extauth-build-automation
    git merge datawire/extauth
    git push
    ```

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

**This may well fail the first time in a new container** -- there's some weirdness with Bazel and external dependencies. If you get a failure complaining about external dependencies, run it again.

Once the build is finished, your newly-built Envoy will be visible in the container  as `/xfer/ci/envoy-static-build`, and from the host as `ci/envoy-static-build` in your `datawire/envoy` directory. **Note well: the built Envoy is _not_ in the `DATAWIRE` directory.**

## Testing

Once you have a good build:

1. **On the host**:
   - you'll need an idle window, Python 3, and Flask
   - get back to the `DATAWIRE` directory
   - run the test extauth service:

    ```
    python simple-auth-server.py
    ```

2. **In the container**:
   - start the test Envoy running (it listens on port 9999 and uses the Envoy config in `DATAWIRE/envoy-test.json`)
   - **IMPORTANT NOTE** If you're on Linux then you need to modify `envoy-test.json` to use `tcp://localhost:3000` instead of `tcp://docker.for.mac.localhost:3000`


    ```
    cd ~/envoy
    sh DATAWIRE/envoy-test.sh
    ```

3. **On the host**:
   - you'll need another idle window, Python 3, and Requests
   - get back to the `DATAWIRE` directory
   - start the authentication test:

    ```
    python authtest.py http://localhost:9999 test-1.yaml
    ```

   - `authtest.py` will run several loops and generate a lot of output. Make sure it doesn't say anything failed.

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
