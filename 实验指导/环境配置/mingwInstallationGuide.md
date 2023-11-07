# MinGW-w64 Installation Guide

This guide will help you install MinGW-w64 for Windows without the `ucrt` flag.

## Install MSYS2
Install MSYS2 from this [Installer.](https://github.com/msys2/msys2-installer/releases/download/2023-10-26/msys2-x86_64-20231026.exe)  


## Install the Toolchain

Once MSYS2 is installed, open the MSYS2 terminal :

1. Install the MinGW-w64 toolchain with the following command:

    ```
    pacman -S --needed base-devel mingw-w64-x86_64-toolchain
    ```

2. Press `Enter` to accept the default packages.

3. Type `Y` to proceed with the installation when prompted.

## Update the Windows PATH Environment Variable

To add MinGW-w64 to your PATH:

1. In the Windows search bar, type `Settings` and open your Windows Settings.

2. Search for "Edit environment variables for your account".

3. Under User variables, find and select the `Path` variable, then click `Edit`.

4. Click `New` and add the MinGW-w64 bin folder path to the list. For default installations, this is typically:

    ```
    C:\msys64\mingw64\bin
    ```

5. Click `OK` to save the updated PATH.

Remember to reopen any console windows for the new PATH to be recognized.

## Known Issue

Please note that there is a known issue with the `ucrt` version in our project. It is recommended to use the standard MinGW-w64 version.
