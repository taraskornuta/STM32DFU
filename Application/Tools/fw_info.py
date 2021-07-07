import argparse
import os
import zlib
import time
import struct
import re
import pkg_resources

# typedef struct {
#   uint32_t magic;           // patern num to finde end of image
#   struct {
#     uint8_t major;
#     uint8_t minor;
#     uint8_t patch;
#     uint8_t reserved1;
#   } version;
#   uint32_t time;            // build time utc
#   uint32_t sha;             // commit sha
#   uint8_t reserved2[12];
#   uint32_t crc32;           // firmware image crc32
# } fw_info_t;


parser = argparse.ArgumentParser()
parser.add_argument("-i", "--infile", required = True)
parser.add_argument("-o", "--outfile", required = True)
parser.add_argument("-v", "--version", type=str, required = True)
args = parser.parse_args()


def get_git_sha():
    stream = os.popen('git describe --tags --always')
    output = stream.read()
    if output.find("fatal:"):
        return 0
    else:
        return int(output)

fw_data = 0
fw_size = 0

with open(args.infile, "rb") as binary_file:
    fw_data = bytearray(binary_file.read())
    f1 = re.search(b'\xaf\xbe\xad\xde', fw_data)  #looking for magic number which means start of fw_info struct
    binary_file.close()

version = pkg_resources.parse_version(args.version)

VER_MAJOR = version.major
VER_MINOR = version.minor
VER_PATCH = version.micro
BUILD_TIME = int(time.time())
GIT_SHA    = get_git_sha()
print("VERSION    : {:d}.{:d}.{:d}\nBUILD_TIME : {:d}\nGIT_SHA    : {:d}".format(VER_MAJOR, VER_MINOR, VER_PATCH, BUILD_TIME, GIT_SHA))

fw_info_body = bytearray(struct.pack("BBBxIIxxxxxxxxxxxx", VER_MAJOR, VER_MINOR, VER_PATCH, BUILD_TIME, GIT_SHA))

with open(args.outfile, "wb+") as binary_file:
    fw_len = len(fw_data)
    copied_fw_data = bytearray()

    for x in range(fw_len - len(fw_info_body) - 4):        #copy all fw accept fw_info section - magic number
        copied_fw_data.append(fw_data[x])

    for x in range(len(fw_info_body)):                 #start of version section
        copied_fw_data.append(fw_info_body[x])

    crc32 = zlib.crc32(copied_fw_data) & 0xffffffff
    crc = crc32.to_bytes(4, byteorder="little", signed=False)
    print("CRC32      :", hex(crc32))
    for x in range(len(crc)):                          #append computed crc of whole fw + (fw_info - crc)
        copied_fw_data.append(crc[x])
    #print(copied_fw_data.hex())
    binary_file.write(copied_fw_data)
    binary_file.close()




