# Caffe-mini
Zero dependents caffe for testing phase

## Compile it with Visual Studio 2015
First clone this project from the original git:

    git clone https://github.com/ufoym/Caffe-mini.git

Then create an empty project with Visual Studio 2015. In the Solution Explorer, right click Source Files -> Add Existing Item, then choose the caffe.cpp from the project you forked from git.

Before you can run it, you must do these things:

    Right click on your project's entry in solution explorer,
	
    Select Properties,
	
	Choose C/C++, then click the General,
	
	In the "Additional Include Directories", add the whole project you forked from git, it is \path\to\Caffe-mini
	
	Still in C/C++, Select Preprocessor,
	
    add these flags to "Preprocessor Definitions":
	
        USE_EIGEN
		
        _CRT_SECURE_NO_DEPRECATE
		
        _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS=1
        
    one flag per line.
	
After this, choose Source Files in Solution Explorer, right click and add New Filter, the Filter name should be 'caffe', and then copy all the files in the folder 'caffe' from the git project, to this New Filter

Create another New Filter called 'google', again, copy all the files in the original 'google' folder to this new Filter

Now, you can build your own solution
	

