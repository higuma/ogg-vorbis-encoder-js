  var encoder_init = Module._encoder_init,
      encoder_clear = Module._encoder_clear,
      encoder_analysis_buffer = Module._encoder_analysis_buffer,
      encoder_process = Module._encoder_process,
      encoder_data_len = Module._encoder_data_len,
      encoder_transfer_data = Module._encoder_transfer_data,
      HEAPU8 = Module.HEAPU8,
      HEAPU32 = Module.HEAPU32,
      HEAPF32 = Module.HEAPF32;

  var Encoder = function(sampleRate, numChannels, quality) {
    this.numChannels = numChannels;
    this.oggBuffers = [];
    this.encoder = encoder_init(this.numChannels, sampleRate, quality);
  };

  Encoder.prototype.encode = function(buffers) {
    var length = buffers[0].length;
    var analysis_buffer = encoder_analysis_buffer(this.encoder, length) >> 2;
    for (var ch = 0; ch < this.numChannels; ++ch)
      HEAPF32.set(buffers[ch], HEAPU32[analysis_buffer + ch] >> 2);
    this.process(length);
  };

  Encoder.prototype.finish = function(mimeType) {
    this.process(0);
    var blob = new Blob(this.oggBuffers, {type: mimeType || 'audio/ogg'});
    this.cleanup();
    return blob;
  };

  Encoder.prototype.cancel = Encoder.prototype.cleanup = function() {
    encoder_clear(this.encoder);
    delete this.encoder;
    delete this.oggBuffers;
  };

  Encoder.prototype.process = function(length) {
    encoder_process(this.encoder, length);
    var len = encoder_data_len(this.encoder);
    if (len > 0) {
      var data = encoder_transfer_data(this.encoder);
      this.oggBuffers.push(new Uint8Array(HEAPU8.subarray(data, data + len)));
    }
  };

  self.OggVorbisEncoder = Encoder;
})(self);
