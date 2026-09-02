# OpenEEBUS by NIBE

> ⚠️ **Development Status**
>
> This library is currently under active development. While we strive to maintain a stable and consistent interface, please be aware that breaking changes may occur as we refine the design and functionality. We do our best to minimize disruption and communicate significant updates clearly.

OpenEEBUS by NIBE is a Apache 2.0 licensed software library that implements the EEBUS protocol for communication between devices in a home automation environment. It's easy to build and cross-build via cmake.

## License

This repository is licensed under the Apache 2.0 License.

    Copyright 2025 NIBE AB

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

## EEBUS Protocol

The EEBUS protocol is a communication standard designed for home automation and smart grid applications. It enables devices from different manufacturers to communicate with each other in a standardized way, facilitating interoperability and integration.

The source code in this repository implements the EEBUS protocol, based on the [SHIP and SPINE specifications](https://www.eebus.org/media-downloads/) and to utilize the library you need to have some basic understanding of the EEBUS protocol.

### Supported Use Cases

| Use Case |  Actors  | Scenarios | Description                          |
|----------|----------|-----------|--------------------------------------|
| LPC      |  EG, CS  |   1 - 4   | Limitation of power consumption      |
| LPP      |  EG, CS  |   1 - 4   | Limitation of power production       |
| MPC      |  MA, MU  |   1 - 5   | Monitoring of power consumption      |
| MGCP     |  MA, GCP |   1 - 7   | Monitoring of grid connection point  |

### Limitations

The current implementation of the OpenEEBUS by NIBE library supports a subset of the EEBUS protocol features. Notably, it does not yet include support for:

* Multiple remote SHIP node connections
* SPINE message approve/deny procedure

## Documentation

### Contribution

* [Contributing Guidelines](CONTRIBUTING.md)
* [Code of Conduct](CODE_OF_CONDUCT.md)
* [License](LICENSE.txt)
* [Vulnerability reporting policy](SECURITY.md)

### Building and Running

* [Building on Linux](docs/build_linux.md)
* [Building on macOS](docs/build_macos.md)
* [Building on Windows](docs/build_windows.md)

### Configuration

* [Compile-Time Configuration](docs/configuration.md)

### API Changes

* [API Changes](docs/api_changes.md)

## Support

NIBE does not offer support or implementation help for the OpenEEBUS by NIBE library but we encourage contributions and discussions within the open-source community. Please refer to the [Contributing Guidelines](CONTRIBUTING.md) for more details on how.

> ❗ **Important – NIBE Product Support**
> 
> This repository is <ins>not</ins> for support requests or error reports related to NIBE products or the usage of EEBUS within NIBE products.
> Such inquiries will not be handled here. Please contact NIBE Customer Support through the official support channels for further help.

## Acknowledgements

OpenEEBUS by NIBE has been developed by NIBE AB but it wouldn't have been possible without the support of the open-source community. We'd especially like to thank [Andreas Linde](http://www.andreaslinde.de) and the [eebus-go](https://github.com/enbility/eebus-go) community for inspiration and collaboration.
