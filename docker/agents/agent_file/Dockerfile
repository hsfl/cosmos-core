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
RUN mkdir -p /cosmos/source
RUN mkdir -p /cosmos/bin

# Set the working directory
WORKDIR /cosmos/source

# Clone source code
RUN git clone https://github.com/hsfl/cosmos-core.git core

# Configure project
WORKDIR core/build
RUN ./do_cmake_linux
# Build project
WORKDIR linux
RUN make -j8 agent agent_file
# Manually move the compiled targets to /cosmos/bin
WORKDIR programs/agents/
RUN cp agent agent_file /cosmos/bin/

############################################
# FINAL IMAGE
FROM ubuntu:20.04

# Create a non-root user and group
RUN groupadd -r cosmosuser && useradd -r -s /bin/bash -g cosmosuser -m cosmosuser

# Copy the binaries from the builder stage
WORKDIR /home/cosmosuser/cosmos/bin
COPY --from=builder /cosmos/bin/agent .
COPY --from=builder /cosmos/bin/agent_file .

# Change ownership of the binaries to the non-root user
RUN chown -R cosmosuser:cosmosuser .

# Switch to the non-root user
USER cosmosuser

# Add cosmos binary directory to path
ENV PATH="$PATH:/home/cosmosuser/cosmos/bin"

ENTRYPOINT ["agent_file"]