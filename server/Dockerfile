# syntax=docker/dockerfile:1
FROM debian:bookworm AS build
RUN apt-get update \
    && apt-get install -y --no-install-recommends ca-certificates cmake g++ git libpq-dev make \
    && rm -rf /var/lib/apt/lists/*
WORKDIR /src
COPY . .
RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF \
    && cmake --build build -j"$(nproc)" --target messenger-server

FROM debian:bookworm-slim
RUN apt-get update \
    && apt-get install -y --no-install-recommends libpq5 \
    && rm -rf /var/lib/apt/lists/* \
    && useradd --system --no-create-home messenger
COPY --from=build /src/build/messenger-server /usr/local/bin/messenger-server
USER messenger
EXPOSE 9000
CMD ["messenger-server"]
