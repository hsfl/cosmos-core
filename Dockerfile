FROM ubuntu:18.04

# Install CMake
RUN apt-get update \
  && apt-get install build-essential wget libz-dev gcc-7 g++-7 cmake git openssl libssl-dev libsasl2-dev libboost-system-dev libboost-filesystem-dev libboost-chrono-dev libboost-program-options-dev libboost-test-dev -y

# Retrieve required repositories
RUN wget https://github.com/mongodb/mongo-c-driver/releases/download/1.13.1/mongo-c-driver-1.13.1.tar.gz \
  && tar xzf mongo-c-driver-1.13.1.tar.gz
RUN git clone https://github.com/mongodb/mongo-cxx-driver.git --branch releases/stable --depth 1

# Copy COSMOS
WORKDIR /cosmos
COPY . ./core
WORKDIR /cosmos/core/agent_build

RUN cmake .. \
  && make -j4 \
  && make install

# Mongo C Installation
WORKDIR /mongo-c-driver-1.13.1/cmake-build
RUN cmake -DENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF .. \
  && make -j4 \
  && make install

# Mongo CXX Installation
WORKDIR /mongo-cxx-driver/build
RUN cmake -DCMAKE_BUILD_TYPE=Release -DBSONCXX_POLY_USE_BOOST=1 -DCMAKE_INSTALL_PREFIX=/usr/local .. \
  && make -j4 \
  && make install

# COSMOS MongoDB Installation
WORKDIR /cosmos/projects
RUN git clone https://github.com/spjy/cosmos-mongodb.git
WORKDIR /cosmos/projects/cosmos-mongodb/agent_build
RUN cmake ../source \
  && make -j4

ENV LD_LIBRARY_PATH="/usr/local/lib"
ENV PATH="/cosmos/projects/cosmos-mongodb/agent_build:/root/cosmos/bin:${PATH}"
RUN chmod +x /cosmos/core/entrypoint.sh
ENTRYPOINT ["cosmos/entrypoint.sh"]
