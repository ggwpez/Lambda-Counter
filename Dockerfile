FROM alpine:latest

RUN apk add --no-cache make g++ gmp-dev binutils
WORKDIR /root/
ADD . .
RUN make clean && make

FROM alpine:latest
WORKDIR /root/
COPY --from=0 /root/lambda_counter .

ENTRYPOINT ["/root/lambda_counter"]
