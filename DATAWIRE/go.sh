set -ex

BAZELCMD="build"

if [ "$1" = "test" ]; then
    BAZELCMD="test"
    shift
fi

TARGET=${1:-//source/exe:envoy-static}

rsync -Pav /xfer/ ~/envoy
cd ~/envoy

# bazel "$BAZELCMD" --spawn_strategy=standalone --verbose_failures -c dbg --config=clang-asan "$TARGET"
# bazel "$BAZELCMD" --verbose_failures -c dbg --config=clang-asan "$TARGET"
bazel "$BAZELCMD" --verbose_failures -c dbg "$TARGET"

rm -f /xfer/ci/envoy-static-binary
cp bazel-bin/source/exe/envoy-static /xfer/ci/envoy-static-binary
strip bazel-bin/source/exe/envoy-static -o bazel-bin/source/exe/envoy-stripped
cp bazel-bin/source/exe/envoy-stripped /xfer/ci/envoy-stripped-binary

# if [ -z "$1" ]; then sh envoy-test.sh; fi
