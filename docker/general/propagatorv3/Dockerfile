# Tag: hsfl2/propagatorv3:latest
# Tag: hsfl2/propagatorv3:0.1.0

############################################
# BUILD STAGE
FROM ubuntu:20.04 AS builder

# Set TZ to avoid tzdata prompt
ENV TZ=UTC \
    DEBIAN_FRONTEND=noninteractive

# Install necessary packages
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    && apt-get -y autoremove \
    && apt-get autoclean \
    && rm -rf /var/lib/apt/lists/*

# Set up basic COSMOS directory structure
WORKDIR /cosmos
RUN mkdir -p \
    ./bin \
    ./nodes \
    ./source

# Clone resources. Necessary for coordinate conversions.
WORKDIR /cosmos
RUN git clone https://github.com/hsfl/cosmos-resources.git resources

# Clone source code
WORKDIR /cosmos/source
RUN git clone https://github.com/hsfl/cosmos-core.git core

# Configure project
WORKDIR core/build
RUN ./do_cmake_linux
# Build project
WORKDIR linux
RUN make -j8 propagatorv3
# Manually move the compiled targets to /cosmos/bin
WORKDIR programs/general/
RUN cp propagatorv3 /cosmos/bin/

############################################
# FINAL IMAGE
FROM ubuntu:20.04

# cosmosuser user and group IDs (26767 = COSMOS)
ARG UID=26767
ARG GID=26767

# Copy resources folder from the builder stage
WORKDIR /home/cosmosuser/cosmos
COPY --from=builder /cosmos/resources .

# Copy the binaries from the builder stage
WORKDIR /home/cosmosuser/cosmos/bin
COPY --from=builder /cosmos/bin/propagatorv3 .

# Copy over a demo realm
WORKDIR /home/cosmosuser/cosmos/realms
COPY --from=hsfl2/realms:demo /demo ./demo

# Set up basic COSMOS directory structure
WORKDIR /home/cosmosuser/cosmos
RUN mkdir -p \
    ./source \
    ./nodes \
    ./bin

# Following best practices for security, create a non-root user and group
RUN groupadd --gid "${GID}" cosmosuser && \
    useradd -s /bin/bash --uid "${UID}" --gid "${GID}"  -m cosmosuser && \
    chown -R cosmosuser:cosmosuser /home/cosmosuser/cosmos

# Switch to the non-root user
USER cosmosuser

# Add cosmos binary directory to path
ENV PATH="$PATH:/home/cosmosuser/cosmos/bin"

COPY ./docker-entrypoint.sh /
ENTRYPOINT ["/docker-entrypoint.sh"]