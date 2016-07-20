# mini-tor

mini-tor is a proof-of-concept utility for accessing internet content and hidden service content **(.onion)** via tor routers. this utility is aiming for as smallest size as possible (currently ~50kb, compressed with UPX ~24kb), which is achieved by using Microsoft CryptoAPI instead of embedding OpenSSL.

### Usage

accessing internet content via mini-tor:
```c
> mini-tor.exe "http://torstatus.blutmagie.de/router_detail.php?FP=847b1f850344d7876491a54892f904934e4eb85d"
```
accessing hidden service content via mini-tor:

```c
> mini-tor.exe "http://duskgytldkxiuqc6.onion/fedpapers/federndx.htm"
```

### Remarks

* s i stated above, this application is proof of concept.
* you can sometimes expect buggy behavior.
* this is definitelly not full-blown secure replacement for tor. 

### Compilation

compile **mini-tor** using Visual Studio 2015 Update 3. solution file is included. no other dependencies are required.


### License

all the code in this repository is open-source under the MIT license. see the **LICENSE.txt** file in this repository.
