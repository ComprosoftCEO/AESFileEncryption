# AES File Encryption (AFE)
C utility to encrypt a file using AES-256

<br>

## Compiling
This program is designed to work with the cross-platform IDE [Code::Blocks](http://www.codeblocks.org/). For Windows platforms, be sure to download and install Code::Blocks with MinGW included.

Before compiling, download and install the [GNU Realine Library](https://tiswww.case.edu/php/chet/readline/rltop.html):
* __Debian Linux:__ sudo apt-get install libreadline6 libreadline6-dev
* __Fedora Linux:__ sudo yum install libreadline6 libreadline6-dev
* __Windows:__ Download [Readline for Windows](http://gnuwin32.sourceforge.net/packages/readline.htm).
  1. Copy the contents of the "include" folder into _\<CodeBlocks\>\MinGW\include_
  2. Copy the contents of the "lib" folder into _\<CodeBlocks\>\MinGW\lib_
  3. Copy the runtime binaries (history5.dll and readline5.dll) into the same folder as the target application (found in _AesFileEncryption\bin\Debug_ or _AesFileEncryption\bin\Release_)

When compiling on Windows, you might need to define the constant *\_WIN32*. In the Code::Blocks toolbar, go to Settings -> Compiler Settings -> Global Compiler Settings. Under the tab #defines, add the line *\_Win32*.

Open AesFileEncryption.cbp in Code::Blocks to edit the project and compile the code.

<br>

## Usage
__afe (-e | -d | -v) \[-r\] \<in-file\> \[out-file\]__
* -e = Encrypt file
* -d = Decrypt file
* -v = Verify encryption of \<in-file\> to confirm that password is correct
* -r = Remove the backup file (only if \[out-file\] isn't specified)

If \[out-file\] isn't specified, then encryption or decryption overwrites \<in-file\>. However, a backup is created named \<in-file\>.bak.

<br>

## Credits
* AES implementation is from the repository [tiny-AES-c](https://github.com/kokke/tiny-AES-c) by kokke.
* SHA-256 is from the repository [crypto-algorithms](https://github.com/B-Con/crypto-algorithms/) by Brad Conte (B-Con).
