FROM ubuntu:18.04
LABEL maintainer="Wonyong Kim <chokoble33@gmail.com>"

RUN apt-get update -y && \
    apt-get install -y \
    build-essential \
    curl \
    wget \
    unzip \
    vim && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

COPY installers /tmp/installers
ENV BAZEL_VERSION 0.17.2
RUN bash /tmp/installers/install_bazel.sh

USER felicia