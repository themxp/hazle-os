FROM alpine:3.19

RUN apk add --no-cache \
    build-base \
    nasm \
    gcc \
    make \
    grub \
    grub-bios \
    xorriso \
    mtools \
    qemu-system-i386

WORKDIR /workspace

CMD ["/bin/sh"]
