#!/bin/bash

set -e

ROOT=$(cd $(dirname $0)/..; pwd)

cd "$ROOT"

# -- this section is ripped from ci/run_envoy_docker.sh
. ci/envoy_build_sha.sh

[[ -z "${IMAGE_NAME}" ]] && IMAGE_NAME="envoyproxy/envoy-build-ubuntu"
# The IMAGE_ID defaults to the CI hash but can be set to an arbitrary image ID (found with 'docker
# images').
[[ -z "${IMAGE_ID}" ]] && IMAGE_ID="${ENVOY_BUILD_SHA}"
[[ -z "${ENVOY_DOCKER_BUILD_DIR}" ]] && ENVOY_DOCKER_BUILD_DIR=/tmp/envoy-docker-build
# --

docker rm -f envoy-build
docker pull "${IMAGE_NAME}":"${IMAGE_ID}"

echo 'Now you can run /xfer/DATAWIRE/go.sh to copy, rebuild, and run Envoy'

docker run --privileged -it -u 0:0 -p 8443:8443 -p 8001:8001 -v "$(pwd):/xfer" --name envoy-build \
    "${IMAGE_NAME}":"${IMAGE_ID}" /bin/bash
