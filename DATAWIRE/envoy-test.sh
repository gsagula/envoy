#!/bin/sh

HERE=$(cd $(dirname $0); pwd)

if [ -z "$ENVOY" ]; then
    if [ \( "$1" = "-s" \) -o \( "$1" = "--stripped" \) ]; then
        echo "Using stripped binary"
        ENVOY="${HERE}/../bazel-bin/source/exe/envoy-stripped"
    else
        echo "Using unstripped binary"
        ENVOY="${HERE}/../bazel-bin/source/exe/envoy-static"
    fi
fi

ENVOY_JSON="${HERE}/envoy-test.json"

"$ENVOY" -c "$ENVOY_JSON" -l debug 2>&1 | egrep --color 'ExtAuth|deferring|deferred|$'
