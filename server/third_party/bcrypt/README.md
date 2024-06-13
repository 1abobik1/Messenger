# bcrypt

Source: [hilch/Bcrypt.cpp](https://github.com/hilch/Bcrypt.cpp), commit `0d18b6a` (2023-12-04),
license in [LICENSE](LICENSE) (BSD 4-Clause) and in the file headers (OpenBSD, Niels Provos).

Changes: `src/openbsd.h` takes the salt from `std::random_device` instead of `rand()` + `srand(time(NULL))`.
