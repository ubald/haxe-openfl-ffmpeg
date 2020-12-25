package ffmpeg;

import cpp.Lib;
import haxe.io.BytesData;
import openfl.utils.ByteArray;
import openfl.utils.Endian;

class FFMPEG {
    static public function open(fileName: String): ByteArray {
        var buffer: ByteArray = new ByteArray();
        buffer.endian = Endian.LITTLE_ENDIAN;
        hx_ffmpeg_read_file(fileName, function(channel: BytesData): Void {
            buffer.writeUTFBytes(channel.toString());
            /*for ( i in 0...channel.length ) {
            	buffer.writeByte( untyped channel[i] );
            }*/
        });
        buffer.position = 0;
        return buffer;
    }

    private static var hx_ffmpeg_read_file = Lib.load("openfl-ffmpeg", "hx_ffmpeg_read_file", 2);
}