ARG UBUNTU_VERSION=18.04
FROM ubuntu:${UBUNTU_VERSION}

RUN apt-get update \
    && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
        binutils cmake g++ libgtest-dev make python3 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace
