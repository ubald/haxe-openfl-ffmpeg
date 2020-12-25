#!/usr/bin/env bash
set -e

FLAGS=""
FLAGS="$FLAGS --disable-everything"
#FLAGS="$FLAGS --disable-encoders"
#FLAGS="$FLAGS --disable-protocols"
#FLAGS="$FLAGS --disable-filters"

FLAGS="$FLAGS --enable-protocol=file"

FLAGS="$FLAGS --enable-decoder=mpeg1video"
FLAGS="$FLAGS --enable-decoder=mpeg2video"
FLAGS="$FLAGS --enable-decoder=mp1"
FLAGS="$FLAGS --enable-decoder=mp2"
FLAGS="$FLAGS --enable-decoder=mp3"
FLAGS="$FLAGS --enable-decoder=indeo5"
FLAGS="$FLAGS --enable-decoder=pcm_s16le"
FLAGS="$FLAGS --enable-decoder=h263"
FLAGS="$FLAGS --enable-decoder=h264"
FLAGS="$FLAGS --enable-decoder=vp8"
FLAGS="$FLAGS --enable-decoder=vp9"
FLAGS="$FLAGS --enable-decoder=vorbis"

FLAGS="$FLAGS --enable-parser=mpegaudio"
FLAGS="$FLAGS --enable-parser=mpegvideo"
FLAGS="$FLAGS --enable-parser=mpeg4video"
FLAGS="$FLAGS --enable-parser=aac"
FLAGS="$FLAGS --enable-parser=h263"
FLAGS="$FLAGS --enable-parser=h264"
FLAGS="$FLAGS --enable-parser=vp8"

FLAGS="$FLAGS --enable-demuxer=mpegvideo"
FLAGS="$FLAGS --enable-demuxer=mpegps"
FLAGS="$FLAGS --enable-demuxer=mpegts"
FLAGS="$FLAGS --enable-demuxer=mp3"
FLAGS="$FLAGS --enable-demuxer=avi"
FLAGS="$FLAGS --enable-demuxer=ogg"
FLAGS="$FLAGS --enable-demuxer=matroska"

FLAGS="$FLAGS --enable-filter=aresample"

FLAGS="$FLAGS --disable-asm"

FLAGS="$FLAGS --enable-pic"
FLAGS="$FLAGS --enable-static"
FLAGS="$FLAGS --disable-shared"
FLAGS="$FLAGS --disable-debug"

pushd ffmpeg
./configure $FLAGS
make -j$(nproc)
popd

mkdir -p libs/Linux

cp -f ffmpeg/libavcodec/libavcodec.a libs/Linux/libavcodec.a
cp -f ffmpeg/libavdevice/libavdevice.a libs/Linux/libavdevice.a
cp -f ffmpeg/libavfilter/libavfilter.a libs/Linux/libavfilter.a
cp -f ffmpeg/libavformat/libavformat.a libs/Linux/libavformat.a
cp -f ffmpeg/libavutil/libavutil.a libs/Linux/libavutil.a
cp -f ffmpeg/libswresample/libswresample.a libs/Linux/libswresample.a
cp -f ffmpeg/libswscale/libswscale.a libs/Linux/libswscale.a

#cp -f ffmpeg/libavcodec/libavcodec.so libs/Linux/libavcodec.so
#cp -f ffmpeg/libavdevice/libavdevice.so libs/Linux/libavdevice.so
#cp -f ffmpeg/libavfilter/libavfilter.so libs/Linux/libavfilter.so
#cp -f ffmpeg/libavformat/libavformat.so libs/Linux/libavformat.so
#cp -f ffmpeg/libavutil/libavutil.so libs/Linux/libavutil.so
#cp -f ffmpeg/libswresample/libswresample.so libs/Linux/libswresample.so
#cp -f ffmpeg/libswscale/libswscale.so libs/Linux/libswscale.so

mkdir -p ndll/Linux64
pushd project
haxelib run hxcpp build.xml -Dlinux
popd