#!/bin/bash

# proxy server URL
PROXY="http://localhost:8080" # Leave empty to bypass proxy

# Base URL of the Python web-server
BASE_URL="http://localhost:8000/api"

# Declare the Json Mock endpoints and their max ID
# You can lower the the max IDs for the resources as desired.
# [posts]=100
# [comments]=500
# [albums]=100
# [photos]=5000
# [todos]=200
# [users]=10
declare -A RESOURCES=(
    [posts]=5
    [comments]=5
    [albums]=5
    [photos]=10
    [todos]=20
    [users]=5
)

# Function to make a request via curl
make_request() {
    local url="$1"
    if [ -n "$PROXY" ]; then
        curl --proxy "$PROXY" "$url" --silent --output /dev/null --write-out "GET $url -> Status: %{http_code}\n"
    else
        # bypasses proxy server
        curl "$url" --silent --output /dev/null --write-out "GET $url -> Status: %{http_code}\n"
    fi
}

echo "Sending requests through proxy: $PROXY"

# Loop through all resources and their IDs
for resource in "${!RESOURCES[@]}"; do
    max_id=${RESOURCES[$resource]}
    echo -e "\nTesting /$resource (1 to $max_id)"

    for ((id = 1; id <= max_id; id++)); do
        url="$BASE_URL/$resource/$id"
        make_request "$url"
    done
done
