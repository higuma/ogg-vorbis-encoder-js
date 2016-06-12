# OggVorbisEncoder.js

## What is it?

OggVorbisEncoder.js is a JavaScript library that encodes audio data to Ogg Vorbis on web browsers.

[libogg](https://xiph.org/ogg/) and [libvorbis](https://xiph.org/vorbis/) are used for encoding engine. [Emscripten](http://emscripten.org) is used to convert libogg/libvorbis C code into JavaScript.

### Acknowledgement

It was originally a fork of <https://github.com/Garciat/libvorbis.js> (and also its ancestors). API has been totally re-designed and simplified. Now it has been a lower layer part of [WebAudioRecorder.js](https://github.com/higuma/web-audio-recorder-js).

## Demo

<https://higuma.github.io/ogg-vorbis-encoder-js/>

## Library files

`lib/` contains library files.

* `OggVorbisEncoder.js`: JavaScript library (uncompressed)
* `OggVorbisEncoder.min.js`: JavaScript library (minified)
* `OggVorbisEncoder.min.js.mem`: memory initializer data for OggVorbisEncoder.min.js

### Using library

Uncompressesd library is a single file. You can use it from both HTML and Web Worker.

* from HTML: `<script src="javascripts/OggVorbisEncoder.js"></script>`
* from Worker: `importScripts("javascripts/OggVorbisEncoder.js");`

Using minified library is same way. But you must pay attention to memory initializer location.

* from HTML: default directory is same as HTML which loads OggVorbisEncoder.min.js
* from Worker: default directory is same as OggVorbisEncoder.min.js path

To change memory initializer path from HTML:

``` html
<script>
// default path is on the same directory as this HTML
OggVorbisEncoderConfig = {
  memoryInitializerPrefixURL: "javascripts/"   // must end with slash
  // => changed to javascripts/OggVorbisEncoder.min.js.mem
};
</script>
<script src="javascripts/OggVorbisEncoder.min.js"></script>
```

From Worker:

``` javascript
// default path is on the same directory as OggVorbisEncoder.min.js
self.OggVorbisEncoderConfig = {
  memoryInitializerPrefixURL: "javascripts/memory/"
  // => changed to javascripts/memory/OggVorbisEncoder.min.js.mem
};
importScripts("javascripts/OggVorbisEncoder.min.js");
```

## API

``` javascript
encoder = new OggVorbisEncoder(sampleRate, numChannels, quality)
```

Create an encoder object.

* Parameters
    * `samleRate`: sampling rate [Hz]
    * `numChannels`: number of audio channels
    * `quality`: Vorbis quality (`-0.1 <= quality <= 1`)
* Returns
    * encoder object

> Current implementation supports VBR encoding only.

``` javascript
encoder.encode(buffers)
```

Encode audio buffers.

* Parameters
    * `buffers`: array of sample buffers (`[Float32Array, Float32Array ...]`)
* Returns
    * (none)

`buffers` must be an array of Float32Array audio data (range = [-1, 1]). Array length must be same as number of channels. It supports stream (incremental) processing. Sample buffers are processed to Ogg Vorbis stream and appended to internal data.

``` javascript
blob = encoder.finish([mimeType])
```

Finish encoding and get Ogg Vorbis as a Blob.

* Parameters
    * `mimeType`(optional): MIME type (default = `"audio/ogg"`)
* Returns
    * Blob object

After calling `.finish()`, all internal data is cleared (to avoid resource leak). You must create a new encoder object to encode another audio data.

``` javascript
encoder.cancel()
```

* Parameters
    * (none)
* Returns
    * (none)

Cancel encoding and clear all internal data.

You should call `.cancel()` manually to avoid resource leak when recording is canceled (encoder's internal memory is not deallocated by unbinding an object variable).

## Build

Emscripten and ruby are required to build the library.

```
$ rake
```

Download and extract libogg + libvorbis, build library files (see [Rakefile](Rakefile) for more details).

## License

libogg and libvorbis are released under Xiph's BSD-like license below. JavaScript-converted part of this library follows the same license.

<http://www.xiph.org/licenses/bsd/>

C and JavaScript wrapper API part of this library is released under MIT licence (see [LICENSE.txt](LICENSE.txt)).
