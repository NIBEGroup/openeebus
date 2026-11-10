# Building OpenEEBUS by NIBE for Windows

This document explains how to build and run OpenEEBUS by NIBE on a Windows system for development and testing.

## Dependencies Installation

1. Install vcpkg
   ```pwsh
   git clone https://github.com/microsoft/vcpkg  
   cd .\vcpkg  
   .\bootstrap-vcpkg.bat  
   vcpkg integrate install
   ```

   Then Make sure to set the `VCPKG_ROOT` environment variable referring the vcpkg folder

1. Download and install the Visual Studio Community edition from https://visualstudio.microsoft.com/vs/community/.

1. Install OpenSSL
   ```pwsh
   vcpkg install openssl:x64-windows
   ```

1. Install libwebsockets
   ```pwsh
   vcpkg install libwebsockets:x64-windows 
   ```

1. Install cjson
   ```pwsh
   vcpkg install cjson:x64-windows 
   ```

1. Install Google Test
   ```pwsh
   vcpkg install gtest
   ```

1. Download and install the Bonjour SDK for Windows and Bonjour Print Services for Windows from https://developer.apple.com/bonjour/.  
   Then make sure that `BONJOUR_SDK_HOME` environment variable referring the Bonjour SDK folder exists or add it otherwise.

1. Install PowerShell Core

   ```pwsh
   winget install Microsoft.PowerShell
   ```

   Then add the pwsh.exe containing folder to `PATH`. PowerShell Core is used for system configuration and specific post-build steps.

## Building OpenEEBUS by NIBE in VSCode

1. Open the eebus folder in VS Code
1. Select the SDK e.g. "Visual Studio Community 2022 Release - amd64"
1. Run CMake: Configure
1. Run CMake: Build

## Running the heat_pump Example

The `heat_pump` example is built within the OpenEEBUS repository and no additional repository is needed.

Building it on Windows may require some extra steps related to enabling mDNS and HTTP. In case if mDNS announce or scanning is not working or HTTP server does not respond try following:

1. Enable mDNS Response on Windows (Administrator Required)
   Run PowerShell as Administrator and execute:
   ```pwsh
   \# Enable mDNS in Windows DNS cache  
   New-ItemProperty -Path "HKLM:\SYSTEM\CurrentControlSet\Services\Dnscache\Parameters" -Name "EnableMDNS" -Value 1 -PropertyType DWORD -Force  
    
   \# Restart DNS Client service  
   Restart-Service -Name "Dnscache" -Force  
    
   \# Restart Bonjour service  
   net stop "Bonjour Service"  
   net start "Bonjour Service"
   ```
1. Check Windows Firewall
   The firewall might be blocking mDNS traffic (port 5353 UDP):
   ```pwsh
   \# Allow mDNS through Windows Firewall (Run as Administrator)  
   New-NetFirewallRule -DisplayName "mDNS" -Direction Inbound -Protocol UDP -LocalPort 5353 -Action Allow  
   New-NetFirewallRule -DisplayName "mDNS" -Direction Outbound -Protocol UDP -LocalPort 5353 -Action Allow
   ```

1. Verify Network Profile. Make sure your network is set to "Private" not "Public":
   * Go to Settings > Network & Internet > Status
   * Click on your network connection
   * Under "Network profile", select "Private"

1. Enable port e.g. 4711 in firewall
   ```pwsh
   netsh advfirewall firewall add rule name="Heat Pump EEBUS" dir=in action=allow protocol=TCP localport=4711
   ```

# Testing

See [Testing](testing.md) for the test layout and commands to build and run unit and integration tests,
including configuration selection for Visual Studio builds.
