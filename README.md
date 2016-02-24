# Caffe-mini
Zero dependents caffe for testing phase

## Tips for Visual Studio 2015
If you want to compile it with Visual Studio 2015, you can add these flags:

    Right click on your project's entry in solution explorer,
    Select Properties,
    Select Configuration: All Configurations,
    Expand the C/C++ tree entry,
    Select Preprocessor,
    add these flags to "Preprocessor Definitions":
        USE_EIGEN
        _CRT_SECURE_NO_DEPRECATE
        _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS=1
        
    one flag per line.
The '_CRT_SECURE_NO_DEPRECATE' flag will fix the problem "'fopen' was declared deprecated"

The '_SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS=1' flag could fix the problem "hash_map is deprecated and will be removed"
