// Copyright 2019 bianchui. All rights reserved.
#ifndef xxprofile_version_h
#define xxprofile_version_h

#define XXCOMPRESS_ZLIB 0
#define XXCOMPRESS_LZO 1
#define XXCOMPRESS_LZ4 2

#define XXCOMPRESS_CHUNKED_FLAG 0x10

#define XXCOMPRESS_ZLIB_CHUNKED 0x10
#define XXCOMPRESS_LZO_CHUNKED 0x11
#define XXCOMPRESS_LZ4_CHUNKED 0x12

#define XXCOMPRESS_NOW XXCOMPRESS_LZ4

#define XX_IsCompress(x) (XXCOMPRESS_NOW == XXCOMPRESS_##x)

XX_NAMESPACE_BEGIN(xxprofile);

struct EVersion {
    enum Enum {
        V0 = 0,
        V1 = 1,
        V2 = 2,
        V3 = 3,
        
        NOW = V3,
    };
};

struct ECompressMethod {
    enum Enum {
        Zlib = XXCOMPRESS_ZLIB,
        Lzo = XXCOMPRESS_LZO,
        Lz4 = XXCOMPRESS_LZ4,

        ZlibChunked = XXCOMPRESS_ZLIB_CHUNKED,
        LzoChunked = XXCOMPRESS_LZO_CHUNKED,
        Lz4Chunked = XXCOMPRESS_LZ4_CHUNKED,

        NOW = XXCOMPRESS_NOW,
    };
};

// file format
// struct File {
//     struct FileHeader {
//         /*0x00*/ uint32_t magic   = "XPAR";
//         /*0x04*/ uint32_t version = EVersion;
//         /*0x08*/ uint32_t flags   = Archive::Flags;
//         /*0x0C*/ uint32_t compressMethod = 0;
//     };
//
//     struct Name {
//         if (XX_PROFILE_DEBUG_Name_Serialize) {
//             /*0x00*/ uint32_t id;
//         }
//         /*0x00*/ uint32_t length;
//         /*0x04*/ char name[length];
//     };
//
//     struct Names {
//         /*0x00*/ uint32_t nameCount;
//         if (nameCount > 0) {
//             /*0x04*/ uint32_t startIndex;
//             /*0x08*/ Name names[nameCount];
//         }
//     };
//
//     struct ChunkData_v2 {
//         /*0x00*/ uint32_t sizeOrg; // chunkNodeCount = sizeOrg / sizeof(XXProfileTreeNode)
//         /*0x04*/ uint32_t sizeCom;
//         if (sizeCom == 0) {
//             /*0x08*/ XXProfileTreeNode nodes[chunkNodeCount];
//         } else {
//             /*0x08*/ uint8_t compressed[sizeCom]; // expend to XXProfileTreeNode nodes[chunkNodeCount];
//         }
//     };
//
//     struct FrameData {
//         if (fileHdr.version == EVersion::V3) {
//             /*0x00*/ uint32_t threadId;
//         }
//         /*0x00*/ uint32_t frameId;
//         /*0x04*/ Names names;
//         /*0x0?*/ uint32_t nodeCount;
//         if (fileHdr.version == EVersion::V1) {
//             /*    */ XXProfileTreeNode nodes[nodeCount];
//         } else if (fileHdr.version >= EVersion::V2) {
//             /*    */ ChunkData_v2 chunks[nodeCount / ChunkData_v2.chunkNodeCount];
//         }
//     };
//
//     struct ProfileData {
//         /*0x00*/ double secondsPerCycle;
//         /*0x08*/ FrameData frames[];
//     };
//
//     /*0x00*/ FileHeader fileHdr;
//     /*0x10*/ ProfileData data;
// };

XX_NAMESPACE_END(xxprofile);

#endif /* xxprofile_version_h */
