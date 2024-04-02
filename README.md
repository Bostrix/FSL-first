# Instructions for Compiling and Running first
## Clone the Repository

Begin by cloning the project repository from GitHub onto your local machine. You can do this by running the following command in your terminal or command prompt:

```bash
git clone https://github.com/Bostrix/FSL-first.git
```
This command will create a local copy of the project in a directory named "FSL-first".

## Navigate to Project Directory
Change your current directory to the newly cloned project directory using the following command:
```bash
cd FSL-first
```
## Installing Dependencies

Before compiling, make sure you have the following dependencies installed:

- Blas
- Lapack
- zlib

You can install these dependencies on Ubuntu using the following commands:

```bash
sudo apt-get install libblas-dev libblas3
sudo apt-get install liblapack-dev liblapack3
sudo apt-get install zlib1g zlib1g-dev
```

## Modify Makefile:
- After installing the necessary tools, modify the makefile to include additional LDFLAGS for the required libraries. Open the makefile and add the following lines to the Makefile. 
```bash
# Additional LDFLAGS for znzlib library
ZNZLIB_LDFLAGS = -L/path/to/your/znzlib -lfsl-znz

# Additional LDFLAGS for meshclass library
MESHCLASS_LDFLAGS = -L/path/to/your/meshclass -lfsl-meshclass

# Additional LDFLAGS for first_lib library
FIRSTLIB_LDFLAGS = -L/path/to/your/first_lib -lfsl-first_lib
```
Replace `Path to your library` with the actual path to your directories. Make sure you added `$(ZNZLIB_LDFLAGS)`,`$(MESHCLASS_LDFFLAGS)` and `$(FIRSTLIB_LDFFLAGS)`  in the compile step of the makefile.
- Modify Makefile of meshclass:
Modify the makefile of meshclass to include additional LDFLAGS for the required libraries. You are using newimage, miscmaths, NewNifti, cprob, znzlib, utils libraries as LDFLAGS variable in meshclass Makefile. 

 Replace Path to your appropriate library with the actual path to your directories.
```bash
LDFLAGS += -L/path/to/your/newimage/directory -L/path/to/your/miscmaths/directory -L/path/to/your/NewNifti/directory -L/path/to/your/cprob/directory -L/path/to/your/znzlib/directory -L/path/to/your/utils/directory
```
Once the modifications have been saved, you may use the following command in your terminal to rebuild the meshclass separately:
```bash
make
```
The command executes the Makefile in the mesh class and rebuilds it separately based on the modified configuration and source code changes. After re-running the make command, return to the main first directory by `cd ..` and attempt to rebuild the project again.
## Compiling 
Execute the appropriate compile command to build the first tool.
```bash
make clean
make
```
- Resolving Shared Library Errors
When running an executable on Linux, you may encounter errors related to missing shared libraries.This typically manifests as messages like:
```bash
./first: error while loading shared libraries: libexample.so: cannot open shared object file:No such file or directory
```
To resolve these errors,Pay attention to the names of the missing libraries mentioned in the error message.Locate the missing libraries on your system. If they are not present, you may need to install the corresponding packages.If the libraries are installed in custom directories, you need to specify those directories using the `LD_LIBRARY_PATH` environment variable. For example:
```bash
export LD_LIBRARY_PATH=/path/to/custom/libraries:$LD_LIBRARY_PATH
```
Replace `/path/to/custom/libraries` with the actual path to the directory containing the missing libraries.Once the `LD_LIBRARY_PATH` is set, attempt to run the executable again.If you encounter additional missing library errors, repeat steps until all dependencies are resolved.

- Resolving "The environment variable FSLOUTPUTTYPE is not defined" errors
If you encounter an error related to the FSLOUTPUTTYPE environment variable not being set.Setting it to `NIFTI_GZ` is a correct solution, as it defines the output format for FSL tools to NIFTI compressed with gzip.Here's how you can resolve:
```bash
export FSLOUTPUTTYPE=NIFTI_GZ
```
By running this command, you've set the `FSLOUTPUTTYPE` environment variable to `NIFTI_GZ`,which should resolve the error you encountered.

## Running first

After successfully compiling, you can run first by executing:
```bash
./first -i <input_image> -k <output_name> -m <input_model> -l <flirt_matrix>
```
This command will execute the first tool.
