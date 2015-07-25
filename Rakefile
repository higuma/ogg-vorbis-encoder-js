require 'fileutils'

LIBOGG_VER = '1.3.2'
LIBVORBIS_VER = '1.3.5'
LIBOGG = "libogg-#{LIBOGG_VER}"
LIBVORBIS = "libvorbis-#{LIBVORBIS_VER}"
LIBOGG_TARBALL = "#{LIBOGG}.tar.gz"
LIBVORBIS_TARBALL = "#{LIBVORBIS}.tar.gz"
BASE_URL = 'http://downloads.xiph.org/releases'
LIBOGG_URL = "#{BASE_URL}/ogg/#{LIBOGG_TARBALL}"
LIBVORBIS_URL = "#{BASE_URL}/vorbis/#{LIBVORBIS_TARBALL}"

task :libogg do
  sh "wget #{LIBOGG_URL}" unless File.exist? LIBOGG_TARBALL
  sh "tar zxf #{LIBOGG_TARBALL}" unless File.exist? LIBOGG
end

task :libvorbis do
  sh "wget #{LIBVORBIS_URL}" unless File.exist? LIBVORBIS_TARBALL
  sh "tar zxf #{LIBVORBIS_TARBALL}" unless File.exist? LIBVORBIS
end

INCLUDES = [
  'include',
  "#{LIBOGG}/include",
  "#{LIBVORBIS}/include",
  "#{LIBVORBIS}/lib"
].map {|include| "-I #{include}" }
 .join ' '

EMCC_COMPILE_OPTIONS = "-O3 -ffast-math #{INCLUDES}"
EMCC_LINK_OPTIONS = [
  '-s ALLOW_MEMORY_GROWTH=0',
  '-s ASM_JS=1',
  '-s EXPORTED_FUNCTIONS=@src/exports.json',
  '--pre-js src/pre.js',
  '--post-js src/post.js'
].join ' '

OUTPUT_DIR = 'output'
LIBOGG_OUTPUT_DIR = "output/#{LIBOGG}"
LIBOGG_FILES = [
  'bitwise.c',
  'framing.c'
]
LIBVORBIS_OUTPUT_DIR = "output/#{LIBVORBIS}"
LIBVORBIS_FILES = [
  'analysis.c',
  # 'barkmel.c', 
  'bitrate.c',
  'block.c',
  'codebook.c',
  'envelope.c',
  # 'floor0.c',
  'floor1.c',
  'info.c',
  # 'lookup.c',
  'lpc.c',
  # 'lsp.c',
  'mapping0.c',
  'mdct.c',
  'psy.c',
  # 'psytune.c',
  'registry.c',
  'res0.c',
  'sharedbook.c',
  'smallft.c',
  # 'synthesis.c',
  # 'tone.c',
  'vorbisenc.c',
  # 'vorbisfile.c',
  'window.c'
]
SRC_OUTPUT_DIR = "output/src"
SRC_FILES = [
  'encoder.c'
]

def enum_sources(files, dir)
  files.map {|file| "#{dir}/#{file}" }.sort
end

def enum_targets(sources, dir)
  sources.map {|source| "#{dir}/#{File.basename(source).sub /c$/, 'o'}" }
end

LIBOGG_SOURCES = enum_sources LIBOGG_FILES, "#{LIBOGG}/src"
LIBVORBIS_SOURCES = enum_sources LIBVORBIS_FILES, "#{LIBVORBIS}/lib"
SRC_SOURCES = enum_sources SRC_FILES, 'src'

LIBOGG_OUTPUTS = enum_targets LIBOGG_SOURCES, LIBOGG_OUTPUT_DIR
LIBVORBIS_OUTPUTS = enum_targets LIBVORBIS_SOURCES, LIBVORBIS_OUTPUT_DIR
SRC_OUTPUTS = enum_targets SRC_SOURCES, SRC_OUTPUT_DIR

def compile_files(sources, target_dir)
  FileUtils.makedirs target_dir
  targets = enum_targets sources, target_dir
  targets.each.zip(sources).each do |target, source|
    sh "emcc #{EMCC_COMPILE_OPTIONS} -o #{target} #{source}"
  end
end

task compile_ogg: :libogg do
  compile_files LIBOGG_SOURCES, LIBOGG_OUTPUT_DIR
end

task compile_vorbis: :libvorbis do
  compile_files LIBVORBIS_SOURCES, LIBVORBIS_OUTPUT_DIR
end

task :compile_src do
  compile_files SRC_SOURCES, SRC_OUTPUT_DIR
end

task build_lib: [:compile_ogg, :compile_vorbis, :compile_src] do
  FileUtils.makedirs 'lib'
  sources = (LIBOGG_OUTPUTS + LIBVORBIS_OUTPUTS + SRC_OUTPUTS).join ' '
  sh "emcc -O1 #{EMCC_LINK_OPTIONS} -o lib/OggVorbisEncoder.js #{sources}"
  sh "emcc -O3 #{EMCC_LINK_OPTIONS} -o lib/OggVorbisEncoder.min.js #{sources}"
end

task :libclean do
  sh 'rm -rf lib'
end

task :clean do
  sh 'rm -rf output'
end

task distclean: :clean do
  sh "rm -rf #{LIBOGG}"
  sh "rm -rf #{LIBVORBIS}"
  sh "rm -f #{LIBOGG_TARBALL}"
  sh "rm -f #{LIBVORBIS_TARBALL}"
end

task default: :build_lib
