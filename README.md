[![Build status](https://ci.appveyor.com/api/projects/status/hjxm9hfjwljab2am?svg=true)](https://ci.appveyor.com/project/wbenny/mini-tor)

# mini-tor

mini-tor is a proof-of-concept utility for accessing internet content and hidden service content **(.onion)** via tor routers. this utility is aiming for as smallest size as possible (currently ~47kb, compressed with kkrunchy ~20kb), which is achieved by using Microsoft CryptoAPI/CNG instead of embedding OpenSSL.

### Usage

accessing internet content via mini-tor:
```c
> mini-tor.exe "http://torstatus.blutmagie.de/router_detail.php?FP=847b1f850344d7876491a54892f904934e4eb85d"
```
accessing hidden service content via mini-tor:

```c
> mini-tor.exe "http://duskgytldkxiuqc6.onion/fedpapers/federndx.htm"
```

it can even access https content:
```c
> mini-tor.exe "https://check.torproject.org/"
> mini-tor.exe "https://www.facebookcorewwwi.onion/"
```

add -v, -vv or -vvv for verbosity:
```c
> mini-tor.exe -v "http://duskgytldkxiuqc6.onion/fedpapers/federndx.htm"
> mini-tor.exe -vv "https://check.torproject.org/"
> mini-tor.exe -vvv "https://www.facebookcorewwwi.onion/"
```

you can disable logging by commenting out `#define MINI_LOG_ENABLED` in `mini/logger.h`.
this will also result in fairly reduced size of the executable.

### Description

* **mini-tor** supports both TAP & NTOR handshake.
  * TAP uses ordinary DH with 1024 bit exponent.
  * NTOR is newer type of handshake and uses curve25519.
  * you can control which handshake is used by default by changing `preferred_handshake_type` in `mini/tor/common.h`
* **mini-tor** can use either CryptoAPI or newer CNG api.
  * configurable via `mini/crypto/common.h`.
  * note that curve25519 is supported by CNG only on win10+.
    * there is a *curve25519-donna* implementation included. you can enable it by setting `MINI_CRYPTO_CURVE25519_NAMESPACE` to `ext` to get it running on older systems.
  * note that `cng::dh<>` will work only on win8.1+, because of usage `BCRYPT_KDF_RAW_SECRET` for fetching shared secret.
    * you can use `capi::dh<>` by setting `MINI_CRYPTO_DH_NAMESPACE` to `capi`.
  * anything else should run fine on win7+ (anything older is not yet supported).
* **mini-tor** creates TLS connections via SCHANNEL (look at `mini/net/ssl_socket.cpp` & `mini/net/detail/ssl_context.cpp`).
* **mini-tor** does not use default CRT or STL, everything is implemented from scratch.
* older version of **mini-tor** based purely on CryptoAPI can be found in git tag **ms_crypto_api** (unmaintained).

### Remarks

* as i stated above, this application is proof of concept.
* you can sometimes expect buggy behavior.
* this is definitelly not full-blown secure replacement for tor. 

### Compilation

compile **mini-tor** using Visual Studio 2017. solution file is included. no other dependencies are required.


### License

all the code in this repository is open-source under the MIT license. see the **LICENSE.txt** file in this repository.


if you find this project interesting, you can buy me a beer

```
  BTC 12hwTTPYDbkVqsfpGjrsVa7WpShvQn24ro
  LTC LLDVqnBEMS8Tv7ZF1otcy56HDhkXVVFJDH
```