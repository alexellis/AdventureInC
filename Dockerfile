FROM alpine

RUN apk --update add gcc make musl-dev

RUN mkdir -p /code/adventureinc
WORKDIR /code/adventureinc
ADD ./ ./
RUN make

CMD ["./game"]

