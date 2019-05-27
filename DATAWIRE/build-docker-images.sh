#ECHO=echo

VERSION=$(git describe --tags --exclude 'ambassador-*')
REGISTRY=${1:-datawire}

for combo in ubuntu-unstripped alpine-unstripped alpine-stripped; do
    dockerfile="Dockerfile-$combo"
    basetag="ambassador-envoy-$combo:$VERSION"
    tag="$basetag"

    if [ "$REGISTRY" != "-" ]; then
        tag="$REGISTRY/$basetag"
    fi

    cp "$dockerfile" "../ci/datawire-$dockerfile"

    if $ECHO docker build -f "../ci/datawire-$dockerfile" -t "$tag" ../ci; then
        if [ "$REGISTRY" != "-" ]; then
            $ECHO docker push "$tag"
        fi
    fi

    rm -f "../ci/datawire-$dockerfile"   
done
