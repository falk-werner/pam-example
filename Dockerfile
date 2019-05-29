ARG REGISTRY_PREFIX=''
ARG CODENAME=bionic

FROM ${REGISTRY_PREFIX}ubuntu:${CODENAME}

RUN set -x \
    && apt update \
    && apt upgrade -y \
    && apt install -y --no-install-recommends \
        ca-certificates \
        curl \
        build-essential \
        cmake \
        libpam0g-dev \
        vim

ARG BUILD_DIR=/tmp/build

ARG DUMB_INIT_VERSION=1.2.2
RUN set -x \
  && mkdir -p "${BUILD_DIR}" \
  && cd "${BUILD_DIR}" \
  && curl -fSL -s -o dumb-init-${DUMB_INIT_VERSION}.tar.gz https://github.com/Yelp/dumb-init/archive/v${DUMB_INIT_VERSION}.tar.gz \
  && tar -xf dumb-init-${DUMB_INIT_VERSION}.tar.gz \
  && cd "dumb-init-${DUMB_INIT_VERSION}" \
  && make \
  && chmod +x dumb-init \
  && mv dumb-init /usr/local/bin/dumb-init \
  && dumb-init --version \
  && cd \
  && rm -rf "${BUILD_DIR}"

ARG USERID=1000
RUN set -x \
  && useradd --uid "$USERID" --groups shadow --create-home --shell /bin/bash user

RUN set -x \
  && useradd -ms /bin/bash alice

RUN echo "alice:alice" | chpasswd

RUN set -x \
  && useradd -ms /bin/bash bob

RUN echo "bob:bob" | chpasswd

COPY . /home/user/pam-example
RUN set -x \
    && cd /home/user/pam-example/.build \
    && cmake -DCMAKE_BUILD_TYPE=Debug .. \
    && make \
    && make install

RUN set -x \
    && chown -R user:user /home/user/pam-example

WORKDIR "/home/user/pam-example/.build"

ENTRYPOINT ["dumb-init", "--"]
