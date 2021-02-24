## How to Generate Production Code in windows
1. Install git and log in github/gitlab 
2. Install Arduino-cli and install avr core
Follow these steps for build the firmware for production environment. We will perform all the actions using windows powershell 
3. First open powershell in administrator mode and run the followinf command to access the shell to run script
```
set-executionpolicy unrestricted
 ```
4. Then run the shell script `.\download.ps1` to download all the libraries from github/gitlab to the lib directory under src directory. Please make sure that all the libraries are in master branch and you are logged in github/gitlab
```
.\download.ps1
```

## How to build a production code
We will use arduino-cli to compile the projects and generate hex file. For production build a global compiler flag `PROD_BUILD` used throughout all the libraries and main codebase. So arduino cli has to pass the compiler flag while build the projects. 
```
arduino-cli compile --build-property  build.extra_flags=-DPROD_BUILD
```