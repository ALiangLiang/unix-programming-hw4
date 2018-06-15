# unix-programming-hw4

## Author

0656088 劉威良

## Features

- Chat with friends on same lan environment.
- Prevent echo (receive and print message from local).
- Send and receive in the same time. Implement with "fork".
  - Establish a child process to setup a listener. Used to receive messages from other user immediatly.
- Divide code in to two part(main & hw4). Because I want to auto testing this project with gtest(google test framework).

## Build

```sh
sudo make
```

## Test (Not completed yet)

Because I have another homework of software-testing, I use this project as testing target. But until deadline I'm not finish testing code yet.

```sh
sudo make test
```
