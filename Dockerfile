FROM ubuntu:16.04

WORKDIR /root/lanchat

RUN apt update
RUN apt install build-essential g++ libgtest-dev cmake google-mock python2.7 python-pip -y
RUN cd /usr/src/gtest && cmake CMakeLists.txt && make && cp *.a /usr/lib
RUN pip install watchdog

CMD ["make", "watch"]
