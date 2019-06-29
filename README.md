# mhz14a

MH-Z14A CO2 sensor reader

## Installation

Program uses cmake as a build system. It is encouraged to compile it out of
source, e.g. in root of the repository one can execute:

```
mkdir -p build && cd build
cmake ..
make
sudo make install
```

By default `mhz14a` binary should be installed to /usr/local/bin. If you have it
added to your `$PATH`, you are able to type:

```
mhz14a -h
```

This shows short summary of program options.

## Usage

Basic usage of the program is as simple as executing:

```
mhz14a -r
```

However this requires sensor to be connected to `/dev/ttyS0`, so if it somewhere
else, it has to be passed explicitly, like:

```
mhz14a -r -d /dev/ttyUSB0
```

In non-interactive uses, it is good idea to pass some timeout, as otherwise
program will hang forever in case of faulty transmission, and number of retries
(defaults to 1). For this purpose following command could be used to wait 3
times maximum of 30 seconds:

```
mhz14a -r -d /dev/ttyUSB0 -t 30 -T 3
```

## Bug reports

All bugs should be reported via Github. To make diagnosis easier, before
gathering faulty situation report, logging should be turned on by passing
`--log=DEBUG` parameter. In case of crashes, coredumps are welcome and having
program distribution with debugging symbols is encouraged, to allow better
understanding of the problem.

## Testing

By default unit tests are switched off when build the program. For compilation
and running, they require cmocka to be installed in system. Then one can
reconfigure cmake by appending `-DENABLE_TESTS=ON` to cmake invokation in
installation step. After that, compilation will also compile test programs,
which can be run using `make test` command.

## License

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
